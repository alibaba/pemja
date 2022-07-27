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
#ifndef _Included_pyjiterator
#define _Included_pyjiterator

JcpAPI_DATA(PyTypeObject) PyJIterator_Type;

/* Public interface */
JcpAPI_FUNC(PyObject*) JcpPyJIterator_New(JNIEnv*, jobject, jclass);

#define PyJIterator_Check(op) \
        PyObject_TypeCheck(op, &PyJIterator_Type)
#define PyJIterator_CheckExact(op) op->ob_type == &PyJIterator_Type

#endif
