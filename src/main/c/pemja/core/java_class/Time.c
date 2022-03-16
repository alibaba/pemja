// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "Pemja.h"

#include "java_class/Time.h"

static jmethodID init_time = 0;
static jmethodID getTime = 0;

jobject
JavaSqlTime_New(JNIEnv* env, jlong jval)
{
    if (!init_time) {
        init_time = (*env)->GetMethodID(env, JSQLTIME_TYPE, "<init>", "(J)V");
    }
    return (*env)->NewObject(env, JSQLTIME_TYPE, init_time, jval);
}

jlong
JavaSqlTime_getTime(JNIEnv* env, jobject obj)
{
    if (!getTime) {
        getTime = (*env)->GetMethodID(env, JSQLTIME_TYPE, "getTime", "()J");
    }
    return (*env)->CallLongMethod(env, obj, getTime);
}
