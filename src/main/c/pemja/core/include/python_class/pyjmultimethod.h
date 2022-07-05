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
#ifndef _Included_pyjmultimethod
#define _Included_pyjmultimethod

typedef struct {
    PyObject_HEAD

    /* A list stores the underling PyJMethodObjects.*/
    PyObject*   methods;
} PyJMultiMethodObject;

JcpAPI_DATA(PyTypeObject) PyJMultiMethod_Type;

/* Public interface */

/* Creates a new empty PyJMultiMethodObject. */
JcpAPI_FUNC(PyJMultiMethodObject*) JcpPyJMultiMethod_New(void);

/* Appends a PyJMethodObject to the PyJMultiMethodObject. */
JcpAPI_FUNC(int) JcpPyJMultiMethod_Append(PyJMultiMethodObject*, PyJMethodObject*);

#define PyJMultiMethod_Check(op) \
        PyObject_TypeCheck(op, &PyJMultiMethod_Type)
#define PyJMultiMethod_CheckExact(op) Py_IS_TYPE(op, &PyJMultiMethod_Type)

#endif
