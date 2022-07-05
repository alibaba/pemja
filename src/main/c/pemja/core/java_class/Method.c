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

#include "java_class/Method.h"

static jmethodID getParameterTypes = 0;
static jmethodID getModifiers = 0;
static jmethodID getReturnType = 0;
static jmethodID isVarArgs = 0;

jobjectArray
JavaMethod_getParameterTypes(JNIEnv* env, jobject this)
{
    if (!getParameterTypes) {
        getParameterTypes = (*env)->GetMethodID(env, JMETHOD_TYPE, "getParameterTypes",
                                                "()[Ljava/lang/Class;");
    }
    return (*env)->CallObjectMethod(env, this, getParameterTypes);
}

jint
JavaMethod_getModifiers(JNIEnv* env, jobject this)
{
    if (!getModifiers) {
        getModifiers = (*env)->GetMethodID(env, JMETHOD_TYPE, "getModifiers",
                                           "()I");
    }
    return (*env)->CallIntMethod(env, this, getModifiers);
}

jclass
JavaMethod_getReturnType(JNIEnv* env, jobject this)
{
    if (!getReturnType) {
        getReturnType = (*env)->GetMethodID(env, JMETHOD_TYPE, "getReturnType",
                                            "()Ljava/lang/Class;");
    }
    return (*env)->CallObjectMethod(env, this, getReturnType);
}

jboolean
JavaMethod_isVarArgs(JNIEnv* env, jobject this)
{

    if (!isVarArgs) {
        isVarArgs = (*env)->GetMethodID(env, JMETHOD_TYPE, "isVarArgs", "()Z");
    }

    return (*env)->CallBooleanMethod(env, this, isVarArgs);
}
