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

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

/** Provides access to the JDK's registry of loaded native libraries. */
final class NativeLibraryRegistry {

    private static final String NATIVE_LIBRARIES_CLASS = "jdk.internal.loader.NativeLibraries";
    private static final String LOADED_LIBRARY_NAMES_FIELD = "loadedLibraryNames";
    private static final String UNSAFE_CLASS = "sun.misc.Unsafe";

    private NativeLibraryRegistry() {}

    @SuppressWarnings("unchecked")
    static Collection<String> getLoadedLibraryNames() {
        try {
            Field field = findLoadedLibraryNamesField();
            Object value = readStaticField(field);
            if (!(value instanceof Collection)) {
                throw new IllegalStateException(
                        String.format(
                                "Unexpected type for %s: %s",
                                LOADED_LIBRARY_NAMES_FIELD,
                                value == null ? "null" : value.getClass().getName()));
            }
            return (Collection<String>) value;
        } catch (ReflectiveOperationException e) {
            throw new IllegalStateException("Failed to access loaded native library registry", e);
        }
    }

    static boolean removeLoadedLibrary(String libraryPath) {
        Collection<String> loadedLibraryNames = getLoadedLibraryNames();
        synchronized (loadedLibraryNames) {
            return removeLoadedLibrary(loadedLibraryNames, libraryPath);
        }
    }

    static boolean removeLoadedLibrary(Collection<String> loadedLibraryNames, String libraryPath) {
        File libraryFile = new File(libraryPath);
        Set<String> candidatePaths = new HashSet<>();
        candidatePaths.add(libraryFile.getAbsolutePath());
        try {
            candidatePaths.add(libraryFile.getCanonicalPath());
        } catch (IOException ignored) {
            // The absolute path can still match the registry if canonicalization fails.
        }
        return loadedLibraryNames.removeIf(candidatePaths::contains);
    }

    private static Field findLoadedLibraryNamesField()
            throws ClassNotFoundException, NoSuchFieldException {
        try {
            return ClassLoader.class.getDeclaredField(LOADED_LIBRARY_NAMES_FIELD);
        } catch (NoSuchFieldException ignored) {
            return Class.forName(NATIVE_LIBRARIES_CLASS)
                    .getDeclaredField(LOADED_LIBRARY_NAMES_FIELD);
        }
    }

    private static Object readStaticField(Field field) throws ReflectiveOperationException {
        Class<?> unsafeClass = Class.forName(UNSAFE_CLASS);
        Field unsafeField = unsafeClass.getDeclaredField("theUnsafe");
        unsafeField.setAccessible(true);
        Object unsafe = unsafeField.get(null);
        Method staticFieldBase = unsafeClass.getMethod("staticFieldBase", Field.class);
        Method staticFieldOffset = unsafeClass.getMethod("staticFieldOffset", Field.class);
        Method getObject = unsafeClass.getMethod("getObject", Object.class, long.class);
        Object base = staticFieldBase.invoke(unsafe, field);
        long offset = (Long) staticFieldOffset.invoke(unsafe, field);
        return getObject.invoke(unsafe, base, offset);
    }
}
