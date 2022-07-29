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

int
JcpPyErr_Throw(JNIEnv* env)
{

    PyObject *type, *value, *traceback;
    PyObject *args;
    PyObject *message;
    PyObject *type_str, *value_str;
    PyObject *traceback_module;
    PyObject *extract_method;
    PyObject *pymsg = NULL;
    PyObject *pystack = NULL;

    jobject jpyexception = NULL;

    if (!PyErr_Occurred()) {
        return 0;
    }

    PyErr_Fetch(&type, &value, &traceback);

    if (type) {
        // get the message of exception.
        if (PyObject_TypeCheck(value, (PyTypeObject*) PyExc_BaseException)) {
            args = PyObject_GetAttrString(value, "args");
            if (args != NULL && PyTuple_Check(args) && PyTuple_Size(args) > 0) {
                message = PyTuple_GetItem(args, 0);
                Py_INCREF(message);
                Py_DECREF(value);
                Py_DECREF(args);
                value = message;
            }
        }

        type_str = PyObject_Str(type);
        value_str = PyObject_Str(value);

        if (value_str != NULL && PyUnicode_Check(value_str)) {
            pymsg = PyUnicode_FromFormat("%U: %U", type_str, value_str);
            Py_DECREF(type_str);
            Py_DECREF(value_str);
        }

        jpyexception = JavaPythonException_New(env, JcpPyString_AsJString(env, pymsg));

        if (traceback) {
            traceback_module = PyImport_ImportModule("traceback");

            if (traceback_module == NULL) {
                printf("Failed to import `traceback` module\n");
            }

            extract_method = PyUnicode_FromString("extract_tb");

            if (traceback_module != NULL && extract_method != NULL) {
                pystack = PyObject_CallMethodObjArgs(traceback_module, extract_method,
                                                    traceback, NULL);
            }

            if (PyErr_Occurred()) {
                PyErr_Print();
            }

            Py_XDECREF(traceback_module);
            Py_XDECREF(extract_method);
        }

        if (pystack != NULL) {
            jsize stackSize = (jsize) PyList_Size(pystack);
            jobjectArray stack_elements = (*env)->NewObjectArray(env, stackSize,
                                                                JSTACK_TRACE_ELEMENT_TYPE, NULL);

            if ((*env)->ExceptionCheck(env) || !stack_elements) {
                PyErr_Format(PyExc_RuntimeError, "Failed to create stack_elements.");
                Py_DECREF(pystack);
                return 1;
            }

            for (int i = 0; i < stackSize; i++) {
                PyObject* stack_frame = PyList_GetItem(pystack, i);
                // self.filename, self.lineno, self.name, self.line
                const char* frame_filename = PyUnicode_AsUTF8(PySequence_GetItem(stack_frame, 0));
                int frame_lineno = (int) PyLong_AsLong(PySequence_GetItem(stack_frame, 1));
                const char* frame_func_name = PyUnicode_AsUTF8(PySequence_GetItem(stack_frame, 2));
                PyObject* frame_line = PySequence_GetItem(stack_frame, 3);

                if (frame_line != Py_None) {
                    int name_size = strlen(frame_filename);
                    // create the file name without `.py` suffix.
                    char* frame_filename_no_suffix = malloc(sizeof(char) * (name_size + 1));
                    strcpy(frame_filename_no_suffix, frame_filename);
                    char* lastDot = strrchr(frame_filename_no_suffix, '.');
                    if (lastDot != NULL) {
                        *lastDot = '\0';
                    }

                    // create the file name without directory prefix.
                    char* frame_filename_no_dir = malloc(sizeof(char) * (name_size + 1));
                    char* lastFileSep = strrchr(frame_filename, '/');
                    if (lastFileSep != NULL) {
                        strcpy(frame_filename_no_dir, lastFileSep + 1);
                    } else {
                        strcpy(frame_filename_no_dir, frame_filename);
                    }

                    jstring jframe_filename_no_dir = (*env)->NewStringUTF(env,
                                                                          frame_filename_no_dir);

                    jstring jframe_file_no_suffix = (*env)->NewStringUTF(env,
                                                                        frame_filename_no_suffix);

                    jstring jframe_func_name = (*env)->NewStringUTF(env, frame_func_name);

                    jobject stack_trace_element = JavaStackTraceElement_New(env,
                                                                            jframe_file_no_suffix,
                                                                            jframe_func_name,
                                                                            jframe_filename_no_dir,
                                                                            frame_lineno);

                    if ((*env)->ExceptionCheck(env) || !stack_trace_element) {
                        PyErr_Format(PyExc_RuntimeError,
                                     "Failed to create `StackTraceElement` for %s:%i.",
                                     frame_func_name, frame_line);
                        free(frame_filename_no_suffix);
                        free(frame_filename_no_dir);
                        Py_DECREF(pystack);
                        return 1;
                    }

                    (*env)->SetObjectArrayElement(env, stack_elements, i, stack_trace_element);
                    free(frame_filename_no_suffix);
                    free(frame_filename_no_dir);
                    (*env)->DeleteLocalRef(env, jframe_filename_no_dir);
                    (*env)->DeleteLocalRef(env, jframe_file_no_suffix);
                    (*env)->DeleteLocalRef(env, jframe_func_name);
                    (*env)->DeleteLocalRef(env, stack_trace_element);
                }
            }

            Py_DECREF(pystack);

            jobjectArray jstack = JavaThrowable_getStackTrace(env, jpyexception);
            jsize jstack_length = (*env)->GetArrayLength(env, jstack);

            // create a new stack including python stack.
            jobjectArray reverse_jstack = (*env)->NewObjectArray(env,
                                                                (jsize) (stackSize + jstack_length),
                                                                JSTACK_TRACE_ELEMENT_TYPE,
                                                                NULL);

            for (int i = stackSize - 1; i > -1; i--) {
                jobject stack_trace_element = (*env)->GetObjectArrayElement(env, stack_elements, i);
                if (stack_trace_element != NULL) {
                    (*env)->SetObjectArrayElement(env, reverse_jstack, i, stack_trace_element);
                    (*env)->DeleteLocalRef(env, stack_trace_element);
                }
            }

            for (int i = 0; i < jstack_length; i++) {
                jobject stack_trace_element = (*env)->GetObjectArrayElement(env, jstack, i);
                if (stack_trace_element != NULL) {
                    (*env)->SetObjectArrayElement(env, reverse_jstack, i + stackSize,
                                                  stack_trace_element);
                    (*env)->DeleteLocalRef(env, stack_trace_element);
                }
            }
            (*env)->DeleteLocalRef(env, jstack);
            (*env)->DeleteLocalRef(env, stack_elements);

            JavaThrowable_setStackTrace(env, jpyexception, reverse_jstack);
            if ((*env)->ExceptionCheck(env)) {
                fprintf(stderr,
                        "Error while throwing a Python exception, unexpected java exception.\n");
                PyErr_Restore(type, value, traceback);
                PyErr_Print();
                return 1;
            }
            (*env)->DeleteLocalRef(env, reverse_jstack);
        }
    }

    Py_XDECREF(type);
    Py_XDECREF(value);
    Py_XDECREF(traceback);

    if (jpyexception != NULL) {
        (*env)->Throw(env, jpyexception);
    }

    return 1;
}

int
JcpJavaErr_Throw(JNIEnv *env)
{
    jthrowable exception = NULL;
    PyObject *jpyExc;
    jobjectArray stack;

    if (!(*env)->ExceptionCheck(env)) {
        return 0;
    }

    if ((exception = (*env)->ExceptionOccurred(env)) == NULL) {
        return 0;
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }

    (*env)->ExceptionClear(env);

    stack = JavaThrowable_getStackTrace(env, exception);
    if ((*env)->ExceptionCheck(env)) {
        PyErr_Format(PyExc_RuntimeError,
                     "Throwing java exception in pyjobject failed.");
        return 1;

    }
    (*env)->DeleteLocalRef(env, stack);

    jpyExc = JcpPyJObject_New(env, &PyJObject_Type, exception, NULL);
    if (!jpyExc) {
        return 1;
    }

    PyErr_SetObject(PyExc_RuntimeError, jpyExc);
    Py_DECREF(jpyExc);
    (*env)->DeleteLocalRef(env, exception);
    return 1;
}

int
JcpPyErr_ThrowMsg(JNIEnv* env, const char* msg)
{
    return (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, msg);
}