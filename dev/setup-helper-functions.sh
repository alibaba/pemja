function github_checkout {
  local REPO=$1
  shift
  local VERSION=$1
  shift
  # shellcheck disable=SC2124
  local GIT_CLONE_PARAMS=$@
  # shellcheck disable=SC2155
  local DIRNAME=$(basename "$REPO")
  # shellcheck disable=SC2164
  cd "${DEPENDENCY_DIR}"
  if [ -z "${DIRNAME}" ]; then
    echo "Failed to get repo name from ${REPO}"
    exit 1
  fi

  if [ -d "${DIRNAME}" ];then
    echo "${DIRNAME} already exists. Delete?";
    # shellcheck disable=SC2162
    read res
    if [ "$res" = "yes" ];then
        rm -rf "${DIRNAME}"
    fi
  fi
  if [ ! -d "${DIRNAME}" ]; then
    git clone -q -b "$VERSION" "$GIT_CLONE_PARAMS" "https://github.com/${REPO}.git"
  fi
  # shellcheck disable=SC2164
  cd "${DIRNAME}"
}

function wget_and_untar {
  local URL=$1
  local DIR=$2
  local STRIP_COMPONENTS=1
  if [[ $# -eq 3 ]] ; then
      STRIP_COMPONENTS=$3
  fi


  if [ -d "${DIR}" ];then
    echo "Warning: ${DIR} already exists, if you want to re-built the dependency, please manually remove the directory."
    return
  fi

  mkdir -p "${DIR}"
  wget --no-check-certificate -o logs/"${DIR}".log --max-redirect 3 -O - "${URL}" | tar -xz -C "${DIR}" --strip-components="$STRIP_COMPONENTS"
  touch new_flags/"${DIR}"
}

function cmake_install {
  # shellcheck disable=SC2155
  local NAME=$(basename "$(pwd)")
  local BINARY_DIR=_build
  if [ -d "${BINARY_DIR}" ] && prompt "Do you want to rebuild ${NAME}?"; then
    rm -rf "${BINARY_DIR}"
  fi
  mkdir -p "${BINARY_DIR}"


  cmake -Wno-dev -B"${BINARY_DIR}" \
    -G "Unix Makefiles" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_CXX_STANDARD=17 \
    "${INSTALL_PREFIX+-DCMAKE_PREFIX_PATH=}${INSTALL_PREFIX-}" \
    "${INSTALL_PREFIX+-DCMAKE_INSTALL_PREFIX=}${INSTALL_PREFIX-}" \
    -DBUILD_TESTING=OFF \
    -DBENCHMARK_ENABLE_TESTING=OFF \
    -DFAT_RUNTIME=OFF \
    "$@"
  make -C "${BINARY_DIR}" install -j 16
}

function auto_install() {
  # shellcheck disable=SC2155
  local NAME=$(basename "$(pwd)")
  ./configure --prefix="${INSTALL_PREFIX}" "$@"
  make -j 16
  make install
}

function wget_and_untar_header {
  local URL=$1
  local DIR=$2
  local STRIP_COMPONENTS=1
  if [[ $# -eq 3 ]] ; then
      STRIP_COMPONENTS=$3
  fi

  mkdir -p "${DIR}"
  wget --no-check-certificate --max-redirect 3 -O - "${URL}" | tar -xz -C "${DIR}" --strip-components="$STRIP_COMPONENTS"
}
