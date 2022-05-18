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
#ifndef _Included_pyjfield
#define _Included_pyjfield

typedef struct {
    PyObject_HEAD

    /* The corresponding Java Field Object */
    jobject fd;

    /* The field name */
    PyObject* fd_name;

    /* The corresponding Field ID for the Java Field object */
    jfieldID fd_id;

    /* The field class Type */
    jclass fd_type;

    /* The field class type id */
    int fd_type_id;

    /* The flag decides whether it is a static field */
    int fd_is_static;

    /* The flag decides whether the field has been initialized */
    int fd_is_initialized;
} PyJFieldObject;

JcpAPI_DATA(PyTypeObject) PyJField_Type;

/* Public interface */

/* Creates a new PyJFieldObject with a Java Field Object. */
JcpAPI_FUNC(PyJFieldObject*) JcpPyJField_New(JNIEnv*, jobject);

/* Gets the filed of the PyJObject. */
JcpAPI_FUNC(PyObject*) JcpPyJField_Get(PyJFieldObject*, PyJObject*);

/* Sets the field value of the PyJObject. */
JcpAPI_FUNC(int) JcpPyJField_Set(PyJFieldObject*, PyJObject*, PyObject*);

#define PyJField_Check(op) \
        PyObject_TypeCheck(op, &PyJField_Type)
#define PyJField_CheckExact(op) Py_IS_TYPE(op, &PyJField_Type)

#endif
