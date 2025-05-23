#
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
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.19)
PROJECT(PEMJA)

OPTION(WITH_ASAN "build with ASAN" OFF)

SET(CMAKE_C_STANDARD 17)
SET(CMAKE_C_STANDARD_REQUIRED ON)

# include cpython
FIND_PACKAGE(Python REQUIRED COMPONENTS Interpreter Development)

# include JNI
IF (DEFINED ENV{JAVA_HOME})
    SET(JAVA_HOME $ENV{JAVA_HOME})
ELSE ()
    FAIL()
ENDIF ()

SET(JAVA_INCLUDE_PATH ${JAVA_HOME}/include)
IF (APPLE)
    SET(JAVA_INCLUDE_PATH2 ${JAVA_HOME}/include/darwin)
ELSEIF (UNIX)
    SET(JAVA_INCLUDE_PATH2 ${JAVA_HOME}/include/linux)
ELSE ()
    MESSAGE("Not supported target ${CMAKE_SYSTEM}")
    RETURN()
ENDIF ()
FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})

SET(SRC_DIR ${PROJECT_SOURCE_DIR}/pemja)
MESSAGE(STATUS "SRC_DIR path: ${SRC_DIR}")

AUX_SOURCE_DIRECTORY(${SRC_DIR}/core ROOT_SRCS)
AUX_SOURCE_DIRECTORY(${SRC_DIR}/utils ROOT_SRCS)

ADD_LIBRARY(pemja SHARED ${ROOT_SRCS})
TARGET_INCLUDE_DIRECTORIES(pemja PRIVATE
        ${SRC_DIR}/core/include
        ${SRC_DIR}/utils/include
        ${Python_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(pemja PRIVATE ${Python_LIBRARIES})

IF (WITH_ASAN)
    TARGET_COMPILE_OPTIONS(pemja PRIVATE -fsanitize=address)
    TARGET_LINK_OPTIONS(pemja PRIVATE -fsanitize=address)
ENDIF ()

