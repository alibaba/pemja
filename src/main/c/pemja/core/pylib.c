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

static PyThreadState *JcpMainThreadState = NULL;

/*
* Create redirection module.
*/

static PyObject*
stdout_redirection(PyObject *self, PyObject *args)
{
    const char *string;

    if(!PyArg_ParseTuple(args, "s", &string))
        return NULL;

    fprintf(stdout, "%s", string);
    fflush(stdout);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
stderr_redirection(PyObject *self, PyObject *args)
{
    const char *string;

    if(!PyArg_ParseTuple(args,"s", &string))
        return NULL;

    fprintf(stderr, "%s", string);
    fflush(stderr);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef redirectionMethods[] = {
    {"stdout_redirect",           (PyCFunction)stdout_redirection, METH_VARARGS, ""},
    {"stderr_redirect",           (PyCFunction)stderr_redirection, METH_VARARGS, ""},
    {NULL,                        NULL, 0, NULL} /*sentinel */
};

static struct PyModuleDef redirection_module_def = {
    PyModuleDef_HEAD_INIT,
    "redirection",       /* m_name */
    NULL,                /* m_doc */
    -1,                  /* m_size */
    redirectionMethods,  /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};


/*
* Create pemja module.
*/

static PyObject*
pemja_find_class(PyObject *self, PyObject *args)
{
    JcpThread *jcp_thread;
    PyObject* result;

    char *name, *p;

    JNIEnv* env;
    jclass clazz;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    // get JcpThread
    jcp_thread = JcpThread_Get();
    if (!jcp_thread) {
        if (!PyErr_Occurred()) {
            PyErr_Format(PyExc_RuntimeError, "Invalid JcpThread pointer.");
        }
        return NULL;
    }

    // get JNIEnv*
    env = jcp_thread->env;

    // convert python class name to java class name. e.g. java.lang.Object -> java/lang/Object
    for (p = name; *p != '\0'; p++) {
        if (*p == '.') {
            *p = '/';
        }
    }

    clazz = (*env)->FindClass(env, name);
    // failed to find the class
    if ((*env)->ExceptionOccurred(env)) {
        return NULL;
    }

    result = JcpPyJClass_New(env, clazz);

    (*env)->DeleteLocalRef(env, clazz);
    return result;
}


static PyMethodDef pemja_methods[] = {
    {"findClass",                   (PyCFunction) pemja_find_class, METH_VARARGS, ""},
    {NULL,                          NULL, 0, NULL} /*sentinel */
};

static struct PyModuleDef pemja_module_def = {
    PyModuleDef_HEAD_INIT,
    "_pemja",             /* m_name */
    NULL,                /* m_doc */
    -1,                  /* m_size */
    pemja_methods,       /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};


/*
* Initialize pemja module
*/
static PyObject*
pemja_module_init(JNIEnv* env)
{
    PyObject    *sys_modules   = NULL;
    PyObject    *pemja_module  = NULL;

    // create _pemja module
    pemja_module = PyModule_Create(&pemja_module_def);
    if (pemja_module == NULL) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE, "Failed to create `_pemja` module.");
        return NULL;
    }

    sys_modules = PyImport_GetModuleDict();
    if (PyDict_SetItemString(sys_modules, "_pemja", pemja_module) == -1) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE,
                        "Failed to add `_pemja` module to sys.modules");
        return NULL;
    }

    // import _pemja module
    pemja_module = PyImport_ImportModule("_pemja");
    if (!pemja_module) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE, "Failed to import `_pemja` module");
        return NULL;
    }

    // stuff for making new pyjarray objects
    PyModule_AddIntConstant(pemja_module, "JBOOLEAN_ID", JBOOLEAN_ID);
    PyModule_AddIntConstant(pemja_module, "JINT_ID", JINT_ID);
    PyModule_AddIntConstant(pemja_module, "JLONG_ID", JLONG_ID);
    PyModule_AddIntConstant(pemja_module, "JSTRING_ID", JSTRING_ID);
    PyModule_AddIntConstant(pemja_module, "JDOUBLE_ID", JDOUBLE_ID);
    PyModule_AddIntConstant(pemja_module, "JSHORT_ID", JSHORT_ID);
    PyModule_AddIntConstant(pemja_module, "JFLOAT_ID", JFLOAT_ID);
    PyModule_AddIntConstant(pemja_module, "JCHAR_ID", JCHAR_ID);
    PyModule_AddIntConstant(pemja_module, "JBYTE_ID", JBYTE_ID);

    return pemja_module;
}

/**
* Get the JcpThread.
*/
JcpThread*
JcpThread_Get(void)
{
    PyObject  *tdict, *t, *key;
    JcpThread *ret = NULL;

    key = PyUnicode_FromString(DICT_KEY);
    if ((tdict = PyThreadState_GetDict()) != NULL && key != NULL) {
        t = PyDict_GetItem(tdict, key); /* borrowed */
        if (t != NULL && !PyErr_Occurred()) {
            ret = (JcpThread*) PyCapsule_GetPointer(t, NULL);
        }
    }
    Py_XDECREF(key);
    if (!ret && !PyErr_Occurred()) {
        PyErr_Format(PyExc_RuntimeError, "No JcpThread instance available on current thread.");
    }
    return ret;
}


JNIEnv*
JcpThreadEnv_Get(void)
{
    JavaVM *jvm;
    JNIEnv *env;
    jsize nVMs;


    JNI_GetCreatedJavaVMs(&jvm, 1, &nVMs);

    (*jvm)->AttachCurrentThreadAsDaemon(jvm, (void**) &env, NULL);

    return env;
}

/*
 * Initialize Python main Interpreter and this method will be called at startup and be called only
 * once.
 */


static int
pyjtypes_init()
{
    if (PyType_Ready(&PyJObject_Type) < 0) {
        return -1;
    }

    // class
    if (!PyJClass_Type.tp_base) {
        PyJClass_Type.tp_base = &PyJObject_Type;
    }

    if (PyType_Ready(&PyJClass_Type) < 0) {
        return -1;
    }

    // iterable
    if (!PyJIterable_Type.tp_base) {
        PyJIterable_Type.tp_base = &PyJObject_Type;
    }

    if (PyType_Ready(&PyJIterable_Type) < 0) {
        return -1;
    }

    // iterator
    if (!PyJIterator_Type.tp_base) {
        PyJIterator_Type.tp_base = &PyJObject_Type;
    }

    if (PyType_Ready(&PyJIterator_Type) < 0) {
        return -1;
    }

    // collection
    if (!PyJCollection_Type.tp_base) {
        PyJCollection_Type.tp_base = &PyJIterable_Type;
    }

    if (PyType_Ready(&PyJCollection_Type) < 0) {
        return -1;
    }

    return 0;
}

void
JcpPy_Initialize(JNIEnv *env)
{

    PyObject* sys_modules        = NULL;
    PyObject* redirection_module = NULL;

    if (JcpMainThreadState != NULL) {
        // this shouldn't happen but to be safe, don't initialize twice
        return;
    }

    // Cache java classes
    Jcp_CacheClasses(env);

    // Initialize Python
    Py_Initialize();

#if PY_MINOR_VERSION < 9
    // Initialize Python thread support
    PyEval_InitThreads();
#endif

    if (pyjtypes_init() < 0) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE, "Failed to initialize pyjtypes.");
        return;
    }

    // save a pointer to the main PyThreadState object
    JcpMainThreadState = PyThreadState_Get();

    // create redirection module
    redirection_module = PyModule_Create(&redirection_module_def);
    if (redirection_module == NULL) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE, "Failed to create `redirection` module.");
        goto EXIT;
    }

    sys_modules = PyImport_GetModuleDict();
    if (PyDict_SetItemString(sys_modules, "redirection", redirection_module) == -1) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE,
                        "Failed to add `redirection` module to sys.modules");
        goto EXIT;
    }

    // import redirection module
    redirection_module = PyImport_ImportModule("redirection");
    if (!redirection_module) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE, "Failed to import `redirection` module");
        goto EXIT;
    }
    Py_DECREF(redirection_module);

    // set sys.argv
    wchar_t *argv[] = {L"jcp", 0};

    PySys_SetArgv(1, argv);

EXIT:
    PyEval_ReleaseThread(JcpMainThreadState);
}


/*
 * Finalize Python Interpreter and release resources.
 */

void
JcpPy_Finalize(JavaVM *vm)
{

    JNIEnv *env;

    // shutdown python
    PyEval_AcquireThread(JcpMainThreadState);
    Py_Finalize();

    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_8) != JNI_OK) {
        // failed to get a JNIEnv*, we can hope it's just shutting down fast
        return;
    } else {
        // delete global references
        Jcp_UnRefCacheClasses(env);
    }
}


/*
 * Initialize JcpThread and attach a new PyThreadState to it.
 */

intptr_t
JcpPy_InitThread(JNIEnv *env, int type)
{

    JcpThread *jcp_thread;

    PyObject  *tdict, *globals = NULL, *key, *t;

    jcp_thread = malloc(sizeof(JcpThread));
    if (!jcp_thread) {
        return 0;
    }

    if (type == 0) {
        // create new ThreadState
        jcp_thread->tstate = PyThreadState_New(JcpMainThreadState->interp);

    } else if (type == 1) {
        // create sub interpreter
        PyEval_AcquireThread(JcpMainThreadState);

        jcp_thread->tstate = Py_NewInterpreter();

        PyEval_SaveThread();
    } else {
        PyErr_Format(PyExc_RuntimeError, "Unknown exec type `%d` ", type);
    }

    PyEval_AcquireThread(jcp_thread->tstate);

    if (type == 0) {
        globals = PyDict_New();
        PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());
    } else if (type == 1) {
        PyObject *mod_main = PyImport_AddModule("__main__");    /* borrowed */
        if (mod_main == NULL) {
            PyEval_ReleaseThread(jcp_thread->tstate);
            return 0;
        }
        globals = PyModule_GetDict(mod_main);
        Py_INCREF(globals);
    }

    if ((tdict = PyThreadState_GetDict()) != NULL) {
        // Capsule objects let you wrap a C "void *" pointer in a Python object.
        t = PyCapsule_New((void *) jcp_thread, NULL, NULL);
        key = PyUnicode_FromString(DICT_KEY);

        PyDict_SetItem(tdict, key, t);   /* takes ownership */

        Py_DECREF(key);
        Py_DECREF(t);
    }

    // Init JcpThread
    jcp_thread->globals = globals;
    jcp_thread->env = env;
    jcp_thread->cache_function_name = NULL;
    jcp_thread->cache_object_name = NULL;
    jcp_thread->cache_method_name = NULL;
    jcp_thread->cache_callable = NULL;
    jcp_thread->name_to_attrs = NULL;
    jcp_thread->pemja_module = pemja_module_init(env);

    PyEval_ReleaseThread(jcp_thread->tstate);

    return (intptr_t) jcp_thread;
}


/*
 * Finalize JcpThread.
 */

void
JcpPy_FinalizeThread(intptr_t ptr)
{

    JcpThread *jcp_thread;

    PyObject* tdict, *key;

    jcp_thread = (JcpThread *) ptr;
    if (!jcp_thread) {
        printf("WARNING: thread_close, invalid JcpThread pointer.\n");
        return;
    }

    PyEval_AcquireThread(jcp_thread->tstate);

    key = PyUnicode_FromString(DICT_KEY);
    if ((tdict = PyThreadState_GetDict()) != NULL && key != NULL) {
        PyDict_DelItem(tdict, key);
    }

    Py_DECREF(key);

    Py_CLEAR(jcp_thread->globals);
    Py_CLEAR(jcp_thread->name_to_attrs);
    Py_CLEAR(jcp_thread->pemja_module);

    if (jcp_thread->cache_function_name) {
        free(jcp_thread->cache_function_name);
    }

    if (jcp_thread->cache_object_name) {
        free(jcp_thread->cache_object_name);
    }

    if (jcp_thread->cache_method_name) {
        free(jcp_thread->cache_method_name);
    }

    Py_XDECREF(jcp_thread->cache_callable);

    if (jcp_thread->tstate->interp == JcpMainThreadState->interp) {
        PyThreadState_Clear(jcp_thread->tstate);
        PyEval_ReleaseThread(jcp_thread->tstate);
        PyThreadState_Delete(jcp_thread->tstate);
    } else {
        Py_EndInterpreter(jcp_thread->tstate);
        PyThreadState_Swap(JcpMainThreadState);
        PyEval_ReleaseThread(JcpMainThreadState);
    }

    free(jcp_thread);
}


/* Add path to search path of Main Interpreter */

void
JcpPy_AddSearchPath(JNIEnv *env, jstring path)
{

    PyObject* path_list;
    PyObject* py_path;

    PyEval_AcquireThread(JcpMainThreadState);

    path_list = PySys_GetObject("path");

    if (path_list != NULL) {
        Py_INCREF(path_list);

        if (path != NULL) {
            py_path = JcpPyString_FromJString(env, path);

            if (py_path != NULL) {
                PyList_Insert(path_list, 0, py_path);
                Py_DECREF(py_path);
            }
        }
    }

    PyEval_ReleaseThread(JcpMainThreadState);
}


/* Import module to the Main Interpreter so that the module can be shared by all interpreters */

void
JcpPy_ImportModule(JNIEnv *env, jstring name)
{

    const char* module_name;

    PyObject* sys_modules;
    PyObject* module;

    PyEval_AcquireThread(JcpMainThreadState);

    sys_modules = PyImport_GetModuleDict();

    if (sys_modules == NULL) {
        (*env)->ThrowNew(env, JILLEGAL_STATE_EXEC_TYPE, "Failed to import `sys` module.");
        goto exit;
    }

    module_name = JcpString_FromJString(env, name);
    module = PyImport_ImportModule(module_name);

    if (module) {
        Py_DECREF(module);
    } else {
        PyErr_Format(PyExc_RuntimeError, "Failed to import module `%s` ", module_name);
        JcpPyErr_Throw(env);
    }

    JcpString_Clear(env, name, module_name);

exit:
    PyEval_ReleaseThread(JcpMainThreadState);
}

// ------------------------------ set/get parameters methods--------------------------------


/* Function to set a Java boolean value in the specified PyThreadState */

void
JcpPyObject_SetJBoolean(JNIEnv *env, intptr_t ptr, const char *name, jboolean value)
{

    Jcp_BEGIN_ALLOW_THREADS

    _JcpPyObject_SetPyObject(jcp_thread->globals, name, JcpPyBool_FromLong((long) value));

    Jcp_END_ALLOW_THREADS
}


/* Function to set a Java int value in the specified PyThreadState */

void
JcpPyObject_SetJInt(JNIEnv *env, intptr_t ptr, const char *name, jint value)
{

    Jcp_BEGIN_ALLOW_THREADS

    _JcpPyObject_SetPyObject(jcp_thread->globals, name, JcpPyInt_FromInt((int) value));

    Jcp_END_ALLOW_THREADS
}



/* Function to set a Java long value in the specified PyThreadState */

void
JcpPyObject_SetJLong(JNIEnv *env, intptr_t ptr, const char *name, jlong value)
{

    Jcp_BEGIN_ALLOW_THREADS

    _JcpPyObject_SetPyObject(jcp_thread->globals, name, JcpPyInt_FromLong((long) value));

    Jcp_END_ALLOW_THREADS
}


/* Function to set a Java double value in the specified PyThreadState */

void
JcpPyObject_SetJDouble(JNIEnv *env, intptr_t ptr, const char *name, jdouble value)
{

    Jcp_BEGIN_ALLOW_THREADS

    _JcpPyObject_SetPyObject(jcp_thread->globals, name, JcpPyFloat_FromDouble((double) value));

    Jcp_END_ALLOW_THREADS
}


/* Function to set a Java String in the specified PyThreadState */

void
JcpPyObject_SetJString(JNIEnv *env, intptr_t ptr, const char *name, jstring value)
{

    Jcp_BEGIN_ALLOW_THREADS

    _JcpPyObject_SetPyObject(jcp_thread->globals, name, JcpPyString_FromJString(env, value));

    Jcp_END_ALLOW_THREADS
}


/* Function to set a Java Object in the specified PyThreadState */

void
JcpPyObject_SetJObject(JNIEnv *env, intptr_t ptr, const char *name, jobject value)
{

    Jcp_BEGIN_ALLOW_THREADS

    _JcpPyObject_SetPyObject(jcp_thread->globals, name, JcpPyObject_FromJObject(env, value));

    Jcp_END_ALLOW_THREADS
}


/* Function to get the Java Object by the name in the specified PyThreadState */

jobject
JcpPyObject_GetJObject(JNIEnv *env, intptr_t ptr, const char *name, jclass clazz)
{

    PyObject* pyobject;

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    pyobject = PyDict_GetItemString(jcp_thread->globals, name);

    if (pyobject != NULL) {
        result = JcpPyObject_AsJObject(env, pyobject, clazz);
    }

    Jcp_END_ALLOW_THREADS

    return result;
}

// ----------------------------------------------------------------------------------------

// ------------------------------ Core JcpPyObject call functions -------------------------


/* Call a callable Python object without any arguments */

jobject
JcpPyObject_CallNoArgs(JNIEnv *env, intptr_t ptr, const char *name)
{

    PyObject* callable = NULL;
    PyObject* py_ret = NULL;

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    callable = _JcpPyFunction_Load(env, jcp_thread, name);

    if (callable) {
#if PY_MINOR_VERSION >= 9
        py_ret = PyObject_CallNoArgs(callable);
#else
        py_ret = PyObject_CallFunctionObjArgs(callable, NULL);
#endif

        if (!JcpPyErr_Throw(env)) {
            result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);
            Py_DECREF(py_ret);
        }
    }

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object with only one jboolean argument */

jobject
JcpPyObject_CallOneJBooleanArg(JNIEnv *env, intptr_t ptr, const char *name, jboolean arg)
{

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_CallOneArg(env, jcp_thread, name, JcpPyBool_FromLong((long) arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object with only one jint argument */

jobject
JcpPyObject_CallOneJIntArg(JNIEnv *env, intptr_t ptr, const char *name, jint arg)
{

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_CallOneArg(env, jcp_thread, name, JcpPyInt_FromInt(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object with only one jlong argument */

jobject
JcpPyObject_CallOneJLongArg(JNIEnv *env, intptr_t ptr, const char *name, jlong arg)
{

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_CallOneArg(env, jcp_thread, name, JcpPyInt_FromLong(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object with only one jdouble argument */

jobject
JcpPyObject_CallOneJDoubleArg(JNIEnv *env, intptr_t ptr, const char *name, jdouble arg)
{

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_CallOneArg(env, jcp_thread, name, JcpPyFloat_FromDouble(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object with only one jstring argument */

jobject
JcpPyObject_CallOneJStringArg(JNIEnv *env, intptr_t ptr, const char *name, jstring arg)
{

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_CallOneArg(env, jcp_thread, name, JcpPyString_FromJString(env, arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object with only one jobject argument */

jobject
JcpPyObject_CallOneJObjectArg(JNIEnv *env, intptr_t ptr, const char *name, jobject arg)
{

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_CallOneArg(env, jcp_thread, name, JcpPyObject_FromJObject(env, arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call a callable Python object */

jobject
JcpPyObject_Call(JNIEnv *env, intptr_t ptr, const char *name, jobjectArray args, jobject kwargs)
{

    int arg_len = 0;

    PyObject* py_arg1;
    PyObject* py_arg2;
    PyObject* py_args       = NULL;
    PyObject* py_kwargs     = NULL;
    PyObject* py_ret        = NULL;
    PyObject* callable      = NULL;

    jobject element;
    jobject result          = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    if (args != NULL) {
        arg_len = (*env)->GetArrayLength(env, args);
    }

    callable = _JcpPyFunction_Load(env, jcp_thread, name);

    if (!callable) {
        JcpPyErr_Throw(env);
        goto exit;
    }

    if (kwargs != NULL) {

        py_args = PyTuple_New(arg_len);

        for (int i = 0; i < arg_len; i++) {
            element = (*env)->GetObjectArrayElement(env, args, i);
            PyTuple_SetItem(py_args, i, JcpPyObject_FromJObject(env, element));
            (*env)->DeleteLocalRef(env, element);
        }

        py_kwargs = JcpPyDict_FromJMap(env, kwargs);
        py_ret = PyObject_Call(callable, py_args, py_kwargs);
        Py_DECREF(py_args);
        Py_DECREF(py_kwargs);

    } else {
        // create a Tuple to store args will lost some performance, so for the function which only
        // have less than two arguments, we will call PyObject_CallFunctionObjArgs to speed up.
        if (arg_len == 0) {
            py_ret = PyObject_CallFunctionObjArgs(callable, NULL);
        } else if (arg_len  == 1) {
            element = (*env)->GetObjectArrayElement(env, args, 0);
            PyObject* py_arg = JcpPyObject_FromJObject(env, element);
            (*env)->DeleteLocalRef(env, element);

            // call the callable function
            py_ret = PyObject_CallFunctionObjArgs(callable, py_arg, NULL);

            Py_DECREF(py_arg);
        } else if (arg_len == 2) {
            // get 1st input argument
            element = (*env)->GetObjectArrayElement(env, args, 0);
            py_arg1 = JcpPyObject_FromJObject(env, element);
            (*env)->DeleteLocalRef(env, element);
            // get 2nd input argument
            element = (*env)->GetObjectArrayElement(env, args, 1);
            py_arg2 = JcpPyObject_FromJObject(env, element);
            (*env)->DeleteLocalRef(env, element);

            // call the callable function
            py_ret = PyObject_CallFunctionObjArgs(callable, py_arg1, py_arg2, NULL);

            Py_DECREF(py_arg1);
            Py_DECREF(py_arg2);
        } else {
            py_args = PyTuple_New(arg_len);

            for (int i = 0; i < arg_len; i++) {
                element = (*env)->GetObjectArrayElement(env, args, i);
                PyTuple_SetItem(py_args, i, JcpPyObject_FromJObject(env, element));
                (*env)->DeleteLocalRef(env, element);
            }

             // call the callable function
            py_ret = PyObject_CallObject(callable, py_args);
            Py_DECREF(py_args);
        }
    }

    if (JcpPyErr_Throw(env) || !py_ret) {
        goto exit;
    }

    result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);

    if (!result) {
        JcpPyErr_Throw(env);
    }

exit:
    Py_XDECREF(py_ret);

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' without arguments */

jobject
JcpPyObject_CallMethodNoArgs(JNIEnv *env, intptr_t ptr, const char *obj, const char *cname)
{

    PyObject* self;
    PyObject* name;
    PyObject* py_ret = NULL;

    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    self = PyDict_GetItemString(jcp_thread->globals, obj);

    if (self) {
        name = PyUnicode_FromString(cname);

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


/* Call the method named 'name' of object 'obj' with only one jboolean argument */

jobject
JcpPyObject_CallMethodOneJBooleanArg(JNIEnv *env, intptr_t ptr, const char *obj,
                                     const char *name, jboolean arg)
{

    jobject result;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_Call_MethodOneArg(env, jcp_thread, obj, name, JcpPyBool_FromLong(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' with only one jint argument */

jobject
JcpPyObject_CallMethodOneJIntArg(JNIEnv *env, intptr_t ptr, const char *obj,
                                 const char *name, jint arg)
{

    jobject result;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_Call_MethodOneArg(env, jcp_thread, obj, name, JcpPyInt_FromInt(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' with only one jlong argument */

jobject
JcpPyObject_CallMethodOneJLongArg(JNIEnv *env, intptr_t ptr, const char *obj,
                                  const char *name, jlong arg)
{

    jobject result;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_Call_MethodOneArg(env, jcp_thread, obj, name, JcpPyInt_FromLong(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' with only one jdouble argument */

jobject
JcpPyObject_CallMethodOneJDoubleArg(JNIEnv *env, intptr_t ptr, const char *obj,
                                    const char *name, jdouble arg)
{

    jobject result;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_Call_MethodOneArg(env, jcp_thread, obj, name, JcpPyFloat_FromDouble(arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' with only one jstring argument */

jobject
JcpPyObject_CallMethodOneJStringArg(JNIEnv *env, intptr_t ptr, const char *obj,
                                    const char *name, jstring arg)
{

    jobject result;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_Call_MethodOneArg(env, jcp_thread, obj, name,
                                            JcpPyString_FromJString(env, arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' with only one jobject argument */

jobject
JcpPyObject_CallMethodOneJObjectArg(JNIEnv *env, intptr_t ptr, const char *obj,
                                    const char *name, jobject arg)
{

    jobject result;

    Jcp_BEGIN_ALLOW_THREADS

    result = _JcpPyObject_Call_MethodOneArg(env, jcp_thread, obj, name,
                                            JcpPyObject_FromJObject(env, arg));

    Jcp_END_ALLOW_THREADS

    return result;
}


/* Call the method named 'name' of object 'obj' with a variable number of Java arguments */

jobject
JcpPyObject_CallMethod(JNIEnv *env, intptr_t ptr, const char *obj,
                       const char *name, jobjectArray args)
{

    int arg_len = 0;

    PyObject* callable = NULL;
    PyObject* py_args = NULL;
    PyObject* py_ret = NULL;

    jobject element;
    jobject result = NULL;

    Jcp_BEGIN_ALLOW_THREADS

    if (args != NULL) {
        arg_len = (*env)->GetArrayLength(env, args);
    }

    callable = _JcpPyObjectMethod_Load(jcp_thread, obj, name);

    if (callable == NULL) {
        JcpPyErr_Throw(env);
        goto exit;
    }

    py_args = PyTuple_New(arg_len);

    for (int i = 0; i < arg_len; i++) {
        element = (*env)->GetObjectArrayElement(env, args, i);
        PyTuple_SetItem(py_args, i, JcpPyObject_FromJObject(env, element));
        (*env)->DeleteLocalRef(env, element);
    }

    py_ret = PyObject_Call(callable, py_args, NULL);
    Py_DECREF(py_args);

    if (JcpPyErr_Throw(env) || !py_ret) {
        goto exit;
    }

    result = JcpPyObject_AsJObject(env, py_ret, JOBJECT_TYPE);

    if (!result) {
        JcpPyErr_Throw(env);
    }

exit:
    Py_XDECREF(py_ret);
    Jcp_END_ALLOW_THREADS

    return result;
}

// ----------------------------------------------------------------------------------------


/* Exec python code */

void
JcpExec(JNIEnv *env, intptr_t ptr, const char *code)
{

    PyObject* result;

    if (code == NULL) {
        return;
    }

    Jcp_BEGIN_ALLOW_THREADS

    result = PyRun_String(code, Py_file_input, jcp_thread->globals, jcp_thread->globals);

    if (result) {
        Py_DECREF(result);
    } else {
        JcpPyErr_Throw(env);
    }

    Jcp_END_ALLOW_THREADS
}
