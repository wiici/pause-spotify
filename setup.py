import subprocess
from tkinter import filedialog
import pathlib
import logging
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--debug", dest="isDebug", action="store_true", default=False)
parser.add_argument(
    "--generator", dest="cmakeGenerator", choices=["Ninja", "Visual Studio 17 2022"], default="Ninja"
)
parser.add_argument(
    "--enable-clang-tidy", dest="enableClangTidy", action="store_true", default=False
)
args = parser.parse_args()

loggingLevel = logging.INFO
if args.isDebug:
    loggingLevel = logging.DEBUG
logging.basicConfig(format="%(message)s", level=loggingLevel)

cmakeToolset = ""
if "Visual Studio" in args.cmakeGenerator:
    cmakeToolset = "ClangCl"

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
vcpkgTargetTriplet = "x64-windows"
buildStaticLibsFlag = "FALSE"
enableClangTidyFlag = "FALSE"
if args.enableClangTidy:
    enableClangTidyFlag = "TRUE"

if args.isDebug:
    cmakeLogLevel = "TRACE"

logging.info("\n=== Configuring CMake project...")

cmakeCmd = [
    cmake_exe,
    "--fresh",
    "-S", projectRootDir,
    "-B", targetBuildDir,
    "-G", f"{args.cmakeGenerator}",
    "-T", f"{cmakeToolset}",
    "-D", f"CMAKE_TOOLCHAIN_FILE={vcpkgCmakeFile}",
    "-D", f"VCPKG_TARGET_TRIPLET={vcpkgTargetTriplet}",
    "-D", f"ENABLE_CLANG_TIDY:BOOL={enableClangTidyFlag}",
    "-D", "CMAKE_EXPORT_COMPILE_COMMANDS=ON",
    f"--log-level={cmakeLogLevel}",
]

logging.debug(f'\nCMake command:\n "{cmakeCmd}"\n')

result = subprocess.run(cmakeCmd)

if result.returncode == 0:
    logging.info("\n\033[32m Finished configuring CMake project \033[0m")
else:
    logging.error("\n\033[31m Configuring CMake project failed \033[0m")
