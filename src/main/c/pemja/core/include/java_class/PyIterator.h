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

#ifndef _Included_pemja_core_object_PyIterator
#define _Included_pemja_core_object_PyIterator

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     pemja_core_object_PyIterator
 * Method:    next
 * Signature: (JJ)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_object_PyIterator_next(JNIEnv *,
                                                                 jobject, jlong,
                                                                 jlong);

jobject JavaPyIterator_New(JNIEnv *, jlong, jlong);

#ifdef __cplusplus
}
#endif
#endif
