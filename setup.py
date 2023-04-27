import subprocess
from tkinter import filedialog
import pathlib
import logging
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--debug", dest="isDebug", action="store_true", default=False)
parser.add_argument(
    "--compiler", dest="compilerIdStr", choices=["clang", "msvc"], default="clang"
)
args = parser.parse_args()

loggingLevel = logging.INFO
if args.isDebug:
    loggingLevel = logging.DEBUG
logging.basicConfig(format="%(message)s", level=loggingLevel)

c_compiler = ""
cpp_compiler = ""
cmakeGenerator = ""
match args.compilerIdStr:
    case "clang":
        c_compiler = "clang"
        cpp_compiler = "clang++"
        cmakeGenerator = "Ninja"
    case "msvc":
        c_compiler = "cl"
        cpp_compiler = "cl"
        cmakeGenerator = "Visual Studio 17 2022"

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
    "-G", f"{cmakeGenerator}",
    "-D", f"CMAKE_CXX_COMPILER={cpp_compiler}",
    "-D", f"CMAKE_C_COMPILER={c_compiler}",
    "-D", f"CMAKE_TOOLCHAIN_FILE={vcpkgCmakeFile}",
    f"--log-level={cmakeLogLevel}",
]

logging.debug(f'\nCMake command:\n "{cmakeCmd}"\n')

result = subprocess.run(cmakeCmd)

if result.returncode == 0:
    logging.info("\n\033[32m Finished configuring CMake project \033[0m")
else:
    logging.error("\n\033[31m Configuring CMake project failed \033[0m")
