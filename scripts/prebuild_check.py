#!/usr/bin/env python3
"""
This script checks whether the project is ready for use:
- skip the check if the build folder is not empty (repo was alredy used)
- check submodules are initialized
- check python dependencies are installed
"""
import os
import sys
import logging
import platform
import subprocess
from pathlib import Path
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import pkg_resources # pylint: disable=C0413

SCRIPT_NAME = os.path.splitext(os.path.basename(__file__))[0]
REPO_DIR = Path(os.path.dirname(os.path.realpath(__file__))).parent.absolute()
BUILD_DIR = os.path.join(REPO_DIR, "build")
LIBPARAMS_DIR = os.path.join(REPO_DIR, "Libs", "libparams")

logger = logging.getLogger(SCRIPT_NAME)

def is_github_ci() -> bool:
    return os.getenv("GITHUB_ACTIONS") == "true"

def is_dir_nonempty(directory) -> bool:
    return os.path.isdir(directory) and os.listdir(directory)

def is_first_build() -> bool:
    return not is_dir_nonempty(BUILD_DIR)

def is_precommit_hook_configured(repo_path: str = REPO_DIR) -> bool:
    pre_commit_hook_path = os.path.join(repo_path, '.git', 'hooks', 'pre-commit')
    return os.path.isfile(pre_commit_hook_path) and os.access(pre_commit_hook_path, os.X_OK)

def get_all_branch_tags() -> str:
    cmd = ["git", "tag", "--merged"]
    tags = subprocess.check_output(cmd, stderr=subprocess.DEVNULL, text=True).strip()
    return tags

def get_associated_tag() -> str:
    cmd = ["git", "describe", "--tags", "--abbrev=0"]
    associated_tag = subprocess.check_output(cmd, stderr=subprocess.DEVNULL, text=True).strip()
    return associated_tag

class PrebuildChecker:
    def __init__(self, prompt):
        self.prompt = prompt

    @staticmethod
    def check_platform() -> None:
        """
        Platform must be linux, darwin or windows
        """
        system_platform = platform.system().lower()
        if system_platform == "linux":
            logger.info("Platform: Linux")
        elif system_platform == "darwin":
            logger.info("Platform: macOS")
        elif system_platform == "windows":
            logger.info("Platform: Windows")
        else:
            logger.error("Unknown platform: %s", system_platform)
            sys.exit(1)

    @staticmethod
    def check_branch_tags() -> None:
        """
        Repository must have a tag associated to the branch
        """
        try:
            if get_all_branch_tags():
                logger.debug("The associated with the current branch tag: %s", get_associated_tag())
            else:
                logger.error("No tags are associated with the current branch.")
                logger.error("Ensure you have tags in the branch or fetch them 'git fetch --tags'.")
                sys.exit(1)
        except subprocess.CalledProcessError as err:
            logger.error("Error while checking tags: %s", str(err))
            sys.exit(1)

    @staticmethod
    def check_python_requirements(requirements_file='requirements.txt') -> None:
        """
        All dependencies must be installed.
        """
        try:
            with open(requirements_file, 'r', encoding="utf-8") as file:
                requirements = file.read().splitlines()
            pkg_resources.require(requirements)
        except pkg_resources.DistributionNotFound as err:
            logger.error(str(err))
            sys.exit(1)
        except pkg_resources.VersionConflict as err:
            logger.error(str(err))
            sys.exit(1)

    def check_submodules(self) -> None:
        """
        All submodules must be initialized
        """
        if is_dir_nonempty(LIBPARAMS_DIR):
            return

        if not sys.stdin.isatty():
            logger.error(
                "Submodules are nto found. "
                "Please update them with git submodule update --init --recursive"
            )
            sys.exit(1)

        cmd = ['git', 'submodule', 'update', '--init', '--recursive']
        logger.error("Submodules do not exist. Run: '%s'", ' '.join(cmd))
        if not self.prompt:
            sys.exit(1)

        try:
            user_input = input("Do you want to run this command now? [y/N]:\n").strip().lower()
            if user_input == 'y':
                subprocess.run(cmd, check=True)
                logger.info("Submodules initialized successfully.")
            else:
                sys.exit(1)
        except subprocess.CalledProcessError as err:
            logger.error("Failed to initialize submodules: %s", err)
            sys.exit(1)
        except KeyboardInterrupt:
            sys.exit(1)

    def check_pre_commit_hook(self) -> None:
        """
        Check if the pre-commit hook is configured. If not, prompt the user to create it.
        """

        if not sys.stdin.isatty():
            logger.info("Non-interactive mode detected. Skipping pre-commit hook setup.")
            return

        if is_precommit_hook_configured():
            logger.info("The pre-commit hook is already configured.")
            return

        logger.info("The pre-commit hook is not configured.")
        if not self.prompt:
            return

        hook_path = os.path.join('.git', 'hooks', 'pre-commit')
        hook_script = (
            "#!/bin/bash\n"
            "CRNT_DIR=$( cd -- \"$( dirname -- \"${BASH_SOURCE[0]}\" )\" &> /dev/null && pwd )\n"
            "GIT_DIR=\"$(dirname \"$CRNT_DIR\")\"\n"
            "REPO_DIR=\"$(dirname \"$GIT_DIR\")\"\n"
            "${REPO_DIR}/scripts/pre_commit.sh\n"
        )
        try:
            user_input = input("Do you want to create a pre-commit hook? [y/N]:\n").strip().lower()
            if user_input != 'y':
                logger.info("Skipped pre-commit hook setup.")
                return

            # Write the hook script
            with open(hook_path, 'w', encoding="utf-8") as hook_file:
                hook_file.write(hook_script)

            # Make the hook executable
            os.chmod(hook_path, 0o700)
            logger.info("Pre-commit hook created successfully at '%s'.", hook_path)

        except KeyboardInterrupt:
            sys.exit(1)

if __name__ == "__main__":
    logging.basicConfig(level=logging.ERROR)
    logger.setLevel(logging.DEBUG)

    if is_first_build():
        logger.info("This is the first build. Checking all the requirements...")

        checker = PrebuildChecker(prompt=(not is_github_ci()))

        checker.check_platform()
        checker.check_branch_tags()
        checker.check_python_requirements()
        checker.check_submodules()
        checker.check_pre_commit_hook()
