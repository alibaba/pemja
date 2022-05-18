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

#include "java_class/Throwable.h"

static jmethodID getStackTrace = 0;
static jmethodID setStackTrace = 0;

jobjectArray
JavaThrowable_getStackTrace(JNIEnv* env, jobject this)
{
    if (!getStackTrace) {
        getStackTrace = (*env)->GetMethodID(env, JTHROWABLE_TYPE, "getStackTrace",
                                            "()[Ljava/lang/StackTraceElement;");
    }
    return (jobjectArray) (*env)->CallObjectMethod(env, this, getStackTrace);
}

void
JavaThrowable_setStackTrace(JNIEnv* env, jobject this, jobjectArray jval)
{
    if (!setStackTrace) {
        setStackTrace = (*env)->GetMethodID(env, JTHROWABLE_TYPE, "setStackTrace",
                                            "([Ljava/lang/StackTraceElement;)V");
    }
    (*env)->CallVoidMethod(env, this, setStackTrace, jval);
}

