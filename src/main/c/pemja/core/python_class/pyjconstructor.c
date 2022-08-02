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

#include "structmember.h"

static int
pyjconstructor_init(JNIEnv* env, PyJMethodObject* self)
{
    jobjectArray parameters;

    if ((*env)->PushLocalFrame(env, 16) != 0) {
        return -1;
    }

    self->md_id = (*env)->FromReflectedMethod(env, self->md);
    parameters = JavaConstructor_getParameterTypes(env, self->md);

    if (!parameters) {
        goto EXIT_ERROR;
    }

    self->md_params = (*env)->NewGlobalRef(env, parameters);
    self->md_params_num = (*env)->GetArrayLength(env, parameters);
    self->md_return_id = JOBJECT_ID;
    self->md_is_static = 1;

    (*env)->PopLocalFrame(env, NULL);
    return 0;

EXIT_ERROR:
    (*env)->PopLocalFrame(env, NULL);
    return -1;
}


static PyObject *
pyjconstructor_call(PyJMethodObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject            *arg, *pyobject;
    PyJClassObject      *clazz;

    JNIEnv              *env;
    jvalue              *jargs;
    jobject             object;

    if (kwargs != NULL) {
        PyErr_Format(PyExc_RuntimeError, "Keywords are not supported in constructing Object.");
    }

    // `JClass` Object as the first argument.
    if (self->md_params_num != PyTuple_GET_SIZE(args) - 1) {
        PyErr_Format(PyExc_RuntimeError,
                     "Invalid number of arguments: %i, expected %i for constructor",
                     (int) PyTuple_GET_SIZE(args) -1 ,
                     self->md_params_num);
        return NULL;
    }

    arg = PyTuple_GetItem(args, 0);
    if (!PyJClass_Check(arg)) {
        PyErr_Format(PyExc_RuntimeError, "The first argument type must be a Java Class Type");
    }

    clazz = (PyJClassObject*) arg;

    env = JcpThreadEnv_Get();
    if ((*env)->PushLocalFrame(env, 16 + self->md_params_num) != 0) {
        return NULL;
    }

    jargs = (jvalue *) PyMem_Malloc(sizeof(jvalue) * self->md_params_num);

    for (int i = 0; i < self->md_params_num; i++) {
        arg = PyTuple_GetItem(args, i + 1);
        jclass paramType = (jclass) (*env)->GetObjectArrayElement(env, self->md_params, i);
        if (JcpJavaErr_Throw(env)) {
            goto EXIT_ERROR;
        }
        jargs[i] = JcpPyObject_AsJValue(env, arg, paramType);
        (*env)->DeleteLocalRef(env, paramType);
        if (JcpJavaErr_Throw(env) || PyErr_Occurred()) {
            goto EXIT_ERROR;
        }
    }

    object = (*env)->NewObjectA(env, clazz->clazz, self->md_id, jargs);
    if (!object || JcpJavaErr_Throw(env)) {
        // failed to construct the object.
        goto EXIT_ERROR;
    }

    pyobject = JcpPyObject_FromJObject(env, object);
    if (!pyobject) {
        goto EXIT_ERROR;
    }

    PyMem_Free(jargs);
    (*env)->PopLocalFrame(env, NULL);
    return pyobject;

EXIT_ERROR:
    PyMem_Free(jargs);
    (*env)->PopLocalFrame(env, NULL);
    return NULL;
}


/* Creates a new PyJConstructor with a Java Object. */

PyObject*
JcpPyJConstructor_New(JNIEnv *env, jobject constructor)
{
    PyJMethodObject    *self;

    if (PyType_Ready(&PyJMethod_Type) < 0) {
        return NULL;
    }

    if (!PyJConstructor_Type.tp_base) {
        PyJConstructor_Type.tp_base = &PyJMethod_Type;
        if (PyType_Ready(&PyJConstructor_Type) < 0) {
            return NULL;
        }
    }

    self = PyObject_NEW(PyJMethodObject, &PyJConstructor_Type);
    self->md = (*env)->NewGlobalRef(env, constructor);
    self->md_name = PyUnicode_FromString("<init>");

    if (pyjconstructor_init(env, self) < 0 || JcpJavaErr_Throw(env)) {
        Py_DECREF(self);
        return NULL;
    }

    return (PyObject*) self;
}

PyTypeObject PyJConstructor_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJConstructor",                     /* tp_name */
    sizeof(PyJMethodObject),                    /* tp_basicsize */
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
    (ternaryfunc) pyjconstructor_call,          /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    "Java Constructor Object s",                /* tp_doc */
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
