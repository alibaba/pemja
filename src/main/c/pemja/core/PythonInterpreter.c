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

#include "MainInterpreter.h"
#include "PythonInterpreter.h"

// ---------------------------------- jni functions ------------------------


/* The VM calls JNI_OnLoad when the native library is loaded */
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    return JNI_VERSION_1_8;
}


/*
 * The VM calls JNI_OnUnload when the class loader containing the native library is
 * garbage collected.
 */
JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JcpPy_Finalize(vm);
}

/*
 * Class:     pemja_core_PythonInterpreter_MainInterpreter
 * Method:    initialize
 * Signature: (V)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_00024MainInterpreter_initialize
  (JNIEnv *env, jobject obj)
{
    JcpPy_Initialize(env);
}

/*
 * Class:     pemja_core_PythonInterpreter_MainInterpreter
 * Method:    addToPath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_00024MainInterpreter_addToPath
  (JNIEnv *env, jobject obj, jstring path)
{
    JcpPy_AddSearchPath(env, path);
}

/*
 * Class:     pemja_core_PythonInterpreter_MainInterpreter
 * Method:    importModule
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_00024MainInterpreter_importModule
  (JNIEnv *env, jobject obj, jstring name) {
    JcpPy_ImportModule(env, name);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    init
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_pemja_core_PythonInterpreter_init(JNIEnv *env, jobject obj, jint type) {
    return JcpPy_InitThread(env, type);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    finalize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_finalize
  (JNIEnv *env, jobject obj, jlong ptr)
{
    JcpPy_FinalizeThread(ptr);
}

// ----------------------------------------------------------------------

// ------------------------------ set()/get methods----------------------

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    set
 * Signature: (JLjava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_set__JLjava_lang_String_2Z
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jboolean value)
{

    const char *cname;

    cname = JcpString_FromJString(env, name);

    JcpPyObject_SetJBoolean(env, (intptr_t) ptr, cname, value);

    JcpString_Clear(env, name, cname);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    set
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_set__JLjava_lang_String_2I
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jint value)
{

    const char *cname;

    cname = JcpString_FromJString(env, name);

    JcpPyObject_SetJInt(env, (intptr_t) ptr, cname, value);

    JcpString_Clear(env, name, cname);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    set
 * Signature: (JLjava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_set__JLjava_lang_String_2J
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jlong value)
{

    const char *cname;

    cname = JcpString_FromJString(env, name);

    JcpPyObject_SetJLong(env, (intptr_t) ptr, cname, value);

    JcpString_Clear(env, name, cname);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    set
 * Signature: (JLjava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_set__JLjava_lang_String_2D
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jdouble value)
{

    const char *cname;

    cname = JcpString_FromJString(env, name);

    JcpPyObject_SetJDouble(env, (intptr_t) ptr, cname, value);

    JcpString_Clear(env, name, cname);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    set
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_set__JLjava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jstring value)
{

    const char *cname;

    cname = JcpString_FromJString(env, name);

    JcpPyObject_SetJString(env, (intptr_t) ptr, cname, value);

    JcpString_Clear(env, name, cname);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    set
 * Signature: (JLjava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_set__JLjava_lang_String_2Ljava_lang_Object_2
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jobject value)
{

    const char *cname;

    cname = JcpString_FromJString(env, name);

    JcpPyObject_SetJObject(env, (intptr_t) ptr, cname, value);

    JcpString_Clear(env, name, cname);
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    get
 * Signature: (JLjava/lang/String;Ljava/lang/Class;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_get
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jclass clazz)
{

    const char *cname;
    jobject result;

    cname = JcpString_FromJString(env, name);

    result = JcpPyObject_GetJObject(env, (intptr_t) ptr, cname, clazz);

    JcpString_Clear(env, name, cname);

    return result;
}

// ----------------------------------------------------------------------

// ------------------------------ invoke() methods-----------------------

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeNoArgs
 * Signature: (JLjava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeNoArgs
  (JNIEnv *env, jobject obj, jlong ptr, jstring name)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);

    result = JcpPyObject_CallNoArgs(env, (intptr_t) ptr, cname);

    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeOneArgBoolean
 * Signature: (JLjava/lang/String;Ljava/lang/String;Z)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeOneArgBoolean
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jboolean arg)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_CallOneJBooleanArg(env, (intptr_t) ptr, cname, arg);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeOneArgInt
 * Signature: (JLjava/lang/String;Ljava/lang/String;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeOneArgInt
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jint arg)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_CallOneJIntArg(env, (intptr_t) ptr, cname, arg);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeOneArgLong
 * Signature: (JLjava/lang/String;J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeOneArgLong
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jlong arg)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_CallOneJLongArg(env, (intptr_t) ptr, cname, arg);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeOneArgDouble
 * Signature: (JLjava/lang/String;D)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeOneArgDouble
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jdouble arg)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_CallOneJDoubleArg(env, (intptr_t) ptr, cname, arg);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeOneArgString
 * Signature: (JLjava/lang/String;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeOneArgString
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jstring arg)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_CallOneJStringArg(env, (intptr_t) ptr, cname, arg);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeOneArgObject
 * Signature: (JLjava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeOneArgObject
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jobject arg)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_CallOneJObjectArg(env, (intptr_t) ptr, cname, arg);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invoke
 * Signature: (JLjava/lang/String;[Ljava/lang/Object;Ljava/util/Map;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invoke
  (JNIEnv *env, jobject obj, jlong ptr, jstring name, jobjectArray args, jobject kwargs)
{

    const char* cname;
    jobject result;

    cname = JcpString_FromJString(env, name);
    result = JcpPyObject_Call(env, (intptr_t) ptr, cname, args, kwargs);
    JcpString_Clear(env, name, cname);

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodNoArgs
 * Signature: (JLjava/lang/String;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodNoArgs
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodNoArgs(env, (intptr_t) ptr, objname, methodname);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodOneArgBoolean
 * Signature: (JLjava/lang/String;Ljava/lang/String;Z)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodOneArgBoolean
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jboolean arg)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodOneJBooleanArg(env, (intptr_t) ptr, objname, methodname, arg);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodOneArgInt
 * Signature: (JLjava/lang/String;Ljava/lang/String;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodOneArgInt
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jint arg)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodOneJIntArg(env, (intptr_t) ptr, objname, methodname, arg);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodOneArgLong
 * Signature: (JLjava/lang/String;Ljava/lang/String;J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodOneArgLong
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jlong arg)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodOneJLongArg(env, (intptr_t) ptr, objname, methodname, arg);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodOneArgDouble
 * Signature: (JLjava/lang/String;Ljava/lang/String;D)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodOneArgDouble
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jdouble arg)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodOneJDoubleArg(env, (intptr_t) ptr, objname, methodname, arg);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodOneArgString
 * Signature: (JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodOneArgString
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jstring arg)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodOneJStringArg(env, (intptr_t) ptr, objname, methodname, arg);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethodOneArgObject
 * Signature: (JLjava/lang/String;Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethodOneArgObject
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jobject arg)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethodOneJObjectArg(env, (intptr_t) ptr, objname, methodname, arg);

    Jcp_END_INVOKE_METHODS

    return result;
}

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    invokeMethod
 * Signature: (JLjava/lang/String;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_pemja_core_PythonInterpreter_invokeMethod
  (JNIEnv *env, jobject obj, jlong ptr, jstring jobjname, jstring jmethodname, jobjectArray args)
{

    jobject result;

    Jcp_BEGIN_INVOKE_METHODS

    result = JcpPyObject_CallMethod(env, (intptr_t) ptr, objname, methodname, args);

    Jcp_END_INVOKE_METHODS

    return result;
}

// ----------------------------------------------------------------------

/*
 * Class:     pemja_core_PythonInterpreter
 * Method:    exec
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pemja_core_PythonInterpreter_exec
  (JNIEnv *env, jobject obj, jlong ptr, jstring jcode)
{

    const char* code;

    code = JcpString_FromJString(env, jcode);
    JcpExec(env, (intptr_t) ptr, code);
    JcpString_Clear(env, jcode, code);
}
