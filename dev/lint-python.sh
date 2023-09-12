#!/usr/bin/env bash
################################################################################
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

# lint-python.sh
# This script will prepare a virtual environment for many kinds of checks, such as tox check, flake8 check.
#

# Download some software, such as miniconda.sh
function download() {
    local DOWNLOAD_STATUS=
    if hash "wget" 2>/dev/null; then
        # because of the difference of all versions of wget, so we turn of the option --show-progress
        wget "$1" -O "$2" -q -T20 -t3
        DOWNLOAD_STATUS="$?"
    else
        curl "$1" -o "$2" --progress-bar --connect-timeout 20 --retry 3
        DOWNLOAD_STATUS="$?"
    fi
    if [[ ${DOWNLOAD_STATUS} -ne 0 ]]; then
        echo "Dowload failed.You can try again"
        exit ${DOWNLOAD_STATUS}
    fi
}

# Printing infos both in log and console
function print_function() {
    local STAGE_LENGTH=48
    local left_edge_len=
    local right_edge_len=
    local str
    case "$1" in
        "STAGE")
            left_edge_len=$(((STAGE_LENGTH-${#2})/2))
            right_edge_len=$((STAGE_LENGTH-${#2}-left_edge_len))
            str="$(seq -s "=" ${left_edge_len} | tr -d "[:digit:]")""$2""$(seq -s "=" ${right_edge_len} | tr -d "[:digit:]")"
            ;;
        "STEP")
            str="$2"
            ;;
        *)
            str="seq -s "=" $STAGE_LENGTH | tr -d "[:digit:]""
            ;;
    esac
    echo ${str} | tee -a ${LOG_FILE}
}

function regexp_match() {
    if echo $1 | grep -e $2 &>/dev/null; then
        echo true
    else
        echo false
    fi
}

# decide whether a array contains a specified element.
function contains_element() {
    arr=($1)
    if echo "${arr[@]}" | grep -w "$2" &>/dev/null; then
        echo true
    else
        echo false
    fi
}

# Checkpoint the stage:step for convenient to re-exec the script with
# skipping those success steps.
# The format is "${Stage}:${Step}". e.g. Install:4
function checkpoint_stage() {
    if [[ ! -d `dirname ${STAGE_FILE}` ]]; then
        mkdir -p `dirname ${STAGE_FILE}`
    fi
    echo "$1:$2">"$STAGE_FILE"
}

# Restore the stage:step
function restore_stage() {
    if [[ -f "$STAGE_FILE" ]]; then
        local lines=$(awk '{print NR}' ${STAGE_FILE})
        if [[ ${lines} -eq 1 ]]; then
            local first_field=$(cat ${STAGE_FILE} | cut -d ":" -f 1)
            local second_field=$(cat ${STAGE_FILE} | cut -d ":" -f 2)
            check_valid_stage ${first_field} ${second_field}
            if [[ $? -eq 0 ]]; then
                STAGE=${first_field}
                STEP=${second_field}
                return
            fi
        fi
    fi
    STAGE="install"
    STEP=0
}

# Decide whether the stage:step is valid.
function check_valid_stage() {
    case $1 in
        "install")
            if [[ $2 -le ${STAGE_INSTALL_STEPS} ]] && [[ $2 -ge 0 ]]; then
                return 0
            fi
            ;;
        *)
            ;;
    esac
    return 1
}

function parse_component_args() {
    local REAL_COMPONENTS=()
    for component in ${INSTALLATION_COMPONENTS[@]}; do
        # because all other components depends on conda, the install of conda is
        # required component.
        if [[ "$component" == "basic" ]] || [[ "$component" == "miniconda" ]]; then
            continue
        fi
        if [[ "$component" == "all" ]]; then
            component="environment"
        fi
        if [[ `contains_element "${SUPPORTED_INSTALLATION_COMPONENTS[*]}" "${component}"` = true ]]; then
            REAL_COMPONENTS+=(${component})
        else
            echo "unknown install component ${component}, currently we only support installing basic,py_env,tox,flake8,sphinx,mypy,all."
            exit 1
        fi
    done
    if [[ `contains_element "${REAL_COMPONENTS[*]}" "environment"` = false ]]; then
        SUPPORTED_INSTALLATION_COMPONENTS=(${REAL_COMPONENTS[@]})
    fi
}

# For convenient to index something binded to OS.
# Now, the script only make a distinction between 'Mac' and 'Non-Mac'.
function get_os_index() {
    if [ $1 == "Darwin" ]; then
        return 0
    else
        return 1
    fi
}

# Considering the file size of miniconda.sh,
# "wget" is better than "curl" in the weak network environment.
function install_wget() {
    if [ $1 == "Darwin" ]; then
        hash "brew" 2>/dev/null
        if [ $? -ne 0 ]; then
            $((/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)") 2>&1 >/dev/null)
            if [ $? -ne 0 ]; then
                echo "Failed to install brew"
                exit 1
            fi
        fi

        hash "wget" 2>/dev/null
        if [ $? -ne 0 ]; then
            brew install wget 2>&1 >/dev/null
            if [ $? -ne 0 ]; then
                echo "Failed to install wget"
                exit 1
            fi
        fi
    fi
}

# The script choose miniconda as our package management tool.
# The script use miniconda to create all kinds of python versions and
# some pakcages including checks such as tox and flake8.

function install_miniconda() {
    OS_TO_CONDA_URL=("https://repo.anaconda.com/miniconda/Miniconda3-py39_23.5.2-0-MacOSX-x86_64.sh" \
        "https://repo.anaconda.com/miniconda/Miniconda3-py39_23.5.2-0-Linux-x86_64.sh")
    print_function "STEP" "download miniconda..."
    if [ ! -f "$CONDA_INSTALL" ]; then
        download ${OS_TO_CONDA_URL[$1]} $CONDA_INSTALL_SH
        chmod +x $CONDA_INSTALL_SH
        if [ $? -ne 0 ]; then
            echo "Please manually chmod +x $CONDA_INSTALL_SH"
            exit 1
        fi
        if [ -d "$CURRENT_DIR/.conda" ]; then
            rm -rf "$CURRENT_DIR/.conda"
            if [ $? -ne 0 ]; then
                echo "Please manually rm -rf $CURRENT_DIR/.conda directory.\
                Then retry to exec the script."
                exit 1
            fi
        fi
    fi
    print_function "STEP" "download miniconda... [SUCCESS]"

    print_function "STEP" "installing conda..."
    if [ ! -d "$CURRENT_DIR/.conda" ]; then
        ${CONDA_INSTALL_SH} -b -p ${CURRENT_DIR}/.conda 2>&1 >/dev/null
        if [ $? -ne 0 ]; then
            echo "install miniconda failed"
            exit $CONDA_INSTALL_STATUS
        fi
    fi
    print_function "STEP" "install conda ... [SUCCESS]"
}

# Install some kinds of py env.
function install_py_env() {
    py_env=("3.8" "3.9" "3.10" "3.11")
    for ((i=0;i<${#py_env[@]};i++)) do
        if [[ -d "$CURRENT_DIR/.conda/envs/${py_env[i]}" ]]; then
            rm -rf "$CURRENT_DIR/.conda/envs/${py_env[i]}"
            if [[ $? -ne 0 ]]; then
                echo "rm -rf $CURRENT_DIR/.conda/envs/${py_env[i]} failed, please \
                rm -rf $CURRENT_DIR/.conda/envs/${py_env[i]} manually.\
                Then retry to exec the script."
                exit 1
            fi
        fi
        print_function "STEP" "installing python${py_env[i]}..."
        max_retry_times=3
        retry_times=0
        install_command="$CONDA_PATH create --name ${py_env[i]} -y -q python=${py_env[i]}"
        ${install_command} 2>&1 >/dev/null
        status=$?
        while [[ ${status} -ne 0 ]] && [[ ${retry_times} -lt ${max_retry_times} ]]; do
            retry_times=$((retry_times+1))
            # sleep 3 seconds and then reinstall.
            sleep 3
            echo "conda install ${py_env[i]} retrying ${retry_times}/${max_retry_times}"
            ${install_command} 2>&1 >/dev/null
            status=$?
        done
        if [[ ${status} -ne 0 ]]; then
            echo "conda install ${py_env[i]} failed after retrying ${max_retry_times} times.\
            You can retry to execute the script again."
            exit 1
        fi
        print_function "STEP" "install python${py_env[i]}... [SUCCESS]"
    done
}

# Install tox.
# In some situations,you need to run the script with "sudo". e.g. sudo ./lint-python.sh
function install_tox() {
    source ${CONDA_HOME}/bin/activate
    if [[ -f "$TOX_PATH" ]]; then
        ${PIP_PATH} uninstall tox -y -q 2>&1 >/dev/null
        if [[ $? -ne 0 ]]; then
            echo "pip uninstall tox failed \
            please try to exec the script again.\
            if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
            exit 1
        fi
    fi

    # tox 3.14.0 depends on both 0.19 and 0.23 of importlib_metadata at the same time and
    # conda will try to install both these two versions and it will cause problems occasionally.
    # Using pip as the package manager could avoid this problem.
    ${CURRENT_DIR}/install_command.sh -q virtualenv==16.0.0 tox==3.14.0 2>&1 >/dev/null
    if [[ $? -ne 0 ]]; then
        echo "pip install tox failed \
        please try to exec the script again.\
        if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
        exit 1
    fi
    conda deactivate
}

# Install flake8.
# In some situations,you need to run the script with "sudo". e.g. sudo ./lint-python.sh
function install_flake8() {
    source ${CONDA_HOME}/bin/activate
    if [[ -f "$FLAKE8_PATH" ]]; then
        ${PIP_PATH} uninstall flake8 -y -q 2>&1 >/dev/null
        if [[ $? -ne 0 ]]; then
            echo "pip uninstall flake8 failed \
            please try to exec the script again.\
            if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
            exit 1
        fi
    fi

    ${CURRENT_DIR}/install_command.sh -q flake8==3.7.9 2>&1 >/dev/null
    if [[ $? -ne 0 ]]; then
        echo "pip install flake8 failed \
        please try to exec the script again.\
        if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
        exit 1
    fi
    conda deactivate
}

# Install sphinx.
# In some situations,you need to run the script with "sudo". e.g. sudo ./lint-python.sh
function install_sphinx() {
    source ${CONDA_HOME}/bin/activate
    if [[ -f "$SPHINX_PATH" ]]; then
        ${PIP_PATH} uninstall Sphinx -y -q 2>&1 >/dev/null
        if [[ $? -ne 0 ]]; then
            echo "pip uninstall sphinx failed \
            please try to exec the script again.\
            if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
            exit 1
        fi
    fi

    ${CURRENT_DIR}/install_command.sh -q Sphinx==2.4.4 2>&1 >/dev/null
    if [[ $? -ne 0 ]]; then
        echo "pip install sphinx failed \
        please try to exec the script again.\
        if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
        exit 1
    fi
    conda deactivate
}


# Install mypy.
# In some situations, you need to run the script with "sudo". e.g. sudo ./lint-python.sh
function install_mypy() {
    source ${CONDA_HOME}/bin/activate
    if [[ -f "$MYPY_PATH" ]]; then
        ${PIP_PATH} uninstall mypy -y -q 2>&1 >/dev/null
        if [[ $? -ne 0 ]]; then
            echo "pip uninstall mypy failed \
            please try to exec the script again.\
            if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
            exit 1
        fi
    fi
    ${CURRENT_DIR}/install_command.sh -q mypy==0.790 2>&1 >/dev/null
    if [[ $? -ne 0 ]]; then
        echo "pip install mypy failed \
        please try to exec the script again.\
        if failed many times, you can try to exec in the form of sudo ./lint-python.sh -f"
        exit 1
    fi
    conda deactivate
}

function need_install_component() {
    if [[ `contains_element "${SUPPORTED_INSTALLATION_COMPONENTS[*]}" "$1"` = true ]]; then
        echo true
    else
        echo false
    fi
}


# In this function, the script will prepare all kinds of python environments and checks.
function install_environment() {

    print_function "STAGE" "installing environment"

    local sys_os=`uname -s`
    #get the index of the SUPPORT_OS array for convinient to intall tool.
    get_os_index ${sys_os}
    local os_index=$?

    # step-1 install wget
    # the file size of the miniconda.sh is too big to use "wget" tool to download instead
    # of the "curl" in the weak network environment.
    print_function "STEP" "installing wget..."
    if [[ ${STEP} -lt 1 ]]; then
        install_wget ${SUPPORT_OS[$os_index]}
        STEP=1
        checkpoint_stage ${STAGE} ${STEP}
    fi
    print_function "STEP" "install wget... [SUCCESS]"

    # step-2 install miniconda
    print_function "STEP" "installing miniconda..."
    if [[ ${STEP} -lt 2 ]]; then
        create_dir ${CURRENT_DIR}/download
        install_miniconda $os_index
        STEP=2
        checkpoint_stage $STAGE $STEP
    fi
    print_function "STEP" "install miniconda... [SUCCESS]"

    # step-3 install python environment which includes
    # 3.8 3.9 3.10 3.11
    if [[ ${STEP} -lt 3 ]] && [[ `need_install_component "py_env"` = true ]]; then
        print_function "STEP" "installing python environment..."
        install_py_env
        STEP=3
        checkpoint_stage $STAGE $STEP
        print_function "STEP" "install python environment... [SUCCESS]"
    fi

    # step-4 install tox
    if [[ ${STEP} -lt 4 ]] && [[ `need_install_component "tox"` = true ]]; then
        print_function "STEP" "installing tox..."
        install_tox
        STEP=4
        checkpoint_stage $STAGE $STEP
        print_function "STEP" "install tox... [SUCCESS]"
    fi

    # step-5 install  flake8
    if [[ ${STEP} -lt 5 ]] && [[ `need_install_component "flake8"` = true ]]; then
        print_function "STEP" "installing flake8..."
        install_flake8
        STEP=5
        checkpoint_stage $STAGE $STEP
        print_function "STEP" "install flake8... [SUCCESS]"
    fi

    # step-6 install sphinx
    if [[ ${STEP} -lt 6 ]] && [[ `need_install_component "sphinx"` = true ]]; then
        print_function "STEP" "installing sphinx..."
        install_sphinx
        STEP=6
        checkpoint_stage ${STAGE} ${STEP}
        print_function "STEP" "install sphinx... [SUCCESS]"
    fi

    # step-7 install mypy
    if [[ ${STEP} -lt 7 ]] && [[ `need_install_component "mypy"` = true ]]; then
        print_function "STEP" "installing mypy..."
        install_mypy
        STEP=7
        checkpoint_stage ${STAGE} ${STEP}
        print_function "STEP" "install mypy... [SUCCESS]"
    fi

    print_function "STAGE"  "install environment... [SUCCESS]"
}

# create dir if needed
function create_dir() {
    if [[ ! -d $1 ]]; then
        mkdir -p $1
        if [[ $? -ne 0 ]]; then
            echo "mkdir -p $1 failed. you can mkdir manually or exec the script with \
            the command: sudo ./lint-python.sh"
            exit 1
        fi
    fi
}

# Set created py-env in $PATH for tox's creating virtual env
function activate () {
    if [[ ! -d ${CURRENT_DIR}/.conda/envs ]]; then
        echo "For some unknown reasons, missing the directory $CURRENT_DIR/.conda/envs,\
        you should exec the script with the option: -f"
        exit 1
    fi

    for py_dir in ${CURRENT_DIR}/.conda/envs/*
    do
        PATH=${py_dir}/bin:$PATH
    done
    export PATH 2>/dev/null
    if [[ $? -ne 0 ]]; then
        echo "For some unknown reasons, the py package is not complete,\
        you should exec the script with the option: -f"
        exit 1
    fi
}

# Reset the $PATH
function deactivate() {
    # reset old environment variables
    # ! [ -z ${VAR+_} ] returns true if VAR is declared at all
    if ! [[ -z "${_OLD_PATH+_}" ]] ; then
        PATH="$_OLD_PATH"
        export PATH
        unset _OLD_PATH
    fi
}

# get all supported install components functions
function get_all_supported_install_components() {
    _OLD_IFS=$IFS
    IFS=$'\n'
    for fun in $(declare -F); do
        if [[ `regexp_match "${fun:11}" "^install_"` = true ]]; then
            SUPPORTED_INSTALLATION_COMPONENTS+=("${fun:19}")
        fi
    done
    IFS=$_OLD_IFS
    # we don't need to expose "install_wget" to user.
    local DELETE_COMPONENTS=("wget")
    local REAL_COMPONENTS=()
    for component in ${SUPPORTED_INSTALLATION_COMPONENTS[@]}; do
        if [[ `contains_element "${DELETE_COMPONENTS[*]}" "${component}"` = false ]]; then
            REAL_COMPONENTS+=("${component}")
        fi
    done
    SUPPORTED_INSTALLATION_COMPONENTS=(${REAL_COMPONENTS[@]})
}

# CURRENT_DIR is "dev/"
CURRENT_DIR="$(cd "$( dirname "$0" )" && pwd)"

# JCP_DIR is "jcp"
JCP_DIR=$(dirname "$CURRENT_DIR")

# conda home path
CONDA_HOME=${CURRENT_DIR}/.conda

# conda path
CONDA_PATH=${CONDA_HOME}/bin/conda

# pip path
PIP_PATH=${CONDA_HOME}/bin/pip

# tox path
TOX_PATH=${CONDA_HOME}/bin/tox

# flake8 path
FLAKE8_PATH=${CONDA_HOME}/bin/flake8

# sphinx path
SPHINX_PATH=${CONDA_HOME}/bin/sphinx-build

# mypy path
MYPY_PATH=${CONDA_HOME}/bin/mypy

_OLD_PATH="$PATH"

SUPPORT_OS=("Darwin" "Linux")

# the file stores the success step in installing progress.
STAGE_FILE=${CURRENT_DIR}/.stage.txt

# the dir includes all kinds of py env installed.
VIRTUAL_ENV=${CONDA_HOME}/envs

LOG_DIR=${CURRENT_DIR}/log

if [[ "$JCP_STRING" == "" ]]; then
    JCP_STRING="$USER"
fi
if [[ "$HOSTNAME" == "" ]]; then
    HOSTNAME="$HOST"
fi

# the log file stores the checking result.
LOG_FILE=${LOG_DIR}/jcp-${JCP_STRING}-python-$HOSTNAME.log
create_dir ${LOG_DIR}

# clean LOG_FILE content
echo >${LOG_FILE}

# miniconda script
CONDA_INSTALL_SH=${CURRENT_DIR}/download/miniconda.sh

# stage "install" includes the num of steps.
STAGE_INSTALL_STEPS=7

# whether force to restart the script.
FORCE_START=0

SUPPORT_CHECKS=()

SUPPORTED_INSTALLATION_COMPONENTS=()

# search all supported install functions and put them into SUPPORTED_INSTALLATION_COMPONENTS array
get_all_supported_install_components

INSTALLATION_COMPONENTS=()
# parse_opts
USAGE="
usage: $0 [options]
-h          print this help message and exit
-f          force to exec from the progress of installing environment
-s [basic,py_env,tox,flake8,sphinx,mypy,all]
            install environment with specified components which split by comma(,)
            note:
                This option is used to install environment components and will skip all subsequent checks,
                so do not use this option with -e,-i simultaneously.
Examples:
  ./lint-python -s basic        =>  install environment with basic components.
  ./lint-python -s py_env       =>  install environment with python env(3.8,3.9,3.10,3.11).
  ./lint-python -s all          =>  install environment with all components such as python env,tox,flake8,sphinx,mypy etc.
  ./lint-python -s tox,flake8   =>  install environment with tox,flake8.
  ./lint-python -s tox -f       =>  reinstall environment with tox.
  ./lint-python                 =>  install all environment.
  ./lint-python -f              =>  reinstall environment with all components and exec all checks.
"
while getopts "hfs:l" arg; do
    case "$arg" in
        h)
            printf "%s\\n" "$USAGE"
            exit 2
            ;;
        f)
            FORCE_START=1
            ;;
        s)
            INSTALLATION_COMPONENTS=($(echo $OPTARG | tr ',' ' ' ))
            ;;
        ?)
            printf "ERROR: did not recognize option '%s', please try -h\\n" "$1"
            exit 1
            ;;
    esac
done

# decides whether to skip check stage
skip_checks=0
if [[ ! -z "$INSTALLATION_COMPONENTS" ]]; then
    parse_component_args
    skip_checks=1
fi

# collect checks according to the options
collect_checks

# If exec the script with the param: -f, all progress will be re-run
if [[ ${FORCE_START} -eq 1 ]]; then
    STAGE="install"
    STEP=0
    checkpoint_stage ${STAGE} ${STEP}
else
    restore_stage
fi

# install environment
install_environment

pushd "$JCP_DIR" &> /dev/null
