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

#include "java_class/PyIterator.h"

static jmethodID init_PyIterator = 0;

jobject
JavaPyIterator_New(JNIEnv* env, jlong tstate, jlong pyobject)
{
    if (!init_PyIterator) {
        init_PyIterator = (*env)->GetMethodID(env, JPYITERPRETER_TYPE, "<init>", "(JJ)V");
    }
    return (*env)->NewObject(env, JPYITERPRETER_TYPE, init_PyIterator, tstate, pyobject);
}

JNIEXPORT jobject JNICALL Java_pemja_core_object_PyIterator_next
  (JNIEnv *env, jobject this, jlong ptr, jlong ptr_obj)
{
    PyObject *pyobject, *item;

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    pyobject = (PyObject*) ptr_obj;

    item = PyIter_Next(pyobject);

    if (PyErr_Occurred()) {
        JcpPyErr_Throw(env);
        Py_XDECREF(item);
    } else {
        if (item) {
            result = JcpPyObject_AsJObject(env, item, JOBJECT_TYPE);
            Py_DECREF(item);
        } else {
            // last value;
            (*env)->ThrowNew(env, JNOSUCHELEMENT_EXEC_TYPE, "StopIteration");
        }
    }

    Jcp_END_ALLOW_THREADS

    return result;
}