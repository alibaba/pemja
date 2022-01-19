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

#include <pyutils.h>
#include <pyexceptions.h>
#include <datetime.h>

#include <java_class/JavaClass.h>

#define DEFINE_CLASS_VAR(var, name) jclass var = NULL;
CLASS_TABLE(DEFINE_CLASS_VAR)

void
Jcp_CacheClasses(JNIEnv *env)
{
    jclass clazz;

    CLASS_TABLE(CACHE_CLASS)
}

void
Jcp_UnRefCacheClasses(JNIEnv *env)
{
    CLASS_TABLE(UNREF_CACHE_CLASS);
}


/* Function to return a const char* from a Java String Object */

void
JcpString_Clear(JNIEnv *env, jstring s, const char *v)
{
    if (v != NULL && s != NULL) {
        (*env)->ReleaseStringUTFChars(env, s, v);
        (*env)->DeleteLocalRef(env, s);
    }
}


/* Function to release the memory of const char* converted by Java String Object*/

const char *
JcpString_FromJString(JNIEnv* env, jstring s)
{
    if (s == NULL) {
        return NULL;
    }

    return (*env)->GetStringUTFChars(env, s, 0);
}

// ---------------------------------  Java object to Python object ---------------------------------

/* Function to return a Python Object from a Java Object */

PyObject *
JcpPyObject_FromJObject(JNIEnv* env, jobject value)
{

    jclass clazz = NULL;

    PyObject* result = NULL;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    clazz = (*env)->GetObjectClass(env, value);

    if ((*env)->IsSameObject(env, clazz, JSTRING_TYPE)) {
        result = JcpPyString_FromJString(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JLONG_OBJ_TYPE)) {
        result = JcpPyInt_FromJLong(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JINT_OBJ_TYPE)) {
        result = JcpPyInt_FromJInteger(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JBOOLEAN_OBJ_TYPE)) {
        result = JcpPyBool_FromJBoolean(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JDOUBLE_OBJ_TYPE)) {
        result = JcpPyFloat_FromJDouble(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JFLOAT_OBJ_TYPE)) {
        result = JcpPyFloat_FromJFloat(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JBYTE_ARRAY_TYPE)) {
        result = JcpPyBytes_FromJByteArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JBOOLEAN_ARRAY_TYPE)) {
        result = JcpPyList_FromJBooleanArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JSHORT_ARRAY_TYPE)) {
        result = JcpPyList_FromJShortArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JINT_ARRAY_TYPE)) {
        result = JcpPyList_FromJIntArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JLONG_ARRAY_TYPE)) {
        result = JcpPyList_FromJLongArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JFLOAT_ARRAY_TYPE)) {
        result = JcpPyList_FromJFloatArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JDOUBLE_ARRAY_TYPE)) {
        result = JcpPyList_FromJDoubleArray(env, value);
    } else if ((*env)->IsInstanceOf(env, value, JOBJECT_ARRAY_TYPE)) {
        result = JcpPyList_FromJObjectArray(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JSQLDATE_TYPE)) {
        result = JcpPyDate_FromJSqlDate(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JSQLTIME_TYPE)) {
        result = JcpPyTime_FromJSqlTime(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JSQLTIMESTAMP_TYPE)) {
        result = JcpPyDateTime_FromJSqlTimestamp(env, value);
    } else if ((*env)->IsInstanceOf(env, value, JCOLLECTION_TYPE)) {
        result = JcpPyList_FromJCollectionObject(env, value);
    } else if ((*env)->IsInstanceOf(env, value, JMAP_TYPE)) {
        result = JcpPyDict_FromJMap(env, value);
    }  else if ((*env)->IsSameObject(env, clazz, JBIGDECIMAL_TYPE)) {
        result = JcpPyDecimal_FromJBigDecimal(env, value);
    }  else {
        char error[80];

        strcpy(error, "Unsupported java object type ");
        strcat(error, JcpString_FromJString(env, JavaObject_toString(env, value)));
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, error);
    }

    if (!result) {
        JcpPyErr_Throw(env);
    }

    return result;
}

/* ------ Functions to return a Python primitive object from a C primitive value ----- */


/* Function to return a Python Bool from a long value */

PyObject*
JcpPyBool_FromLong(long value)
{

    return PyBool_FromLong(value);
}


/* Function to return a Python Int from a int value */

PyObject*
JcpPyInt_FromInt(int value)
{

    return PyLong_FromLong(value);
}


/* Function to return a Python Float from a float value */

PyObject*
JcpPyInt_FromLong(long value)
{

    return PyLong_FromLongLong(value);
}


/* Function to return a Python Float from a double value */

PyObject*
JcpPyFloat_FromDouble(double value)
{

    return PyFloat_FromDouble(value);
}

// ------------------------------------------------------------------------------------


/* Function to return a Python String object from a Java String object */

PyObject *
JcpPyString_FromJString(JNIEnv* env, jstring value)
{

    int size;
    const jchar *chars;

    PyObject* s;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    chars = (*env)->GetStringChars(env, value, 0);
    size = (*env)->GetStringLength(env, value);
    s = PyUnicode_DecodeUTF16((const char*) chars, size * 2, NULL, NULL);
    (*env)->ReleaseStringChars(env, value, chars);

    return s;
}


/* ---------- Functions to return a Python primitive object from a Java object -------- */

/* Function to return a Python Bool from a Java Boolean object value */

PyObject *
JcpPyBool_FromJBoolean(JNIEnv* env, jobject value)
{

    jboolean b;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    b = JavaBoolean_booleanValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyBool_FromLong((long) b);
}


/* Function to return a Python Int from a Java Byte object value */

PyObject *
JcpPyInt_FromJByte(JNIEnv* env, jobject value)
{

    jbyte b;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    b = JavaNumber_byteValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyInt_FromInt((int) b);
}


/* Function to return a Python Int from a Java Short object value */

PyObject *
JcpPyInt_FromJShort(JNIEnv* env, jobject value)
{

    jshort s;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    s = JavaNumber_shortValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyInt_FromInt((int) s);
}


/* Function to return a Python Int from a Java Integer object value */

PyObject *
JcpPyInt_FromJInteger(JNIEnv* env, jobject value)
{

    jint i;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    i = JavaNumber_intValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyInt_FromInt((int) i);
}


/* Function to return a Python Int from a Java Long object value */

PyObject *
JcpPyInt_FromJLong(JNIEnv* env, jobject value)
{

    jlong l;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    l = JavaNumber_longValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyInt_FromLong((long) l);
}


/* Function to return a Python Float from a Java Float object value */

PyObject *
JcpPyFloat_FromJFloat(JNIEnv* env, jobject value)
{

    jfloat f;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    f = JavaNumber_floatValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyFloat_FromDouble((double) f);
}


/* Function to return a Python Float from a Java Double object value */

PyObject *
JcpPyFloat_FromJDouble(JNIEnv* env, jobject value)
{

    jdouble d;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    d = JavaNumber_doubleValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyFloat_FromDouble((double) d);
}

// ------------------------------------------------------------------------------------


/* Function to return a Python bytes from a Java byte array */

PyObject*
JcpPyBytes_FromJByteArray(JNIEnv* env, jbyteArray value)
{

    int length;
    char* chars;

    jbyte* bytes;

    PyObject* result;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    length = (*env)->GetArrayLength(env, value);
    bytes = (*env)->GetByteArrayElements(env, value, 0);
    chars = (char*) bytes;
    chars[length] = '\0';
    result = PyBytes_FromStringAndSize(chars, length);
    (*env)->ReleaseByteArrayElements(env, value, bytes, JNI_ABORT);

    return result;
}

/* Function to return a Python List from a Java boolean array */


JcpAPI_FUNC(PyObject*) JcpPyList_FromJBooleanArray(JNIEnv* env, jbooleanArray value)
{
    int length;

    jboolean* booleans;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    booleans = (*env)->GetBooleanArrayElements(env, value, 0);

    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        PyList_SetItem(result, i, JcpPyBool_FromLong(booleans[i]));
    }

    return result;
}


/* Function to return a Python List from a Java short array */

JcpAPI_FUNC(PyObject*) JcpPyList_FromJShortArray(JNIEnv* env, jshortArray value)
{
    int length;

    jshort* shorts;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    shorts = (*env)->GetShortArrayElements(env, value, 0);

    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        PyList_SetItem(result, i, JcpPyInt_FromInt(shorts[i]));
    }

    return result;
}


/* Function to return a Python List from a Java int array */

JcpAPI_FUNC(PyObject*) JcpPyList_FromJIntArray(JNIEnv* env, jintArray value)
{
    int length;

    jint* ints;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    ints = (*env)->GetIntArrayElements(env, value, 0);

    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        PyList_SetItem(result, i, JcpPyInt_FromInt(ints[i]));
    }

    return result;
}


/* Function to return a Python List from a Java long array */

JcpAPI_FUNC(PyObject*) JcpPyList_FromJLongArray(JNIEnv* env, jlongArray value)
{
    int length;

    jlong* longs;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    longs = (*env)->GetLongArrayElements(env, value, 0);

    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        PyList_SetItem(result, i, JcpPyInt_FromLong(longs[i]));
    }

    return result;
}


/* Function to return a Python List from a Java float array */

JcpAPI_FUNC(PyObject*) JcpPyList_FromJFloatArray(JNIEnv* env, jfloatArray value)
{
    int length;

    jfloat* floats;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    floats = (*env)->GetFloatArrayElements(env, value, 0);

    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        PyList_SetItem(result, i, JcpPyFloat_FromDouble(floats[i]));
    }

    return result;
}


/* Function to return a Python List from a Java double array */

JcpAPI_FUNC(PyObject*) JcpPyList_FromJDoubleArray(JNIEnv* env, jdoubleArray value)
{
    int length;

    jdouble* doubles;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    doubles = (*env)->GetDoubleArrayElements(env, value, 0);

    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        PyList_SetItem(result, i, JcpPyFloat_FromDouble(doubles[i]));
    }

    return result;
}


/* Function to return a Python List from a Java object array */

PyObject*
JcpPyList_FromJObjectArray(JNIEnv* env, jobjectArray value)
{

    int length;

    jobject element;

    PyObject* result;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    length = (*env)->GetArrayLength(env, value);
    result = PyList_New(length);

    for (int i = 0; i < length; i++) {
        element = (*env)->GetObjectArrayElement(env, value, i);
        PyList_SetItem(result, i, JcpPyObject_FromJObject(env, element));
        (*env)->DeleteLocalRef(env, element);
    }

    return result;
}


/* Function to return a Python List from a Java Collection object */

PyObject*
JcpPyList_FromJCollectionObject(JNIEnv* env, jobject value)
{

    int i = 0;
    int size;

    jobject iter;
    jobject next;

    PyObject* result;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    iter = JavaIterable_iterator(env, value);
    size = JavaCollection_size(env, value);

    result = PyList_New(size);
    if (result == NULL) {
        return NULL;
    }

    while(JavaIterator_hasNext(env, iter)) {
        next = JavaIterator_next(env, iter);
        PyList_SetItem(result, i++, JcpPyObject_FromJObject(env, next));
        (*env)->DeleteLocalRef(env, next);
    }
    (*env)->DeleteLocalRef(env, iter);

    return result;
}


/* Function to return a Python Dict from a Java Map object */

PyObject*
JcpPyDict_FromJMap(JNIEnv* env, jobject value)
{

    jobject iter;
    jobject next;
    jobject k;
    jobject v;

    PyObject* dict;
    PyObject* pk;
    PyObject* pv;

    dict = PyDict_New();
    iter = JavaIterable_iterator(env, JavaMap_entrySet(env, value));

    while(JavaIterator_hasNext(env, iter)) {
        next = JavaIterator_next(env, iter);
        if (!next) {
            break;
        }

        k = JavaMapEntry_getKey(env, next);
        v = JavaMapEntry_getValue(env, next);

        if ((*env)->ExceptionCheck(env)) {
            break;
        }

        pk = JcpPyObject_FromJObject(env, k);

        if (!pk) {
            break;
        }

        pv = JcpPyObject_FromJObject(env, v);

        if (!pv) {
            Py_DECREF(pk);
            break;
        }

        if (PyDict_SetItem(dict, pk, pv)) {
            Py_DECREF(pk);
            Py_DECREF(pv);
            break;
        }

        Py_DECREF(pk);
        Py_DECREF(pv);

        (*env)->DeleteLocalRef(env, next);

        if (k) {
            (*env)->DeleteLocalRef(env, k);
        }

        if (v) {
            (*env)->DeleteLocalRef(env, v);
        }
    }

    (*env)->DeleteLocalRef(env, iter);

    return dict;
}


// --------------------- Java Sql time type to Python object --------------------------

/* Function to return a Python Date from a Java Sql Date object */

PyObject*
JcpPyDate_FromJSqlDate(JNIEnv* env, jobject value)
{

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    if (!PyDateTimeAPI) {
        PyDateTime_IMPORT;
    }

    int year = JavaSqlDate_getYear(env, value);

    int month = JavaSqlDate_getMonth(env, value);

    int day = JavaSqlDate_getDate(env, value);

    return PyDate_FromDate(year + 1900, month + 1, day);;
}


/* Function to return a Python Time from a Java Sql Time object */

PyObject*
JcpPyTime_FromJSqlTime(JNIEnv* env, jobject value)
{

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    if (!PyDateTimeAPI) {
        PyDateTime_IMPORT;
    }

    long time = JavaSqlTime_getTime(env, value);

    int milliseconds = time % 1000;

    int seconds = time / 1000;

    int minutes = seconds / 60;

    seconds = seconds % 60;

    int hours = minutes / 60;

    minutes = minutes % 60;

    return PyTime_FromTime(hours, minutes, seconds, milliseconds * 1000);
}


/* Function to return a Python DateTime from a Java Sql Timestamp object */

PyObject*
JcpPyDateTime_FromJSqlTimestamp(JNIEnv* env, jobject value)
{

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    if (!PyDateTimeAPI) {
        PyDateTime_IMPORT;
    }

    int year = JavaSqlTimestamp_getYear(env, value);

    int month = JavaSqlTimestamp_getMonth(env, value);

    int date = JavaSqlTimestamp_getDate(env, value);

    int hours = JavaSqlTimestamp_getHours(env, value);

    int minutes = JavaSqlTimestamp_getMinutes(env, value);

    int seconds = JavaSqlTimestamp_getSeconds(env, value);

    int nanos = JavaSqlTimestamp_getNanos(env, value);

    return PyDateTime_FromDateAndTime(year + 1900, month + 1, date, hours, minutes, seconds, nanos / 1000);
}


/* Function to return a Python Decimal from a Java BigDecimal object */

PyObject*
JcpPyDecimal_FromJBigDecimal(JNIEnv* env, jobject value)
{

    PyObject *module;
    PyObject *clazz;
    PyObject* str;
    PyObject *result = NULL;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    module = PyImport_ImportModule((char *) "decimal");

    if (module == NULL) {
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to import `decimal` module.");
        return NULL;
    }

    Py_DECREF(module);

    clazz = PyObject_GetAttrString(module, (char *) "Decimal");

    if (clazz == NULL) {
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to import `decimal.Decimal` class.");
        return NULL;
    }

    str = JcpPyString_FromJString(env, JavaBigDecimal_toString(env, value));

    if (str) {

        result = PyObject_CallFunctionObjArgs(clazz, str, NULL);

        Py_DECREF(str);
    }

    Py_DECREF(clazz);

    return result;
}

// -------------------------------------------------------------------------------------------------


// ---------------------  Python object to Java object --------------------------------


/* Function to return a Java Object from a Python Object */

jobject
JcpPyObject_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    if (pyobject == Py_None) {
        return NULL;
    } else if (PyUnicode_Check(pyobject)) {
        return JcpPyString_AsJString(env, pyobject);
    } else if (PyBool_Check(pyobject)) {
        return JcpPyBool_AsJObject(env, pyobject, clazz);
    } else if (PyLong_CheckExact(pyobject)) {
        return JcpPyInt_AsJObject(env, pyobject, clazz);
    } else if (PyFloat_CheckExact(pyobject)) {
        return JcpPyFloat_AsJObject(env, pyobject, clazz);
    } else if (PyBytes_CheckExact(pyobject)) {
        return JcpPyBytes_AsJObject(env, pyobject, clazz);
    } else if (PyList_CheckExact(pyobject)) {
        return JcpPyList_AsJObject(env, pyobject, clazz);
    } else if (PyDict_CheckExact(pyobject)) {
        return JcpPyDict_AsJObject(env, pyobject);
    } else if (JcpPyDecimal_Check(pyobject)) {
        return JcpPyDecimal_AsJObject(env, pyobject);
    } else {
        // the macro PyDateTime_IMPORT must be invoked.
        if (!PyDateTimeAPI) {
            PyDateTime_IMPORT;
        }

        if (PyDateTime_CheckExact(pyobject)) {
            return JcpPyDateTime_AsJObject(env, pyobject);
        } else if (PyDate_CheckExact(pyobject)) {
            return JcpPyDate_AsJObject(env, pyobject);
        } else if (PyTime_CheckExact(pyobject)) {
            return JcpPyTime_AsJObject(env, pyobject);
        }
    }

    (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to convert python object to java object");
    return NULL;
}


/* ----- Functions to return a Java primitive value from a Python primitive Object --- */


/* Function to return a Java boolean value from a Python Bool object */

jboolean
JcpPyBool_AsJBoolean(PyObject* pyobject)
{
    if (PyObject_IsTrue(pyobject)) {
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}


/* Function to return a Java byte value from a Python Int object */

jbyte
JcpPyInt_AsJByte(PyObject* pyobject)
{

    long value;

    value = JcpPyInt_AsLong(pyobject);

    if (value < JBYTE_MIN || value > JBYTE_MAX) {
        PyErr_Format(PyExc_OverflowError, "%ld is outside the valid range of a Java byte.", value);
        return -1;
    }

    return (jbyte) value;
}


/* Function to return a Java short value from a Python Int object */

jshort
JcpPyInt_AsJShort(PyObject* pyobject)
{

    long value;

    value = JcpPyInt_AsLong(pyobject);

    if (value < JSHORT_MIN || value > JSHORT_MAX) {
        PyErr_Format(PyExc_OverflowError, "%ld is outside the valid range of a Java short.", value);
        return -1;
    }

    return (jshort) value;
}


/* Function to return a Java int value from a Python Int object */

jint
JcpPyInt_AsJInt(PyObject* pyobject)
{

    long value;

    value = JcpPyInt_AsLong(pyobject);

    if (value < JINT_MIN || value > JINT_MAX) {
        PyErr_Format(PyExc_OverflowError, "%ld is outside the valid range of a Java int.", value);
        return -1;
    }

    return (jint) value;
}


/* Function to return a Java long value from a Python Int object */

jlong
JcpPyInt_AsJLong(PyObject* pyobject)
{

    long long value;

    value = JcpPyInt_AsLongLong(pyobject);

    if (value < JLONG_MIN || value > JLONG_MAX) {
        PyErr_Format(PyExc_OverflowError, "%lld is outside the valid range of a Java long.", value);
        return -1;
    }

    return (jlong) value;
}


/* Function to return a Java float value from a Python Float object */

jfloat
JcpPyFloat_AsJFloat(PyObject* pyobject)
{
    return (jfloat) PyFloat_AsDouble(pyobject);
}


/* Function to return a Java double value from a Python Float object */

jdouble
JcpPyFloat_AsJDouble(PyObject* pyobject)
{
    return (jdouble) PyFloat_AsDouble(pyobject);
}

// ------------------------------------------------------------------------------------


/* ---- Functions to return a Java primitive wrapper object from a Python primitive object ---- */


/* Function to return a Java Object from a Python Bool value */

jobject
JcpPyBool_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jboolean z;
    jobject result = NULL;

    if ((*env)->IsAssignableFrom(env, JBOOLEAN_OBJ_TYPE, clazz)) {
        z = JcpPyBool_AsJBoolean(pyobject);

        if (PyErr_Occurred()) {
            return NULL;
        }

        result = JavaBoolean_New(env, z);
    }

    if (!result) {
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to convert python bool to java object");
    }

    return result;
}


/* Function to return a Java Object from a Python Int value */

jobject
JcpPyInt_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jbyte b;
    jshort s;
    jint i;
    jlong l;
    jobject result = NULL;

    if ((*env)->IsAssignableFrom(env, JLONG_OBJ_TYPE, clazz)) {
        l = JcpPyInt_AsJLong(pyobject);

        if (l == -1 && PyErr_Occurred()) {
            return NULL;
        }

        result = JavaLong_New(env, l);
    } else if ((*env)->IsAssignableFrom(env, JBYTE_OBJ_TYPE, clazz)) {
        b = JcpPyInt_AsJByte(pyobject);

        if (b == -1 && PyErr_Occurred()) {
            return NULL;
        }

        result = JavaByte_New(env, b);
    } else if ((*env)->IsAssignableFrom(env, JSHORT_OBJ_TYPE, clazz)) {
        s = JcpPyInt_AsJShort(pyobject);

        if (s == -1 && PyErr_Occurred()) {
            return NULL;
        }

        result = JavaShort_New(env, s);
    } else if ((*env)->IsAssignableFrom(env, JINT_OBJ_TYPE, clazz)) {
        i = JcpPyInt_AsJInt(pyobject);

        if (i == -1 && PyErr_Occurred()) {
            return NULL;
        }

        result = JavaInteger_New(env, i);
    }

    if (!result) {
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to convert python int to java object");
    }

    return result;
}


/* Function to return a Java Object from a Python Float value */

jobject
JcpPyFloat_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jfloat f;
    jdouble d;
    jobject result = NULL;

    if ((*env)->IsAssignableFrom(env, JDOUBLE_OBJ_TYPE, clazz)) {
        d = JcpPyFloat_AsJDouble(pyobject);

        if (d == -1.0 && PyErr_Occurred()) {
            return NULL;
        }

        result = JavaDouble_New(env, d);
    } else if ((*env)->IsAssignableFrom(env, JFLOAT_OBJ_TYPE, clazz)) {
        f = JcpPyFloat_AsJFloat(pyobject);

        if (f == -1.0 && PyErr_Occurred()) {
            return NULL;
        }

        result = JavaFloat_New(env, f);
    }

    if (!result) {
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to convert python float to java object");
    }

    return result;
}

// ------------------------------------------------------------------------------------


/* Function to return a long from a Python Int Object */

long
JcpPyInt_AsLong(PyObject* pyobject)
{

    PyObject *pyint;

    long result;

    pyint = PyNumber_Index(pyobject);

    if (pyint == NULL) {
        return -1;
    }

    result = PyLong_AsLong(pyint);
    Py_DECREF(pyint);

    return result;
}


/* Function to return a long from a Python Int Object */

long long
JcpPyInt_AsLongLong(PyObject* pyobject)
{

    PyObject *pyint;

    long long result;

    pyint = PyNumber_Index(pyobject);

    if (pyint == NULL) {
        return -1;
    }

    result = PyLong_AsLongLong(pyint);
    Py_DECREF(pyint);

    return result;
}


/* Function to return a Java Object from a Python bytes value */

jobject
JcpPyBytes_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jsize length;
    jbyteArray array;

    length = (jsize) PyBytes_Size(pyobject);
    array = (*env)->NewByteArray(env, length);
    (*env)->SetByteArrayRegion(env, array, 0, length, (jbyte*) PyBytes_AS_STRING(pyobject));

    return array;
}


/* Function to return a Java Object from a Python String value */

jobject
JcpPyString_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jobject result = NULL;

    result = JcpPyString_AsJString(env, pyobject);

    if (!result) {
        (*env)->ThrowNew(env, JPYTHONEXCE_TYPE, "Failed to convert python string to java object");
    }

    return result;
}


/* Function to return a Java char value from a Python String Object */

jchar
JcpPyString_AsJChar(PyObject* pyobject)
{

    if (PyUnicode_Check(pyobject)) {
        if (PyUnicode_READY(pyobject) != 0) {
            return 0;
        } else if (PyUnicode_GET_LENGTH(pyobject) == 1) {
            if (PyUnicode_KIND((pyobject)) == PyUnicode_1BYTE_KIND) {
                return (jchar) PyUnicode_1BYTE_DATA(pyobject)[0];
            } else if (PyUnicode_KIND((pyobject)) == PyUnicode_2BYTE_KIND) {
                return (jchar) PyUnicode_2BYTE_DATA(pyobject)[0];
            }
        }
    }

    PyErr_Format(PyExc_TypeError, "Expected char but received a %s.", pyobject->ob_type->tp_name);

    return 0;
}


/* Function to return a Java String object from a Python String Object */

jstring
JcpPyString_AsJString(JNIEnv* env, PyObject* pyobject)
{

    Py_ssize_t length;
    PyObject *pyunicode;
    Py_UCS2* data;
    PyObject* bytes = NULL;

    char* chars;

    jstring result  = NULL;


    pyunicode = PyObject_Str(pyobject);

    if (pyunicode == NULL) {
        return NULL;
    }

    if (PyUnicode_READY(pyunicode) != 0) {
        Py_DECREF(pyunicode);
        return NULL;
    } else if (PyUnicode_KIND((pyunicode)) == PyUnicode_2BYTE_KIND) {
        data = PyUnicode_2BYTE_DATA(pyunicode);
        length = PyUnicode_GET_LENGTH(pyunicode);
        result = (*env)->NewString(env, (jchar*) data, (jsize) length);
        Py_DECREF(pyunicode);

        return result;
    }

    bytes = PyUnicode_AsUTF16String(pyunicode);

    if (bytes == NULL) {
        Py_DECREF(pyunicode);
        return NULL;
    }

    chars = PyBytes_AS_STRING(bytes);
    /* +2 is to strip the BOM */
    result = (*env)->NewString(env, (jchar*) (chars + 2), (jsize) (PyBytes_GET_SIZE(bytes) - 2) / 2);
    Py_DECREF(bytes);
    Py_DECREF(pyunicode);

    return result;
}

/* Function to return a Java Object from a Python List object */

jobject
JcpPyList_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    int length;
    jobjectArray array;
    jobject element;

    length= PyList_Size(pyobject);

     array = (*env)->NewObjectArray(env, length, JOBJECT_TYPE, NULL);

    for (int i = 0; i < length; i++) {
        element = JcpPyObject_AsJObject(env, PyList_GetItem(pyobject, i), JOBJECT_TYPE);
        (*env)->SetObjectArrayElement(env, array, i, element);
    }

    return array;
}


/* Function to return a Java Map from a Python Dict object */

jobject
JcpPyDict_AsJObject(JNIEnv* env, PyObject* pyobject)
{

    jobject key;
    jobject value;
    jobject result;

    Py_ssize_t pos = 0;
    PyObject* py_key;
    PyObject* py_value;

    result = JavaMap_NewHashMap(env);

    while (PyDict_Next(pyobject, &pos, &py_key, &py_value)) {
        key = JcpPyObject_AsJObject(env, py_key, JOBJECT_TYPE);
        value = JcpPyObject_AsJObject(env, py_value, JOBJECT_TYPE);
        JavaMap_put(env, result, key, value);
        (*env)->DeleteLocalRef(env, key);
        (*env)->DeleteLocalRef(env, value);
    }

    return result;
}


/* Function to return a Java Sql Date object from a Python Date object */

jobject
JcpPyDate_AsJObject(JNIEnv* env, PyObject* pyobject)
{

    int year = PyDateTime_GET_YEAR(pyobject);

    int month = PyDateTime_GET_MONTH(pyobject);

    int day = PyDateTime_GET_DAY(pyobject);

    return JavaSqlDate_New(env, year - 1900, month - 1, day);
}


/* Function to return a Java Sql Time object from a Python Time object */

jobject
JcpPyTime_AsJObject(JNIEnv* env, PyObject* pyobject)
{

    int hours = PyDateTime_TIME_GET_HOUR(pyobject);

    int minutes = PyDateTime_TIME_GET_MINUTE(pyobject);

    int seconds = PyDateTime_TIME_GET_SECOND(pyobject);

    int microseconds = PyDateTime_TIME_GET_MICROSECOND(pyobject);

    return JavaSqlTime_New(env, hours * 3600000 + minutes * 60000 + seconds * 1000 + microseconds / 1000);
}


/* Function to return a Java Sql Timestamp object from a Python DateTime object */

jobject
JcpPyDateTime_AsJObject(JNIEnv* env, PyObject* pyobject)
{

    int year = PyDateTime_GET_YEAR(pyobject);

    int month = PyDateTime_GET_MONTH(pyobject);

    int day = PyDateTime_GET_DAY(pyobject);

    int hour = PyDateTime_DATE_GET_HOUR(pyobject);

    int minute = PyDateTime_DATE_GET_MINUTE(pyobject);

    int second = PyDateTime_DATE_GET_SECOND(pyobject);

    int microsecond = PyDateTime_DATE_GET_MICROSECOND(pyobject);

    return JavaSqlTimestamp_New(env, year - 1900, month - 1, day, hour, minute, second, microsecond * 1000);
}


int
JcpPyDecimal_Check(PyObject* pyobject)
{

    int result;

    PyObject* module;
    PyObject* clazz;

    module = PyImport_ImportModule((char *) "decimal");

    if (module == NULL) {
        PyErr_Format(PyExc_RuntimeError, "Failed to import `decimal` module.");
        return 0;
    }

    Py_DECREF(module);

    clazz = PyObject_GetAttrString(module, (char *) "Decimal");

    if (clazz == NULL) {
        PyErr_Format(PyExc_RuntimeError, "Failed to import `decimal.Decimal` class.");
        return 0;
    }

    result = PyObject_IsInstance(pyobject, clazz);
    Py_DECREF(clazz);

    return result;
}


/* Function to return a Java BigDecimal object from a Python Decimal object */

jobject
JcpPyDecimal_AsJObject(JNIEnv* env, PyObject* pyobject)
{

    PyObject* str;

    jobject result = NULL;

    str = PyObject_Str(pyobject);

    if (str) {
        result = JavaBigDecimal_New(env, JcpPyString_AsJString(env, str));
        Py_DECREF(str);
    }

    return result;
}
