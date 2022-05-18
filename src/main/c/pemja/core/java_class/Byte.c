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

#include "java_class/Byte.h"

static jmethodID init_B = 0;

jobject
JavaByte_New(JNIEnv* env, jbyte jval)
{
    if (!init_B) {
        init_B = (*env)->GetMethodID(env, JBYTE_OBJ_TYPE, "<init>", "(B)V");
    }
    return (*env)->NewObject(env, JBYTE_OBJ_TYPE, init_B, jval);
}
