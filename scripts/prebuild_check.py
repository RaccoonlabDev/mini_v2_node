#!/usr/bin/env python3
"""
This script checks whether the project is ready for use:
- skip the check if the build folder is not empty (repo was alredy used)
- check submodules are initialized
- check python dependencies are installed
"""
import os
import sys
from pathlib import Path
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import pkg_resources

REPO_DIR = Path(os.path.dirname(os.path.realpath(__file__))).parent.absolute()
BUILD_DIR = os.path.join(REPO_DIR, "build")
LIBPARAMS_DIR = os.path.join(REPO_DIR, "Libs", "libparams")

def check_folder_exist_and_not_empty(dir) -> bool:
    return os.path.isdir(dir) and os.listdir(dir)

def check_first_build() -> bool:
    return not check_folder_exist_and_not_empty(BUILD_DIR)

def check_python_requirements(requirements_file='requirements.txt') -> None:
    try:
        with open(requirements_file, 'r') as f:
            requirements = f.read().splitlines()
        pkg_resources.require(requirements)
    except pkg_resources.DistributionNotFound as e:
        print(f"[Error]: {e}")
        sys.exit(1)
    except pkg_resources.VersionConflict as e:
        print(f"[Error]: {e}")
        sys.exit(1)

def check_submodules() -> None:
    if not check_folder_exist_and_not_empty(LIBPARAMS_DIR):
        print("[Error]: submodules not exist. Run: 'git submodule update --init --recursive'")
        sys.exit(1)

if __name__ == "__main__" and check_first_build():
    check_python_requirements()
    check_submodules()
