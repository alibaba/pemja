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

#include "java_class/Object.h"

static jmethodID equals = 0;
static jmethodID toString = 0;

jboolean
JavaObject_equals(JNIEnv* env, jobject this, jobject other)
{
    if (!equals) {
        equals = (*env)->GetMethodID(env, JOBJECT_TYPE, "equals", "(Ljava/lang/Object;)Z");
    }
    return (*env)->CallBooleanMethod(env, this, equals, other);
}

jstring
JavaObject_toString(JNIEnv* env, jobject obj)
{

    if (!toString) {
        toString = (*env)->GetMethodID(env, JOBJECT_TYPE, "toString", "()Ljava/lang/String;");
    }

    return (*env)->CallObjectMethod(env, obj, toString);
}
