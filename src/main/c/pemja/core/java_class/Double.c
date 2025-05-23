// Copyright 2022 Alibaba Group Holding Limited.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "java_class/Double.h"

#include "Pemja.h"

static jmethodID init_D = 0;

jobject JavaDouble_New(JNIEnv* env, jdouble jval) {
  if (!init_D) {
    init_D = (*env)->GetMethodID(env, JDOUBLE_OBJ_TYPE, "<init>", "(D)V");
  }
  return (*env)->NewObject(env, JDOUBLE_OBJ_TYPE, init_D, jval);
}
