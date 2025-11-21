#!/usr/bin/env python3

import argparse
import urllib.request
import zipfile
import tarfile
import shutil
import os
from pathlib import Path

RAYLIB_VERSION = "5.5"
RAYLIB_DIR = Path("lib/raylib")

def download_file(url, filename):
    print(f"Downloading {filename}...")
    urllib.request.urlretrieve(url, filename)
    print(f"✓ Downloaded {filename}")

def setup_mac():
    url = f"https://github.com/raysan5/raylib/releases/download/{RAYLIB_VERSION}/raylib-{RAYLIB_VERSION}_macos.tar.gz"
    archive = "raylib_macos.tar.gz"

    download_file(url, archive)

    with tarfile.open(archive, "r:gz") as tar:
        tar.extractall()

    src_dir = Path(f"raylib-{RAYLIB_VERSION}_macos")
    if not src_dir.exists():
        src_dir = Path("raylib-5.5_macos")

    for header in (src_dir / "include").glob("*.h"):
        shutil.copy2(header, RAYLIB_DIR / "include")

    for lib in (src_dir / "lib").glob("*"):
        shutil.copy2(lib, RAYLIB_DIR / "lib")

    os.remove(archive)
    shutil.rmtree(src_dir)
    print("✓ macOS libraries installed")

def setup_linux():
    url = f"https://github.com/raysan5/raylib/releases/download/{RAYLIB_VERSION}/raylib-{RAYLIB_VERSION}_linux_amd64.tar.gz"
    archive = "raylib_linux.tar.gz"

    download_file(url, archive)

    with tarfile.open(archive, "r:gz") as tar:
        tar.extractall()

    src_dir = Path(f"raylib-{RAYLIB_VERSION}_linux_amd64")
    if not src_dir.exists():
        src_dir = Path("raylib-5.5_linux_amd64")

    for header in (src_dir / "include").glob("*.h"):
        shutil.copy2(header, RAYLIB_DIR / "include")

    for so_file in (src_dir / "lib").glob("libraylib.so*"):
        if so_file.name.startswith("libraylib.so"):
            dest_so = RAYLIB_DIR / "lib" / "libraylib.so"
            shutil.copy2(so_file, dest_so)
            break

    os.remove(archive)
    shutil.rmtree(src_dir)
    print("✓ Linux libraries installed")

def setup_windows():
    url = f"https://github.com/raysan5/raylib/releases/download/{RAYLIB_VERSION}/raylib-{RAYLIB_VERSION}_win64_mingw-w64.zip"
    archive = "raylib_windows.zip"

    download_file(url, archive)

    with zipfile.ZipFile(archive, "r") as zip_ref:
        zip_ref.extractall()

    src_dir = Path(f"raylib-{RAYLIB_VERSION}_win64_mingw-w64")
    if not src_dir.exists():
        src_dir = Path("raylib-5.5_win64_mingw-w64")

    for header in (src_dir / "include").glob("*.h"):
        shutil.copy2(header, RAYLIB_DIR / "include")

    shutil.copy2(src_dir / "lib" / "libraylib.a", RAYLIB_DIR / "lib")
    shutil.copy2(src_dir / "lib" / "raylib.dll", RAYLIB_DIR / "lib")

    os.remove(archive)
    shutil.rmtree(src_dir)
    print("✓ Windows libraries installed")

def main():
    parser = argparse.ArgumentParser(description="Download raylib libraries for specified platforms")
    parser.add_argument("--mac", action="store_true", help="Download macOS libraries")
    parser.add_argument("--linux", action="store_true", help="Download Linux libraries")
    parser.add_argument("--windows", action="store_true", help="Download Windows libraries")

    args = parser.parse_args()

    if not any([args.mac, args.linux, args.windows]):
        print("Error: Please specify at least one platform (--mac, --linux, or --windows)")
        return 1

    (RAYLIB_DIR / "include").mkdir(parents=True, exist_ok=True)
    (RAYLIB_DIR / "lib").mkdir(parents=True, exist_ok=True)

    if args.mac:
        setup_mac()
    if args.linux:
        setup_linux()
    if args.windows:
        setup_windows()

    print(f"\n✓ Setup complete for raylib {RAYLIB_VERSION}")
    return 0

if __name__ == "__main__":
    exit(main())