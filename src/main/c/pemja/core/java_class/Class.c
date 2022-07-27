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

#include "java_class/Class.h"

static jmethodID getName = 0;
static jmethodID getConstructors = 0;
static jmethodID getMethods = 0;
static jmethodID getFields = 0;
static jmethodID getComponentType = 0;
static jmethodID isArray = 0;

jstring
JavaClass_getName(JNIEnv* env, jobject this)
{
    if (!getName) {
        getName = (*env)->GetMethodID(env, JCLASS_TYPE, "getName", "()Ljava/lang/String;");
    }
    return (*env)->CallObjectMethod(env, this, getName);
}

jobjectArray
JavaClass_getConstructors(JNIEnv* env, jobject this)
{
    if (!getConstructors) {
        getConstructors = (*env)->GetMethodID(env, JCLASS_TYPE, "getConstructors",
                                              "()[Ljava/lang/reflect/Constructor;");
    }
    return (*env)->CallObjectMethod(env, this, getConstructors);
}

jobjectArray
JavaClass_getMethods(JNIEnv* env, jobject this)
{
    if (!getMethods) {
        getMethods = (*env)->GetMethodID(env, JCLASS_TYPE, "getMethods",
                                        "()[Ljava/lang/reflect/Method;");
    }
    return (*env)->CallObjectMethod(env, this, getMethods);
}

jobjectArray
JavaClass_getFields(JNIEnv* env, jobject this)
{
    if (!getFields) {
        getFields = (*env)->GetMethodID(env, JCLASS_TYPE, "getFields",
                                        "()[Ljava/lang/reflect/Field;");
    }
    return (*env)->CallObjectMethod(env, this, getFields);
}

jclass
JavaClass_getComponentType(JNIEnv* env, jobject this)
{
    if (!getComponentType) {
        getComponentType = (*env)->GetMethodID(env, JCLASS_TYPE, "getComponentType",
                                               "()Ljava/lang/Class;");
    }
    return (*env)->CallObjectMethod(env, this, getComponentType);
}

jboolean
JavaClass_isArray(JNIEnv* env, jobject this)
{
    if (!isArray) {
        isArray = (*env)->GetMethodID(env, JCLASS_TYPE, "isArray", "()Z");
    }

    return (*env)->CallBooleanMethod(env, this, isArray);
}
