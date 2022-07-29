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

#include <datetime.h>

#include "java_class/JavaClass.h"
#include "python_class/PythonClass.h"

#define DEFINE_CLASS_VAR(var, name) jclass var = NULL;
CLASS_TABLE(DEFINE_CLASS_VAR)

// primitive class
jclass JBOOLEAN_TYPE = NULL;
jclass JBYTE_TYPE = NULL;
jclass JSHORT_TYPE = NULL;
jclass JINT_TYPE = NULL;
jclass JLONG_TYPE = NULL;
jclass JFLOAT_TYPE = NULL;
jclass JDOUBLE_TYPE = NULL;
jclass JCHAR_TYPE = NULL;

// void class
jclass JVOID_TYPE = NULL;

void
Jcp_CacheClasses(JNIEnv *env)
{
    jclass clazz, voidClazz;
    jfieldID fieldId;

    CLASS_TABLE(CACHE_CLASS)

    // cache primitive class
    CACHE_PRIMITIVE_CLASS(JBOOLEAN_TYPE, JBOOLEAN_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JBYTE_TYPE, JBYTE_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JSHORT_TYPE, JSHORT_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JINT_TYPE, JINT_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JLONG_TYPE, JLONG_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JFLOAT_TYPE, JFLOAT_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JDOUBLE_TYPE, JDOUBLE_ARRAY_TYPE)
    CACHE_PRIMITIVE_CLASS(JCHAR_TYPE, JCHAR_ARRAY_TYPE)

    if (JVOID_TYPE == NULL) {
        clazz = (*env)->FindClass(env, "java/lang/Void");

        fieldId = (*env)->GetStaticFieldID(env, clazz, "TYPE", "Ljava/lang/Class;");

        voidClazz = (jclass) (*env)->GetStaticObjectField(env, clazz, fieldId);

        JVOID_TYPE = (*env)->NewGlobalRef(env, voidClazz);
        (*env)->DeleteLocalRef(env, voidClazz);
        (*env)->DeleteLocalRef(env, clazz);
    }
}

void
Jcp_UnRefCacheClasses(JNIEnv *env)
{
    CLASS_TABLE(UNREF_CACHE_CLASS);

    UNREF_CACHE_CLASS(JBOOLEAN_TYPE, NULL)
    UNREF_CACHE_CLASS(JBYTE_TYPE, NULL)
    UNREF_CACHE_CLASS(JSHORT_TYPE, NULL)
    UNREF_CACHE_CLASS(JINT_TYPE, NULL)
    UNREF_CACHE_CLASS(JLONG_TYPE, NULL)
    UNREF_CACHE_CLASS(JFLOAT_TYPE, NULL)
    UNREF_CACHE_CLASS(JDOUBLE_TYPE, NULL)
    UNREF_CACHE_CLASS(JVOID_TYPE, NULL)
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


/* Function to get the Object Id from the Java Class */

int
JcpJObject_GetObjectId(JNIEnv* env, jclass clazz)
{
    jstring classname;
    const char* cname;
    char* msg;

    // check whether it is a Java Object.
    if ((*env)->IsAssignableFrom(env, clazz, JOBJECT_TYPE)) {

        if ((*env)->IsSameObject(env, clazz, JSTRING_TYPE)) {
            return JSTRING_ID;
        }

        // byte array type
        if ((*env)->IsSameObject(env, clazz, JBYTE_ARRAY_TYPE)) {
            return JBYTES_ID;
        }

        // list type
        if ((*env)->IsAssignableFrom(env, clazz, JLIST_TYPE)) {
            return JLIST_ID;
        }

        // map type
        if ((*env)->IsAssignableFrom(env, clazz, JMAP_TYPE)) {
            return JMAP_ID;
        }

        // array type
        if (JavaClass_isArray(env, clazz)) {
            return JARRAY_ID;
        }

        return JOBJECT_ID;
    }

    // check primitive type.
    if ((*env)->IsSameObject(env, clazz, JBOOLEAN_TYPE)) {
        return JBOOLEAN_ID;
    } else if ((*env)->IsSameObject(env, clazz, JINT_TYPE)) {
         return JINT_ID;
    } else if ((*env)->IsSameObject(env, clazz, JDOUBLE_TYPE)) {
         return JDOUBLE_ID;
    } else if ((*env)->IsSameObject(env, clazz, JLONG_TYPE)) {
        return JLONG_ID;
    } else if ((*env)->IsSameObject(env, clazz, JFLOAT_TYPE)) {
        return JFLOAT_ID;
    } else if ((*env)->IsSameObject(env, clazz, JBYTE_TYPE)) {
        return JBYTE_ID;
    } else if ((*env)->IsSameObject(env, clazz, JSHORT_TYPE)) {
        return JSHORT_ID;
    } else if ((*env)->IsSameObject(env, clazz, JCHAR_TYPE)) {
        return JCHAR_ID;
    }

    // void type
    if ((*env)->IsSameObject(env, clazz, JVOID_TYPE)) {
        return JVOID_ID;
    }

    msg = malloc(sizeof(char) * 200);
    memset(msg, '\0', 200);

    classname = JavaClass_getName(env, clazz);
    cname = JcpString_FromJString(env, classname);
    sprintf(msg, "Failed to get the object id to the class %s.", cname);
    JcpString_Clear(env, classname, cname);

    JcpPyErr_ThrowMsg(env, msg);
    free(msg);

    return -1;
}

// ---------------------------------  Java object to Python object ---------------------------------

/* Function to returns the match degree of the PyObject and the jclass */

int
JcpPyObject_match(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    int object_id;

    object_id = JcpJObject_GetObjectId(env, clazz);

    if (PyBool_Check(pyobject)) {
        switch (object_id) {
            case JBOOLEAN_ID:
                return 4;
            case JOBJECT_ID:
                if ((*env)->IsSameObject(env, JBOOLEAN_OBJ_TYPE, clazz)) {
                    return 3;
                } else if ((*env)->IsSameObject(env, JOBJECT_TYPE, clazz)) {
                    return 1;
                }else if ((*env)->IsAssignableFrom(env, JBOOLEAN_OBJ_TYPE, clazz)) {
                    return 2;
                }
        }
    } else if (PyLong_CheckExact(pyobject)) {
        switch (object_id) {
            case JLONG_ID:
                return 10;
            case JINT_ID:
                return 9;
            case JDOUBLE_ID:
                return 8;
            case JFLOAT_ID:
                return 7;
            case JSHORT_ID:
                return 6;
            case JBYTE_ID:
                return 5;
            case JOBJECT_ID:
                if ((*env)->IsSameObject(env, JLONG_OBJ_TYPE, clazz)) {
                    return 4;
                } else if ((*env)->IsSameObject(env, JINT_OBJ_TYPE, clazz)) {
                    return 3;
                } else if ((*env)->IsSameObject(env, JOBJECT_TYPE, clazz)) {
                    return 1;
                } else if ((*env)->IsAssignableFrom(env, JLONG_OBJ_TYPE, clazz)) {
                    return 2;
                }
        }
    } else if (PyFloat_CheckExact(pyobject)) {
        switch (object_id) {
            case JDOUBLE_ID:
                return 6;
            case JFLOAT_ID:
                return 5;
            case JOBJECT_ID:
                if ((*env)->IsSameObject(env, JDOUBLE_OBJ_TYPE, clazz)) {
                    return 4;
                } else if ((*env)->IsSameObject(env, JFLOAT_OBJ_TYPE, clazz)) {
                    return 3;
                } else if ((*env)->IsSameObject(env, JOBJECT_TYPE, clazz)) {
                    return 1;
                } else if ((*env)->IsAssignableFrom(env, JDOUBLE_OBJ_TYPE, clazz)) {
                    return 2;
                }
        }
    } else if (PyUnicode_Check(pyobject)) {
        switch (object_id) {
            case JSTRING_ID:
                return 2;
            case JOBJECT_ID:
                if ((*env)->IsAssignableFrom(env, JSTRING_TYPE, clazz)) {
                    return 1;
                }
        }
    } else if (PyBytes_CheckExact(pyobject)) {
        if (object_id == JBYTES_ID) {
            return 2;
        } else if ((*env)->IsAssignableFrom(env, JBYTE_ARRAY_TYPE, clazz)) {
            return 1;
        }
    } else if (PyList_CheckExact(pyobject)) {
        if (object_id == JLIST_ID) {
            return 2;
        } else if ((*env)->IsAssignableFrom(env, JLIST_TYPE, clazz)) {
            return 1;
        }
    } else if (PyDict_CheckExact(pyobject)) {
        if (object_id == JMAP_ID) {
            return 2;
        } else if ((*env)->IsAssignableFrom(env, JMAP_TYPE, clazz)) {
            return 1;
        }
    } else if (PyTuple_CheckExact(pyobject)) {
        if (object_id == JARRAY_ID) {
            return 2;
        } else if ((*env)->IsSameObject(env, clazz, JOBJECT_TYPE)) {
            return 1;
        }
    }
    else if (PyJObject_Check(pyobject)) {
        if ((*env)->IsInstanceOf(env, ((PyJObject*) pyobject)->object, clazz)) {
            return 1;
        }
    }

    return 0;
}


/* Function to return a Python Object from a Java Object */

PyObject *
JcpPyObject_FromJObject(JNIEnv* env, jobject value)
{

    jstring classname;
    const char* cname;
    char* msg;
    jclass clazz = NULL;

    PyObject* result = NULL;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    clazz = (*env)->GetObjectClass(env, value);

    if ((*env)->IsSameObject(env, clazz, JSTRING_TYPE)) {
        result = JcpPyString_FromJString(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JBOOLEAN_OBJ_TYPE)) {
        result = JcpPyBool_FromJBoolean(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JBYTE_ARRAY_TYPE)) {
        result = JcpPyBytes_FromJByteArray(env, value);
    } else if ((*env)->IsAssignableFrom(env, clazz, JNUMBER_TYPE)) {
        if ((*env)->IsSameObject(env, clazz, JLONG_OBJ_TYPE)) {
            result = JcpPyInt_FromJLong(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JINT_OBJ_TYPE)) {
            result = JcpPyInt_FromJInteger(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JDOUBLE_OBJ_TYPE)) {
            result = JcpPyFloat_FromJDouble(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JFLOAT_OBJ_TYPE)) {
            result = JcpPyFloat_FromJFloat(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JBYTE_OBJ_TYPE)) {
            result = JcpPyInt_FromJByte(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JSHORT_OBJ_TYPE)) {
            result = JcpPyInt_FromJShort(env, clazz);
        } else if ((*env)->IsSameObject(env, clazz, JBIGDECIMAL_TYPE)) {
            result = JcpPyDecimal_FromJBigDecimal(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JBIGINTEGER_TYPE)) {
            result = JcpPyDecimal_FromJBigInteger(env, value);
        } else {
            msg = malloc(sizeof(char) * 200);
            memset(msg, '\0', 200);

            classname = JavaClass_getName(env, clazz);
            cname = JcpString_FromJString(env, classname);
            sprintf(msg, "Unknown Number class %s.", cname);
            JcpString_Clear(env, classname, cname);

            JcpPyErr_ThrowMsg(env, msg);
            free(msg);
        }
    } else if (JavaClass_isArray(env, clazz)) {
        if ((*env)->IsSameObject(env, clazz, JBOOLEAN_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJBooleanArray(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JSHORT_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJShortArray(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JINT_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJIntArray(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JLONG_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJLongArray(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JFLOAT_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJFloatArray(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JDOUBLE_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJDoubleArray(env, value);
        } else if ((*env)->IsInstanceOf(env, value, JOBJECT_ARRAY_TYPE)) {
            result = JcpPyTuple_FromJObjectArray(env, value);
        } else {
            msg = malloc(sizeof(char) * 200);
            memset(msg, '\0', 200);

            classname = JavaClass_getName(env, clazz);
            cname = JcpString_FromJString(env, classname);
            sprintf(msg, "Unknown Array class %s.", cname);
            JcpString_Clear(env, classname, cname);

            JcpPyErr_ThrowMsg(env, msg);
            free(msg);
        }
    } else if ((*env)->IsAssignableFrom(env, clazz, JLIST_TYPE)) {
        result = JcpPyList_FromJListObject(env, value);
    } else if ((*env)->IsAssignableFrom(env, clazz, JMAP_TYPE)) {
        result = JcpPyDict_FromJMap(env, value);
    } else if ((*env)->IsSameObject(env, clazz, JCHAR_OBJ_TYPE)) {
        result = JcpPyString_FromJChar(env, value);
    } else if ((*env)->IsAssignableFrom(env, clazz, JUTILDATE_TYPE)) {
        if ((*env)->IsSameObject(env, clazz, JSQLDATE_TYPE)) {
            result = JcpPyDate_FromJSqlDate(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JSQLTIME_TYPE)) {
            result = JcpPyTime_FromJSqlTime(env, value);
        } else if ((*env)->IsSameObject(env, clazz, JSQLTIMESTAMP_TYPE)) {
            result = JcpPyDateTime_FromJSqlTimestamp(env, value);
        } else {
            msg = malloc(sizeof(char) * 200);
            memset(msg, '\0', 200);

            classname = JavaClass_getName(env, clazz);
            cname = JcpString_FromJString(env, classname);
            sprintf(msg, "Unknown java/util/Date class %s.", cname);
            JcpString_Clear(env, classname, cname);

            JcpPyErr_ThrowMsg(env, msg);
            free(msg);
        }
    } else if ((*env)->IsAssignableFrom(env, clazz, JCOLLECTION_TYPE)) {
        result = JcpPyJCollection_New(env, value, clazz);
    } else if ((*env)->IsAssignableFrom(env, clazz, JITERABLE_TYPE)) {
        result = JcpPyJIterable_New(env, value, clazz);
    } else if ((*env)->IsAssignableFrom(env, clazz, JITERATOR_TYPE)) {
        result = JcpPyJIterator_New(env, value, clazz);
    } else if ((*env)->IsAssignableFrom(env, clazz, JPYOBJECT_TYPE)) {
        result = (PyObject*) JavaPyObject_GetPyobject(env, value);
    } else {
        result = JcpPyJObject_New(env, &PyJObject_Type, value, clazz);
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

/* Function to return a Python String from a jchar value */
PyObject*
JcpPyString_FromChar(jchar value)
{
    Py_UCS2 c = (Py_UCS2) value;
    return PyUnicode_FromKindAndData(PyUnicode_2BYTE_KIND, &c, 1);
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

PyObject *
JcpPyString_FromJChar(JNIEnv* env, jobject value)
{
    jchar c;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    c = JavaCharacter_charValue(env, value);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }

    return JcpPyString_FromChar(c);
}

// ------------------------------------------------------------------------------------


/* Function to return a Python bytes from a Java byte array */

PyObject*
JcpPyBytes_FromJByteArray(JNIEnv* env, jbyteArray value)
{

    int length;

    jbyte* bytes;

    PyObject* result;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    length = (*env)->GetArrayLength(env, value);
    bytes = (*env)->GetByteArrayElements(env, value, 0);
    result = PyBytes_FromStringAndSize((char*) bytes, length);
    (*env)->ReleaseByteArrayElements(env, value, bytes, JNI_ABORT);

    return result;
}

/* Function to return a Python Tuple from a Java boolean array */


JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJBooleanArray(JNIEnv* env, jbooleanArray value)
{
    int length;

    jboolean* booleans;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    booleans = (*env)->GetBooleanArrayElements(env, value, 0);

    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        PyTuple_SetItem(result, i, JcpPyBool_FromLong(booleans[i]));
    }

    return result;
}


/* Function to return a Python Tuple from a Java short array */

JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJShortArray(JNIEnv* env, jshortArray value)
{
    int length;

    jshort* shorts;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    shorts = (*env)->GetShortArrayElements(env, value, 0);

    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        PyTuple_SetItem(result, i, JcpPyInt_FromInt(shorts[i]));
    }

    return result;
}


/* Function to return a Python Tuple from a Java int array */

JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJIntArray(JNIEnv* env, jintArray value)
{
    int length;

    jint* ints;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    ints = (*env)->GetIntArrayElements(env, value, 0);

    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        PyTuple_SetItem(result, i, JcpPyInt_FromInt(ints[i]));
    }

    return result;
}


/* Function to return a Python Tuple from a Java long array */

JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJLongArray(JNIEnv* env, jlongArray value)
{
    int length;

    jlong* longs;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    longs = (*env)->GetLongArrayElements(env, value, 0);

    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        PyTuple_SetItem(result, i, JcpPyInt_FromLong(longs[i]));
    }

    return result;
}


/* Function to return a Python Tuple from a Java float array */

JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJFloatArray(JNIEnv* env, jfloatArray value)
{
    int length;

    jfloat* floats;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    floats = (*env)->GetFloatArrayElements(env, value, 0);

    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        PyTuple_SetItem(result, i, JcpPyFloat_FromDouble(floats[i]));
    }

    return result;
}


/* Function to return a Python Tuple from a Java double array */

JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJDoubleArray(JNIEnv* env, jdoubleArray value)
{
    int length;

    jdouble* doubles;

    PyObject* result;

    length = (*env)->GetArrayLength(env, value);
    doubles = (*env)->GetDoubleArrayElements(env, value, 0);

    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        PyTuple_SetItem(result, i, JcpPyFloat_FromDouble(doubles[i]));
    }

    return result;
}


/* Function to return a Python Tuple from a Java object array */

PyObject*
JcpPyTuple_FromJObjectArray(JNIEnv* env, jobjectArray value)
{

    int length;

    jobject element;

    PyObject* result;

    if (value == NULL) {
        Py_RETURN_NONE;
    }

    length = (*env)->GetArrayLength(env, value);
    result = PyTuple_New(length);

    for (int i = 0; i < length; i++) {
        element = (*env)->GetObjectArrayElement(env, value, i);
        PyTuple_SetItem(result, i, JcpPyObject_FromJObject(env, element));
        (*env)->DeleteLocalRef(env, element);
    }

    return result;
}

/* Function to return a Python List from a Java List object */
PyObject*
JcpPyList_FromJListObject(JNIEnv* env, jobject value) {

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
        JcpPyErr_ThrowMsg(env, "Failed to import `decimal` module.");
        return NULL;
    }

    clazz = PyObject_GetAttrString(module, (char *) "Decimal");

    Py_DECREF(module);

    if (clazz == NULL) {
        JcpPyErr_ThrowMsg(env, "Failed to import `decimal.Decimal` class.");
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

/* Function to return a Python Decimal from a Java BigInteger object */

PyObject*
JcpPyDecimal_FromJBigInteger(JNIEnv* env, jobject value)
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
        JcpPyErr_ThrowMsg(env, "Failed to import `decimal` module.");
        return NULL;
    }

    clazz = PyObject_GetAttrString(module, (char *) "Decimal");

    Py_DECREF(module);

    if (clazz == NULL) {
        JcpPyErr_ThrowMsg(env, "Failed to import `decimal.Decimal` class.");
        return NULL;
    }

    str = JcpPyString_FromJString(env, JavaBigInteger_toString(env, value));

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
    } else if (PyJObject_Check(pyobject)) {
        return (*env)->NewLocalRef(env, ((PyJObject*) pyobject)->object);
    } else if (PyGen_CheckExact(pyobject)) {
        return JcpPyGenerator_AsJObject(env, pyobject);
    } else if (PyBool_Check(pyobject)) {
        return JcpPyBool_AsJObject(env, pyobject, clazz);
    } else if (PyLong_CheckExact(pyobject)) {
        return JcpPyInt_AsJObject(env, pyobject, clazz);
    } else if (PyFloat_CheckExact(pyobject)) {
        return JcpPyFloat_AsJObject(env, pyobject, clazz);
    } else if (PyBytes_CheckExact(pyobject)) {
        return JcpPyBytes_AsJObject(env, pyobject);
    } else if (PyList_CheckExact(pyobject)) {
        return JcpPyList_AsJObject(env, pyobject);
    } else if (PyTuple_CheckExact(pyobject)) {
        return JcpPyTuple_AsJObject(env, pyobject, clazz);
    } else if (PyDict_CheckExact(pyobject)) {
        return JcpPyDict_AsJObject(env, pyobject);
    } else if (JcpPyDecimal_Check(pyobject) == 1) {
        return JcpPyDecimal_AsJObject(env, pyobject, clazz);
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
        } else {
            return JcpPyObject_AsJPyObject(env, pyobject);
        }
    }
}


/* Function to return a jvalue from a Python Object */

JcpAPI_FUNC(jvalue) JcpPyObject_AsJValue(JNIEnv* env, PyObject* pyobject, jclass clazz)
{
    int object_id;
    jvalue result;

    object_id = JcpJObject_GetObjectId(env, clazz);

    switch (object_id) {
        case JSTRING_ID:
            result.l = JcpPyString_AsJString(env, pyobject);
            break;
        case JOBJECT_ID:
            if (PyJObject_Check(pyobject)) {
                result.l = (*env)->NewLocalRef(env, ((PyJObject*) pyobject)->object);
            } else {
                result.l = JcpPyObject_AsJObject(env, pyobject, clazz);
            }
            break;
        case JBYTES_ID:
            result.l = JcpPyBytes_AsJObject(env, pyobject);
            break;
        case JLIST_ID:
            result.l = JcpPyList_AsJObject(env, pyobject);
            break;
        case JMAP_ID:
            result.l = JcpPyDict_AsJObject(env, pyobject);
            break;
        case JINT_ID:
            result.i = JcpPyInt_AsJInt(pyobject);
            break;
        case JDOUBLE_ID:
            result.d = JcpPyFloat_AsJDouble(pyobject);
            break;
        case JFLOAT_ID:
            result.f = JcpPyFloat_AsJFloat(pyobject);
            break;
        case JLONG_ID:
            result.j = JcpPyInt_AsJLong(pyobject);
            break;
        case JBOOLEAN_ID:
            result.z = JcpPyBool_AsJBoolean(pyobject);
            break;
        case JBYTE_ID:
            result.b = JcpPyInt_AsJByte(pyobject);
            break;
        case JSHORT_ID:
            result.s = JcpPyInt_AsJShort(pyobject);
            break;
        case JARRAY_ID:
            result.l = JcpPyTuple_AsJObject(env, pyobject, clazz);
            break;
        default:
            PyErr_Format(PyExc_TypeError,
                        "Unrecognized class %s.",
                        JcpString_FromJString(env, JavaClass_getName(env, clazz)));
            result.l = NULL;
    }

    return result;
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

    jstring classname;
    const char* cname;
    char* msg;
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
        msg = malloc(sizeof(char) * 200);
        memset(msg, '\0', 200);

        classname = JavaClass_getName(env, clazz);
        cname = JcpString_FromJString(env, classname);
        sprintf(msg, "Unknown Number class %s.", cname);
        JcpString_Clear(env, classname, cname);

        JcpPyErr_ThrowMsg(env, msg);
        free(msg);
    }

    return result;
}


/* Function to return a Java Object from a Python Int value */

jobject
JcpPyInt_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jstring classname;
    const char* cname;
    char* msg;
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
        msg = malloc(sizeof(char) * 200);
        memset(msg, '\0', 200);

        classname = JavaClass_getName(env, clazz);
        cname = JcpString_FromJString(env, classname);
        sprintf(msg, "Unknown Number class %s.", cname);
        JcpString_Clear(env, classname, cname);

        JcpPyErr_ThrowMsg(env, msg);
        free(msg);
    }

    return result;
}


/* Function to return a Java Object from a Python Float value */

jobject
JcpPyFloat_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    jstring classname;
    const char* cname;
    char* msg;
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
        msg = malloc(sizeof(char) * 200);
        memset(msg, '\0', 200);

        classname = JavaClass_getName(env, clazz);
        cname = JcpString_FromJString(env, classname);
        sprintf(msg, "Unknown Number class %s.", cname);
        JcpString_Clear(env, classname, cname);

        JcpPyErr_ThrowMsg(env, msg);
        free(msg);
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
JcpPyBytes_AsJObject(JNIEnv* env, PyObject* pyobject)
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

    jstring classname;
    const char* cname;
    char* msg;
    jobject result = NULL;

    result = JcpPyString_AsJString(env, pyobject);

    if (!result) {
        msg = malloc(sizeof(char) * 200);
        memset(msg, '\0', 200);

        classname = JavaClass_getName(env, clazz);
        cname = JcpString_FromJString(env, classname);
        sprintf(msg, "Unknown Number class %s.", cname);
        JcpString_Clear(env, classname, cname);

        JcpPyErr_ThrowMsg(env, msg);
        free(msg);
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

/* Function to return a Java ArrayList from a Python List object */

jobject
JcpPyList_AsJObject(JNIEnv* env, PyObject* pyobject)
{

    int length;
    jobject element;
    jobject list;

    length= PyList_Size(pyobject);

    list = JavaList_NewArrayList(env);

    for (int i = 0; i < length; i++) {
        element = JcpPyObject_AsJObject(env, PyList_GetItem(pyobject, i), JOBJECT_TYPE);
        JavaList_Add(env, list, element);
    }

    return list;
}


/* Function to return a Java Array from a Python Tuple object */

jobject
JcpPyTuple_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{
    jstring classname;
    const char* cname;
    char* msg;
    int length;
    jobjectArray array = NULL;
    jobject element;

    length = PyTuple_Size(pyobject);

    if ((*env)->IsSameObject(env, clazz, JOBJECT_TYPE)) {

        array = (*env)->NewObjectArray(env, length, JOBJECT_TYPE, NULL);

        for (int i = 0; i < length; i++) {
            element = JcpPyObject_AsJObject(env, PyTuple_GetItem(pyobject, i), JOBJECT_TYPE);
            (*env)->SetObjectArrayElement(env, array, i, element);
        }

    } else if ((*env)->IsSameObject(env, clazz, JINT_ARRAY_TYPE)) {
        array = (*env)->NewIntArray(env, length);
        jint* ints = (*env)->GetIntArrayElements(env, array, 0);

        for (int i = 0; i < length; i++) {
            ints[i] = JcpPyInt_AsJInt(PyTuple_GetItem(pyobject, i));
        }

        (*env)->ReleaseIntArrayElements(env, array, ints, 0);

    } else if ((*env)->IsSameObject(env, clazz, JDOUBLE_ARRAY_TYPE)) {

        array = (*env)->NewDoubleArray(env, length);
        jdouble* doubles = (*env)->GetDoubleArrayElements(env, (jdoubleArray) array, 0);

        for (int i = 0; i < length; i++) {
            doubles[i] = JcpPyFloat_AsJDouble(PyTuple_GetItem(pyobject, i));
        }

        (*env)->ReleaseDoubleArrayElements(env, array, doubles, 0);

    } else if ((*env)->IsSameObject(env, clazz, JLONG_ARRAY_TYPE)) {

        array = (*env)->NewLongArray(env, length);
        jlong* longs = (*env)->GetLongArrayElements(env, (jlongArray) array, 0);

        for (int i = 0; i < length; i++) {
            longs[i] = JcpPyInt_AsJLong(PyTuple_GetItem(pyobject, i));
        }

        (*env)->ReleaseLongArrayElements(env, array, longs, 0);

    } else if ((*env)->IsSameObject(env, clazz, JFLOAT_ARRAY_TYPE)) {

        array = (*env)->NewFloatArray(env, length);
        jfloat* floats = (*env)->GetFloatArrayElements(env, (jfloatArray) array, 0);

        for (int i = 0; i < length; i++) {
            floats[i] = JcpPyFloat_AsJFloat(PyTuple_GetItem(pyobject, i));
        }

        (*env)->ReleaseFloatArrayElements(env, array, floats, 0);

    } else if ((*env)->IsSameObject(env, clazz, JBOOLEAN_ARRAY_TYPE)) {

        array = (*env)->NewBooleanArray(env, length);
        jboolean* booleans = (*env)->GetBooleanArrayElements(env, (jbooleanArray) array, 0);

        for (int i = 0; i < length; i++) {
            booleans[i] = JcpPyBool_AsJBoolean(PyTuple_GetItem(pyobject, i));
        }

        (*env)->ReleaseBooleanArrayElements(env, array, booleans, 0);

    } else if ((*env)->IsSameObject(env, clazz, JSHORT_ARRAY_TYPE)) {

        array = (*env)->NewShortArray(env, length);
        jshort* shorts = (*env)->GetShortArrayElements(env, (jshortArray) array, 0);

        for (int i = 0; i < length; i++) {
            shorts[i] = JcpPyInt_AsJShort(PyTuple_GetItem(pyobject, i));
        }

        (*env)->ReleaseShortArrayElements(env, array, shorts, 0);

    } else if (JavaClass_isArray(env, clazz)) {
        jclass elementType = JavaClass_getComponentType(env, clazz);

        array = (*env)->NewObjectArray(env, length, elementType, NULL);

        for (int i = 0; i < length; i++) {
            element = JcpPyObject_AsJObject(env, PyTuple_GetItem(pyobject, i), elementType);
            (*env)->SetObjectArrayElement(env, array, i, element);
        }
    } else {
        msg = malloc(sizeof(char) * 200);
        memset(msg, '\0', 200);

        classname = JavaClass_getName(env, clazz);
        cname = JcpString_FromJString(env, classname);
        sprintf(msg, "Unknown Number class %s.", cname);
        JcpString_Clear(env, classname, cname);

        JcpPyErr_ThrowMsg(env, msg);
        free(msg);
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

    clazz = PyObject_GetAttrString(module, (char *) "Decimal");

    Py_DECREF(module);

    if (clazz == NULL) {
        PyErr_Format(PyExc_RuntimeError, "Failed to import `decimal.Decimal` class.");
        return 0;
    }

    result = PyObject_IsInstance(pyobject, clazz);
    Py_DECREF(clazz);

    return result;
}


/* Function to return a Java BigDecimal/BigInteger object from a Python Decimal object */

jobject
JcpPyDecimal_AsJObject(JNIEnv* env, PyObject* pyobject, jclass clazz)
{

    PyObject* str;

    jobject result = NULL;

    str = PyObject_Str(pyobject);

    if (str) {

        if ((*env)->IsSameObject(env, clazz, JBIGINTEGER_TYPE)) {
            result = JavaBigInteger_New(env, JcpPyString_AsJString(env, str));
        } else {
            result = JavaBigDecimal_New(env, JcpPyString_AsJString(env, str));
        }

        Py_DECREF(str);
    }

    return result;
}


/* Function to return a Java Generator Object from a Python Generator object */

jobject JcpPyGenerator_AsJObject(JNIEnv* env, PyObject* pyobject)
{
    PyObject* iter;

    jobject jiter;

    iter = PyObject_GetIter(pyobject);

    jiter = JavaPyIterator_New(env, (intptr_t) JcpThread_Get(), (intptr_t) iter);
    return jiter;
}


/* Function to return a Java PyObject from a Python object */

jobject JcpPyObject_AsJPyObject(JNIEnv* env, PyObject* pyobject)
{
    Py_INCREF(pyobject);
    return JavaPyObject_New(env, (intptr_t) JcpThread_Get(), (intptr_t) pyobject);
}