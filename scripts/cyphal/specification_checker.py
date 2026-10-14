#!/usr/bin/env python3
# Copied from PonomarevDA/tools at 3c17c06 (MIT, Copyright (c) 2023 Dmitry Ponomarev).
# Checks a Cyphal node against the specification. Run the node, compile the DSDL with yakut and set
# UAVCAN__CAN__IFACE, UAVCAN__CAN__MTU and UAVCAN__NODE__ID first, as the CI Cyphal SITL step does.
import asyncio
import os
import sys
import time
import pathlib
import random
import string
import re
import pytest

# The Python DSDL that yakut compiled: YAKUT_PATH, else build/yakut_out of node-core
# pylint: disable-next=wrong-import-position
sys.path.insert(0, os.environ.get("YAKUT_PATH", str(pathlib.Path(__file__).resolve().parents[2] / "build" / "yakut_out")))
import pycyphal.application
# pylint: disable=import-error
import uavcan.node
import uavcan.node.Heartbeat_1_0
import uavcan.node.GetInfo_1_0
import uavcan.node.port.List_1_0
import uavcan.register
import uavcan.register.List_1_0
from utils import CyphalTools

@pytest.fixture(autouse=True)
async def run_around_tests():
    """
    We need to keep a pause before exit.
    Task was destroyed but it is pending
    """
    yield
    await asyncio.sleep(0.001)

@pytest.fixture(scope="session")
def event_loop():
    try:
        loop = asyncio.get_running_loop()
    except RuntimeError:
        loop = asyncio.new_event_loop()
    yield loop
    loop.close()



@pytest.mark.asyncio
class TestNodeName:
    @staticmethod
    async def test_node_name():
        print("Node name must follow a specific pattern:")
        cyphal_node = await CyphalTools.get_node()
        dest_node_id = await CyphalTools.find_online_node()

        request = uavcan.node.GetInfo_1_0.Request()
        client = cyphal_node.make_client(uavcan.node.GetInfo_1_0, dest_node_id)
        response = await client.call(request)
        client.close()
        assert response is not None, "The node has not respond on GetInfo request."
        print(f"- 1/2. The node has been respond on GetInfo request.")

        name = CyphalTools.np_array_to_string(response[0].name)
        assert TestNodeName._check_node_name(name), f"The node name '{name}' does not follow the node name pattern."
        print(f"- 2/2. The node '{name}' follows the node name pattern.")

    @staticmethod
    def _check_node_name(node_name):
        pattern = r'^[a-z0-9_\-]+(\.[a-z0-9_\-]+)+$'
        return re.match(pattern, node_name) is not None


@pytest.mark.asyncio
class TestHearbeat:
    """https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/node/7509.Heartbeat.1.0.dsdl"""

    @staticmethod
    async def test_frequency(seconds: int = 5):
        print("Heartbeat frequency should be 1.0 Hz (check 5 seconds):")
        cyphal_node = await CyphalTools.get_node()
        dest_node_id = await CyphalTools.find_online_node()
        sub = cyphal_node.make_subscriber(uavcan.node.Heartbeat_1_0)

        timestamps = [time.time()]

        while len(timestamps) < seconds + 2:
            transfer_from = await sub.receive_for(1.1)
            crnt_time_sec = time.time()
            elapsed_time = crnt_time_sec - timestamps[-1]
            assert elapsed_time <= 1.1, f"Heartbeat has not been appeared for {elapsed_time :.3f} seconds already."

            if transfer_from[1].source_node_id == dest_node_id:
                if len(timestamps) != 1:
                    print(f"- {len(timestamps) - 1}/{seconds}. {elapsed_time :.3f} seconds after previous Heartbeat.")
                    assert elapsed_time >= 0.9, f"Heartbeat has been appeared too erly: {elapsed_time :.3f} seconds."
                timestamps.append(crnt_time_sec)


@pytest.mark.asyncio
class TestPersistentMemory():

    """Persistent memory check: 1. set random, 2. save, 3. reboot, 4. get"""
    @staticmethod
    async def test_persistent_memory() -> None:
        print("TestPersistentMemory:")

        tested_node_info = await CyphalTools.get_tested_node_info()
        dest_node_name = tested_node_info['name']
        if dest_node_name == 'org.opencyphal.yakut.monitor':
            print("Skip test because yakut doesn't support ExecuteCommand.")
            return

        cyphal_node = await CyphalTools.get_node()
        dest_node_id = await CyphalTools.find_online_node()
        register_name = "uavcan.node.description"
        random_test_value = ''.join(random.choices(string.ascii_lowercase, k=10))
        cmd_client = cyphal_node.make_client(uavcan.node.ExecuteCommand_1_1, dest_node_id)
        cmd_client.close()

        set_request = uavcan.register.Access_1_0.Request()
        set_request.name.name = register_name
        set_request.value.string = uavcan.primitive.String_1_0(random_test_value)
        access_client = cyphal_node.make_client(uavcan.register.Access_1_0, dest_node_id)
        access_response = await access_client.call(set_request)
        access_client.close()
        assert access_response is not None, "Access retrive failed!"
        value = CyphalTools.np_array_to_string(access_response[0].value.string.value)
        assert value == random_test_value, f"1/4. Accees expected {random_test_value}, got {value}!"
        print(f"- 1/4. y r {dest_node_id} uavcan.node.description {random_test_value} # success")

        save_request = uavcan.node.ExecuteCommand_1_1.Request(command = 65530)
        cmd_client = cyphal_node.make_client(uavcan.node.ExecuteCommand_1_1, dest_node_id)
        cmd_response = await cmd_client.call(save_request)
        cmd_client.close()
        assert cmd_response is not None, "2/4. ExecuteCommand retrive failed!"
        print(f"- 2/4. y r {dest_node_id} 65530 # success")

        reboot_request = uavcan.node.ExecuteCommand_1_1.Request(command = 65535)
        cmd_client = cyphal_node.make_client(uavcan.node.ExecuteCommand_1_1, dest_node_id)
        await cmd_client.call(reboot_request)
        cmd_client.close()
        assert cmd_response is not None, "2/4. ExecuteCommand retrive failed!"
        print(f"- 3/4. y r {dest_node_id} 65535 # success")

        get_request = uavcan.register.Access_1_0.Request()
        get_request.name.name = register_name
        access_client = cyphal_node.make_client(uavcan.register.Access_1_0, dest_node_id)
        access_response = await access_client.call(set_request)
        access_client.close()
        assert access_response is not None, "4/4. Access retrive failed!"
        value = CyphalTools.np_array_to_string(access_response[0].value.string.value)
        assert value == random_test_value, f"4/4. Accees expects {random_test_value}, got {value}!"
        print(f"- 4/4. y r {dest_node_id} uavcan.node.description # success: {random_test_value}")


@pytest.mark.asyncio
class TestPortList:
    """https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/node/port/7510.List.1.0.dsdl"""

    @staticmethod
    async def test_port_list():
        tested_node_info = await CyphalTools.get_tested_node_info()
        dest_node_name = tested_node_info['name']
        port_list_msg = await CyphalTools.get_port_list()
        assert port_list_msg is not None, "uavcan.port.List was not published!"

        print(f"Check port.List of {dest_node_name}:")
        print(f"- register.Access {port_list_msg.servers.mask[384]}")
        print(f"- register.List   {port_list_msg.servers.mask[385]}")
        print(f"- GetInfo         {port_list_msg.servers.mask[430]}")
        if not dest_node_name == 'org.opencyphal.yakut.monitor':
            print(f"- ExecuteCommand  {port_list_msg.servers.mask[435]}")

        assert port_list_msg.servers.mask[384], "register.Access is not supported"
        assert port_list_msg.servers.mask[385], "register.List is not supported"
        assert port_list_msg.servers.mask[430], "GetInfo is not supported"
        if not dest_node_name == 'org.opencyphal.yakut.monitor':
            assert port_list_msg.servers.mask[435], "ExecuteCommand is not supported"


@pytest.mark.asyncio
class TestRegisters:
    """https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/register/384.Access.1.0.dsdl"""

    @staticmethod
    async def test_registers_name():
        dest_node_id = await CyphalTools.find_online_node()
        register_names = await CyphalTools.register_list(dest_node_id)
        assert len(register_names) >= 2, "Node should have at least 2 registers!"

        print("Registers:")
        all_register_correct = True
        for register_name in register_names:
            register_correct = TestRegisters.check_register_name(register_name)
            print(f"- {register_name :<30} : {register_correct}")
            if not register_correct:
                all_register_correct = False
        assert all_register_correct

    @staticmethod
    async def test_default_registers_existance():
        cyphal_node = await CyphalTools.get_node()
        dest_node_id = await CyphalTools.find_online_node()

        required_register_names = [
            "uavcan.node.id",
            "uavcan.node.description",
        ]

        access_request = uavcan.register.Access_1_0.Request()
        access_client = cyphal_node.make_client(uavcan.register.Access_1_0, dest_node_id)
        access_response = None
        print("Check required registers existance:")
        for register_name in required_register_names:
            access_request.name.name = register_name
            access_response = await access_client.call(access_request)
            assert access_response is not None
            reply_not_empty = access_response[0].value.empty is None
            print(f"- {register_name :<30} : {reply_not_empty}")
            assert reply_not_empty

    @staticmethod
    async def test_port_register_types():
        cyphal_node = await CyphalTools.get_node()
        dest_node_id = await CyphalTools.find_online_node()
        all_register_names = await CyphalTools.register_list(dest_node_id)

        access_request = uavcan.register.Access_1_0.Request()
        access_client = cyphal_node.make_client(uavcan.register.Access_1_0, dest_node_id)
        
        print("Check port registers type:")
        for register_name in all_register_names:
            port_type, port_reg_type = CyphalTools.get_port_type_by_register_name(register_name)
            if port_type is None or port_reg_type is None:
                print(f"- {register_name :<30} skip")
                continue

            access_request.name.name = register_name
            access_response = await access_client.call(access_request)

            assert access_response is not None

            access_response = access_response[0]
            if port_reg_type == "id":
                assert access_response.value.natural16 is not None, f"- {register_name} should have natural16 type, but it is not"
                assert access_response._mutable and access_response.persistent, f"- {register_name} should be mutable and persistent, but it is {access_response}"
                print(f"- {register_name :<30} is natural16, mutable and persistent.")
            elif port_reg_type == "type":
                assert access_response.value.string is not None, f"- {register_name} should have string type, but it is not"
                assert access_response._mutable is False and access_response.persistent is True, f"- {register_name} should be immutable and persistent, but it is {access_response}"
                print(f"- {register_name :<30} is string, immutable and persistent")

    @staticmethod
    def check_register_name(register_name):
        pattern = r'^[a-z][a-z0-9._]*(\.[a-z0-9._]+)+$'
        return re.match(pattern, register_name) is not None

async def main():
    print("Cyphal specification checker:")
    await TestRegisters.test_registers_name()
    await TestRegisters.test_default_registers_existance()
    await TestRegisters.test_port_register_types()
    await TestPortList.test_port_list()
    await TestPersistentMemory.test_persistent_memory()
    await TestHearbeat.test_frequency()
    await TestNodeName.test_node_name()

def run_cyphal_standard_checker():
    asyncio.run(main())


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Cyphal specification checker')
    args = parser.parse_args()
    run_cyphal_standard_checker()
