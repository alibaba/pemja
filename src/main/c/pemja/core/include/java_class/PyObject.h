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

#ifndef _Included_pemja_core_object_PyObject
#define _Included_pemja_core_object_PyObject

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     pemja_core_object_PyObject
 * Method:    decRef
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_pemja_core_object_PyObject_decRef(JNIEnv *, jobject,
                                                              jlong, jlong);

/*
 * Class:     pemja_core_object_PyObject
 * Method:    getAttr
 * Signature: (JJLjava/lang/String;Ljava/lang/Class;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_getAttr(
    JNIEnv *, jobject, jlong, jlong, jstring, jclass);

/*
 * Class:     pemja_core_object_PyObject
 * Method:    setAttr
 * Signature: (JJLjava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_pemja_core_object_PyObject_setAttr(JNIEnv *,
                                                               jobject, jlong,
                                                               jlong, jstring,
                                                               jobject);

/*
 * Class:     pemja_core_object_PyObject
 * Method:    invokeMethodNoArgs
 * Signature: (JJLjava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_invokeMethodNoArgs(
    JNIEnv *, jobject, jlong, jlong, jstring);

/*
 * Class:     pemja_core_object_PyObject
 * Method:    invokeMethodOneArg
 * Signature: (JJLjava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_invokeMethodOneArg(
    JNIEnv *, jobject, jlong, jlong, jstring, jobject);

/*
 * Class:     pemja_core_object_PyObject
 * Method:    invokeMethod
 * Signature: (JJLjava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_invokeMethod(
    JNIEnv *, jobject, jlong, jlong, jstring, jobjectArray);

jobject JavaPyObject_New(JNIEnv *, jlong, jlong);

jlong JavaPyObject_GetPyobject(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
