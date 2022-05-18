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

#include "java_class/LocalTime.h"

static jmethodID ofNanoOfDay = 0;
static jmethodID toNanoOfDay = 0;

jobject
JavaLocalTime_ofNanoOfDay(JNIEnv* env, jlong nanoOfDay)
{
    if (!ofNanoOfDay) {
        ofNanoOfDay = (*env)->GetStaticMethodID(env, JLOCALTIME_TYPE, "ofNanoOfDay", "(J)Ljava/time/LocalTime;");
    }
    return (*env)->CallStaticObjectMethod(env, JLOCALTIME_TYPE, ofNanoOfDay, nanoOfDay);
}

jlong
JavaLocalTime_toNanoOfDay(JNIEnv* env, jobject object)
{
    if (!toNanoOfDay) {
        toNanoOfDay = (*env)->GetMethodID(env, JLOCALTIME_TYPE, "toNanoOfDay", "()J");
    }
    return (*env)->CallLongMethod(env, object, toNanoOfDay);
}
