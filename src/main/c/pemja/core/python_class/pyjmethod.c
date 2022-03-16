// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "Pemja.h"

#include "java_class/JavaClass.h"

#include "python_class/PythonClass.h"

static int
pyjmethod_init(JNIEnv *env, PyJMethodObject* self)
{
    jobjectArray parameters;
    jint modifier;
    jclass returnType;

    if ((*env)->PushLocalFrame(env, 16) != 0) {
        return -1;
    }

    self->md_id = (*env)->FromReflectedMethod(env, self->md);
    parameters = JavaMethod_getParameterTypes(env, self->md);

    if (!parameters) {
        goto EXIT_ERROR;
    }

    self->md_params = (*env)->NewGlobalRef(env, parameters);
    self->md_params_num = (*env)->GetArrayLength(env, parameters);

    modifier = JavaMethod_getModifiers(env, self->md);
    self->md_is_static = JavaModifier_isStatic(env, modifier);

    returnType = JavaMethod_getReturnType(env, self->md);
    self->md_return_id = JcpJObject_GetObjectId(env, returnType);

    (*env)->PopLocalFrame(env, NULL);
    return 0;

EXIT_ERROR:
    (*env)->PopLocalFrame(env, NULL);
    return -1;
}

static PyObject *
pyjmethod_call(PyJMethodObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *arg, *pyobject;
    PyJObject  *instance;

    JNIEnv *env;
    jvalue *jargs;

    if (kwargs != NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Keywords are not supported in calling Java method.");
    }

    // PyJObject as the first argument.
    if (self->md_params_num != PyTuple_Size(args) - 1) {
        PyErr_Format(PyExc_RuntimeError,
                     "Invalid number of arguments: %i, expected %i for method",
                     (int) PyTuple_GET_SIZE(args) -1 ,
                     self->md_params_num);
        return NULL;
    }

    arg = PyTuple_GetItem(args, 0);
    if (!PyJObject_Check(arg)) {
        PyErr_Format(PyExc_RuntimeError, "The first argument type must be a Java Object Type");
    }

    instance = (PyJObject*) arg;

    env = JcpThread_Get()->env;

    if ((*env)->PushLocalFrame(env, 16 + self->md_params_num) != 0) {
        return NULL;
    }

    jargs = (jvalue *) PyMem_Malloc(sizeof(jvalue) * self->md_params_num);

    // TODO: support variable args.
    for (int i = 0; i < self->md_params_num; i++) {
        arg = PyTuple_GetItem(args, i + 1);
        jclass paramType = (*env)->GetObjectArrayElement(env, self->md_params, i);
        jargs[i] = JcpPyObject_AsJValue(env, arg, paramType);
        (*env)->DeleteLocalRef(env, paramType);
        if (PyErr_Occurred()) {
            goto EXIT_ERROR;
        }
    }

    switch (self->md_return_id) {
        case JBOOLEAN_ID: {
            jboolean object;

            if (self->md_is_static) {
                object = (*env)->CallStaticBooleanMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallBooleanMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyBool_FromLong((long) object);
            break;
        }
        case JBYTE_ID: {
            jbyte object;

            if (self->md_is_static) {
                object = (*env)->CallStaticByteMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallByteMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyInt_FromInt((int) object);
            break;
        }
        case JSHORT_ID: {
            jshort object;

            if (self->md_is_static) {
                object = (*env)->CallStaticShortMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallShortMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyInt_FromInt((int) object);
            break;
        }
        case JINT_ID: {
            jint object;

            if (self->md_is_static) {
                object = (*env)->CallStaticIntMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallIntMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyInt_FromInt(object);
            break;
        }
        case JLONG_ID: {
            jlong object;

            if (self->md_is_static) {
                object = (*env)->CallStaticLongMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallLongMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyInt_FromLong((long) object);
            break;
        }
        case JFLOAT_ID: {
            jfloat object;

            if (self->md_is_static) {
                object = (*env)->CallStaticFloatMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallFloatMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyFloat_FromDouble((double) object);
            break;
        }
        case JDOUBLE_ID: {
            jdouble object;

            if (self->md_is_static) {
                object = (*env)->CallStaticDoubleMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallDoubleMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyFloat_FromDouble((double) object);
            break;
        }
        case JSTRING_ID: {
            jobject object;

            if (self->md_is_static) {
                object = (*env)->CallStaticObjectMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallObjectMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyString_FromJString(env, (jstring) object);
            break;
        }
        case JOBJECT_ID: {
            jobject object;

            if (self->md_is_static) {
                object = (*env)->CallStaticObjectMethodA(env, instance->clazz, self->md_id, jargs);
            } else {
                object = (*env)->CallObjectMethodA(env, instance->object, self->md_id, jargs);
            }

            pyobject = JcpPyObject_FromJObject(env, object);
            break;
        }
        default: {
            PyErr_SetString(PyExc_TypeError, "Unrecognized type.");
            pyobject = NULL;
        }
    }

    PyMem_Free(jargs);
    (*env)->PopLocalFrame(env, NULL);
    return pyobject;

EXIT_ERROR:
    PyMem_Free(jargs);
    (*env)->PopLocalFrame(env, NULL);
    return NULL;
}


static void
pyjmethod_dealloc(PyJMethodObject* self)
{
    JNIEnv *env  = JcpThread_Get()->env;

    if (env) {
        if (self->md_params) {
            (*env)->DeleteGlobalRef(env, self->md_params);
            self->md_params = NULL;
        }
        if (self->md) {
            (*env)->DeleteGlobalRef(env, self->md);
            self->md = NULL;
        }
    }

    Py_CLEAR(self->md_name);

    PyObject_Del(self);
}

PyJMethodObject*
JcpPyJMethod_New(JNIEnv *env, jobject method)
{
    jstring methodName;

    PyJMethodObject *self;

    if (PyType_Ready(&PyJMethod_Type) < 0) {
        return NULL;
    }

    self = PyObject_NEW(PyJMethodObject, &PyJMethod_Type);

    methodName = (jstring) JavaMember_getName(env, method);

    self->md = (*env)->NewGlobalRef(env, method);
    self->md_id = NULL;
    self->md_name = JcpPyString_FromJString(env, methodName);
    self->md_params = NULL;
    self->md_params_num = -1;
    self->md_is_static = -1;
    self->md_return_id = -1;

    (*env)->DeleteLocalRef(env, methodName);

    if (pyjmethod_init(env, self) < 0) {
        Py_DECREF((PyObject*) self);
        self = NULL;
    }

    return self;
}

int
JcpPyJMethodMatch(PyJMethodObject *self, PyObject* args)
{
    PyObject    *arg;
    PyJObject   *instance;

    JNIEnv      *env;
    jclass      paramType;

    if (PyTuple_Size(args) - 1 != self->md_params_num) {
        return 0;
    }

    arg = PyTuple_GetItem(args, 0);

    if (!PyJObject_Check(arg)) {
        PyErr_Format(PyExc_RuntimeError, "The first argument type must be a Java Object Type");
    }

    instance = (PyJObject*) arg;
    env  = JcpThread_Get()->env;

    for (int i = 0; i < self->md_params_num; i++) {
        arg = PyTuple_GetItem(args, i + 1);
        paramType = (jclass) (*env)->GetObjectArrayElement(env, self->md_params, i);
        if (!JcpPyObject_Check(env, arg, paramType)) {
            (*env)->DeleteLocalRef(env, paramType);
            return 0;
        }
        (*env)->DeleteLocalRef(env, paramType);
    }

    return 1;
}

PyTypeObject PyJMethod_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJMethod",                          /* tp_name */
    sizeof(PyJMethodObject),                    /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor) pyjmethod_dealloc,             /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    (ternaryfunc) pyjmethod_call,               /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    "Java Method Object",                       /* tp_doc */
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
