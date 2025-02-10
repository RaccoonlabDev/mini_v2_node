#!/usr/bin/env python3
"""
Install.py
"""

import sys
import platform
import subprocess


def run_command(command):
    try:
        print(f"$ {command}")
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError:
        print("Error: Command execution failed")
        sys.exit(1)
    except KeyboardInterrupt:
        print("KeyboardInterrupt")

def main():
    system_name = platform.system().lower()

    if system_name == "windows":
        print("Detected Windows. Installing packages...")
        run_command("choco install -y gcc-arm-embedded make cmake")
    
    elif system_name == "linux":
        try:
            # Detect Linux distribution
            with open("/etc/os-release", "r") as f:
                os_info = f.read().lower()

            if "ubuntu" in os_info or "debian" in os_info:
                print("Detected Ubuntu/Debian. Installing packages...")
                run_command("sudo apt-get install -y gcc-arm-none-eabi")
            
            elif "manjaro" in os_info:
                print("Detected Manjaro. Installing packages...")
                run_command("sudo pacman -S cmake arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib")

            else:
                print("Unknown Linux distribution.")
                sys.exit(1)

        except FileNotFoundError:
            print("Could not determine Linux distribution.")
            sys.exit(1)

    elif system_name == "darwin":
        print("macOS is not supported at the moment.")
        sys.exit(1)

    else:
        print("Unknown platform.")
        sys.exit(1)

if __name__ == "__main__":
    main()
