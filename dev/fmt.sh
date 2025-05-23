#!/usr/bin/env bash

DEV_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
PROJECT_BASE_DIR="${DEV_DIR}/.."

which clang-format > /dev/null
ret=$?
if [[ $ret -ne 0 ]]; then
  #skip formatting if clang-format is not available
  exit 0
fi

# format java code
pushd "${PROJECT_BASE_DIR}" || exit
mvn spotless::apply
popd || exit

# format c/c++/h/hpp files
find "$PROJECT_BASE_DIR"/src/main/c/pemja \
  -type f \
  -o -name '*.cpp' \
  -o -name '*.c' \
  -o -name '*.cc' \
  -o -name '*.h' \
  -o -name '*.hpp' \
  | xargs clang-format -style=file -i

# format python files
python -m black "${PROJECT_BASE_DIR}"/src/main/python/pemja
python -m isort "${PROJECT_BASE_DIR}"/src/main/python/pemja
