#!/usr/bin/env bash
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
set -e -x

## 1. install python env
dev/lint-python.sh -s py_env

PY_ENV_DIR=`pwd`/dev/.conda/envs
py_env=("3.8" "3.9" "3.10" "3.11" "3.12" "3.13" "3.13t")
## 2. install dependency
for ((i=0;i<${#py_env[@]};i++)) do
    ${PY_ENV_DIR}/${py_env[i]}/bin/pip install -r dev/dev-requirements.txt
done

## 3. build wheels
if [[ "$(uname)" != "Darwin" ]]; then
    # force the linker to use the older glibc version in Linux
    export CFLAGS="-I. -include dev/glibc_version_fix.h"
fi

for ((i=0;i<${#py_env[@]};i++)) do
    if [[ "$(uname)" == "Darwin" ]]; then
      "${PY_ENV_DIR}"/${py_env[i]}/bin/python setup.py bdist_wheel
    else
      "${PY_ENV_DIR}"/${py_env[i]}/bin/python setup.py bdist_wheel --plat-name manylinux1_x86_64
    fi
done
## see the result
ls -al dist/
