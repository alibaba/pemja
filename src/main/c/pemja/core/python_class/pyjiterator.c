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


/* Creates a new PyJObject with a Java Iterator Object. */

PyObject*
JcpPyJIterator_New(JNIEnv *env, jobject iterator, jclass clazz)
{
    PyObject *self;

    self = JcpPyJObject_New(env, &PyJIterator_Type, iterator, clazz);

    return self;
}

static PyObject*
pyjiterator_next(PyObject* self)
{
    JNIEnv* env;
    jobject object, item;

    PyObject* pyobject = NULL;

    env = JcpThreadEnv_Get();

    object = ((PyJObject*) self)->object;

    if (JavaIterator_hasNext(env, object)) {
        item = JavaIterator_next(env, object);

        pyobject = JcpPyObject_FromJObject(env, item);

        (*env)->DeleteLocalRef(env, item);
    }

    return pyobject;
}


PyTypeObject PyJIterator_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJIterator",                        /* tp_name */
    sizeof(PyJObject),                          /* tp_basicsize */
    0,                                          /* tp_itemsize */
    0,                                          /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    "Java Iterator Object",                     /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    PyObject_SelfIter,                          /* tp_iter */
    (iternextfunc) pyjiterator_next,            /* tp_iternext */
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
