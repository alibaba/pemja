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

#include <java_class/StackTraceElement.h>
#include <pyutils.h>

static jmethodID init_StackTraceElement = 0;

jobject
JavaStackTraceElement_New(JNIEnv* env, jstring declaringClass, jstring methodName, jstring fileName, jint lineNumber)
{
    if (!init_StackTraceElement) {
        init_StackTraceElement = (*env)->GetMethodID(
            env, JSTACK_TRACE_ELEMENT_TYPE, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    }
    return (*env)->NewObject(
        env,
        JSTACK_TRACE_ELEMENT_TYPE,
        init_StackTraceElement,
        declaringClass,
        methodName,
        fileName,
        lineNumber);
}
