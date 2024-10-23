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
#include <stdlib.h>
#include <CommonUtils.h>

#if (defined(_WIN32) || defined(_WIN64))
    #include <windows.h>
    #include <Python.h>

    PyMODINIT_FUNC PyInit_pemja_utils(void) {
        // Sine pemja_utils is not a true Python extension module,
        // we just use it to provide native functions, returning NULL
        // to let "import pemja_utils" fail is enough.
        return NULL;
    }
#else
    // linux and macos
    #include <dlfcn.h>
#endif

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    return JNI_VERSION_1_8;
}

/*
 * Class:     utils_CommonUtils
 * Method:    loadLibrary0
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pemja_utils_CommonUtils_loadLibrary0
  (JNIEnv *env, jobject obj, jstring library)
{
    const char* fileName = (*env)->GetStringUTFChars(env, library, 0);
    #if (defined(_WIN32) || defined(_WIN64))
        HINSTANCE dlresult = LoadLibrary(fileName);
        if (dlresult) {
            FreeLibrary(dlresult);
        } else {
            fprintf(stderr, "load dll failed. 0x%x\n", GetLastError());
            exit(EXIT_FAILURE);
        }
    #else
        void* dlresult = dlopen(fileName, RTLD_NOW | RTLD_GLOBAL);
        if (dlresult) {
            // The dynamic linker maintains reference counts so closing it is a no-op.
            dlclose(dlresult);
        } else {
            /*
                * Ignore errors and hope that the library is loaded globally or the
                * extensions are linked. If developers need to debug the cause they
                * should print the result of dlerror.
                */
            fprintf(stderr, "%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    #endif
}
