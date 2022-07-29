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

#include "java_class/PyObject.h"

static jmethodID init_PyObject = 0;
static jfieldID field_pyobject = 0;

jobject
JavaPyObject_New(JNIEnv* env, jlong tstate, jlong pyobject)
{
    if (!init_PyObject) {
        init_PyObject = (*env)->GetMethodID(env, JPYOBJECT_TYPE, "<init>", "(JJ)V");
    }
    return (*env)->NewObject(env, JPYOBJECT_TYPE, init_PyObject, tstate, pyobject);
}

jlong
JavaPyObject_GetPyobject(JNIEnv* env, jobject this)
{
    if (!field_pyobject) {
        field_pyobject = (*env)->GetFieldID(env, JPYOBJECT_TYPE, "pyobject", "J");
    }

    return (*env)->GetLongField(env, this, field_pyobject);
}


JNIEXPORT void JNICALL Java_pemja_core_object_PyObject_decRef
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj)
{
    PyObject *pyobject;

    Jcp_BEGIN_ALLOW_THREADS

    pyobject = (PyObject*) ptr_obj;

    Py_DECREF(pyobject);

    Jcp_END_ALLOW_THREADS
}

JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_getAttr
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj, jstring attr, jclass clazz)
{
    char*       msg;
    const char  *cattr;
    PyObject    *pattr, *pyobject;
    jobject     result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    pyobject = (PyObject*) ptr_obj;

    cattr = JcpString_FromJString(env, attr);

    pattr = PyObject_GetAttrString(pyobject, cattr);

    if (pattr) {
        result = JcpPyObject_AsJObject(env, pattr, clazz);

        Py_XDECREF(pattr);

        if (PyErr_Occurred()) {
            JcpPyErr_Throw(env);
        }
    } else {
        msg = malloc(sizeof(char) * 200);
        memset(msg, '\0', 200);
        sprintf(msg, "Unknown attribute %s.", cattr);

        JcpPyErr_ThrowMsg(env, msg);

        free(msg);
    }

    JcpString_Clear(env, attr, cattr);

    Jcp_END_ALLOW_THREADS

    return result;
}

JNIEXPORT void JNICALL Java_pemja_core_object_PyObject_setAttr
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj, jstring attr, jobject value)
{
    const char  *cattr;
    PyObject    *pyvalue, *pyobject;

    Jcp_BEGIN_ALLOW_THREADS

    pyobject = (PyObject*) ptr_obj;

    cattr = JcpString_FromJString(env, attr);

    pyvalue = JcpPyObject_FromJObject(env, value);

    PyObject_SetAttrString(pyobject, cattr, pyvalue);

    Py_DECREF(pyvalue);

    Jcp_END_ALLOW_THREADS
}

JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_invokeMethodNoArgs
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj, jstring method)
{
    PyObject* self;
    PyObject* name;
    PyObject* py_ret = NULL;

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    self = (PyObject*) ptr_obj;

    if (self) {

        name = JcpPyString_FromJString(env, method);

#if PY_MINOR_VERSION >= 9
        py_ret = PyObject_CallMethodNoArgs(self, name);
#else
        py_ret = PyObject_CallMethodObjArgs(self, name, NULL);
#endif

        Py_DECREF(name);

        if (!JcpPyErr_Throw(env)) {
            result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);
            Py_DECREF(py_ret);
        }
    }

    Jcp_END_ALLOW_THREADS

    return result;
}

JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_invokeMethodOneArg
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj, jstring method, jobject arg)
{
    PyObject* self;
    PyObject* name;
    PyObject* py_arg;
    PyObject* py_ret = NULL;

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    self = (PyObject*) ptr_obj;

    if (self) {

        name = JcpPyString_FromJString(env, method);

        py_arg = JcpPyObject_FromJObject(env, arg);

        if (py_arg) {
#if PY_MINOR_VERSION >= 9
            py_ret = PyObject_CallMethodOneArg(self, name, py_arg);
#else
            py_ret = PyObject_CallMethodObjArgs(self, name, py_arg, NULL);
#endif
            Py_DECREF(py_arg);
        }
        Py_DECREF(name);

        if (!JcpPyErr_Throw(env)) {
            result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);
            Py_DECREF(py_ret);
        }
    }

    Jcp_END_ALLOW_THREADS

    return result;
}

JNIEXPORT jobject JNICALL Java_pemja_core_object_PyObject_invokeMethod
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj, jstring method, jobjectArray args)
{
    PyObject* self;
    PyObject* name;
    PyObject* callable;
    PyObject* py_args;
    PyObject* py_ret = NULL;
    int arg_len;

    jobject element;
    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    self = (PyObject*) ptr_obj;

    if (self) {

        name = JcpPyString_FromJString(env, method);

        callable = PyObject_GetAttr(self, name);

        if (callable) {
            arg_len = (*env)->GetArrayLength(env, args);
            py_args = PyTuple_New(arg_len);

            for (int i = 0; i < arg_len; i++) {
                element = (*env)->GetObjectArrayElement(env, args, i);
                PyTuple_SetItem(py_args, i, JcpPyObject_FromJObject(env, element));
                (*env)->DeleteLocalRef(env, element);
            }

            py_ret = PyObject_Call(callable, py_args, NULL);
            Py_DECREF(py_args);
        }

        Py_DECREF(name);

        if (!JcpPyErr_Throw(env) || !py_ret) {
            result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);
            Py_DECREF(py_ret);
        }
    }

    Jcp_END_ALLOW_THREADS

    return result;
}
