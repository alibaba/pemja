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

#include "java_class/Field.h"

static jmethodID getType = 0;
static jmethodID getModifiers = 0;

jclass
JavaField_getType(JNIEnv* env, jobject this)
{
    if (!getType) {
        getType = (*env)->GetMethodID(env, JFIELD_TYPE, "getType",
                                      "()Ljava/lang/Class;");
    }
    return (jclass) (*env)->CallObjectMethod(env, this, getType);
}

jint
JavaField_getModifiers(JNIEnv* env, jobject this)
{
    if (!getModifiers) {
        getModifiers = (*env)->GetMethodID(env, JFIELD_TYPE, "getModifiers",
                                           "()I");
    }
    return (*env)->CallIntMethod(env, this, getModifiers);
}
