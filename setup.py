import subprocess
from tkinter import filedialog
import pathlib
import logging
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--debug", dest="isDebug", action="store_true", default=False)
args = parser.parse_args()

loggingLevel = logging.INFO
if args.isDebug:
    loggingLevel = logging.DEBUG
logging.basicConfig(format="%(message)s", level=loggingLevel)

# Can be set explicitly here
vcpkgCmakeFile = ""

if not vcpkgCmakeFile:
    logging.info(
        "Select vcpkg.cmake file located in <vcpkg_project>/scripts/buildsystems/"
    )
    vcpkgCmakeFile = filedialog.askopenfile().name

cmake_exe = "cmake.exe"
projectRootDir = pathlib.Path(__file__).parent.resolve()
targetBuildDir = f"{projectRootDir}/build"
cmakeLogLevel = "WARNING"
if args.isDebug:
    cmakeLogLevel = "TRACE"

logging.info("\n=== Configuring CMake project...")

cmakeCmd = [
    cmake_exe,
    "--fresh",
    "-S", projectRootDir,
    "-B", targetBuildDir,
    "-G", "Ninja",
    "-D", "CMAKE_CXX_COMPILER=clang++",
    "-D", "CMAKE_C_COMPILER=clang",
    "-D", f"CMAKE_TOOLCHAIN_FILE={vcpkgCmakeFile}",
    f"--log-level={cmakeLogLevel}",
]

logging.debug(f'\nCMake command:\n "{cmakeCmd}"\n')

result = subprocess.run(cmakeCmd)

if result.returncode == 0:
    logging.info("\nFinished configuring CMake project")
else:
    logging.error("\nConfiguring CMake project failed")
