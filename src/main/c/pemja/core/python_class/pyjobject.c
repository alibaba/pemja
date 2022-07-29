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
pyjobject_init(JNIEnv *env, PyJObject *self)
{

    int                 len;
    JcpThread           *jcpThread;

    jstring             className;
    jobjectArray        methods, fields;
    jobject             method, field;

    PyObject    *cachedAttrs, *cachedMethod;
    PyJMultiMethodObject *multi_methods;

    if ((*env)->PushLocalFrame(env, 16) != 0) {
        return -1;
    }

    className = JavaClass_getName(env, self->clazz);
    if (!className) {
        goto EXIT_ERROR;
    }

    self->class_name = JcpPyString_FromJString(env, className);
    jcpThread = JcpThread_Get();

    if (jcpThread->name_to_attrs == NULL) {
        jcpThread->name_to_attrs = PyDict_New();
    }

    cachedAttrs = PyDict_GetItem(jcpThread->name_to_attrs, self->class_name);

    if (cachedAttrs == NULL) {
        cachedAttrs = PyDict_New();

        // cache methods
        methods = JavaClass_getMethods(env, self->clazz);
        len = (*env)->GetArrayLength(env, methods);

        for (int i = 0; i < len; i++) {
            method = (*env)->GetObjectArrayElement(env, methods, i);
            PyJMethodObject* pyjmethod =  JcpPyJMethod_New(env, method);
            (*env)->DeleteLocalRef(env, method);

            if (!pyjmethod) {
                continue;
            }

            if (pyjmethod->md_name) {
                cachedMethod = PyDict_GetItem(cachedAttrs, pyjmethod->md_name);
                if (cachedMethod == NULL) {
                    if (PyDict_SetItem(cachedAttrs, pyjmethod->md_name,
                                      (PyObject*) pyjmethod) < 0) {
                        Py_DECREF(pyjmethod);
                        goto EXIT_ERROR;
                    }
                } else {
                    if (PyJMethod_Check(cachedMethod)) {
                        multi_methods = JcpPyJMultiMethod_New();
                        if (JcpPyJMultiMethod_Append(multi_methods,
                                                    (PyJMethodObject*) cachedMethod) < 0) {
                            Py_DECREF(pyjmethod);
                            Py_DECREF(multi_methods);
                            goto EXIT_ERROR;
                        }

                        if (PyDict_SetItem(cachedAttrs, pyjmethod->md_name,
                                           (PyObject*) multi_methods) < 0) {
                            Py_DECREF(pyjmethod);
                            Py_DECREF(multi_methods);
                            goto EXIT_ERROR;
                        }
                    } else {
                        multi_methods = (PyJMultiMethodObject*) cachedMethod;
                    }

                    if (JcpPyJMultiMethod_Append(multi_methods, pyjmethod) < 0) {
                        Py_DECREF(pyjmethod);
                        Py_DECREF(multi_methods);
                        goto EXIT_ERROR;
                    }
                }
            }

            Py_DECREF(pyjmethod);
        }
        (*env)->DeleteLocalRef(env, methods);

        // cache fields
        fields = JavaClass_getFields(env, self->clazz);
        len = (*env)->GetArrayLength(env, fields);

        for (int i = 0; i < len; i++) {
            field = (*env)->GetObjectArrayElement(env, fields, i);
            PyJFieldObject* pyjfield = JcpPyJField_New(env, field);
            (*env)->DeleteLocalRef(env, field);

            if (!pyjfield) {
                continue;
            }

            if (pyjfield->fd_name) {
                if (PyDict_SetItem(cachedAttrs, pyjfield->fd_name, (PyObject*) pyjfield) < 0) {
                    Py_DECREF(pyjfield);
                    goto EXIT_ERROR;
                }
            }
            Py_DECREF(pyjfield);
        }
        (*env)->DeleteLocalRef(env, fields);

        PyDict_SetItem(jcpThread->name_to_attrs, self->class_name, cachedAttrs);
        Py_DECREF(cachedAttrs);
    }

    if (self->object) {
        Py_INCREF(cachedAttrs);
        self->attr = cachedAttrs;
    } else {
        self->attr = PyDict_Copy(cachedAttrs);
    }

    (*env)->PopLocalFrame(env, NULL);
    return 0;

EXIT_ERROR:
    (*env)->PopLocalFrame(env, NULL);
    return -1;
}

static void
pyjobject_dealloc(PyJObject* self)
{
    JNIEnv *env;

    env = JcpThreadEnv_Get();

    if (self->clazz) {
        (*env)->DeleteGlobalRef(env, self->clazz);
    }

    if (self->object) {
        (*env)->DeleteGlobalRef(env, self->object);
    }

    Py_CLEAR(self->attr);
    Py_DECREF(self->class_name);

    PyObject_Del(self);
}

static PyObject*
pyjobject_str(PyJObject *self)
{
    JNIEnv *env;
    jstring str;

    env = JcpThreadEnv_Get();

    if (self->object) {
        str = JavaObject_toString(env, self->object);
    } else {
        str = JavaObject_toString(env, self->clazz);
    }

    return JcpPyString_FromJString(env, str);
}

static PyObject*
pyjobject_getattro(PyObject *self, PyObject *name)
{
    PyObject *attr, *cachedAttrs, *result;

    cachedAttrs = ((PyJObject*) self)->attr;
    attr = PyDict_GetItem(cachedAttrs, name);

    result = NULL;
    if (attr == NULL) {
        return NULL;
    } else if (PyJMethod_Check(attr) || PyJMultiMethod_Check(attr)) {
        result = PyMethod_New(attr, self);
    } else if (PyJField_Check(attr)) {
        result = JcpPyJField_Get((PyJFieldObject*) attr, (PyJObject*) self);
    }
    return result;
}

static int
pyjobject_setattro(PyJObject *self, PyObject *name, PyObject *value)
{
    PyObject *attr, *cachedAttrs;

    cachedAttrs = self->attr;
    attr = PyDict_GetItem(cachedAttrs, name);

    if (attr == NULL) {
        PyErr_Format(PyExc_AttributeError,
                     "'%s' object has no attribute '%s'.",
                     PyUnicode_AsUTF8(self->class_name),
                     PyUnicode_AsUTF8(name));
    }

    if (!PyJField_Check(attr)) {
        PyErr_Format(PyExc_AttributeError,
                     "'%s' object cannot set attribute '%s'.",
                     PyUnicode_AsUTF8(self->class_name),
                     PyUnicode_AsUTF8(name));
    }

    return JcpPyJField_Set((PyJFieldObject*) attr, self, value);
}


PyObject*
JcpPyJObject_New(JNIEnv *env, PyTypeObject *type, jobject object, jclass clazz)
{
    PyJObject *self;

    self = PyObject_NEW(PyJObject, type);

    if (object) {
        self->object = (*env)->NewGlobalRef(env, object);
    } else {
        // Class Type
        self->object = NULL;
    }

    if (clazz) {
        // Class Type
        self->clazz = (*env)->NewGlobalRef(env, clazz);
    } else {
        clazz = (*env)->GetObjectClass(env, object);
        self->clazz = (*env)->NewGlobalRef(env, clazz);
        (*env)->DeleteLocalRef(env, clazz);
        clazz = NULL;
    }

    if (pyjobject_init(env, self) < 0) {
        Py_DECREF((PyObject*) self);
        self = NULL;
    }

    return (PyObject*) self;
}

PyTypeObject PyJObject_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJObject",                          /* tp_name */
    sizeof(PyJObject),                          /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor) pyjobject_dealloc,             /* tp_dealloc */
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
    (reprfunc) pyjobject_str,                   /* tp_str */
    (getattrofunc) pyjobject_getattro,          /* tp_getattro */
    (setattrofunc) pyjobject_setattro,          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                    /* tp_flags */
    "Java Object",                              /* tp_doc */
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
