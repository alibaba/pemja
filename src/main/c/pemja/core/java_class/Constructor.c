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

#include "java_class/Constructor.h"

static jmethodID getParameterTypes = 0;

jobjectArray
JavaConstructor_getParameterTypes(JNIEnv* env, jobject this)
{
    if (!getParameterTypes) {
        getParameterTypes = (*env)->GetMethodID(env, JCONSTRUCTOR_TYPE, "getParameterTypes",
                                                "()[Ljava/lang/Class;");
    }
    return (*env)->CallObjectMethod(env, this, getParameterTypes);
}
