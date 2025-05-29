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
#include "java_class/List.h"
#include "python_class/PythonClass.h"

PyObject* JcpPyJList_New(JNIEnv* env, jobject list, jclass clazz) {
  PyObject* self;

  self = JcpPyJObject_New(env, &PyJList_Type, list, clazz);

  return self;
}

static Py_ssize_t pyjlist_len(PyObject* self) {
  JNIEnv* env;

  env = JcpThreadEnv_Get();

  return JavaCollection_size(env, ((PyJObject*)self)->object);
}

static PyObject* pyjlist_concat(PyObject* self, PyObject* o) {
  JNIEnv* env = JcpThreadEnv_Get();

  jobject list =
      JavaList_NewArrayListWithCollection(env, ((PyJObject*)self)->object);

  if (PyJList_CheckExact(o)) {
    JavaList_AddAll(env, list, ((PyJObject*)o)->object);
  } else {
    jobject other_list = JcpPyObject_AsJObject(env, o, JOBJECT_TYPE);
    JavaList_AddAll(env, list, other_list);
    (*env)->DeleteLocalRef(env, other_list);
  }

  return JcpPyJList_New(env, list, JARRAYLIST_TYPE);
}

static PyObject* pyjlist_repeat(PyObject* self, Py_ssize_t n) {
  JNIEnv* env;
  jobject list;

  env = JcpThreadEnv_Get();

  jobject old_list = ((PyJObject*)self)->object;
  list = JavaList_NewArrayListWithCollection(env, old_list);
  for (int i = 0; i < n - 1; i++) {
    JavaList_AddAll(env, list, old_list);
  }

  return JcpPyJList_New(env, list, JARRAYLIST_TYPE);
}

static PyObject* pyjlist_item(PyObject* self, Py_ssize_t index) {
  JNIEnv* env;

  env = JcpThreadEnv_Get();

  return JcpPyObject_FromJObject(
      env, JavaList_Get(env, ((PyJObject*)self)->object, index));
}

static int pyjlist_ass_item(PyObject* self, Py_ssize_t index, PyObject* o) {
  JNIEnv* env;

  env = JcpThreadEnv_Get();

  int32_t len = JavaCollection_size(env, ((PyJObject*)self)->object);

  if (index < 0 || index >= len) {
    PyErr_SetString(PyExc_IndexError, "list assignment index out of range");
    return -1;
  }

  JavaList_Set(env, ((PyJObject*)self)->object, index,
               JcpPyObject_AsJObject(env, o, JOBJECT_TYPE));

  return 0;
}

static int pyjlist_contains(PyObject* self, PyObject* o) {
  JNIEnv* env;
  jobject value;

  env = JcpThreadEnv_Get();

  value = JcpPyObject_AsJObject(env, o, JOBJECT_TYPE);

  return JavaCollection_contains(env, ((PyJObject*)self)->object, value);
}

static PyObject* pyjlist_inplace_concat(PyObject* self, PyObject* o) {
  JNIEnv* env;
  jobject list;

  env = JcpThreadEnv_Get();

  if (PyJList_CheckExact(o)) {
    JavaList_AddAll(env, ((PyJObject*)self)->object, ((PyJObject*)o)->object);
  } else {
    list = JcpPyObject_AsJObject(env, o, JOBJECT_TYPE);
    JavaList_AddAll(env, ((PyJObject*)self)->object, list);
    (*env)->DeleteLocalRef(env, list);
  }
  return self;
}

static PyObject* pyjlist_inplace_repeat(PyObject* self, Py_ssize_t n) {
  JNIEnv* env;

  env = JcpThreadEnv_Get();

  jobject list = ((PyJObject*)self)->object;
  jobject copy_list = JavaList_NewArrayListWithCollection(env, list);
  for (int i = 0; i < n; i++) {
    JavaList_AddAll(env, list, copy_list);
  }
  return self;
}

static PySequenceMethods pyjlist_as_sequence = {
    pyjlist_len,            /* sq_length */
    pyjlist_concat,         /* sq_concat */
    pyjlist_repeat,         /* sq_repeat */
    pyjlist_item,           /* sq_item */
    0,                      /* sq_slice */
    pyjlist_ass_item,       /* sq_ass_item */
    0,                      /* sq_ass_slice */
    pyjlist_contains,       /* sq_contains */
    pyjlist_inplace_concat, /* sq_inplace_concat */
    pyjlist_inplace_repeat, /* sq_inplace_repeat */
};

PyTypeObject PyJList_Type = {
    PyVarObject_HEAD_INIT(NULL, 0) "pemja.PyJList", /* tp_name */
    sizeof(PyJObject),                              /* tp_basicsize */
    0,                                              /* tp_itemsize */
    0,                                              /* tp_dealloc */
    0,                                              /* tp_print */
    0,                                              /* tp_getattr */
    0,                                              /* tp_setattr */
    0,                                              /* tp_reserved */
    0,                                              /* tp_repr */
    0,                                              /* tp_as_number */
    &pyjlist_as_sequence,                           /* tp_as_sequence */
    0,                                              /* tp_as_mapping */
    PyObject_HashNotImplemented,                    /* tp_hash */
    0,                                              /* tp_call */
    0,                                              /* tp_str */
    0,                                              /* tp_getattro */
    0,                                              /* tp_setattro */
    0,                                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,       /* tp_flags */
    "Java List Object",                             /* tp_doc */
    0,                                              /* tp_traverse */
    0,                                              /* tp_clear */
    0,                                              /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,                                              /* tp_iter */
    0,                                              /* tp_iternext */
    0,                                              /* tp_methods */
    0,                                              /* tp_members */
    0,                                              /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
    0,                                              /* tp_init */
    0,                                              /* tp_alloc */
    0,                                              /* tp_new */
};
