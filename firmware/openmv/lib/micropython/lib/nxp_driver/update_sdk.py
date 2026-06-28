#!/usr/bin/env python3

# This repository contains a trimmed down version of NXP's MCUXpresso SDK
# for use by imx-rt based projects compiling firmware outside ot the 
# MCUXpresso environment.

# This tool grabs the latest (or specified) release from NXP and updates the 
# drivers here for currently-included target chips along with common drivers.

# To add support for a new target chip simply create an empty folder with the
# desired target name in devices folder and re-run this tool, it will be updated
# along with the other existing targets.

# Release / tag name can be passed on command line, otherwise the latest release 
# will be used from: https://github.com/nxp-mcuxpresso/mcux-sdk/releases

import argparse
import itertools
import json
import os
import re
import shutil
import subprocess
import tarfile
from pathlib import Path
from urllib.request import urlretrieve

def update_mcux_sdk(release=None, symlink=False):
    """
    Downloads and updates the MCUXpresso SDK for nxp_driver folder.

    Args:
        release (str, optional): Release tag name. Defaults to "latest".
        symlink (bool): If enabled, common drivers will by linked into target dirs to emulate previous repo structure
    """

    script_dir = Path(__file__).parent.resolve()
    os.chdir(script_dir)

    # Get release details
    print(f"Downloading official mcux-sdk from:")
    if not release or release == "latest":
        url = f"https://api.github.com/repos/nxp-mcuxpresso/mcux-sdk/releases/latest"
        response, _ = urlretrieve(url)
        with open(response, 'r') as f:
            release_details = json.load(f)

        download_url = release_details.get('tarball_url')
        release_tag = release_details.get('tag_name')
    else:
        release_tag = release
        download_url = f"https://github.com/nxp-mcuxpresso/mcux-sdk/archive/refs/tags/{release_tag}.tar.gz"
    print(f" {download_url}")

    # Create and extract SDK directory
    sdk_new_dir = Path(f"mcux_sdk_{release_tag}")
    if not sdk_new_dir.exists():
        sdk_new_dir.mkdir(exist_ok=True)
        print("Downloading and extracting SDK...")
        urlretrieve(download_url, filename='sdk.tar.gz')
        with tarfile.open('sdk.tar.gz', 'r:gz') as tar:
            tar.extractall(path=str(sdk_new_dir))
        os.remove('sdk.tar.gz')

    # Find drivers folder in downloaded directory
    sdk_new = sdk_new_dir.name
    if not (sdk_new_dir / 'drivers').exists():
        sdk_new = next(sdk_new_dir.iterdir())
    if not (sdk_new / 'drivers').exists():
        print(f"ERROR: Unknown directory structure / cannot find drivers in {sdk_new}")
        exit(1)

    print(f"\nUpdating sdk to {release_tag}")
    (script_dir / 'sdk' / 'version.txt').write_text(release_tag)

    backup = script_dir / "backup"
    if backup.exists():
        shutil.rmtree(backup)
    backup.mkdir()
    
    # Update common drivers
    print(f"Updating common sdk/drivers from {sdk_new / 'drivers'}")
    common_drivers = script_dir / 'sdk' / 'drivers'
    if common_drivers.exists():
        common_drivers.rename(backup / 'drivers')
    shutil.copytree(sdk_new / 'drivers', common_drivers)

    # Update drivers for existing target chips
    for target_dir in (script_dir / 'sdk' / 'devices').iterdir():
        target = target_dir.name
        source = sdk_new / 'devices' / target
        if not source.exists():
            print(f"WARNING: No new drivers available for {target} in {source}")
            continue

        print(f"Updating {target} from {source}")
        target_dir.rename(backup / target)
        shutil.copytree(source, target_dir)
        
        if not symlink:
            continue

        # symlink common drivers used by each target into the target folders
        common_driver_dirs = set()
        for cmake_script in itertools.chain(target_dir.glob("all_lib_device_*.cmake"), target_dir.glob("set_device_*.cmake")):
            common_driver_dirs |= set(re.findall(r"\$\{CMAKE_CURRENT_LIST_DIR\}/../../drivers/(\S+?)[/\n]", cmake_script.read_text()))
        for cdd in common_driver_dirs:
            for driver in (common_drivers / cdd).glob("*.[hc]"):
                target = target_dir / "drivers" / driver.name
                if not target.exists():
                    target.symlink_to(f"../../../drivers/{driver.relative_to(common_drivers)}")

    # Git operations
    subprocess.run(['git', 'checkout', '-b', f'sdk_update_{release_tag}'], check=True)
    subprocess.run(['git', 'add', 'sdk'], check=True)
    message = f"Update sdk to {release_tag}\n\nFrom: {download_url}"
    subprocess.run(['git', 'commit', '--quiet', '-F-'], input=message.encode(), check=True)

    print("Finished")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="This tool creates a trimmed down version of NXP's MCUXpresso SDK for use by imx-rt based projects compiling firmware outside of the MCUXpresso environment.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument("-r", "--release", default="latest", help="Release tag name to update to. Defaults to 'latest'.")
    args = parser.parse_args()

    update_mcux_sdk(args.release)
