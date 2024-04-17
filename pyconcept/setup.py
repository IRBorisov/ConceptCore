''' Setup process for CMake originated from https://github.com/pybind/cmake_example '''
import os
import subprocess
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
    A CMakeExtension needs a source directory instead of a file list.
    The name must be the _single_ output extension from the CMake build.
    '''
    def __init__(self, name: str, source: str = '') -> None:
        super().__init__(name, sources=[])
        self.source_dir = os.fspath(Path(source).resolve())


class CMakeBuild(build_ext):
    '''CMake builder extension.'''

    def build_extension(self, target: CMakeExtension) -> None:
        target_fullpath = Path.cwd() / self.get_ext_fullpath(target.name)
        output_folder = target_fullpath.parent.resolve()

        debug_flag = int(os.environ.get('DEBUG', 0)) if self.debug is None else self.debug
        build_type = 'Debug' if debug_flag else 'Release'
        cmake_generator = os.environ.get('CMAKE_GENERATOR', '')
        cmake_args = [
            '--preset conan-default',
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_folder}{os.sep}",
            f"-DCMAKE_BUILD_TYPE={build_type}"
        ]

        if 'CMAKE_ARGS' in os.environ:
            cmake_args += [item for item in os.environ['CMAKE_ARGS'].split(' ') if item]

        build_args = ['--preset conan-debug' if debug_flag else '--preset conan-release']
        if self.compiler.compiler_type != 'msvc':
            if not cmake_generator or cmake_generator == 'Ninja':
                try:
                    import ninja
                    ninja_executable_path = Path(ninja.BIN_DIR) / 'ninja'
                    cmake_args += [
                        '-GNinja',
                        f"-DCMAKE_MAKE_PROGRAM:FILEPATH={ninja_executable_path}",
                    ]
                except ImportError:
                    pass
        else:
            # Single config generators are handled "normally"
            single_config = any(x in cmake_generator for x in ['N~Make', 'Ninja'])

            # CMake allows an arch-in-generator style for backward compatibility
            contains_arch = any(x in cmake_generator for x in ['ARM', 'Win64'])

            # Specify the arch if using MSVC generator, but only if it doesn't
            # contain a backward-compatibility arch spec already in the generator name.
            if not single_config and not contains_arch:
                cmake_args += ['-A', _PLAT_TO_CMAKE[self.plat_name]]

            # Multi-config generators have a different way to specify configs
            if not single_config:
                cmake_args += [
                    f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{build_type.upper()}={output_folder}"
                ]
                build_args += ['--config', build_type]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level across all generators.
        if 'CMAKE_BUILD_PARALLEL_LEVEL' not in os.environ:
            if hasattr(self, 'parallel') and self.parallel:
                build_args += [f"-j{self.parallel}"]

        subprocess.run(['conan', 'profile', 'detect', '--force'], check=True)
        subprocess.run(['conan', 'install', '.'], check=True)
        subprocess.run(['cmake', target.source_dir, *cmake_args], check=True)
        subprocess.run(['cmake', '--build', '.', *build_args], check=True)


setup(
    ext_modules=[CMakeExtension('pyconcept')],
    cmdclass={'build_ext': CMakeBuild}
)
