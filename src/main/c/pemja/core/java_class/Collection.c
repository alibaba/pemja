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

static jmethodID size = 0;
static jmethodID contains = 0;

jint
JavaCollection_size(JNIEnv* env, jobject object)
{
    if (!size) {
        size = (*env)->GetMethodID(env, JCOLLECTION_TYPE, "size", "()I");
    }
    return (*env)->CallIntMethod(env, object, size);
}


jboolean
JavaCollection_contains(JNIEnv* env, jobject this, jobject object)
{
    if (!contains) {
        contains = (*env)->GetMethodID(env, JCOLLECTION_TYPE, "contains", "(Ljava/lang/Object;)Z");
    }
    return (*env)->CallBooleanMethod(env, this, contains, object);
}
