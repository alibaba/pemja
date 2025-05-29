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
#ifndef PEMJA_PYJDICT_H
#define PEMJA_PYJDICT_H

JcpAPI_DATA(PyTypeObject) PyJDict_Type;

/* Public interface */
JcpAPI_FUNC(PyObject*) JcpPyJDict_New(JNIEnv*, jobject, jclass);

#define PyJDict_Check(op) PyObject_TypeCheck(op, &PyJDict_Type)
#define PyJDict_CheckExact(op) op->ob_type == &PyJDict_Type

#endif  // PEMJA_PYJDICT_H
