/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package pemja.core;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import pemja.core.object.PyIterator;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigDecimal;
import java.nio.file.Files;
import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicReference;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

/** Tests for {@link PythonInterpreter}. */
public class PythonInterpreterTest {

    private String tmpDirPath;
    private String[] testDir;

    @Before
    public void prepareTestEnvironment() {
        File tmpDirFile =
                new File(System.getProperty("java.io.tmpdir"), "pemja_" + UUID.randomUUID());
        try {
            if (!tmpDirFile.mkdirs()) {
                throw new RuntimeException(
                        String.format(
                                "The directory %s has been created.",
                                tmpDirFile.getAbsolutePath()));
            }
        } catch (Exception e) {
            throw new RuntimeException(
                    String.format("Create the directory %s failed", tmpDirFile.getAbsolutePath()),
                    e);
        }
        this.tmpDirPath = tmpDirFile.getAbsolutePath();
        this.testDir =
                new String[] {
                    String.join(
                            File.separator,
                            File.separator,
                            System.getProperty("user.dir"),
                            "src",
                            "main",
                            "python"),
                    String.join(
                            File.separator,
                            System.getProperty("user.dir"),
                            "src",
                            "main",
                            "python",
                            "pemja",
                            "tests")
                };
    }

    @After
    public void cleanEnvironment() throws IOException {
        deleteDirectory(new File(tmpDirPath));
    }

    @Test
    public void testSetAndGet() {
        PythonInterpreterConfig config = PythonInterpreterConfig.newBuilder().build();
        try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
            interpreter.set("a", true);
            interpreter.set("b", 127);
            interpreter.set("c", 32767);
            interpreter.set("d", -2147483648);
            interpreter.set("e", -9223372036854775808L);
            interpreter.set("f", 1.12);
            interpreter.set("g", Math.PI);
            interpreter.set("h", '中');
            interpreter.set("i", "中国");
            byte[] bytes = "中国".getBytes();
            interpreter.set("j", bytes);

            assertEquals(true, interpreter.get("a"));
            assertEquals((byte) 127, interpreter.get("b", Byte.class).byteValue());
            assertEquals((short) 32767, interpreter.get("c", Short.class).shortValue());
            assertEquals(-2147483648, interpreter.get("d", Integer.class).intValue());
            assertEquals(-9223372036854775808L, interpreter.get("e"));
            assertEquals(1.12f, interpreter.get("f", Float.class), 0.0001);
            assertEquals(Math.PI, interpreter.get("g"));
            assertEquals('中', interpreter.get("h", String.class).charAt(0));
            assertEquals("中国", interpreter.get("i"));
            byte[] actualBytes = interpreter.get("j", byte[].class);
            assertEquals(bytes.length, actualBytes.length);
            assertArrayEquals(bytes, actualBytes);

            boolean[] k = new boolean[] {true, false};
            short[] m = new short[] {-32768, 32767};
            int[] n = new int[] {Integer.MIN_VALUE, Integer.MAX_VALUE};
            long[] o = new long[] {Long.MIN_VALUE, Long.MAX_VALUE};
            float[] p = new float[] {Float.MIN_VALUE, Float.MAX_VALUE};
            double[] q = new double[] {Double.MIN_VALUE, Double.MAX_VALUE};
            Object[] r = new Object[] {"中国", 123L};

            interpreter.set("k", k);
            interpreter.set("m", m);
            interpreter.set("n", n);
            interpreter.set("o", o);
            interpreter.set("p", p);
            interpreter.set("q", q);
            interpreter.set("r", r);

            Object[] actualK = interpreter.get("k", Object[].class);
            assertEquals(k.length, actualK.length);
            for (int i = 0; i < k.length; i++) {
                assertEquals(k[i], actualK[i]);
            }

            Object[] actualM = interpreter.get("m", Object[].class);
            assertEquals(m.length, actualM.length);
            for (int i = 0; i < m.length; i++) {
                assertEquals(m[i], ((Long) actualM[i]).longValue());
            }

            Object[] actualN = interpreter.get("n", Object[].class);
            assertEquals(n.length, actualN.length);
            for (int i = 0; i < n.length; i++) {
                assertEquals(n[i], ((Long) actualN[i]).longValue());
            }

            Object[] actualO = interpreter.get("o", Object[].class);
            assertEquals(o.length, actualO.length);
            for (int i = 0; i < o.length; i++) {
                assertEquals(o[i], ((Long) actualO[i]).longValue());
            }

            Object[] actualP = interpreter.get("p", Object[].class);
            assertEquals(p.length, actualP.length);
            for (int i = 0; i < actualP.length; i++) {
                assertEquals(Float.compare(p[i], ((Double) actualP[i]).floatValue()), 0);
            }

            Object[] actualQ = interpreter.get("q", Object[].class);
            assertEquals(q.length, actualQ.length);
            for (int i = 0; i < actualQ.length; i++) {
                assertEquals(Double.compare(q[i], (Double) actualQ[i]), 0);
            }

            Object[] actualR = interpreter.get("r", Object[].class);
            assertEquals(r.length, actualR.length);
            assertArrayEquals(r, actualR);

            Date s = new Date(2021 - 1900, 9 - 1, 22);
            interpreter.set("s", s);
            assertEquals(s, interpreter.get("s", Date.class));

            Time t = new Time(100000);
            interpreter.set("t", t);
            assertEquals(t, interpreter.get("t", Time.class));

            Timestamp u = new Timestamp(500000010);
            interpreter.set("u", u);
            assertEquals(u, interpreter.get("u"));
            ArrayList<Object> v = new ArrayList<>();
            v.add(1L);
            v.add("pemja");
            interpreter.set("v", v);
            assertArrayEquals(v.toArray(), (Object[]) interpreter.get("v"));

            Map<Object, Object> w = new HashMap<>();
            w.put(1L, "pemja");
            w.put("pemja", 2L);
            interpreter.set("w", w);
            assertEquals(w, interpreter.get("w"));

            BigDecimal x = new BigDecimal("1000000000000000000.05999999999999999899999999999");
            interpreter.set("x", x);
            assertEquals(x, interpreter.get("x"));
        }
    }

    @Test
    public void testCallPython() {
        try {
            PythonInterpreterConfig config =
                    PythonInterpreterConfig.newBuilder().addPythonPaths(testDir).build();
            Object[] args = new Object[] {1};
            Map<String, Object> kwargs = new HashMap<>();
            kwargs.put("a", 2);
            try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
                interpreter.exec("import test_call");
                assertEquals("no arg", interpreter.invoke("test_call.test_call_no_args"));
                assertEquals(1L, interpreter.invoke("test_call.test_call_one_arg", args));
                assertEquals(1L, interpreter.invoke("test_call.test_call_variable_args", args));
                assertEquals(2L, interpreter.invoke("test_call.test_call_keywords_args", kwargs));
                assertEquals(3L, interpreter.invoke("test_call.test_call_all_args", args, kwargs));
                interpreter.exec("a = test_call.A()");
                assertEquals(3L, interpreter.invokeMethod("a", "add", 3));
                assertEquals(1L, interpreter.invokeMethod("a", "minus", 2));
                assertEquals(7L, interpreter.invokeMethod("a", "add_all", 1, 2, 3));
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed to invoke test_call.py", e);
        }
    }

    @Test
    public void testMultiThreadSameEnvironment() throws InterruptedException {
        File file1 = new File(tmpDirPath + File.separatorChar + "file1");
        File file2 = new File(tmpDirPath + File.separatorChar + "file2");
        file1.mkdirs();
        file2.mkdirs();
        AtomicReference<String> path1 = new AtomicReference<>();
        AtomicReference<String> path2 = new AtomicReference<>();

        Thread threadA =
                new Thread(
                        () -> {
                            PythonInterpreterConfig config1 =
                                    PythonInterpreterConfig.newBuilder()
                                            .setExcType(
                                                    PythonInterpreterConfig.ExecType.MULTI_THREAD)
                                            .addPythonPaths(
                                                    file1.getAbsolutePath(),
                                                    file2.getAbsolutePath())
                                            .build();
                            PythonInterpreter interpreter1 = new PythonInterpreter(config1);
                            interpreter1.exec("import sys");
                            interpreter1.exec("a = str(sys.path)");
                            path1.set(interpreter1.get("a", String.class));
                            interpreter1.close();
                        });

        Thread threadB =
                new Thread(
                        () -> {
                            PythonInterpreterConfig config2 =
                                    PythonInterpreterConfig.newBuilder()
                                            .setExcType(
                                                    PythonInterpreterConfig.ExecType.MULTI_THREAD)
                                            .addPythonPaths(
                                                    file1.getAbsolutePath(),
                                                    file2.getAbsolutePath())
                                            .build();
                            PythonInterpreter interpreter2 = new PythonInterpreter(config2);
                            interpreter2.exec("import sys");
                            interpreter2.exec("a = str(sys.path)");
                            path2.set(interpreter2.get("a", String.class));
                            interpreter2.close();
                        });

        threadA.start();
        threadB.start();
        threadA.join();
        threadB.join();
        String path1String = path1.get();
        String path2String = path1.get();
        if (path1String.length() > path2String.length()) {
            assertEquals(path1String.substring(0, path2String.length()), path2String);
        } else {
            assertEquals(path1String, path2String.substring(0, path1String.length()));
        }
    }

    @Test
    public void testMultiInterpreterDifferentEnvironment() throws InterruptedException {
        File file1 = new File(tmpDirPath + File.separatorChar + "file1");
        File file2 = new File(tmpDirPath + File.separatorChar + "file2");
        file1.mkdirs();
        file2.mkdirs();
        AtomicReference<String> path1 = new AtomicReference<>();
        AtomicReference<String> path2 = new AtomicReference<>();

        Thread threadA =
                new Thread(
                        () -> {
                            PythonInterpreterConfig config1 =
                                    PythonInterpreterConfig.newBuilder()
                                            .setExcType(
                                                    PythonInterpreterConfig.ExecType
                                                            .SUB_INTERPRETER)
                                            .addPythonPaths(file1.getAbsolutePath())
                                            .build();
                            PythonInterpreter interpreter1 = new PythonInterpreter(config1);
                            interpreter1.exec("import sys");
                            interpreter1.exec("a = str(sys.path)");
                            path1.set(interpreter1.get("a", String.class));
                            interpreter1.close();
                        });

        Thread threadB =
                new Thread(
                        () -> {
                            PythonInterpreterConfig config2 =
                                    PythonInterpreterConfig.newBuilder()
                                            .setExcType(
                                                    PythonInterpreterConfig.ExecType
                                                            .SUB_INTERPRETER)
                                            .addPythonPaths(file2.getAbsolutePath())
                                            .build();
                            PythonInterpreter interpreter2 = new PythonInterpreter(config2);
                            interpreter2.exec("import sys");
                            interpreter2.exec("a = str(sys.path)");
                            path2.set(interpreter2.get("a", String.class));
                            interpreter2.close();
                        });

        threadA.start();
        threadB.start();
        threadA.join();
        threadB.join();
        assertNotEquals(path1.get(), path2.get());
    }

    @Test
    public void testMultiThread() throws InterruptedException {
        AtomicReference<Throwable> exceptionReference = new AtomicReference<>();
        Thread threadA =
                new Thread(
                        () -> {
                            try (PythonInterpreter interpreter =
                                    new PythonInterpreter(
                                            PythonInterpreterConfig.newBuilder().build())) {
                                try {
                                    interpreter.exec("import numpy as np");
                                    interpreter.exec("import math");
                                    interpreter.exec(
                                            "in_array = [0, math.pi / 2, np.pi / 3, np.pi]");
                                    Object[] res =
                                            new Object[] {
                                                0L,
                                                1.5707963267948966,
                                                1.0471975511965976,
                                                3.141592653589793
                                            };
                                    assertArrayEquals(
                                            res, interpreter.get("in_array", Object[].class));
                                } catch (Throwable throwable) {
                                    if (exceptionReference.get() == null) {
                                        exceptionReference.set(throwable);
                                    }
                                }
                            }
                        });

        Thread threadB =
                new Thread(
                        () -> {
                            try (PythonInterpreter interpreter =
                                    new PythonInterpreter(
                                            PythonInterpreterConfig.newBuilder().build())) {
                                try {
                                    interpreter.exec("import numpy as np");
                                    interpreter.exec("import math");
                                    interpreter.exec(
                                            "in_array = [0, math.pi / 2, np.pi / 3, np.pi]");
                                    Object[] res =
                                            new Object[] {
                                                0L,
                                                1.5707963267948966,
                                                1.0471975511965976,
                                                3.141592653589793
                                            };
                                    assertArrayEquals(
                                            res, interpreter.get("in_array", Object[].class));
                                } catch (Throwable throwable) {
                                    if (exceptionReference.get() == null) {
                                        exceptionReference.set(throwable);
                                    }
                                }
                            }
                        });
        threadA.start();
        threadB.start();
        threadA.join();
        threadB.join();
        if (exceptionReference.get() != null) {
            throw new RuntimeException(
                    "Error occurs in Python Interpreter threads", exceptionReference.get());
        }
    }

    @Test
    public void testSubInterpreters() throws InterruptedException {
        AtomicReference<Throwable> exceptionReference = new AtomicReference<>();
        Thread threadA =
                new Thread(
                        () -> {
                            try (PythonInterpreter interpreter =
                                    new PythonInterpreter(
                                            PythonInterpreterConfig.newBuilder()
                                                    .setExcType(
                                                            PythonInterpreterConfig.ExecType
                                                                    .SUB_INTERPRETER)
                                                    .build())) {
                                try {
                                    interpreter.exec("import json");
                                    interpreter.exec("data = {'a': 'Runoob', 'b': 7}");
                                    interpreter.exec(
                                            "s = json.dumps(data, sort_keys=True, indent=4, separators=(',', ': '))");
                                    assertEquals(
                                            "{\n"
                                                    + "    \"a\": \"Runoob\",\n"
                                                    + "    \"b\": 7\n"
                                                    + "}",
                                            interpreter.get("s", String.class));
                                } catch (Throwable throwable) {
                                    if (exceptionReference.get() == null) {
                                        exceptionReference.set(throwable);
                                    }
                                }
                            }
                        });

        Thread threadB =
                new Thread(
                        () -> {
                            try (PythonInterpreter interpreter =
                                    new PythonInterpreter(
                                            PythonInterpreterConfig.newBuilder()
                                                    .setExcType(
                                                            PythonInterpreterConfig.ExecType
                                                                    .SUB_INTERPRETER)
                                                    .build())) {
                                try {
                                    interpreter.exec("import json");
                                    interpreter.exec(
                                            "data = [ { 'a' : 1, 'b' : 2, 'c' : 3, 'd' : 4, 'e' : 5 } ]");
                                    interpreter.exec("s = json.dumps(data)");
                                    assertEquals(
                                            "[{\"a\": 1, \"b\": 2, \"c\": 3, \"d\": 4, \"e\": 5}]",
                                            interpreter.get("s", String.class));
                                } catch (Throwable throwable) {
                                    if (exceptionReference.get() == null) {
                                        exceptionReference.set(throwable);
                                    }
                                }
                            }
                        });
        threadA.start();
        threadB.start();
        threadA.join();
        threadB.join();
        if (exceptionReference.get() != null) {
            throw new RuntimeException(
                    "Error occurs in Python Interpreter threads", exceptionReference.get());
        }
    }

    @Test
    public void testExec() {
        String input = "abcde";
        PythonInterpreterConfig config = PythonInterpreterConfig.newBuilder().build();
        try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
            interpreter.set("a", input);
            interpreter.exec("b = a.upper()");
            String result = interpreter.get("b", String.class);
            assertEquals(result, input.toUpperCase());
        }
    }

    @Test
    public void testCallbackJava() {
        try {
            PythonInterpreterConfig config =
                    PythonInterpreterConfig.newBuilder().addPythonPaths(testDir).build();
            try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
                interpreter.exec("import test_call");
                assertEquals(
                        "pemjajavapython7fffffff",
                        interpreter.invoke("test_call.test_callback_java"));
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed to call test_call_java in test_call.py", e);
        }
    }

    @Test
    public void testReturnGenerator() {
        try {
            PythonInterpreterConfig config =
                    PythonInterpreterConfig.newBuilder().addPythonPaths(testDir).build();
            try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
                interpreter.exec("import test_call");
                PyIterator iterators =
                        (PyIterator) interpreter.invoke("test_call.test_return_generator", 3);
                Object[] expected = new Object[] {0L, 1L, 2L, "haha", null};
                int index = 0;
                while (iterators.hasNext()) {
                    assertEquals(expected[index], iterators.next());
                    index++;
                }
                iterators.close();
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed to call test_return_generator in test_call.py", e);
        }
    }

    private static void deleteDirectory(File directory) throws IOException {
        if (directory.isDirectory()) {
            // directory exists and is a directory

            // empty the directory first
            try {
                cleanDirectoryInternal(directory);
            } catch (FileNotFoundException ignored) {
                // someone concurrently deleted the directory, nothing to do for us
                return;
            }

            // delete the directory. this fails if the directory is not empty, meaning
            // if new files got concurrently created. we want to fail then.
            // if someone else deleted the empty directory concurrently, we don't mind
            // the result is the same for us, after all
            Files.deleteIfExists(directory.toPath());
        } else if (directory.exists()) {
            // exists but is file, not directory
            // either an error from the caller, or concurrently a file got created
            throw new IOException(directory + " is not a directory");
        }
        // else: does not exist, which is okay (as if deleted)
    }

    private static void cleanDirectoryInternal(File directory) throws IOException {
        if (Files.isSymbolicLink(directory.toPath())) {
            // the user directories which symbolic links point to should not be cleaned.
            return;
        }
        if (directory.isDirectory()) {
            final File[] files = directory.listFiles();

            if (files == null) {
                // directory does not exist any more or no permissions
                if (directory.exists()) {
                    throw new IOException("Failed to list contents of " + directory);
                } else {
                    throw new FileNotFoundException(directory.toString());
                }
            }

            // remove all files in the directory
            for (File file : files) {
                if (file != null) {
                    deleteFileOrDirectoryInternal(file);
                }
            }
        } else if (directory.exists()) {
            throw new IOException(directory + " is not a directory but a regular file");
        } else {
            // else does not exist at all
            throw new FileNotFoundException(directory.toString());
        }
    }

    private static void deleteFileOrDirectoryInternal(File file) throws IOException {
        if (file.isDirectory()) {
            // file exists and is directory
            deleteDirectory(file);
        } else {
            // if the file is already gone (concurrently), we don't mind
            Files.deleteIfExists(file.toPath());
        }
        // else: already deleted
    }
}
