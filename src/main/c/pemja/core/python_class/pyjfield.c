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

#include "python_class/PythonClass.h"

#include "java_class/JavaClass.h"

static int
pyjfield_init(JNIEnv* env, PyJFieldObject* self)
{
    jint modifier;

    env  = JcpThreadEnv_Get();
    if ((*env)->PushLocalFrame(env, 16) != 0) {
        return -1;
    }

    self->fd_id = (*env)->FromReflectedField(env, self->fd);
    self->fd_type = JavaField_getType(env, self->fd);
    self->fd_type_id = JcpJObject_GetObjectId(env, self->fd_type);

    modifier = JavaField_getModifiers(env, self->fd);
    self->fd_is_static = JavaModifier_isStatic(env, modifier);
    self->fd_is_initialized = 1;

    (*env)->PopLocalFrame(env, NULL);
    return 0;
}

static void
pyjfield_dealloc(PyJFieldObject* self)
{
    JNIEnv *env  = JcpThreadEnv_Get();

    if (self->fd) {
        (*env)->DeleteGlobalRef(env, self->fd);
        self->fd = NULL;
    }

    Py_CLEAR(self->fd_name);

    PyObject_Del(self);
}


/* Creates a new PyJFieldObject with a Java Field Object. */

PyJFieldObject*
JcpPyJField_New(JNIEnv *env, jobject field)
{
    jstring fieldName;

    PyJFieldObject *self;

    if (PyType_Ready(&PyJField_Type) < 0) {
        return NULL;
    }

    self = PyObject_NEW(PyJFieldObject, &PyJField_Type);

    fieldName = (jstring) JavaMember_getName(env, field);

    self->fd = (*env)->NewGlobalRef(env, field);
    self->fd_name = JcpPyString_FromJString(env, fieldName);
    self->fd_id = NULL;
    self->fd_type = NULL;
    self->fd_type_id = -1;
    self->fd_is_static = -1;
    self->fd_is_initialized = 0;

    return self;
}


/* Gets the filed of the PyJObject. */

PyObject*
JcpPyJField_Get(PyJFieldObject* self, PyJObject* pyjobject)
{
    JNIEnv *env;

    PyObject *result;

    env = JcpThreadEnv_Get();

    if (!self->fd_is_initialized) {
        if (pyjfield_init(env, self) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to initialize the PyJFieldObject");
            return NULL;
        }
    }

    result = NULL;

    switch (self->fd_type_id) {
        case JBOOLEAN_ID: {
            jboolean object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticBooleanField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetBooleanField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyBool_FromLong((long) object);
            break;
        }
        case JBYTE_ID: {
            jbyte object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticByteField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetByteField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyInt_FromInt((int) object);
            break;
        }
        case JSHORT_ID: {
            jshort object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticShortField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetShortField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyInt_FromInt((int) object);
            break;
        }
        case JINT_ID: {
            jint object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticIntField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetIntField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyInt_FromInt((int) object);
            break;
        }
        case JLONG_ID: {
            jlong object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticLongField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetLongField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyInt_FromLong((long) object);
            break;
        }
        case JFLOAT_ID: {
            jfloat object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticFloatField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetFloatField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyFloat_FromDouble((double) object);
            break;
        }
        case JDOUBLE_ID: {
            jdouble object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticDoubleField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetDoubleField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyFloat_FromDouble((double) object);
            break;
        }
        case JSTRING_ID: {
            jobject object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticObjectField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetObjectField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyString_FromJString(env, (jstring) object);
            break;
        }
        default: {
            jobject object;

            if (self->fd_is_static) {
                object = (*env)->GetStaticObjectField(env, pyjobject->clazz, self->fd_id);
            } else {
                object = (*env)->GetObjectField(env, pyjobject->object, self->fd_id);
            }

            result = JcpPyObject_FromJObject(env, object);
            break;
        }
    }

    return result;
}


/* Sets the field value of the PyJObject. */

int
JcpPyJField_Set(PyJFieldObject* self, PyJObject* pyjobject, PyObject* value)
{
    JNIEnv *env;
    env = JcpThreadEnv_Get();

    if (!self->fd_is_initialized) {
        if (pyjfield_init(env, self) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to initialize the PyJFieldObject");
            return -1;
        }
    }

    switch (self->fd_type_id) {
        case JBOOLEAN_ID: {
            jboolean object;

            object = JcpPyBool_AsJBoolean(value);

            if (self->fd_is_static) {
                (*env)->SetStaticBooleanField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetBooleanField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JBYTE_ID: {
            jbyte object;

            object = JcpPyInt_AsJByte(value);

            if (self->fd_is_static) {
                (*env)->SetStaticByteField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetByteField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JSHORT_ID: {
            jshort object;

            object = JcpPyInt_AsJShort(value);

            if (self->fd_is_static) {
                (*env)->SetStaticShortField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetShortField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JINT_ID: {
            jint object;

            object = JcpPyInt_AsJInt(value);

            if (self->fd_is_static) {
                (*env)->SetStaticIntField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetIntField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JLONG_ID: {
            jlong object;

            object = JcpPyInt_AsJLong(value);

            if (self->fd_is_static) {
                (*env)->SetStaticLongField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetLongField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JFLOAT_ID: {
            jfloat object;

            object = JcpPyFloat_AsJFloat(value);

            if (self->fd_is_static) {
                (*env)->SetStaticFloatField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetFloatField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JDOUBLE_ID: {
            jdouble object;

            object = JcpPyFloat_AsJDouble(value);

            if (self->fd_is_static) {
                (*env)->SetStaticDoubleField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetDoubleField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        case JSTRING_ID: {
            jstring object;

            object = JcpPyString_AsJString(env, value);

            if (self->fd_is_static) {
                (*env)->SetStaticObjectField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetObjectField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
        default: {
            jobject object;

            object = JcpPyObject_AsJObject(env, value, self->fd_type);

            if (self->fd_is_static) {
                (*env)->SetStaticObjectField(env, pyjobject->clazz, self->fd_id, object);
            } else {
                (*env)->SetObjectField(env, pyjobject->object, self->fd_id, object);
            }

            break;
        }
    }
    return 0;
}

PyTypeObject PyJField_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pemja.PyJField",                           /* tp_name */
    sizeof(PyJFieldObject),                     /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor) pyjfield_dealloc,              /* tp_dealloc */
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
    "Java Field Object",                        /* tp_doc */
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
