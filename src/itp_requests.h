#pragma once

#include "itp_packet.h"
#include "itp_packets.h"
#include "itp_shim.h"
#include <coroutine>
#include <memory>
#include <queue>

namespace itp_packet {

static constexpr char REQUESTS_TAG[] = "mitsubishi_itp.requests";

//
class ITPByteProvider {
 public:
  virtual size_t available() = 0;
  virtual bool read_array(uint8_t *data, size_t len) = 0;
  virtual bool read_byte(uint8_t *data) = 0;
  virtual void write_array(const uint8_t *data, size_t len) = 0;
  virtual ~ITPByteProvider() = default;
};

// Common base class for heatpumps and thermostats to provide packet buffer and check_for_packet functionality
class ITPPacketReader {
 public:
  ITPPacketReader(ITPByteProvider *byte_provider, const char *log_name)
      : byte_provider_{*byte_provider}, log_name_{log_name} {}

 protected:
  ITPByteProvider &byte_provider_;
  uint8_t packet_buffer_[PACKET_MAX_SIZE];
  uint8_t buffer_position_ = 0;
  std::optional<RawPacket> check_for_packet();

 private:
  const char *log_name_;
};

//
// COROUTINE COMPONENTS
//

// Provides an object to receive/manage the coroutine_handle, and check to see if coroutine is still running
struct [[nodiscard("Task maintains coroutine frame and must be stored until routine is complete.")]] Task {
  struct promise_type {
    Task get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };

  std::coroutine_handle<promise_type> handle = nullptr;

  Task(std::coroutine_handle<promise_type> h) : handle(h) {}
  Task() = default;

  // Move constructor: Steals the handle and nulls out the source
  Task(Task && other) noexcept : handle(other.handle) { other.handle = nullptr; }

  // Move assignment: Destroys any existing frame, steals the new one
  Task &operator=(Task &&other) noexcept {
    if (this != &other) {
      if (handle) {
        handle.destroy();
      }
      handle = other.handle;
      other.handle = nullptr;
    }
    return *this;
  }

  // Prevent copies (they mess with handle pointer)
  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;

  bool is_running() const { return handle && !handle.done(); }

  ~Task() {
    if (handle)
      handle.destroy();
  }
};

// Context for requests sent to heat pump (allows passing request/result in and out of coroutine)
struct RequestContext {
  Packet request;
  std::optional<RawPacket> raw_response;
  std::coroutine_handle<> handle;

  RequestContext(Packet request) : request(request) {}
};

// TODO: When we decide to start responding with cached packets, this struct should be modified to:
// - Move push to queue to await_suspend
// - In await_ready() call a lambda or function on Heatpump to ask for cached response

// "Awaiter" for requests sent to heatpump
// On construction, this will keep a non-owned copy of the context pointer for use on resume,
// and move ownership of the context to the provided queue.
template<class PType, class RequestHandler> struct RequestAwaiter {
  static_assert(std::is_base_of_v<Packet, PType>, "PType must derive from Packet");
  RequestContext *ctx_ptr;
  RequestHandler &request_handler;

  RequestAwaiter(std::unique_ptr<itp_packet::RequestContext> &&req, RequestHandler &handler)
      : ctx_ptr(req.get()), request_handler(handler) {
    request_handler.enqueue_request(std::move(req));  // Handler takes ownership
  };

  bool await_ready() const noexcept { return false; }

  void await_suspend(std::coroutine_handle<> h) noexcept { ctx_ptr->handle = h; }

  std::optional<PType> await_resume() noexcept {
    if (ctx_ptr->raw_response) {
      std::optional<PType> response_pkt = Packet::try_from_raw<PType>(std::move(ctx_ptr->raw_response.value()));
      if (response_pkt) {
        response_pkt->set_sequence(ctx_ptr->request.get_sequence());
      }
      return response_pkt;
      // return PType(std::move(ctx_ptr->raw_response.value()));  // Last use of ctx_ptr before Awaiter is destroyed.
    }
    return std::nullopt;
  }
};

}  // namespace itp_packet
