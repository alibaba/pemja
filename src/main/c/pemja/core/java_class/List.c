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

#include "java_class/List.h"

static jmethodID init_ArrayList = 0;
static jmethodID add = 0;

jobject
JavaList_NewArrayList(JNIEnv* env)
{
    if (!init_ArrayList) {
        init_ArrayList = (*env)->GetMethodID(env, JARRAYLIST_TYPE, "<init>", "()V");
    }
    return (*env)->NewObject(env, JARRAYLIST_TYPE, init_ArrayList);
}

jboolean
JavaList_Add(JNIEnv* env, jobject object, jobject value)
{
    if (!add) {
        add = (*env)->GetMethodID(env, JLIST_TYPE, "add", "(Ljava/lang/Object;)Z");
    }
    return (*env)->CallBooleanMethod(env, object, add, value);
}
