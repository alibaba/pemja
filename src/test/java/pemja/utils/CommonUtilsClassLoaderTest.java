/*
 * Copyright 2022 Alibaba Group Holding Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package pemja.utils;

import org.junit.Test;

import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;

/** Tests loading PemJa's native libraries from isolated class loaders. */
public class CommonUtilsClassLoaderTest {

    @Test
    public void testLoadPythonFromTwoIsolatedClassLoaders() throws Exception {
        URL classesUrl = CommonUtils.class.getProtectionDomain().getCodeSource().getLocation();
        try (URLClassLoader firstClassLoader = new URLClassLoader(new URL[] {classesUrl}, null);
                URLClassLoader secondClassLoader =
                        new URLClassLoader(new URL[] {classesUrl}, null)) {
            loadPython(firstClassLoader);
            loadPython(secondClassLoader);
        }
    }

    private static void loadPython(ClassLoader classLoader) throws Exception {
        Class<?> commonUtilsClass = Class.forName("pemja.utils.CommonUtils", true, classLoader);
        Object commonUtils = commonUtilsClass.getField("INSTANCE").get(null);
        Method loadPython = commonUtilsClass.getMethod("loadPython", String.class);
        loadPython.invoke(commonUtils, new Object[] {null});
    }
}
