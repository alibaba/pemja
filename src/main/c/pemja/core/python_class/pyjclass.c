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
pyjclass_init(JNIEnv *env, PyObject *self)
{
    ((PyJClassObject *) self)->constructor = NULL;
    return 0;
}

static int
pyjclass_init_constructors(PyJClassObject *self)
{
    int len;
    JNIEnv *env;
    jobjectArray constructors;
    jobject constructor;

    PyObject *pyjconstructor, *callable;
    PyJMultiMethodObject *methods;

    env = JcpThreadEnv_Get();

    if ((*env)->PushLocalFrame(env, 16) != 0) {
        return -1;
    }

    constructors = JavaClass_getConstructors(env, self->clazz);
    len = (*env)->GetArrayLength(env, constructors);

    for (int i = 0; i < len; i++) {
        constructor = (*env)->GetObjectArrayElement(env, constructors, i);
        pyjconstructor =  JcpPyJConstructor_New(env, constructor);
        if (!constructor) {
            (*env)->DeleteLocalRef(env, constructors);
            goto EXIT_ERROR;
        }

        (*env)->DeleteLocalRef(env, constructor);

        if (i == 0) {
            callable = pyjconstructor;
        } else {
            if (i == 1) {
                methods = JcpPyJMultiMethod_New();
                JcpPyJMultiMethod_Append(methods, (PyJMethodObject*) callable);
            } else {
                methods = (PyJMultiMethodObject*) callable;
            }
            JcpPyJMultiMethod_Append(methods, (PyJMethodObject*) pyjconstructor);
            callable = (PyObject*) methods;

            Py_DECREF(pyjconstructor);
        }
    }

    (*env)->DeleteLocalRef(env, constructors);

    if (callable) {
        self->constructor = callable;
    }

    (*env)->PopLocalFrame(env, NULL);
    return 0;

EXIT_ERROR:
    (*env)->PopLocalFrame(env, NULL);
    return -1;
}

static PyObject *
pyjclass_call(PyJClassObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject    *constructor, *result;

    if (self->constructor == NULL) {
        // initialize constructors
        if (pyjclass_init_constructors(self) < 0) {
            return NULL;
        }

        if (self->constructor == NULL) {
            PyErr_SetString(PyExc_TypeError, "There is no public constructors in the class");
            return NULL;
        }
    }

    constructor = PyMethod_New(self->constructor, (PyObject*) self);
    result = PyObject_Call(constructor, args, kwargs);
    Py_DECREF(constructor);

    return result;
}

static void
pyjclass_dealloc(PyJClassObject* self)
{
    Py_CLEAR(self->constructor);
    PyJClass_Type.tp_base->tp_dealloc((PyObject*) self);
}


/* Creates a new PyJClassObject with a Java Class Object. */

PyObject*
JcpPyJClass_New(JNIEnv *env, jclass clazz)
{
    PyObject *self;

    self = JcpPyJObject_New(env, &PyJClass_Type, NULL, clazz);
    if (self) {
        if (pyjclass_init(env, self) < 0) {
            Py_DECREF(self);
            self = NULL;
        }
    }

    return self;
}

PyTypeObject PyJClass_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJClass",                           /* tp_name */
    sizeof(PyJClassObject),                     /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor) pyjclass_dealloc,              /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    (ternaryfunc) pyjclass_call,                /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    "Java Class Object",                        /* tp_doc */
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
