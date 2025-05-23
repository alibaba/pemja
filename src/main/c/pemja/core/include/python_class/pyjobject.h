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
#ifndef _Included_pyjobject
#define _Included_pyjobject

#include <jni.h>

#define PyJOjbect_HEAD        \
  PyObject_HEAD jclass clazz; \
  jobject object;             \
  PyObject* attr;             \
  PyObject* class_name;

typedef struct {
  PyJOjbect_HEAD
} PyJObject;

JcpAPI_DATA(PyTypeObject) PyJObject_Type;

/* Public interface */

/* Creates a new PyJMethodObject with a Java Object and Java class Object. */
JcpAPI_FUNC(PyObject*)
    JcpPyJObject_New(JNIEnv*, PyTypeObject*, jobject, jclass);

#define PyJObject_Check(op) PyObject_TypeCheck(op, &PyJObject_Type)
#define PyJObject_CheckExact(op) op->ob_type == &PyJObject_Type

#endif
