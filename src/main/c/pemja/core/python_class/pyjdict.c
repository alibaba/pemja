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
#include "java_class/Map.h"
#include "python_class/PythonClass.h"

PyObject* JcpPyJDict_New(JNIEnv* env, jobject dict, jclass clazz) {
  PyObject* self;

  self = JcpPyJObject_New(env, &PyJDict_Type, dict, clazz);

  return self;
}

static int PyJDict_Contains(PyObject* self, PyObject* key) {
  JNIEnv* env = JcpThreadEnv_Get();

  if (PyJDict_CheckExact(key)) {
    return JavaMap_containsKey(env, ((PyJObject*)self)->object,
                               ((PyJObject*)key)->object);
  } else {
    return JavaMap_containsKey(env, ((PyJObject*)self)->object,
                               JcpPyObject_AsJObject(env, key, JOBJECT_TYPE));
  }
}

static PySequenceMethods pyjdict_as_sequence = {
    0,                /* sq_length */
    0,                /* sq_concat */
    0,                /* sq_repeat */
    0,                /* sq_item */
    0,                /* sq_slice */
    0,                /* sq_ass_item */
    0,                /* sq_ass_slice */
    PyJDict_Contains, /* sq_contains */
    0,                /* sq_inplace_concat */
    0,                /* sq_inplace_repeat */
};

static Py_ssize_t pyjdict_length(PyObject* self) {
  JNIEnv* env = JcpThreadEnv_Get();

  return JavaMap_size(env, ((PyJObject*)self)->object);
}

static PyObject* pyjdict_subscript(PyObject* self, PyObject* key) {
  JNIEnv* env = JcpThreadEnv_Get();

  if (PyJDict_CheckExact(key)) {
    return JcpPyObject_FromJObject(env,
                                   JavaMap_get(env, ((PyJObject*)self)->object,
                                               ((PyJObject*)key)->object));
  } else {
    return JcpPyObject_FromJObject(
        env, JavaMap_get(env, ((PyJObject*)self)->object,
                         JcpPyObject_AsJObject(env, key, JOBJECT_TYPE)));
  }
}

static int pyjdict_ass_sub(PyObject* self, PyObject* key, PyObject* value) {
  JNIEnv* env = JcpThreadEnv_Get();

  if (value == NULL) {
    jobject java_key = JcpPyObject_AsJObject(env, key, JOBJECT_TYPE);
    if (JavaMap_containsKey(env, ((PyJObject*)self)->object, java_key)) {
      JavaMap_remove(env, ((PyJObject*)self)->object, java_key);
    } else {
      return -1;
    }
  } else {
    JavaMap_put(env, ((PyJObject*)self)->object,
                JcpPyObject_AsJObject(env, key, JOBJECT_TYPE),
                JcpPyObject_AsJObject(env, value, JOBJECT_TYPE));
  }

  return 0;
}

static PyMappingMethods pyjdict_as_mapping = {
    pyjdict_length,    /* mp_length */
    pyjdict_subscript, /* mp_subscript */
    pyjdict_ass_sub,   /* mp_ass_subscript */
};

static PyObject* PyJObject_keys(PyObject* self) {
  JNIEnv* env = JcpThreadEnv_Get();
  return JcpPyObject_FromJObject(
      env, JavaMap_keySet(env, ((PyJObject*)self)->object));
}

static PyObject* PyJObject_values(PyObject* self) {
  JNIEnv* env = JcpThreadEnv_Get();
  return JcpPyObject_FromJObject(
      env, JavaMap_values(env, ((PyJObject*)self)->object));
}

static PyObject* PyJObject_items(PyObject* self) {
  JNIEnv* env = JcpThreadEnv_Get();
  return JcpPyObject_FromJObject(
      env, JavaMap_entrySet(env, ((PyJObject*)self)->object));
}

static PyObject* PyJObject_to_dict(PyObject* self) {
  JNIEnv* env = JcpThreadEnv_Get();
  return JcpPyDict_FromJMap(env, ((PyJObject*)self)->object);
}

static PyMethodDef dict_methods[] = {
    {"__getitem__", (PyCFunction)pyjdict_subscript, METH_O | METH_COEXIST,
     NULL},
    {"keys", (PyCFunction)PyJObject_keys, METH_NOARGS, NULL},
    {"values", (PyCFunction)PyJObject_values, METH_NOARGS, NULL},
    {"items", (PyCFunction)PyJObject_items, METH_NOARGS, NULL},
    {"to_dict", (PyCFunction)PyJObject_to_dict, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL},
};

PyTypeObject PyJDict_Type = {
    PyVarObject_HEAD_INIT(NULL, 0) "pemja.PyJDict", /* tp_name */
    sizeof(PyJObject),                              /* tp_basicsize */
    0,                                              /* tp_itemsize */
    0,                                              /* tp_dealloc */
    0,                                              /* tp_print */
    0,                                              /* tp_getattr */
    0,                                              /* tp_setattr */
    0,                                              /* tp_reserved */
    0,                                              /* tp_repr */
    0,                                              /* tp_as_number */
    &pyjdict_as_sequence,                           /* tp_as_sequence */
    &pyjdict_as_mapping,                            /* tp_as_mapping */
    PyObject_HashNotImplemented,                    /* tp_hash */
    0,                                              /* tp_call */
    0,                                              /* tp_str */
    0,                                              /* tp_getattro */
    0,                                              /* tp_setattro */
    0,                                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,       /* tp_flags */
    "Java Map Object",                              /* tp_doc */
    0,                                              /* tp_traverse */
    0,                                              /* tp_clear */
    0,                                              /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,                                              /* tp_iter */
    0,                                              /* tp_iternext */
    dict_methods,                                   /* tp_methods */
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
