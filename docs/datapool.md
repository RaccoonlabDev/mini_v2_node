# Datapool

The datapool ([Src/common/datapool.hpp](../Src/common/datapool.hpp)) passes data
between modules without a protocol. A logic module measures and publishes a
topic; one frontend per protocol subscribes and puts it on its bus. Supporting
one more protocol then means writing one more frontend.

## Layout

| Path | What |
|---|---|
| `Src/topics/<topic>.hpp` | one topic: the message and its tag |
| `Src/modules/<module>/` | logic modules, which publish and read topics |
| `Src/modules/<protocol>/<topic>/` | the frontend of a protocol: `dronecan`, `cyphal`, `raw_can` |

A topic may have several publishers and several subscribers, so a module is not
tied to one topic. Naming the module after the topic it publishes, as
`board_status` does, is a style choice, not a rule. An application keeps its own
topics in `src/topics/`.

## Messages

Messages are written by hand now and may be generated from YAML later, so they
keep to what such a schema can say:

- One topic per header: the message `BoardStatus` and its tag `BoardStatusTopic`
  in `board_status.hpp`. The second argument of the tag is the instance count.
- Fields only, no methods, constructors or default values. A field is `bool`,
  `int8_t`…`int64_t`, `uint8_t`…`uint64_t`, `float`, `double`, an enum of the
  same header or a fixed-size array of those. No pointers, strings or nested
  structs.
- An enum has an explicit underlying type: `enum class` for a choice, a plain
  `enum` with prefixed constants for bit flags.
- Units: SI in a comment (`// V`); anything else in the name (`hardware_version_adc`).
- Not available: NaN for a float; an integer field documents its own value.

The compiler checks the gist of the second rule: a message must be an aggregate
with a standard layout.

From [board_status.hpp](../Src/topics/board_status.hpp):

```cpp
struct BoardStatus {
    float temperature;              // K, MCU internal sensor
    float voltage_5v;               // V, the 5 V rail after the DC-DC
    float voltage_vin;              // V, the supply before the DC-DC
    // ...
};

struct BoardStatusTopic : datapool::Topic<BoardStatus> {};
```

Nothing is registered or generated: include the header where you publish or read.

## Publish and read

```cpp
datapool::Publisher<topics::BoardStatusTopic> status_pub;
status_pub.publish(status);

datapool::Subscriber<topics::BoardStatusTopic> status_sub;
topics::BoardStatus status;
if (status_sub.update(&status)) { /* a new sample: put it on the bus */ }
```

| Call | Returns true when |
|---|---|
| `publish(msg)` | the instance exists |
| `updated()` | a sample arrived since this subscriber's last read |
| `copy(&msg)` | anything was published, new or not |
| `update(&msg)` | a new sample was copied |

## Rules

- A topic keeps only the latest sample. A frontend polls faster than the logic
  publishes and sends a sample only on `update()`.
- Publish and read from modules only, never from an interrupt.
- A value the hardware can't measure is NaN, or a value the topic documents.
- The logic module passes `VSSC_BIT_NONE`. The frontend owns what the bus sees,
  the health and the VSSC bit included, and takes the place of the module it
  replaced, so the VSSC byte keeps its layout.
- Storage is static: no heap, the size is known at link time.

## Example: board_status

| Module | Built in |
|---|---|
| `board_status` | with any of its frontends |
| `dronecan/board_status` | DroneCAN targets: dev.Temperature and two CircuitStatus |
| `cyphal/board_status` | Cyphal targets: three `uavcan.si.unit` scalars |
| `raw_can/board_status` | `rl_sitl_raw_can` only: an example of a custom protocol |
