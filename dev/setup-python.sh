#!/usr/bin/env bash

# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

export PYTHON_URL=${PYTHON_URL:-https://www.python.org/ftp/python/3.9.21/Python-3.9.21.tgz}

# ---------- Env Params ----------

# shellcheck disable=SC2006
BASE_DIR=`dirname "$0"`/..
# shellcheck disable=SC2006
BASE_DIR=`cd "$BASE_DIR" || exit;pwd`
echo "$BASE_DIR"
. "$BASE_DIR"/dev/setup-helper-functions.sh
if [ -n "$1" ]; then
  BASE_DIR=$1
  echo "BASE_DIR is set to $1"
fi
if command -v clang && command -v clang++;then
    export USE_CLANG=1

    # shellcheck disable=SC2155
    # shellcheck disable=SC2006
    export CC=`which clang`
    # shellcheck disable=SC2155
    # shellcheck disable=SC2006
    export CXX=`which clang++`
fi

# ---------- Create Dir to Store Source of Dependencies ----------
export DEPENDENCY_DIR=${BASE_DIR}/3rd_party
mkdir -p "$DEPENDENCY_DIR"
cd "$DEPENDENCY_DIR" || exit

mkdir -p sources
rm -rf sources/*
cd sources || exit

mkdir -p logs
mkdir -p new_flags
rm -f new_flags/*
# ---------- Downloading headers of Dependencies ----------
download_tasks=""

wget_and_untar "$PYTHON_URL" cpython  &
download_tasks="$download_tasks $!"

for pid in $download_tasks;do
    wait "$pid";
done

# ---------- Install Dependencies ----------
export INSTALL_PREFIX=$BASE_DIR/pemja_dep_install/

pushd cpython || exit
[ -e ./configure ] || ./autogen.sh
auto_install --enable-shared
popd || exit