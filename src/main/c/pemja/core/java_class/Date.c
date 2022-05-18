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

#include "java_class/Date.h"

static jmethodID init_Date = 0;
static jmethodID getYear = 0;
static jmethodID getMonth = 0;
static jmethodID getDate = 0;

jobject
JavaSqlDate_New(JNIEnv* env, jint year, jint month, jint day)
{
    if (!init_Date) {
        init_Date = (*env)->GetMethodID(env, JSQLDATE_TYPE, "<init>", "(III)V");
    }
    return (*env)->NewObject(env, JSQLDATE_TYPE, init_Date, year, month, day);
}

jint
JavaSqlDate_getYear(JNIEnv* env, jobject obj)
{
    if (!getYear) {
        getYear = (*env)->GetMethodID(env, JSQLDATE_TYPE, "getYear", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getYear);
}

jint
JavaSqlDate_getMonth(JNIEnv* env, jobject obj)
{
    if (!getMonth) {
        getMonth = (*env)->GetMethodID(env, JSQLDATE_TYPE, "getMonth", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getMonth);
}

jint
JavaSqlDate_getDate(JNIEnv* env, jobject obj)
{
    if (!getDate) {
        getDate = (*env)->GetMethodID(env, JSQLDATE_TYPE, "getDate", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getDate);
}
