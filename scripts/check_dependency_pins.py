#!/usr/bin/env python3
# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
"""
Checks that every dependency pin is a commit on the default branch of its repository:
the submodules of .gitmodules and the VERSION declarations of cmake/dependencies.cmake.

A pin to a pull request branch passes the build but breaks once the branch is squash merged
or deleted, so main must never refer to one.

Usage: scripts/check_dependency_pins.py [--repo DIR] [--skip-inaccessible]
"""
import argparse
import os
import pathlib
import re
import subprocess
import sys
import tempfile

DECLARATION = re.compile(r"^\s*nc_declare_dependency\((\S+)\s+(\S+)\s+VERSION\s+(\S+)\)", re.MULTILINE)
COMMIT = re.compile(r"^[0-9a-f]{7,40}$")
# Never ask for a password, and never fetch a missing object on demand: the checks must see only main
GIT_ENV = {**os.environ, "GIT_TERMINAL_PROMPT": "0", "GCM_INTERACTIVE": "never", "GIT_NO_LAZY_FETCH": "1"}


class Inaccessible(Exception):
    pass


def git(*args, cwd=None):
    result = subprocess.run(["git", *args], cwd=cwd, env=GIT_ENV, capture_output=True, text=True)
    if result.returncode != 0:
        raise subprocess.CalledProcessError(result.returncode, args, result.stdout, result.stderr)
    return result.stdout


def github_urls(url_or_repository):
    """HTTPS first, it needs no key for a public repository; SSH for a private one."""
    if "://" not in url_or_repository and "@" not in url_or_repository:
        repository = url_or_repository  # owner/name, as cmake/dependencies.cmake declares it
    else:
        match = re.match(r"^(?:https://|ssh://git@|git@)github\.com[:/](.+?)(?:\.git)?/?$", url_or_repository)
        if not match:
            return [url_or_repository]  # another host, e.g. a gist: as written
        repository = match.group(1)
    return [f"https://github.com/{repository}.git", f"git@github.com:{repository}.git"]


def reachable_url(urls):
    for url in urls:
        try:
            head = git("ls-remote", "--symref", url, "HEAD")
        except subprocess.CalledProcessError:
            continue
        branch = re.search(r"^ref: refs/heads/(\S+)\s+HEAD", head, re.MULTILINE)
        if branch:
            return url, branch.group(1)
    raise Inaccessible(f"can't read {urls[0]}")


def resolve(url, version):
    """The commit a tag or a commit hash points at."""
    if COMMIT.match(version):
        return version
    refs = git("ls-remote", url, f"refs/tags/{version}", f"refs/tags/{version}^{{}}", f"refs/heads/{version}")
    found = dict(reversed(line.split("\t")) for line in refs.splitlines())
    if f"refs/tags/{version}^{{}}" in found:  # an annotated tag: the commit it points at
        return found[f"refs/tags/{version}^{{}}"]
    if f"refs/tags/{version}" in found:
        return found[f"refs/tags/{version}"]
    if f"refs/heads/{version}" in found:
        raise ValueError(f"{version} is a branch: pin a tag or a commit")
    raise ValueError(f"{version} is neither a tag nor a commit")


def default_branch_commits(url, branch, workdir):
    """All the commits of the default branch, without trees or files."""
    clone = pathlib.Path(workdir) / re.sub(r"\W", "_", url)
    if not clone.exists():
        git("clone", "--quiet", "--bare", "--filter=tree:0", "--single-branch", "--no-tags",
            "--branch", branch, url, str(clone))
    return git("rev-list", branch, cwd=clone).split()


def pins(repo):
    """(name, url or repository, version) of every submodule and VERSION declaration."""
    found = []
    gitmodules = repo / ".gitmodules"
    if gitmodules.exists():
        config = git("config", "-f", str(gitmodules), "--get-regexp", r"^submodule\..*\.(path|url)$")
        entries = {}
        for line in config.splitlines():
            key, value = line.split(maxsplit=1)
            name, field = key[len("submodule."):].rsplit(".", 1)
            entries.setdefault(name, {})[field] = value
        for entry in entries.values():
            tree = git("ls-tree", "HEAD", entry["path"], cwd=repo).split()
            if len(tree) >= 3 and tree[1] == "commit":
                found.append((entry["path"], entry["url"], tree[2]))
    declarations = repo / "cmake" / "dependencies.cmake"
    if declarations.exists():
        for name, repository, version in DECLARATION.findall(declarations.read_text()):
            found.append((name, repository, version))
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--repo", type=pathlib.Path, default=pathlib.Path(__file__).resolve().parent.parent,
                        help="the repository whose pins to check, node-core by default")
    parser.add_argument("--skip-inaccessible", action="store_true",
                        help="warn instead of failing on a repository this machine can't read, e.g. a private "
                             "one in a pull request from a fork")
    args = parser.parse_args()

    failed = False
    with tempfile.TemporaryDirectory() as workdir:
        for name, source, version in pins(args.repo.resolve()):
            label = f"{name} {version[:12]}"
            try:
                url, branch = reachable_url(github_urls(source))
                commit = resolve(url, version)
                on_branch = any(c.startswith(commit) for c in default_branch_commits(url, branch, workdir))
            except Inaccessible as error:
                print(f"{'SKIP' if args.skip_inaccessible else 'FAIL'}  {label}: {error}")
                failed |= not args.skip_inaccessible
                continue
            except (ValueError, subprocess.CalledProcessError) as error:
                details = error.stderr.strip() if isinstance(error, subprocess.CalledProcessError) else error
                print(f"FAIL  {label}: {details}")
                failed = True
                continue
            if on_branch:
                print(f"ok    {label}: on {branch} of {url}")
            else:
                print(f"FAIL  {label}: {commit[:12]} is not on {branch} of {url}")
                failed = True
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
