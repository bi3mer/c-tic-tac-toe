#!/usr/bin/env python3

import subprocess
import sys
import os

def main():
    os.makedirs("build", exist_ok=True)
    os.chdir("build")

    result = subprocess.run(["cmake", ".."])
    if result.returncode != 0:
        return result.returncode

    result = subprocess.run(["cmake", "--build", ".", "--parallel"])
    if result.returncode != 0:
        return result.returncode

    executable = os.path.join(".", "bin", "project")
    if sys.platform == "win32":
        executable += ".exe"

    result = subprocess.run([executable])
    return result.returncode

if __name__ == "__main__":
    exit(main())