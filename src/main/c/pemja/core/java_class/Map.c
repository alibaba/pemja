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

#include "java_class/Map.h"

static jmethodID init_HashMap = 0;
static jmethodID entrySet = 0;
static jmethodID put = 0;

jobject
JavaMap_NewHashMap(JNIEnv* env)
{
    if (!init_HashMap) {
        init_HashMap = (*env)->GetMethodID(env, JHASHMAP_TYPE, "<init>", "()V");
    }
    return (*env)->NewObject(env, JHASHMAP_TYPE, init_HashMap);
}

jobject
JavaMap_entrySet(JNIEnv* env, jobject object)
{
    if (!entrySet) {
        entrySet = (*env)->GetMethodID(env, JMAP_TYPE, "entrySet", "()Ljava/util/Set;");
    }
    return (*env)->CallObjectMethod(env, object, entrySet);
}

jobject
JavaMap_put(JNIEnv* env, jobject object, jobject key, jobject value)
{
    if (!put) {
        put = (*env)->GetMethodID(env, JMAP_TYPE, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    }
    return (*env)->CallObjectMethod(env, object, put, key, value);
}
