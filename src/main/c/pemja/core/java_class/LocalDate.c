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

#include "java_class/LocalDate.h"

static jmethodID ofEpochDay = 0;
static jmethodID toEpochDay = 0;

jobject
JavaLocalDate_ofEpochDay(JNIEnv* env, jlong epochDay)
{
    if (!ofEpochDay) {
        ofEpochDay = (*env)->GetStaticMethodID(env, JLOCALDATE_TYPE, "ofEpochDay", "(J)Ljava/time/LocalDate;");
    }
    return (*env)->CallStaticObjectMethod(env, JLOCALDATE_TYPE, ofEpochDay, epochDay);
}

jlong
JavaLocalDate_toEpochDay(JNIEnv* env, jobject object)
{
    if (!toEpochDay) {
        toEpochDay = (*env)->GetMethodID(env, JLOCALDATE_TYPE, "toEpochDay", "()J");
    }
    return (*env)->CallLongMethod(env, object, toEpochDay);
}
