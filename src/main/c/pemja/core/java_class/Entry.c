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

#include "java_class/Entry.h"

static jmethodID getKey = 0;
static jmethodID getValue = 0;

jobject
JavaMapEntry_getKey(JNIEnv* env, jobject jval)
{
    if (!getKey) {
        getKey = (*env)->GetMethodID(env, JMAP_ENTRY_TYPE, "getKey", "()Ljava/lang/Object;");
    }
    return (*env)->CallObjectMethod(env, jval, getKey);
}

jobject
JavaMapEntry_getValue(JNIEnv* env, jobject jval)
{
    if (!getValue) {
        getValue = (*env)->GetMethodID(env, JMAP_ENTRY_TYPE, "getValue", "()Ljava/lang/Object;");
    }
    return (*env)->CallObjectMethod(env, jval, getValue);
}
