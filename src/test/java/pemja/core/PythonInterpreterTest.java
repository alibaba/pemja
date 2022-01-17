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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigDecimal;
import java.nio.file.Files;
import java.nio.file.StandardOpenOption;
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

            assertArrayEquals(k, interpreter.get("k", boolean[].class));
            assertArrayEquals(m, interpreter.get("m", short[].class));
            assertArrayEquals(n, interpreter.get("n", int[].class));
            assertArrayEquals(o, interpreter.get("o", long[].class));

            float[] actualP = interpreter.get("p", float[].class);
            assertEquals(p.length, actualP.length);
            for (int i = 0; i < actualP.length; i++) {
                assertEquals(Float.compare(p[i], actualP[i]), 0);
            }

            double[] actualQ = interpreter.get("q", double[].class);
            assertEquals(q.length, actualQ.length);
            for (int i = 0; i < actualQ.length; i++) {
                assertEquals(Double.compare(q[i], actualQ[i]), 0);
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
    public void testInvoke() {
        String pyPath = String.join(File.separator, tmpDirPath, "invoke.py");
        try {
            File pyFile = new File(pyPath);
            pyFile.createNewFile();
            pyFile.setExecutable(true);
            String pyProgram =
                    "#!/usr/bin/python\n"
                            + "# -*- coding: UTF-8 -*-\n"
                            + "import os\n"
                            + "import sys\n"
                            + "\n"
                            + "class A(object):\n"
                            + "\tdef __init__(self):\n"
                            + "\t\tself._a = 0\n"
                            + "\n"
                            + "\tdef get_value(self):\n"
                            + "\t\treturn self._a\n"
                            + "\n"
                            + "\tdef add(self, n):\n"
                            + "\t\tself._a += n\n"
                            + "\t\treturn self._a\n"
                            + "\n"
                            + "\tdef add_all(self, *args):\n"
                            + "\t\tfor item in args:\n"
                            + "\t\t\tself._a += item\n"
                            + "\t\treturn self._a\n"
                            + "\n"
                            + "\tdef minus(self, n):\n"
                            + "\t\tself._a -= n\n"
                            + "\t\treturn self._a\n"
                            + "\n"
                            + "def no_arg():\n"
                            + "\treturn 'no arg'\n"
                            + "\n"
                            + "def one_arg(arg):\n"
                            + "\treturn arg\n"
                            + "\n"
                            + "def parse(*args, **kwargs):\n"
                            + "\tif args:\n"
                            + "\t\tif kwargs:\n"
                            + "\t\t\treturn args[0] + kwargs['a']\n"
                            + "\t\telse:\n"
                            + "\t\t\treturn args[0]\n"
                            + "\telse:\n"
                            + "\t\tif kwargs:\n"
                            + "\t\t\treturn kwargs['a']\n"
                            + "\t\telse:\n"
                            + "\t\t\treturn 'empty'\n";
            Files.write(pyFile.toPath(), pyProgram.getBytes(), StandardOpenOption.WRITE);

            PythonInterpreterConfig config =
                    PythonInterpreterConfig.newBuilder().addPythonPaths(tmpDirPath).build();
            Object[] args = new Object[] {1};
            Map<String, Object> kwargs = new HashMap<>();
            kwargs.put("a", 2);
            try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
                interpreter.exec("import invoke");
                assertEquals("no arg", interpreter.invoke("invoke.no_arg"));
                assertEquals(1L, interpreter.invoke("invoke.one_arg", args));
                assertEquals(1L, interpreter.invoke("invoke.parse", args));
                assertEquals(2L, interpreter.invoke("invoke.parse", kwargs));
                assertEquals(3L, interpreter.invoke("invoke.parse", args, kwargs));
                interpreter.exec("a = invoke.A()");
                assertEquals(3L, interpreter.invokeMethod("a", "add", 3));
                assertEquals(1L, interpreter.invokeMethod("a", "minus", 2));
                assertEquals(7L, interpreter.invokeMethod("a", "add_all", 1, 2, 3));
            }
        } catch (IOException e) {
            throw new RuntimeException("Failed to invoke.py", e);
        }
    }

    @Test
    public void testMultiInterpreterDifferentEnvironment() {
        File file1 = new File(tmpDirPath + File.separatorChar + "file1");
        File file2 = new File(tmpDirPath + File.separatorChar + "file2");
        file1.mkdirs();
        file2.mkdirs();
        PythonInterpreterConfig config1 =
                PythonInterpreterConfig.newBuilder()
                        .addPythonPaths(file1.getAbsolutePath())
                        .build();

        PythonInterpreterConfig config2 =
                PythonInterpreterConfig.newBuilder()
                        .addPythonPaths(file2.getAbsolutePath())
                        .build();

        PythonInterpreter interpreter1 = new PythonInterpreter(config1);
        interpreter1.exec("import sys");
        interpreter1.exec("a = str(sys.path)");
        PythonInterpreter interpreter2 = new PythonInterpreter(config2);
        interpreter2.exec("import sys");
        interpreter2.exec("a = str(sys.path)");

        // different interpreter has different system path.
        assertNotEquals(interpreter1.get("a"), interpreter2.get("a"));
        interpreter1.close();
        interpreter2.close();
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
                                                                    .SUB_INTERPRETERS)
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
                                                                    .SUB_INTERPRETERS)
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
