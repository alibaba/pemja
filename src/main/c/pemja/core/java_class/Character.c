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

#include "java_class/Character.h"

static jmethodID init_C = 0;
static jmethodID charValue = 0;

jobject
JavaCharacter_New(JNIEnv* env, jchar jval)
{
    if (!init_C) {
        init_C = (*env)->GetMethodID(env, JCHAR_OBJ_TYPE, "<init>", "(C)V");
    }
    return (*env)->NewObject(env, JCHAR_OBJ_TYPE, init_C, jval);
}

jchar
JavaCharacter_charValue(JNIEnv* env, jobject jval)
{
    if (!charValue) {
        charValue = (*env)->GetMethodID(env, JCHAR_OBJ_TYPE, "charValue", "()C");
    }
    return (*env)->CallCharMethod(env, jval, charValue);
}