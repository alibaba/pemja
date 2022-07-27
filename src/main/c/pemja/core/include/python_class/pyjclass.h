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
#ifndef _Included_pyjclass
#define _Included_pyjclass

typedef struct {
    PyJOjbect_HEAD

    /* The PyJConstructor Object used to create Java Object */
    PyObject    *constructor;
} PyJClassObject;

JcpAPI_DATA(PyTypeObject) PyJClass_Type;

/* Public interface */

/* Creates a new PyJClassObject with a Java Class Object. */
JcpAPI_FUNC(PyObject*) JcpPyJClass_New(JNIEnv*, jclass);

#define PyJClass_Check(op) \
        PyObject_TypeCheck(op, &PyJClass_Type)
#define PyJClass_CheckExact(op) op->ob_type == &PyJClass_Type

#endif
