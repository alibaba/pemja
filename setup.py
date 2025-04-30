################################################################################
#
#  Copyright 2022 Alibaba Group Holding Limited.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

import io
import os
import shutil
import sys
import sysconfig
import warnings
from setuptools.command.build_ext import build_ext as old_build_ext

from setuptools import setup, Extension

if sys.version_info < (3, 8):
    print('Python versions prior to 3.7 are not supported for PemJa.',
          file=sys.stderr)
    sys.exit(-1)

this_directory = os.path.abspath(os.path.dirname(__file__))
version_file = os.path.join(this_directory, 'src/main/python/pemja/version.py')

try:
    exec (open(version_file).read())
except IOError:
    print("Failed to load PemJa version file for packaging. {0} not found!".format(version_file),
          file=sys.stderr)
    sys.exit(-1)

VERSION = __version__  # noqa

with io.open(os.path.join(this_directory, 'README.md'), 'r', encoding='utf-8') as f:
    long_description = f.read()

_java_home = None


def get_java_home():
    global _java_home
    if _java_home is not None:
        return _java_home

    env_home = os.environ.get('JAVA_HOME')
    if env_home:
        if os.path.exists(env_home):
            _java_home = env_home
            return env_home
        else:
            print('Path {0} indicated by JAVA_HOME does not exist.'.format(env_home),
                  file=sys.stderr)
            sys.exit(-1)

    if is_windows():
        java_path = shutil.which("java")
        if java_path is None:
            print('Can not determine JAVA_HOME from environment variable nor java executable path.',
                  file=sys.stderr)
            sys.exit(-1)
        exe_home = os.path.dirname(os.path.dirname(java_path))
        _java_home = exe_home
        return exe_home

    return None


def is_osx():
    return 'macosx' in sysconfig.get_platform()


def is_bsd():
    return 'bsd' in sysconfig.get_platform()

def is_windows():
    return 'win' in sysconfig.get_platform()


def get_python_libs():
    libs = []
    if not (is_bsd() or is_windows()):
        libs.append('dl')
    return libs

def get_java_libraries():
    if is_windows():
        return ['jvm']
    return []

def get_java_lib_folders():
    if not is_osx():
        import fnmatch
        if is_windows():
            jre = os.path.join(get_java_home(), 'lib')
        else:
            jre = os.path.join(get_java_home(), 'jre', 'lib')
            if not os.path.exists(jre):
                jre = os.path.join(get_java_home(), 'lib')
        folders = []
        for root, dirnames, filenames in os.walk(jre):
            if is_windows():
                for filename in fnmatch.filter(filenames, '*jvm.lib'):
                    folders.append(os.path.dirname(os.path.join(root, filename)))
            else:
                for filename in fnmatch.filter(filenames, '*jvm.so'):
                    folders.append(os.path.dirname(os.path.join(root, filename)))

        return list(set(folders))
    return []

def get_files(dir, pattern):
    ret = []
    for root, dirs, files in os.walk(dir):
        for f in files:
            if f.endswith(pattern):
                ret.append(os.path.join(root, f))
    return ret


def is_apple_jdk():
    return get_java_home() == '/System/Library/Frameworks/JavaVM.framework'


def get_java_linker_args():
    if is_apple_jdk():
        return ['-framework JavaVM']
    return []


def get_java_include():
    inc_name = 'include'
    if is_apple_jdk():
        inc_name = 'Headers'
    inc = os.path.join(get_java_home(), inc_name)
    if not os.path.exists(inc):
        print("Include folder should be at '{0}' but doesn't exist. "
              "Please check you've installed the JDK properly.".format(inc),
              file=sys.stderr)
        sys.exit(-1)
    jni = os.path.join(inc, "jni.h")
    if not os.path.exists(jni):
        print("jni.h should be in '{0}' but doesn't exist. "
              "Please check you've installed the JDK properly.".format(jni),
              file=sys.stderr)
        sys.exit(-1)

    paths = [inc]

    # Include platform specific headers if found
    include_linux = os.path.join(inc, 'linux')
    if os.path.exists(include_linux):
        paths.append(include_linux)

    include_darwin = os.path.join(inc, 'darwin')
    if os.path.exists(include_darwin):
        paths.append(include_darwin)

    include_bsd = os.path.join(inc, 'freebsd')
    if os.path.exists(include_bsd):
        paths.append(include_bsd)

    include_win32 = os.path.join(inc, 'win32')
    if os.path.exists(include_win32):
        paths.append(include_win32)
    return paths


def get_src_include():
    return ['src/main/c/Include']


def _is_using_gcc(obj):
    is_gcc = False
    if obj.compiler.compiler_type == 'unix':
        cc = sysconfig.get_config_var("CC")
        if not cc:
            cc = ""
        is_gcc = "gcc" in cc
    return is_gcc


class build_ext(old_build_ext):
    def build_extension(self, ext):
        if _is_using_gcc(self):
            if '-std=c99' not in ext.extra_compile_args:
                ext.extra_compile_args.append('-std=c99')
        old_build_ext.build_extension(self, ext)


extensions = ([
    Extension(
        name="pemja_core",
        sources=get_files('src/main/c/pemja/core', '.c'),
        libraries=get_java_libraries() + get_python_libs(),
        library_dirs = get_java_lib_folders(),
        extra_link_args=get_java_linker_args(),
        include_dirs=get_java_include() + ['src/main/c/pemja/core/include'],
        language="3"),
    Extension(
        name="pemja_utils",
        sources=get_files('src/main/c/pemja/utils', '.c'),
        library_dirs = get_java_lib_folders(),
        extra_link_args=get_java_linker_args(),
        include_dirs=get_java_include() + ['src/main/c/pemja/utils/include'],
        language="3")
])

PACKAGE_DATA = {
    'pemja': ['README.txt']
}

PACKAGE_DIR = {
    '': 'src/main/python'
}

setup(
    name='pemja',
    version=VERSION,
    packages=["pemja"],
    include_package_data=True,
    package_dir=PACKAGE_DIR,
    package_data=PACKAGE_DATA,
    author='Apache Software Foundation',
    license='https://www.apache.org/licenses/LICENSE-2.0',
    author_email='hxbks2ks@gmail.com',
    python_requires='>=3.8',
    install_requires=['find-libpython'],
    cmdclass={'build_ext': build_ext},
    description='PemJa',
    long_description=long_description,
    long_description_content_type='text/markdown',
    zip_safe=False,
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: Python :: Implementation :: CPython',
        'Operating System :: Unix',
        'Operating System :: MacOS',
        'Operating System :: Microsoft :: Windows',
    ],
    ext_modules=extensions)
