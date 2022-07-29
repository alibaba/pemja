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


/* Creates a new PyJObject with a Java Collection Object. */

PyObject*
JcpPyJCollection_New(JNIEnv *env, jobject collection, jclass clazz)
{
    PyObject *self;

    self = JcpPyJObject_New(env, &PyJCollection_Type, collection, clazz);

    return self;
}

static Py_ssize_t
pyjcollection_len(PyObject *self)
{
    JNIEnv* env;

    env = JcpThreadEnv_Get();

    return JavaCollection_size(env, ((PyJObject*) self)->object);
}

static int
pyjcollection_contains(PyObject* self, PyObject* o)
{
    JNIEnv* env;
    jobject value;

    env = JcpThreadEnv_Get();

    value = JcpPyObject_AsJObject(env, o, JOBJECT_TYPE);

    return JavaCollection_contains(env, ((PyJObject*) self)->object, value);
}

static PySequenceMethods pyjcollection_seq_methods = {
    pyjcollection_len,      /* sq_length */
    0,                      /* sq_concat */
    0,                      /* sq_repeat */
    0,                      /* sq_item */
    0,                      /* sq_slice */
    0,                      /* sq_ass_item */
    0,                      /* sq_ass_slice */
    pyjcollection_contains, /* sq_contains */
    0,                      /* sq_inplace_concat */
    0,                      /* sq_inplace_repeat */
};

PyTypeObject PyJCollection_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJCollection",                      /* tp_name */
    sizeof(PyJObject),                          /* tp_basicsize */
    0,                                          /* tp_itemsize */
    0,                                          /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    &pyjcollection_seq_methods,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Java Collection Object",                   /* tp_doc */
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
