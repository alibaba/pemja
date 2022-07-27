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

#include "java_class/BigInteger.h"

static jmethodID init_BigInteger = 0;
static jmethodID toString = 0;

jobject
JavaBigInteger_New(JNIEnv* env, jstring value)
{

    if (!init_BigInteger) {
        init_BigInteger = (*env)->GetMethodID(env, JBIGINTEGER_TYPE, "<init>",
                                             "(Ljava/lang/String;)V");
    }

    return (*env)->NewObject(env, JBIGINTEGER_TYPE, init_BigInteger, value);
}

jstring
JavaBigInteger_toString(JNIEnv* env, jobject obj)
{

    if (!toString) {
        toString = (*env)->GetMethodID(env, JBIGINTEGER_TYPE, "toString", "()Ljava/lang/String;");
    }

    return (*env)->CallObjectMethod(env, obj, toString);
}
