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
#ifndef _Included_java_lang_Number
#define _Included_java_lang_Number

#include <jni.h>

jbyte JavaNumber_byteValue(JNIEnv*, jobject);
jshort JavaNumber_shortValue(JNIEnv*, jobject);
jint JavaNumber_intValue(JNIEnv*, jobject);
jlong JavaNumber_longValue(JNIEnv*, jobject);
jfloat JavaNumber_floatValue(JNIEnv*, jobject);
jdouble JavaNumber_doubleValue(JNIEnv*, jobject);

#endif
