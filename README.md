# itp-packet
Packet parsing, generation, and utilities for the ITP protocol

## Usage
In the [Mitsubishi ITP ESPHome component](https://github.com/muart-group/esphome-components/tree/dev/components/mitsubishi_itp) this library is primarily used by:
- Using the RawPacket constructor to create a RawPacket from an array of bytes.
- Using specific Packet constructors to wrap the RawPacket with useful functions.
- Implementing the PacketProcessor interface to easily handle incoming packets.

## Including
To include in your custom component, you can use:
```python
async def to_code(config):
    cg.add_library(
        name="itp-packet",
        repository="https://github.com/muart-group/itp-packet.git",
        version="main"
    )
```

To include in your config from the GitHub repo, use:
```yaml
esphome:
  ...
  libraries:
    - itp-packet=https://github.com/muart-group/itp-packet.git#main
```

For local development you can include the path to this repo locally using ESPHome config:
```yaml
esphome:
  ...
  libraries:
    - itp-packet=file:///workspaces/itp-packet
```

