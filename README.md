# Mini v2 Cyphal/DroneCAN application template

This repo introduces a simple ready-to-use Cyphal/DroneCAN application template for [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) node.

Please, refer Wiki for details:
- [Cyphal template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/cyphal),
- [DroneCAN template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/dronecan).

### 1. Brief hardware info

| View | Top view | Bottom view | Pinout |
| ---- | --- | ------ |-|
| <img src="https://docs.raccoonlab.co/assets/img/view.bbf1e631.png" alt="drawing" width="200"> | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="200"> | <img src="https://docs.raccoonlab.co/assets/img/view_bottom.ee41f6d5.png" alt="drawing" width="200"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing" width="200"> |

### 2. Brief Cyphal template application capabilities

By default the application is as simple as possible. It just blinks the RGB LED, subscribes to the setpoint topic to control a single PWM (PWM1) and publishes a feedback with the latest applied setpoint. It doesn't have any safety features like TTL, but you are free to extend it as you want.

The default capabilities of the node are shown on the picture below:

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/yukon.png" alt="drawing">

### 3. Q&A

If you are strugguling with the software building, please refer to the build workflow [build.yml](.github/workflows/build.yml) for a hint. If it doesn't help, you can open [an issue]( https://github.com/RaccoonlabDev/mini_v2_node/issues?q=is%3Aissue+).

### 4. More examples

- [RL UAV Lights node](https://github.com/RaccoonlabDev/uav_lights_node/tree/lights) is based on this template. You can consider it as an example.
