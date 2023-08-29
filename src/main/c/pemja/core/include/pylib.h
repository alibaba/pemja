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
#ifndef _Included_pylib
#define _Included_pylib

#define DICT_KEY "jcp"

struct __JcpThread {

    /* The attached variable objects of the Thread */
    PyObject      *globals;

    /* The attached PyThreadState of the Thread */
    PyThreadState *tstate;

    /* The attached JNIEnv of the Thread */
    JNIEnv        *env;

    /* The pemja module */
    PyObject      *pemja_module;

    /* The cached callable function name */
    char          *cache_function_name;

    /* The cached callable object name. */
    char          *cache_object_name;

    /* The cached callable method name. */
    char          *cache_method_name;

    /* The cached callable object */
    PyObject      *cache_callable;

    /* A cached Dict which mappes class name to methods and fields.*/
    PyObject      *name_to_attrs;
};

typedef struct __JcpThread JcpThread;


#define Jcp_BEGIN_ALLOW_THREADS { \
                        JcpThread* jcp_thread; \
                        jcp_thread = (JcpThread*) ptr; \
                        PyEval_AcquireThread(jcp_thread->tstate);

#define Jcp_END_ALLOW_THREADS    PyEval_ReleaseThread(jcp_thread->tstate); \
                 }


JcpAPI_FUNC(JcpThread*) JcpThread_Get(void);

JcpAPI_FUNC(JNIEnv*) JcpThreadEnv_Get(void);


/* Initialization and finalization */
JcpAPI_FUNC(void) JcpPy_Initialize(JNIEnv *);
JcpAPI_FUNC(void) JcpPy_Finalize(JavaVM *);
JcpAPI_FUNC(intptr_t) JcpPy_InitThread(JNIEnv *, int);
JcpAPI_FUNC(void) JcpPy_FinalizeThread(intptr_t);

/* Add path to search path of Main Interpreter */
JcpAPI_FUNC(void) JcpPy_AddSearchPath(JNIEnv *, jstring);

/* Import module to the Main Interpreter so that the module can be shared by all interpreters */
JcpAPI_FUNC(void) JcpPy_ImportModule(JNIEnv *, jstring);

// ------------------------------ set/get parameters methods----------------------

/* Function to set a Java boolean value in the specified PyThreadState */
JcpAPI_FUNC(void) JcpPyObject_SetJBoolean(JNIEnv *, intptr_t, const char *, jboolean);

/* Function to set a Java int value in the specified PyThreadState */
JcpAPI_FUNC(void) JcpPyObject_SetJInt(JNIEnv *, intptr_t, const char *, jint);

/* Function to set a Java long value in the specified PyThreadState */
JcpAPI_FUNC(void) JcpPyObject_SetJLong(JNIEnv *, intptr_t, const char *, jlong);

/* Function to set a Java double value in the specified PyThreadState */
JcpAPI_FUNC(void) JcpPyObject_SetJDouble(JNIEnv *, intptr_t, const char *, jdouble);

/* Function to set a Java String in the specified PyThreadState */
JcpAPI_FUNC(void) JcpPyObject_SetJString(JNIEnv *, intptr_t, const char *, jstring);

/* Function to set a Java Object in the specified PyThreadState */
JcpAPI_FUNC(void) JcpPyObject_SetJObject(JNIEnv *, intptr_t, const char *, jobject);

static inline void
_JcpPyObject_SetPyObject(PyObject* globals, const char *name, PyObject* value)
{
    if (value) {
        PyDict_SetItemString(globals, name, value);
        Py_DECREF(value);
    }
}

/* Function to get the Java Object by the name in the specified PyThreadState */
JcpAPI_FUNC(jobject) JcpPyObject_GetJObject(JNIEnv *, intptr_t, const char *, jclass);

// ------------------------------ Core JcpPyObject call functions----------------------
static inline void
_clear_jcp_cache(JcpThread* jcp_thread) {
   // remove the legacy cache name
    if (jcp_thread->cache_function_name) {
        free(jcp_thread->cache_function_name);
        jcp_thread->cache_function_name = NULL;
    }

    // remove the legacy cache object name
    if (jcp_thread->cache_object_name) {
        free(jcp_thread->cache_object_name);
        jcp_thread->cache_object_name = NULL;
    }

    // remove the legacy cache method name
    if (jcp_thread->cache_method_name) {
        free(jcp_thread->cache_method_name);
        jcp_thread->cache_method_name = NULL;
    }

    // remove the legacy cache callable
    if (jcp_thread->cache_callable) {
        Py_DECREF(jcp_thread->cache_callable);
    }
}

static inline char*
_str_create_and_copy(const char* s)
{

    int size;
    char* ns;

    size = sizeof(char) * (strlen(s) + 1);
    ns = malloc(size);
    memset(ns, '\0', size);
    strcpy(ns, s);

    return ns;
}

static inline PyObject*
_JcpPyFunction_Load(JNIEnv* env, JcpThread* jcp_thread, const char *name)
{

    char* cache_function_name;
    char* dot;
    char* module_name;

    PyObject* globals;
    PyObject* module;
    PyObject* callable;

    cache_function_name = jcp_thread->cache_function_name;

    if (cache_function_name != NULL && strcmp(cache_function_name, name) == 0) {
        return jcp_thread->cache_callable;
    } else {
        globals = jcp_thread->globals;
        callable = PyDict_GetItemString(globals, name);

        if (!callable) {

            dot = strchr(name, '.');

            if (dot) {
                module_name = malloc((dot - name + 1) * sizeof(char));
                strncpy(module_name, name, dot - name);
                module_name[dot - name] = '\0';
                module = PyDict_GetItemString(globals, module_name);

                if (module) {
                    callable = PyObject_GetAttrString(module, dot + 1);

                    if (callable == NULL) {
                        PyErr_Format(PyExc_RuntimeError,
                                    "Failed to find the function `%s` in module `%s` ",
                                    dot + 1,
                                    module_name);
                    }
                } else {
                    PyErr_Format(PyExc_RuntimeError,
                                "Failed to find the module `%s` ",
                                module_name);
                }

                free(module_name);
            } else {
                PyErr_Format(PyExc_RuntimeError, "Failed to find the function `%s` ", name);
            }
        }

        if (callable) {

            _clear_jcp_cache(jcp_thread);

            // cache function name
            jcp_thread->cache_function_name = _str_create_and_copy(name);

            // cache callable
            jcp_thread->cache_callable = callable;
        }

        return callable;
    }
}

static inline PyObject*
_JcpPyObjectMethod_Load(JcpThread* jcp_thread, const char* obj_name, const char *name)
{

    char* cache_object_name;
    char* cache_method_name;

    PyObject* py_name;
    PyObject* callable;

    cache_object_name = jcp_thread->cache_object_name;
    cache_method_name = jcp_thread->cache_method_name;

    if (cache_object_name != NULL && strcmp(cache_object_name, obj_name) == 0
            && cache_method_name != NULL && strcmp(cache_method_name, name) == 0) {
        return jcp_thread->cache_callable;
    } else {

        py_name = PyUnicode_FromString(name);
        callable = PyObject_GetAttr(PyDict_GetItemString(jcp_thread->globals, obj_name), py_name);
        Py_DECREF(py_name);

        if (callable == NULL) {
            PyErr_Format(PyExc_RuntimeError,
                        "Failed to find the method `%s` in object `%s` ",
                        name,
                        obj_name);

            return NULL;
        }

        _clear_jcp_cache(jcp_thread);

        // cache object name
        jcp_thread->cache_object_name = _str_create_and_copy(obj_name);

        // cache method name
        jcp_thread->cache_method_name = _str_create_and_copy(name);

        // cache callable
        jcp_thread->cache_callable = callable;

        return callable;
    }
}


static inline jobject
_JcpPyCallable_OneArg(JNIEnv *env, PyObject* callable, PyObject* arg)
{

    PyObject* py_ret = NULL;

    jobject result = NULL;

    if (callable) {
#if PY_MINOR_VERSION >= 9
        py_ret = PyObject_CallOneArg(callable, arg);
#else
        py_ret = PyObject_CallFunctionObjArgs(callable, arg, NULL);
#endif

        if (!JcpPyErr_Throw(env)) {
            result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);
            Py_DECREF(py_ret);
        }
    }

    return result;
}


static inline jobject
_JcpPyObject_CallOneArg(JNIEnv *env, JcpThread* jcp_thread, const char *name, PyObject* arg)
{

    PyObject* callable = NULL;

    jobject result = NULL;

    if (arg) {
        callable = _JcpPyFunction_Load(env, jcp_thread, name);

        if (callable == NULL) {
            Py_DECREF(arg);
            JcpPyErr_Throw(env);
            return NULL;
        }

        result = _JcpPyCallable_OneArg(env, callable, arg);
        Py_DECREF(arg);
    }

    return result;
}

/* Call a callable Python object without any arguments */
JcpAPI_FUNC(jobject) JcpPyObject_CallNoArgs(JNIEnv *, intptr_t, const char *);

/* Call a callable Python object with only one jboolean argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallOneJBooleanArg(JNIEnv *, intptr_t, const char *, jboolean);

/* Call a callable Python object with only one jint argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallOneJIntArg(JNIEnv *, intptr_t, const char *, jint);

/* Call a callable Python object with only one jlong argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallOneJLongArg(JNIEnv *, intptr_t, const char *, jlong);

/* Call a callable Python object with only one jdouble argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallOneJDoubleArg(JNIEnv *, intptr_t, const char *, jdouble);

/* Call a callable Python object with only one jstring argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallOneJStringArg(JNIEnv *, intptr_t, const char *, jstring);

/* Call a callable Python object with only one jobject argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallOneJObjectArg(JNIEnv *, intptr_t, const char *, jobject);

/* Call a callable Python object */
JcpAPI_FUNC(jobject) JcpPyObject_Call(JNIEnv *, intptr_t, const char *, jobjectArray, jobject);


static inline jobject
_JcpPyObject_Call_MethodOneArg(JNIEnv *env, JcpThread* jcp_thread, const char *obj,
                               const char *name, PyObject* arg)
{

    PyObject* callable = NULL;

    jobject result = NULL;

    if (arg) {
        callable = _JcpPyObjectMethod_Load(jcp_thread, obj, name);

        if (callable == NULL) {
            Py_DECREF(arg);
            JcpPyErr_Throw(env);
            return NULL;
        }

        result = _JcpPyCallable_OneArg(env, callable, arg);
        Py_DECREF(arg);
    }

    return result;
}

/* Call the method named 'name' of object 'obj' without arguments */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodNoArgs(JNIEnv *, intptr_t, const char *, const char *);

/* Call the method named 'name' of object 'obj' with only one jboolean argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodOneJBooleanArg(JNIEnv *, intptr_t, const char *, const char *, jboolean);

/* Call the method named 'name' of object 'obj' with only one jint argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodOneJIntArg(JNIEnv *, intptr_t, const char *, const char *, jint);

/* Call the method named 'name' of object 'obj' with only one jlong argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodOneJLongArg(JNIEnv *, intptr_t, const char *, const char *, jlong);

/* Call the method named 'name' of object 'obj' with only one jdouble argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodOneJDoubleArg(JNIEnv *, intptr_t, const char *, const char *, jdouble);

/* Call the method named 'name' of object 'obj' with only one jstring argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodOneJStringArg(JNIEnv *, intptr_t, const char *, const char *, jstring);

/* Call the method named 'name' of object 'obj' with only one jobject argument */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethodOneJObjectArg(JNIEnv *, intptr_t, const char *, const char *, jobject);

/* Call the method named 'name' of object 'obj' with a variable number of Java arguments */
JcpAPI_FUNC(jobject) JcpPyObject_CallMethod(JNIEnv *, intptr_t, const char *, const char *, jobjectArray);

// ----------------------------------------------------------------------------------------

/* Exec python code */
JcpAPI_FUNC(void) JcpExec(JNIEnv *, intptr_t, const char *);

#endif // ifndef _Included_pylib