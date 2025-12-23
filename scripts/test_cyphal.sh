#!/bin/bash
# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.

y rl 42

# crct
y r 42 uavcan.pub.crct.temperature.id 1000
y r 42 uavcan.pub.crct.5v.id 1001
y r 42 uavcan.pub.crct.vin.id 1002

y r 42 uavcan.pub.crct.temperature.id
y r 42 uavcan.pub.crct.5v.id
y r 42 uavcan.pub.crct.vin.id

# feedback
y r 42 uavcan.pub.udral_feedback1.id 1100
y r 42 uavcan.pub.udral_feedback2.id 1101
y r 42 uavcan.pub.udral_feedback3.id 1102
y r 42 uavcan.pub.udral_feedback4.id 1103
y r 42 uavcan.pub.zubax_feedback1.id 1200
y r 42 uavcan.pub.zubax_feedback2.id 1201
y r 42 uavcan.pub.zubax_feedback3.id 1202
y r 42 uavcan.pub.zubax_feedback4.id 1203

y r 42 uavcan.pub.udral_feedback1.id
y r 42 uavcan.pub.udral_feedback2.id
y r 42 uavcan.pub.udral_feedback3.id
y r 42 uavcan.pub.udral_feedback4.id
y r 42 uavcan.pub.zubax_feedback1.id
y r 42 uavcan.pub.zubax_feedback2.id
y r 42 uavcan.pub.zubax_feedback3.id
y r 42 uavcan.pub.zubax_feedback4.id

# rcout setpoint
y r 42 pwm1.ch 0
y r 42 pwm2.ch 0
y r 42 pwm3.ch 0
y r 42 pwm4.ch 0

y r 42 uavcan.sub.setpoint.id 2000
y r 42 uavcan.sub.setpoint.id

y cmd 42 65530 # save params
y cmd 42 65535 # reboot the node to apply the changes

timeout 5 y sub 1000:uavcan.si.unit.temperature.Scalar.1.0 --count=1
timeout 5 y sub 1001:uavcan.si.unit.voltage.Scalar.1.0 --count=1
timeout 5 y sub 1002:uavcan.si.unit.voltage.Scalar.1.0 --count=1

timeout 5 y sub 1100:reg.udral.service.actuator.common.Feedback.0.1 --count=1
timeout 5 y sub 1101:reg.udral.service.actuator.common.Feedback.0.1 --count=1
timeout 5 y sub 1102:reg.udral.service.actuator.common.Feedback.0.1 --count=1
timeout 5 y sub 1103:reg.udral.service.actuator.common.Feedback.0.1 --count=1
timeout 5 y sub 1200:zubax.telega.CompactFeedback.1.0 --count=1
timeout 5 y sub 1201:zubax.telega.CompactFeedback.1.0 --count=1
timeout 5 y sub 1202:zubax.telega.CompactFeedback.1.0 --count=1
timeout 5 y sub 1203:zubax.telega.CompactFeedback.1.0 --count=1

timeout 10 y pub -T 0.05 2000:reg.udral.service.actuator.common.sp.Vector4.0.1 \
    "[!$ \"sin(t * pi * 2 * 0.1) * 0.5 + 0.5\", !$ \"sin(t * pi * 2 * 0.1) * 0.5 + 0.5\", !$ \"sin(t * pi * 2 * 0.1) * 0.5 + 0.5\", !$ \"sin(t * pi * 2 * 0.1) * 0.5 + 0.5\"]"
