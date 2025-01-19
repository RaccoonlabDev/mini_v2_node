import time
from typing import Any, Dict
import dronecan
from dronecan.node import Node
import numpy as np
from raccoonlab_tools.dronecan.global_node import DronecanNode
import pandas as pd
import datetime
import argparse
from raccoonlab_tools.common.device_manager import DeviceManager

last_received_status_timestamp = 0

def esc_status_clb(msg: dronecan.node.TransferEvent):
    global last_received_status_timestamp
    last_received_status_timestamp = time.time()

class DronecanCommander:
    node = None
    def __init__(self, can_transport=None, fool_frequency=100, timeout=10):
        self.sum_delay = 0.0
        self.count = 0
        if can_transport is None:
            can_transport = DeviceManager.get_device_port()

        self.node = dronecan.make_node(can_transport, node_id=100, bitrate=1000000, baudrate=1000000)
        self.command = dronecan.uavcan.equipment.esc.RawCommand(cmd=[0]*8)
        self.fool_commands = [
                        dronecan.uavcan.equipment.actuator.Command(actuator_id=i, command_value=0)
                        for i in range(8)]
        self.fool_command = dronecan.uavcan.equipment.actuator.ArrayCommand(
                            commands =self.fool_commands)
        self.last_last_received_status_timestamp = 0
        self.prev_fool_broadcast_time = 0
        self.prev_broadcast_time = time.time()
        self.mean_values = []
        self.delays = {"delay": [], "timestamp": []}
        self.i = 0
        self.start_time = 0
        self.end_time = 0
        self.fool_timeout = 1/fool_frequency
        self.timeout = timeout

    def publish(self, command: dronecan.uavcan.equipment.esc.RawCommand) -> None:
        self.command = command
        self.node.broadcast(self.command)
        self.prev_broadcast_time = time.time()

    def publish_fool(self) -> None:
        if self.fool_timeout < time.time() - self.prev_fool_broadcast_time:
            self.prev_fool_broadcast_time = time.time()
            self.node.broadcast(self.fool_command)
            self.i += 1

    def spin(self) -> None:
        global last_received_status_timestamp
        self.node.spin(timeout=0.00001)
        self.publish_fool()
        if last_received_status_timestamp > self.last_last_received_status_timestamp:
            self.delays["delay"].append(last_received_status_timestamp - self.prev_broadcast_time)
            self.delays["timestamp"].append(last_received_status_timestamp)
            self.last_last_received_status_timestamp = last_received_status_timestamp
            self.publish(self.command)

    def run(self) -> None:
        self.start_time = time.time()
        self.node.add_handler(dronecan.uavcan.equipment.actuator.Status, actuator_status_clb)
        self.node.add_handler(dronecan.uavcan.equipment.esc.Status, esc_status_clb)
        self.publish(self.command)

        while time.time() - self.start_time < self.timeout:
            self.spin()
        self.end_time = time.time()

        self.frequency = self.i/(self.end_time - self.start_time)
        print(f"mean delay: {np.mean(self.delays['delay'])}\nmax delay: {max(self.delays['delay'])}\nmin delay: {min(self.delays['delay'])}\nbroadcast frequency: {self.frequency}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--can-transport', default=None, help=("Auto detect by default. Options: "
                                                               "slcan:/dev/ttyACM0, "
                                                               "socketcan:slcan0.")
    )
    parser.add_argument('--broadcast-frequency', default=100, help=("Frequency of broadcasting actuator commands in Hz."), type=int)
    parser.add_argument('--timeout', default=10, help=("Test timeout in seconds."), type=int)
    parser.add_argument('--save-to-file', default=False, help=("Save to file."), type=bool)

    args = parser.parse_args()
    commander = DronecanCommander(can_transport=args.can_transport, fool_frequency=args.broadcast_frequency, timeout=args.timeout)

    commander.run()
    if args.save_to_file:
        df = pd.DataFrame(data=commander.delays)
        df.to_csv(f"speed_broadcast_freq_{commander.frequency}{datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}.csv", index=False)

if __name__ == "__main__":
    main()
