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

#include "Pemja.h"

#include "java_class/Float.h"

static jmethodID init_F = 0;

jobject
JavaFloat_New(JNIEnv* env, jfloat jval)
{
    if (!init_F) {
        init_F = (*env)->GetMethodID(env, JFLOAT_OBJ_TYPE, "<init>", "(F)V");
    }
    return (*env)->NewObject(env, JFLOAT_OBJ_TYPE, init_F, jval);
}
