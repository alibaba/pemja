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
#ifndef _Included_pyjmethod
#define _Included_pyjmethod

typedef struct {
    PyObject_HEAD

    /* The corresponding Java Method object */
    jobject md;

    /* The corresponding Method ID for the Java Method object */
    jmethodID md_id;

    /* The method name */
    PyObject *md_name;

    /* The param array class types of the method */
    jobjectArray md_params;

    /* The num of the method params */
    int md_params_num;

    /* The return type id of the method */
    int md_return_id;

    /* The flag decides whether it is a static method */
    int md_is_static;
} PyJMethodObject;

JcpAPI_DATA(PyTypeObject) PyJMethod_Type;

/* Public interface */

/* Creates a new PyJMethodObject with a Java Method Object. */
JcpAPI_FUNC(PyJMethodObject*) JcpPyJMethod_New(JNIEnv*, jobject);

/* Returns whether the input arguments can match the params of the method. */
JcpAPI_FUNC(int) JcpPyJMethodMatch(PyJMethodObject*, PyObject*);

#define PyJMethod_Check(op) \
        PyObject_TypeCheck(op, &PyJMethod_Type)
#define PyJMethod_CheckExact(op) Py_IS_TYPE(op, &PyJMethod_Type)

#endif
