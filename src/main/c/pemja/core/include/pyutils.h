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
#ifndef _Included_pyutils
#define _Included_pyutils

#define CLASS_TABLE(F) \
    F(JBOOLEAN_OBJ_TYPE, "java/lang/Boolean") \
    F(JNUMBER_TYPE, "java/lang/Number") \
    F(JCHARSEQUENCE_TYPE, "java/lang/CharSequence") \
    F(JBYTE_OBJ_TYPE, "java/lang/Byte") \
    F(JSHORT_OBJ_TYPE, "java/lang/Short") \
    F(JINT_OBJ_TYPE, "java/lang/Integer") \
    F(JLONG_OBJ_TYPE, "java/lang/Long") \
    F(JFLOAT_OBJ_TYPE, "java/lang/Float") \
    F(JDOUBLE_OBJ_TYPE, "java/lang/Double") \
    F(JCHAR_OBJ_TYPE, "java/lang/Character") \
    F(JSTRING_TYPE, "java/lang/String") \
    F(JBIGDECIMAL_TYPE, "java/math/BigDecimal") \
    F(JBIGINTEGER_TYPE, "java/math/BigInteger") \
    F(JBOOLEAN_ARRAY_TYPE, "[Z") \
    F(JBYTE_ARRAY_TYPE, "[B") \
    F(JCHAR_ARRAY_TYPE, "[C") \
    F(JSHORT_ARRAY_TYPE, "[S") \
    F(JINT_ARRAY_TYPE, "[I") \
    F(JLONG_ARRAY_TYPE, "[J") \
    F(JFLOAT_ARRAY_TYPE, "[F") \
    F(JDOUBLE_ARRAY_TYPE, "[D") \
    F(JSTRING_ARRAY_TYPE, "[Ljava/lang/String;") \
    F(JOBJECT_ARRAY_TYPE, "[Ljava/lang/Object;") \
    F(JUTILDATE_TYPE, "java/util/Date") \
    F(JSQLDATE_TYPE, "java/sql/Date") \
    F(JSQLTIME_TYPE, "java/sql/Time") \
    F(JSQLTIMESTAMP_TYPE, "java/sql/Timestamp") \
    F(JLOCALDATE_TYPE, "java/time/LocalDate") \
    F(JLOCALTIME_TYPE, "java/time/LocalTime") \
    F(JLOCALDATETIME_TYPE, "java/time/LocalDateTime") \
    F(JITERABLE_TYPE, "java/lang/Iterable") \
    F(JITERATOR_TYPE, "java/util/Iterator") \
    F(JCOLLECTION_TYPE, "java/util/Collection") \
    F(JLIST_TYPE, "java/util/List") \
    F(JARRAYLIST_TYPE, "java/util/ArrayList") \
    F(JMAP_TYPE, "java/util/Map") \
    F(JHASHMAP_TYPE, "java/util/HashMap") \
    F(JMAP_ENTRY_TYPE, "java/util/Map$Entry") \
    F(JILLEGAL_STATE_EXEC_TYPE, "java/lang/IllegalStateException") \
    F(JNOSUCHELEMENT_EXEC_TYPE, "java/util/NoSuchElementException") \
    F(JPYTHONEXCE_TYPE, "pemja/core/PythonException") \
    F(JPYITERPRETER_TYPE, "pemja/core/object/PyIterator") \
    F(JPYOBJECT_TYPE, "pemja/core/object/PyObject") \
    F(JTHROWABLE_TYPE, "java/lang/Throwable") \
    F(JSTACK_TRACE_ELEMENT_TYPE, "java/lang/StackTraceElement") \
    F(JCONSTRUCTOR_TYPE, "java/lang/reflect/Constructor") \
    F(JFIELD_TYPE, "java/lang/reflect/Field") \
    F(JMETHOD_TYPE, "java/lang/reflect/Method") \
    F(JMEMBER_TYPE, "java/lang/reflect/Member") \
    F(JMODIFIER_TYPE, "java/lang/reflect/Modifier") \
    F(JCLASS_TYPE, "java/lang/Class") \
    F(JOBJECT_TYPE, "java/lang/Object")

// Define primitive class type.
extern jclass JBOOLEAN_TYPE;
extern jclass JBYTE_TYPE;
extern jclass JSHORT_TYPE;
extern jclass JINT_TYPE;
extern jclass JLONG_TYPE;
extern jclass JFLOAT_TYPE;
extern jclass JDOUBLE_TYPE;
extern jclass JCHAR_TYPE;

// define void class type
extern jclass JVOID_TYPE;

// Define an extern variable for everything in the class table.
#define DEFINE_CLASS_GLOBAL(var, name) extern jclass var;
CLASS_TABLE(DEFINE_CLASS_GLOBAL)

#define CACHE_CLASS(var, name) \
    if(var == NULL) { \
        clazz = (*env)->FindClass(env, name); \
        var = (*env)->NewGlobalRef(env, clazz); \
        (*env)->DeleteLocalRef(env, clazz); \
    } \

#define UNREF_CACHE_CLASS(var, name) \
    if(var != NULL) { \
        (*env)->DeleteGlobalRef(env, var);\
        var = NULL; \
    }\

#define CACHE_PRIMITIVE_CLASS(primitive, primitive_array) \
    if (primitive == NULL) { \
        clazz = JavaClass_getComponentType(env, primitive_array); \
        primitive = (*env)->NewGlobalRef(env, clazz); \
        (*env)->DeleteLocalRef(env, clazz); \
    } \

// ----------------- Upper and lower bounds of Number type. --------------------------
#define JBYTE_MIN  -128
#define JBYTE_MAX   127

#define JSHORT_MIN -32768
#define JSHORT_MAX  32767

// Some compilers overflow parsing -2147483648 so derive it from JINT_MAX.
#define JINT_MIN    (-1 * JINT_MAX - 1)
#define JINT_MAX    2147483647

#define JLONG_MAX   9223372036854775807
// Some compilers overflow parsing -9223372036854775808, so derive it from JLONG_MAX.
#define JLONG_MIN   (-1 * JLONG_MAX - 1)

#define JCHAR_MAX   0xFFFF

//  -------------------------------------------------------------------------------------

//  ------------------------------- Java Type ID ----------------------------------------
#define JBOOLEAN_ID 0
#define JBYTE_ID    1
#define JSHORT_ID   2
#define JINT_ID     3
#define JLONG_ID    4
#define JFLOAT_ID   5
#define JDOUBLE_ID  6
#define JCHAR_ID    7
#define JSTRING_ID  8
#define JVOID_ID    9
#define JOBJECT_ID  10
#define JBYTES_ID   11
#define JLIST_ID    12
#define JMAP_ID     13
#define JARRAY_ID   14

//  -------------------------------------------------------------------------------------

/* Function to cache java classes in CLASS_TABLE */
JcpAPI_FUNC(void) Jcp_CacheClasses(JNIEnv *env);

/* Function to unref java classes in CLASS_TABLE */
JcpAPI_FUNC(void) Jcp_UnRefCacheClasses(JNIEnv *env);

/* Function to return a const char* from a Java String Object */
JcpAPI_FUNC(const char*) JcpString_FromJString(JNIEnv*, jstring);

/* Function to release the memory of const char* converted by Java String Object */
JcpAPI_FUNC(void) JcpString_Clear(JNIEnv*, jstring, const char*);

/* Function to get the Object Id from the Java Class */
JcpAPI_FUNC(int) JcpJObject_GetObjectId(JNIEnv*, jclass);

// ----------------------- Java object to Python object ---------------------------------

/* Function to returns the match degree of the PyObject and the jclass */
JcpAPI_FUNC(int) JcpPyObject_match(JNIEnv*, PyObject*, jclass);

/* Function to return a Python Object from a Java Object */
JcpAPI_FUNC(PyObject*) JcpPyObject_FromJObject(JNIEnv*, jobject);

/* Functions to return a Python primitive object from a C primitive value */
JcpAPI_FUNC(PyObject*) JcpPyBool_FromLong(long);
JcpAPI_FUNC(PyObject*) JcpPyInt_FromInt(int);
JcpAPI_FUNC(PyObject*) JcpPyInt_FromLong(long);
JcpAPI_FUNC(PyObject*) JcpPyFloat_FromDouble(double);
JcpAPI_FUNC(PyObject*) JcpPyString_FromChar(jchar);

/* Function to return a Python String object from a Java String object */
JcpAPI_FUNC(PyObject*) JcpPyString_FromJString(JNIEnv*, jstring);

/* Functions to return a Python primitive object from a Java object */
JcpAPI_FUNC(PyObject*) JcpPyBool_FromJBoolean(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyInt_FromJByte(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyInt_FromJShort(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyInt_FromJInteger(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyInt_FromJLong(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyFloat_FromJFloat(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyFloat_FromJDouble(JNIEnv*, jobject);
JcpAPI_FUNC(PyObject*) JcpPyString_FromJChar(JNIEnv*, jobject);

/* Function to return a Python bytes from a Java byte array */
JcpAPI_FUNC(PyObject*) JcpPyBytes_FromJByteArray(JNIEnv*, jbyteArray);

/* Function to return a Python Tuple from a Java boolean array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJBooleanArray(JNIEnv*, jbooleanArray);

/* Function to return a Python Tuple from a Java short array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJShortArray(JNIEnv*, jshortArray);

/* Function to return a Python Tuple from a Java int array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJIntArray(JNIEnv*, jintArray);

/* Function to return a Python Tuple from a Java long array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJLongArray(JNIEnv*, jlongArray);

/* Function to return a Python Tuple from a Java float array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJFloatArray(JNIEnv*, jfloatArray);

/* Function to return a Python Tuple from a Java double array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJDoubleArray(JNIEnv*, jdoubleArray);

/* Function to return a Python Tuple from a Java object array */
JcpAPI_FUNC(PyObject*) JcpPyTuple_FromJObjectArray(JNIEnv*, jobjectArray);

/* Function to return a Python List from a Java List object */
JcpAPI_FUNC(PyObject*) JcpPyList_FromJListObject(JNIEnv*, jobject);

/* Function to return a Python Dict from a Java Map object */
JcpAPI_FUNC(PyObject*) JcpPyDict_FromJMap(JNIEnv*, jobject);

/* Function to return a Python Date from a Java Sql Date object */
JcpAPI_FUNC(PyObject*) JcpPyDate_FromJSqlDate(JNIEnv*, jobject);

/* Function to return a Python Time from a Java Sql Time object */
JcpAPI_FUNC(PyObject*) JcpPyTime_FromJSqlTime(JNIEnv*, jobject);

/* Function to return a Python DateTime from a Java Sql Timestamp object */
JcpAPI_FUNC(PyObject*) JcpPyDateTime_FromJSqlTimestamp(JNIEnv*, jobject);

/* Function to return a Python Decimal from a Java BigDecimal object */
JcpAPI_FUNC(PyObject*) JcpPyDecimal_FromJBigDecimal(JNIEnv*, jobject);

/* Function to return a Python Decimal from a Java BigInteger object */
JcpAPI_FUNC(PyObject*) JcpPyDecimal_FromJBigInteger(JNIEnv* env, jobject value);

// ------------------------------------------------------------------------------------

// -----------------------   Python object to Java object -----------------------------

/* Function to return a Java Object from a Python Object */
JcpAPI_FUNC(jobject) JcpPyObject_AsJObject(JNIEnv*, PyObject*, jclass);

/* Function to return a jvalue from a Python Object */
JcpAPI_FUNC(jvalue) JcpPyObject_AsJValue(JNIEnv*, PyObject*, jclass);

/* Functions to return a Java primitive value from a Python primitive object */
JcpAPI_FUNC(jboolean) JcpPyBool_AsJBoolean(PyObject*);
JcpAPI_FUNC(jbyte) JcpPyInt_AsJByte(PyObject*);
JcpAPI_FUNC(jshort) JcpPyInt_AsJShort(PyObject*);
JcpAPI_FUNC(jint) JcpPyInt_AsJInt(PyObject*);
JcpAPI_FUNC(jlong) JcpPyInt_AsJLong(PyObject*);
JcpAPI_FUNC(jfloat) JcpPyFloat_AsJFloat(PyObject*);
JcpAPI_FUNC(jdouble) JcpPyFloat_AsJDouble(PyObject*);

/* Functions to return a Java primitive wrapper object from a Python primitive object */
JcpAPI_FUNC(jobject) JcpPyBool_AsJObject(JNIEnv*, PyObject*, jclass);
JcpAPI_FUNC(jobject) JcpPyInt_AsJObject(JNIEnv*, PyObject*, jclass);
JcpAPI_FUNC(jobject) JcpPyFloat_AsJObject(JNIEnv*, PyObject*, jclass);

/* Function to return a long from a Python Int Object */
JcpAPI_FUNC(long) JcpPyInt_AsLong(PyObject*);

/* Function to return a long from a Python Int Object */
JcpAPI_FUNC(long long) JcpPyInt_AsLongLong(PyObject*);

/* Function to return a Java Object from a Python bytes value */
JcpAPI_FUNC(jobject) JcpPyBytes_AsJObject(JNIEnv*, PyObject*);

/* Function to return a Java Object from a Python String value */
JcpAPI_FUNC(jobject) JcpPyString_AsJObject(JNIEnv*, PyObject*, jclass);

/* Function to return a Java char value from a Python String Object */
JcpAPI_FUNC(jchar) JcpPyString_AsJChar(PyObject*);

/* Function to return a Java String object from a Python String Object */
JcpAPI_FUNC(jstring) JcpPyString_AsJString(JNIEnv*, PyObject*);

/* Function to return a Java ArrayList from a Python List object */
JcpAPI_FUNC(jobject) JcpPyList_AsJObject(JNIEnv*, PyObject*);

/* Function to return a Java Array from a Python Tuple object */
JcpAPI_FUNC(jobject) JcpPyTuple_AsJObject(JNIEnv*, PyObject*, jclass);

/* Function to return a Java Map from a Python Dict object */
JcpAPI_FUNC(jobject) JcpPyDict_AsJObject(JNIEnv*, PyObject*);

/* Function to return a Java Sql Date object from a Python Date object */
JcpAPI_FUNC(jobject) JcpPyDate_AsJObject(JNIEnv*, PyObject*);

/* Function to return a Java Sql Time object from a Python Time object */
JcpAPI_FUNC(jobject) JcpPyTime_AsJObject(JNIEnv*, PyObject*);

/* Function to return a Java Sql Timestamp object from a Python DateTime object */
JcpAPI_FUNC(jobject) JcpPyDateTime_AsJObject(JNIEnv*, PyObject*);

/* Function to check whether the Python object is Python Decimal object */
JcpAPI_FUNC(int) JcpPyDecimal_Check(PyObject*);

/* Function to return a Java BigDecimal/BigInteger object from a Python Decimal object */
JcpAPI_FUNC(jobject) JcpPyDecimal_AsJObject(JNIEnv*, PyObject*, jclass);

/* Function to return a Java Generator Object from a Python Generator object */
JcpAPI_FUNC(jobject) JcpPyGenerator_AsJObject(JNIEnv*, PyObject*);

/* Function to return a Java PyObject from a Python object */
JcpAPI_FUNC(jobject) JcpPyObject_AsJPyObject(JNIEnv*, PyObject*);

#endif // ifndef _Included_pyutils