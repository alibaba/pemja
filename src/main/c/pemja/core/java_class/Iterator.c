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

#include "java_class/Iterator.h"

static jmethodID hasNext = 0;
static jmethodID next = 0;

jboolean
JavaIterator_hasNext(JNIEnv* env, jobject jval)
{
    if (!hasNext) {
        hasNext = (*env)->GetMethodID(env, JITERATOR_TYPE, "hasNext", "()Z");
    }
    return (*env)->CallBooleanMethod(env, jval, hasNext);
}

jobject
JavaIterator_next(JNIEnv* env, jobject jval)
{
    if (!next) {
        next = (*env)->GetMethodID(env, JITERATOR_TYPE, "next", "()Ljava/lang/Object;");
    }
    return (*env)->CallObjectMethod(env, jval, next);
}
