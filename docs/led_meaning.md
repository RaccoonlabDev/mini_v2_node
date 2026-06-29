# LED status meaning

The RGB LED follows a standard pattern common to all RaccoonLab boards — a convention typical for robotic applications.

| LED Pattern & Color | Meaning |
|-|-|
| **Blinking Blue/Red** | Initialization / Calibration       |
| **Blinking Yellow**   | Minor Failure / Warning            |
| **Blinking Magenta**  | Major Failure / Error              |
| **Blinking Red**      | Fatal Malfunction / Critical       |
| **Blinking Blue**     | Ready — No GPS Lock                |
| **Solid Blue**¹       | Engaged — No GPS Lock              |
| **Blinking Green**²   | Ready — 3D Fix (8+ satellites)     |
| **Solid Green**¹²     | Engaged — 3D Fix (8+ satellites)   |

¹ Applicable to nodes that support an **engaged** state (e.g. actuator nodes).

² Applicable to nodes that use **GPS** (e.g. GNSS or combined nodes).
