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

#include "Pemja.h"

#include "java_class/JavaClass.h"

#include "python_class/PythonClass.h"

static int
multi_method_init(PyJMultiMethodObject* self)
{
    self->methods = PyList_New(0);

    if (!self->methods) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to create the attribute `methods` of PyJMultiMethodObject");
        return -1;
    }

    return 0;
}

static PyObject *
multi_method_call(PyJMultiMethodObject *self, PyObject *args, PyObject *kwargs)
{
    Py_ssize_t      method_num;

    PyJMethodObject *method;
    PyObject        *matched_method = NULL;

    if (kwargs != NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Keywords are not supported in calling Java method.");
        return NULL;
    }

    method_num = PyList_Size(self->methods);

    if (self->methods == NULL || method_num == 0) {
        PyErr_SetString(PyExc_RuntimeError, "There are no corresponding Java Methods.");
        return NULL;
    }

    int max_match_degree = 0;
    for (int i = 0; i < method_num; i++) {
        method = (PyJMethodObject*) PyList_GetItem(self->methods, i);
        int match_degree = JcpPyJMethodMatch(method, args);

        if (match_degree > max_match_degree) {
            matched_method = (PyObject*) method;
            max_match_degree = match_degree;
        }
    }

    if (matched_method) {
        return PyObject_Call(matched_method, args, kwargs);
    } else {
        PyErr_SetString(PyExc_RuntimeError, "There are no matched Java Methods.");
        return NULL;
    }
}

static void
multi_method_dealloc(PyJMultiMethodObject *self)
{
    if (self->methods) {
        Py_CLEAR(self->methods);
    }

    PyObject_Del(self);
}

PyJMultiMethodObject*
JcpPyJMultiMethod_New()
{
    PyJMultiMethodObject*   self;

    if (PyType_Ready(&PyJMultiMethod_Type) < 0) {
        return NULL;
    }

    self = PyObject_NEW(PyJMultiMethodObject, &PyJMultiMethod_Type);

    if (multi_method_init(self) < 0) {
        Py_DECREF((PyObject*) self);
        self = NULL;
    }

    return self;
}

int
JcpPyJMultiMethod_Append(PyJMultiMethodObject* self, PyJMethodObject* method)
{
    return PyList_Append(self->methods, (PyObject*) method);
}


PyTypeObject PyJMultiMethod_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJMultiMethod",                     /* tp_name */
    sizeof(PyJMultiMethodObject),               /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor) multi_method_dealloc,          /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    (ternaryfunc) multi_method_call,            /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    "Java Multi Methods",                       /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    0,                                          /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    0,                                          /* tp_alloc */
    0,                                          /* tp_new */
};
