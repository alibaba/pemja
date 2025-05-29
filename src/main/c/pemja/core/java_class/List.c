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

#include "java_class/List.h"

#include "Pemja.h"

static jmethodID init_ArrayList = 0;
static jmethodID init_ArrayList_with_capacity = 0;
static jmethodID init_ArrayList_with_collection = 0;
static jmethodID add = 0;
static jmethodID addAll = 0;
static jmethodID get = 0;
static jmethodID set = 0;

jobject JavaList_NewArrayList(JNIEnv* env) {
  if (!init_ArrayList) {
    init_ArrayList = (*env)->GetMethodID(env, JARRAYLIST_TYPE, "<init>", "()V");
  }
  return (*env)->NewObject(env, JARRAYLIST_TYPE, init_ArrayList);
}

jobject JavaList_NewArrayListWithCapacity(JNIEnv* env, jint capacity) {
  if (!init_ArrayList_with_capacity) {
    init_ArrayList_with_capacity =
        (*env)->GetMethodID(env, JARRAYLIST_TYPE, "<init>", "(I)V");
  }
  return (*env)->NewObject(env, JARRAYLIST_TYPE, init_ArrayList_with_capacity,
                           capacity);
}

jobject JavaList_NewArrayListWithCollection(JNIEnv* env, jobject collection) {
  if (!init_ArrayList_with_collection) {
    init_ArrayList_with_collection = (*env)->GetMethodID(
        env, JARRAYLIST_TYPE, "<init>", "(Ljava/util/Collection;)V");
  }

  return (*env)->NewObject(env, JARRAYLIST_TYPE, init_ArrayList_with_collection,
                           collection);
}

jboolean JavaList_Add(JNIEnv* env, jobject object, jobject value) {
  if (!add) {
    add = (*env)->GetMethodID(env, JLIST_TYPE, "add", "(Ljava/lang/Object;)Z");
  }
  return (*env)->CallBooleanMethod(env, object, add, value);
}

jboolean JavaList_AddAll(JNIEnv* env, jobject object, jobject values) {
  if (!addAll) {
    addAll = (*env)->GetMethodID(env, JLIST_TYPE, "addAll",
                                 "(Ljava/util/Collection;)Z");
  }
  return (*env)->CallBooleanMethod(env, object, addAll, values);
}

jobject JavaList_Get(JNIEnv* env, jobject object, jint index) {
  if (!get) {
    get = (*env)->GetMethodID(env, JLIST_TYPE, "get", "(I)Ljava/lang/Object;");
  }
  return (*env)->CallObjectMethod(env, object, get, index);
}

jobject JavaList_Set(JNIEnv* env, jobject object, jint index, jobject value) {
  if (!set) {
    set = (*env)->GetMethodID(env, JLIST_TYPE, "set",
                              "(ILjava/lang/Object;)Ljava/lang/Object;");
  }
  return (*env)->CallObjectMethod(env, object, set, index, value);
}
