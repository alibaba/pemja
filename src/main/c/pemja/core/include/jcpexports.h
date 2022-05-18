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
#ifndef Jcp_EXPORTS_H
#define Jcp_EXPORTS_H

#ifndef __has_attribute
  #define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif
#if (defined(__GNUC__) && (__GNUC__ >= 4)) ||\
    (defined(__clang__) && __has_attribute(visibility))
    #define Jcp_IMPORTED_SYMBOL __attribute__ ((visibility ("default")))
    #define Jcp_EXPORTED_SYMBOL __attribute__ ((visibility ("default")))
    #define Jcp_LOCAL_SYMBOL  __attribute__ ((visibility ("hidden")))
#else
    #define Jcp_IMPORTED_SYMBOL
    #define Jcp_EXPORTED_SYMBOL
    #define Jcp_LOCAL_SYMBOL
#endif

#endif /* Jcp_EXPORTS_H */
