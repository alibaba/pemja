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

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

/** Tests for {@link NativeLibraryRegistry}. */
public class NativeLibraryRegistryTest {

    @Test
    public void testGetLoadedLibraryNames() {
        assertNotNull(NativeLibraryRegistry.getLoadedLibraryNames());
    }

    @Test
    public void testRemoveLoadedLibraryByExactAbsolutePath() {
        File library = new File("target/test-libraries/libpython3.so").getAbsoluteFile();
        String similarlyNamedLibrary = library.getPath() + ".backup";
        Set<String> loadedLibraryNames =
                new HashSet<>(Arrays.asList(library.getPath(), similarlyNamedLibrary));

        assertTrue(
                NativeLibraryRegistry.removeLoadedLibrary(loadedLibraryNames, library.getPath()));
        assertEquals(new HashSet<>(Arrays.asList(similarlyNamedLibrary)), loadedLibraryNames);
    }

    @Test
    public void testRemoveLoadedLibraryByCanonicalPath() throws Exception {
        Path directory = Files.createTempDirectory("pemja-native-library");
        Path nestedDirectory = Files.createDirectory(directory.resolve("nested"));
        Path canonicalLibrary = Files.createFile(directory.resolve("Python3"));
        Path libraryAlias = nestedDirectory.resolve("..").resolve(canonicalLibrary.getFileName());
        Set<String> loadedLibraryNames =
                new HashSet<>(Arrays.asList(canonicalLibrary.toFile().getCanonicalPath()));
        try {
            assertTrue(
                    NativeLibraryRegistry.removeLoadedLibrary(
                            loadedLibraryNames, libraryAlias.toString()));
            assertTrue(loadedLibraryNames.isEmpty());
        } finally {
            Files.deleteIfExists(canonicalLibrary);
            Files.deleteIfExists(nestedDirectory);
            Files.deleteIfExists(directory);
        }
    }

    @Test
    public void testRemoveLoadedLibraryDoesNotUseSubstringMatching() {
        String requestedLibrary = new File("target/Python3").getAbsolutePath();
        Set<String> loadedLibraryNames = new HashSet<>(Arrays.asList(requestedLibrary + "-other"));

        assertFalse(
                NativeLibraryRegistry.removeLoadedLibrary(loadedLibraryNames, requestedLibrary));
        assertEquals(1, loadedLibraryNames.size());
    }
}
