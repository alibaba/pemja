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

#include "java_class/Boolean.h"

static jmethodID init_Z = 0;
static jmethodID booleanValue = 0;

jobject
JavaBoolean_New(JNIEnv* env, jboolean jval)
{
    if (!init_Z) {
        init_Z = (*env)->GetMethodID(env, JBOOLEAN_OBJ_TYPE, "<init>", "(Z)V");
    }
    return (*env)->NewObject(env, JBOOLEAN_OBJ_TYPE, init_Z, jval);
}

jboolean
JavaBoolean_booleanValue(JNIEnv* env, jobject jval)
{
    if (!booleanValue) {
        booleanValue = (*env)->GetMethodID(env, JBOOLEAN_OBJ_TYPE, "booleanValue", "()Z");
    }
    return (*env)->CallBooleanMethod(env, jval, booleanValue);;
}
