import time
import math
import dronecan

# Configuration
INTERFACE = 'slcan0'
BITRATE = 1000000
NODE_ID = 110

node_info = dronecan.uavcan.protocol.GetNodeInfo.Response()
node_info.name = "org.dronecan.sim.autopilot"
node_info.software_version.major = 1
node_info.hardware_version.unique_id = b'1234567890123456'

def euler_to_quaternion(roll, pitch, yaw):
    cy, sy = math.cos(yaw * 0.5), math.sin(yaw * 0.5)
    cp, sp = math.cos(pitch * 0.5), math.sin(pitch * 0.5)
    cr, sr = math.cos(roll * 0.5), math.sin(roll * 0.5)
    return [
        sr * cp * cy - cr * sp * sy, # x
        cr * sp * cy + sr * cp * sy, # y
        cr * cp * sy - sr * sp * cy, # z
        cr * cp * cy + sr * sp * sy  # w
    ]

def main():
    try:
        node = dronecan.make_node(INTERFACE, bitrate=BITRATE, node_id=NODE_ID)
        #node.add_handler(dronecan.uavcan.protocol.GetNodeInfo, lambda env: node_info)
    except Exception as e:
        print(f"Connection failed: {e}")
        return

    print(f"Simulating Autopilot on {INTERFACE}...")

    # Initialize timers for different message rates
    last_heartbeat_time = time.time()
    last_gimbal_time = time.time()
    heartbeat_interval = 1.0  # Heartbeat at 1 Hz
    gimbal_interval = 1
    
    t = 0
    roll = [10, 20, 30, 40, 0, -40, -30, -20, -10]
    i = 0
    while True:
        current_time = time.time()
        

        if current_time - last_heartbeat_time >= heartbeat_interval:
            nodestatus = dronecan.uavcan.protocol.NodeStatus()
            nodestatus.uptime_sec = int(t)
            nodestatus.health = nodestatus.HEALTH_OK
            nodestatus.mode = nodestatus.MODE_OPERATIONAL
            node.broadcast(nodestatus)
            last_heartbeat_time = current_time
            
            # Update t for heartbeat
            t += heartbeat_interval
        

        if current_time - last_gimbal_time >= gimbal_interval:
            
            pitch = 0
            yaw = 0
            roll_current = math.radians(roll[i])
            i = (i + 1) % len(roll)
            msg = dronecan.uavcan.equipment.camera_gimbal.AngularCommand()
            msg.gimbal_id = 0
            msg.mode.command_mode = 1
            msg.quaternion_xyzw = euler_to_quaternion(roll_current, pitch, yaw)
            
            node.broadcast(msg)
            last_gimbal_time = current_time

        try:
            node.spin(0.01)
        except Exception as e:
            print(f"Error in spin: {e}")
            
        # Small sleep to prevent CPU hogging
        time.sleep(0.01)

if __name__ == '__main__':
    main()
