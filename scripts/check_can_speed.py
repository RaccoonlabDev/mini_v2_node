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

def actuator_status_clb(msg: dronecan.node.TransferEvent):
    global last_received_status_timestamp
    last_received_status_timestamp = time.time()

def esc_status_clb(msg: dronecan.node.TransferEvent):
    global last_received_status_timestamp
    last_received_status_timestamp = time.time()

class DronecanCommander:
    node = None
    def __init__(self, can_transport=None):
        self.sum_delay = 0.0
        self.count = 0
        if can_transport is None:
            can_transport = DeviceManager.get_device_port()

        self.node = dronecan.make_node(can_transport, node_id=100, bitrate=1000000, baudrate=1000000)
        self.prev_broadcast_time = time.time()
        self.command = dronecan.uavcan.equipment.esc.RawCommand(cmd=[0, 0, 0])
        self.last_last_received_status_timestamp = 0
        self.mean_values = []
        self.delays = []

    def publish(self, command: dronecan.uavcan.equipment.esc.RawCommand) -> None:
        self.command = command
        self.node.broadcast(self.command)
        self.prev_broadcast_time = time.time()

    def spin(self) -> None:
        global last_received_status_timestamp
        self.node.spin(timeout=0.001)
        if last_received_status_timestamp != self.last_last_received_status_timestamp:
            self.delays.append(last_received_status_timestamp - self.prev_broadcast_time)
            self.last_last_received_status_timestamp = last_received_status_timestamp
            self.publish(self.command)

    def run(self) -> None:
        self.node.add_handler(dronecan.uavcan.equipment.actuator.Status, actuator_status_clb)
        self.node.add_handler(dronecan.uavcan.equipment.esc.Status, esc_status_clb)
        self.publish(self.command)
        start_time = time.time()

        while time.time() - start_time < 20:
            self.spin()
        print(f"NEW mean delay: {np.mean(self.delays)}\nmax delay: {max(self.delays)}\nmin delay: {min(self.delays)}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--can-transport', default=None, help=("Auto detect by default. Options: "
                                                               "slcan:/dev/ttyACM0, "
                                                               "socketcan:slcan0.")
    )
    args = parser.parse_args()
    commander = DronecanCommander(can_transport=args.can_transport)
    commander.run()

if __name__ == "__main__":
   main()
