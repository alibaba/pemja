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

#include "java_class/Number.h"

static jmethodID byteValue = 0;
static jmethodID shortValue = 0;
static jmethodID intValue = 0;
static jmethodID longValue = 0;
static jmethodID floatValue = 0;
static jmethodID doubleValue = 0;

jbyte
JavaNumber_byteValue(JNIEnv* env, jobject jval)
{
    if (!byteValue) {
        byteValue = (*env)->GetMethodID(env, JNUMBER_TYPE, "byteValue", "()B");
    }
    return (*env)->CallByteMethod(env, jval, byteValue);;
}

jshort
JavaNumber_shortValue(JNIEnv* env, jobject jval)
{
    if (!shortValue) {
        shortValue = (*env)->GetMethodID(env, JNUMBER_TYPE, "shortValue", "()S");
    }
    return (*env)->CallShortMethod(env, jval, shortValue);
}

jint
JavaNumber_intValue(JNIEnv* env, jobject jval)
{
    if (!intValue) {
        intValue = (*env)->GetMethodID(env, JNUMBER_TYPE, "intValue", "()I");
    }
    return (*env)->CallIntMethod(env, jval, intValue);
}

jlong
JavaNumber_longValue(JNIEnv* env, jobject jval)
{
    if (!longValue) {
        longValue = (*env)->GetMethodID(env, JNUMBER_TYPE, "longValue", "()J");
    }
    return (*env)->CallLongMethod(env, jval, longValue);
}

jfloat
JavaNumber_floatValue(JNIEnv* env, jobject jval)
{
    if (!floatValue) {
        floatValue = (*env)->GetMethodID(env, JNUMBER_TYPE, "floatValue", "()F");
    }
    return (*env)->CallFloatMethod(env, jval, floatValue);
}

jdouble
JavaNumber_doubleValue(JNIEnv* env, jobject jval)
{
    if (!doubleValue) {
        doubleValue = (*env)->GetMethodID(env, JNUMBER_TYPE, "doubleValue", "()D");
    }
    return (*env)->CallDoubleMethod(env, jval, doubleValue);
}
