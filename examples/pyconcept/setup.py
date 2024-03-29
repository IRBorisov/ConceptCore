''' Setup process for CMake originated from https://github.com/pybind/cmake_example '''
import os
import subprocess
import sys
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# Convert distutils Windows platform specifiers to CMake -A arguments
_PLAT_TO_CMAKE = {
    'win32': 'Win32',
    'win-amd64': 'x64',
    'win-arm32': 'ARM',
    'win-arm64': 'ARM64',
}

class CMakeExtension(Extension):
    '''
    A CMakeExtension needs a sourcedir instead of a file list.
    The name must be the _single_ output extension from the CMake build.
    '''
    def __init__(self, name: str, sourcedir: str = '') -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    '''CMake builder extenrsion.'''

    def build_extension(self, ext: CMakeExtension) -> None:
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()

        debug = int(os.environ.get('DEBUG', 0)) if self.debug is None else self.debug
        cfg = 'Debug' if debug else 'Release'
        cmake_generator = os.environ.get('CMAKE_GENERATOR', '')
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",            
            f"-DCMAKE_BUILD_TYPE={cfg}"
        ]

        if 'CMAKE_ARGS' in os.environ:
            cmake_args += [item for item in os.environ['CMAKE_ARGS'].split(' ') if item]

        build_args = []
        if self.compiler.compiler_type != 'msvc':
            if not cmake_generator or cmake_generator == "Ninja":
                try:
                    import ninja
                    ninja_executable_path = Path(ninja.BIN_DIR) / "ninja"
                    cmake_args += [
                        "-GNinja",
                        f"-DCMAKE_MAKE_PROGRAM:FILEPATH={ninja_executable_path}",
                    ]
                except ImportError:
                    pass
        else:
            # Single config generators are handled "normally"
            single_config = any(x in cmake_generator for x in ['NMake', 'Ninja'])

            # CMake allows an arch-in-generator style for backward compatibility
            contains_arch = any(x in cmake_generator for x in ['ARM', 'Win64'])

            # Specify the arch if using MSVC generator, but only if it doesn't
            # contain a backward-compatibility arch spec already in the generator name.
            if not single_config and not contains_arch:
                cmake_args += ['-A', _PLAT_TO_CMAKE[self.plat_name]]

            # Multi-config generators have a different way to specify configs
            if not single_config:
                cmake_args += [
                    f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}"
                ]
                build_args += ['--config', cfg]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            if hasattr(self, "parallel") and self.parallel:
                build_args += [f"-j{self.parallel}"]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)

        subprocess.run(
            ["cmake", ext.sourcedir, *cmake_args], cwd=build_temp, check=True
        )
        subprocess.run(
            ["cmake", "--build", ".", *build_args], cwd=build_temp, check=True
        )

setup(
    ext_modules=[CMakeExtension("pyconcept")],
    cmdclass={"build_ext": CMakeBuild}
)
