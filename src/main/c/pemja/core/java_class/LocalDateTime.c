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

#include "java_class/LocalDateTime.h"

static jmethodID of = 0;
static jmethodID toLocalDate = 0;
static jmethodID toLocalTime = 0;

jobject
JavaLocalDateTime_of(JNIEnv* env, jobject date, jobject time)
{
    if (!of) {
        of = (*env)->GetStaticMethodID(env, JLOCALDATETIME_TYPE, "of", "(Ljava/time/LocalDate;Ljava/time/LocalTime;)Ljava/time/LocalDateTime;");
    }
    return (*env)->CallStaticObjectMethod(env, JLOCALDATETIME_TYPE, of, date, time);
}

jobject
JavaLocalDateTime_toLocalDate(JNIEnv* env, jobject object)
{
    if (!toLocalDate) {
        toLocalDate = (*env)->GetMethodID(env, JLOCALDATETIME_TYPE, "toLocalDate", "()Ljava/time/LocalDate;");
    }
    return (*env)->CallObjectMethod(env, object, toLocalDate);
}

jobject
JavaLocalDateTime_toLocalTime(JNIEnv* env, jobject object)
{
    if (!toLocalTime) {
        toLocalTime = (*env)->GetMethodID(env, JLOCALDATETIME_TYPE, "toLocalTime", "()Ljava/time/LocalTime;");
    }
    return (*env)->CallObjectMethod(env, object, toLocalTime);
}
