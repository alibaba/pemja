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

#include "java_class/Timestamp.h"

static jmethodID init_Timestamp = 0;
static jmethodID valueOf = 0;
static jmethodID toLocalDateTime = 0;
static jmethodID getYear = 0;
static jmethodID getMonth = 0;
static jmethodID getDate = 0;
static jmethodID getHours = 0;
static jmethodID getMinutes = 0;
static jmethodID getSeconds = 0;
static jmethodID getNanos = 0;

jobject
JavaSqlTimestamp_New(JNIEnv* env, jint year, jint month, jint day, jint hour, jint minute, jint second, jint nano)
{
    if (!init_Timestamp) {
        init_Timestamp = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "<init>", "(IIIIIII)V");
    }
    return (*env)->NewObject(env, JSQLTIMESTAMP_TYPE, init_Timestamp, year, month, day, hour, minute, second, nano);
}

jobject
JavaSqlTimestamp_valueOf(JNIEnv* env, jobject dateTime)
{
    if (!valueOf) {
        valueOf = (*env)->GetStaticMethodID(env, JSQLTIMESTAMP_TYPE, "valueOf", "(Ljava/time/LocalDateTime;)Ljava/sql/Timestamp;");
    }
    return (*env)->CallStaticObjectMethod(env, JSQLTIMESTAMP_TYPE, valueOf, dateTime);
}

jobject
JavaSqlTimestamp_toLocalDateTime(JNIEnv* env, jobject obj)
{
    if (!toLocalDateTime) {
        toLocalDateTime = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "toLocalDateTime", "()Ljava/time/LocalDateTime;");
    }
    return (*env)->CallObjectMethod(env, obj, toLocalDateTime);
}

jint
JavaSqlTimestamp_getYear(JNIEnv* env, jobject obj)
{
    if (!getYear) {
        getYear = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getYear", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getYear);
}

jint
JavaSqlTimestamp_getMonth(JNIEnv* env, jobject obj)
{
    if (!getMonth) {
        getMonth = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getMonth", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getMonth);
}

jint
JavaSqlTimestamp_getDate(JNIEnv* env, jobject obj)
{
    if (!getDate) {
        getDate = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getDate", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getDate);
}

jint
JavaSqlTimestamp_getHours(JNIEnv* env, jobject obj)
{
    if (!getHours) {
        getHours = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getHours", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getHours);
}

jint
JavaSqlTimestamp_getMinutes(JNIEnv* env, jobject obj)
{
    if (!getMinutes) {
        getMinutes = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getMinutes", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getMinutes);
}

jint
JavaSqlTimestamp_getSeconds(JNIEnv* env, jobject obj)
{
    if (!getSeconds) {
        getSeconds = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getSeconds", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getSeconds);
}

jint
JavaSqlTimestamp_getNanos(JNIEnv* env, jobject obj)
{
    if (!getNanos) {
        getNanos = (*env)->GetMethodID(env, JSQLTIMESTAMP_TYPE, "getNanos", "()I");
    }
    return (*env)->CallIntMethod(env, obj, getNanos);
}
