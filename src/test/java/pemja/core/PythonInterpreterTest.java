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

package pemja.core;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import pemja.core.object.PyIterator;
import pemja.core.object.PyObject;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.nio.file.Files;
import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
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

            boolean[] actualK = interpreter.get("k", boolean[].class);
            assertArrayEquals(k, actualK);

            short[] actualM = interpreter.get("m", short[].class);
            assertArrayEquals(m, actualM);

            int[] actualN = interpreter.get("n", int[].class);
            assertArrayEquals(n, actualN);

            long[] actualO = interpreter.get("o", long[].class);
            assertArrayEquals(o, actualO);

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
            assertEquals(v, interpreter.get("v"));

            Map<Object, Object> w = new HashMap<>();
            w.put(1L, "pemja");
            w.put("pemja", 2L);
            interpreter.set("w", w);
            assertEquals(w, interpreter.get("w"));

            BigDecimal x = new BigDecimal("1000000000000000000.05999999999999999899999999999");
            interpreter.set("x", x);
            assertEquals(x, interpreter.get("x"));

            BigInteger y = new BigInteger("1000000000000000000");
            interpreter.set("y", y);
            assertEquals(y, interpreter.get("y", BigInteger.class));

            Object[][] z = new String[3][3];
            z[0] = new String[] {"pemja", "is", "cool"};
            z[1] = new String[] {"p", "e", "m"};
            z[2] = new String[] {"ja", "v", "a"};
            interpreter.set("z", z);
            assertArrayEquals(z, interpreter.get("z", Object[][].class));

            int[][] a1 = new int[2][2];
            a1[0] = new int[] {1, 2};
            a1[1] = new int[] {3, 4};
            interpreter.set("a1", a1);
            assertArrayEquals(a1, interpreter.get("a1", int[][].class));
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

                Object javaObject = new TestObject();
                assertEquals(
                        javaObject, interpreter.invoke("test_call.test_call_one_arg", javaObject));
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed to invoke test_call.py", e);
        }
    }

    @Test
    public void testCallPythonWithAllTypes() throws Exception {
        PythonInterpreterConfig config =
                PythonInterpreterConfig.newBuilder().addPythonPaths(testDir).build();
        try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
            interpreter.exec("import test_call");
            assertEquals(true, interpreter.invoke("test_call.test_call_one_arg", true));
            assertEquals(127L, interpreter.invoke("test_call.test_call_one_arg", 127));
            assertEquals(32767L, interpreter.invoke("test_call.test_call_one_arg", 32767));
            assertEquals(
                    -2147483648L, interpreter.invoke("test_call.test_call_one_arg", -2147483648));
            assertEquals(
                    -9223372036854775808L,
                    interpreter.invoke("test_call.test_call_one_arg", -9223372036854775808L));
            assertEquals(1.12, interpreter.invoke("test_call.test_call_one_arg", 1.12));
            assertEquals(Math.PI, interpreter.invoke("test_call.test_call_one_arg", Math.PI));
            assertEquals("中", interpreter.invoke("test_call.test_call_one_arg", '中'));
            assertEquals("中国", interpreter.invoke("test_call.test_call_one_arg", "中国"));
            assertArrayEquals(
                    "中国".getBytes(),
                    (byte[])
                            interpreter.invoke(
                                    "test_call.test_call_one_arg", (Object) "中国".getBytes()));

            boolean[] arg0 = new boolean[] {true, false};
            Object[] res0 =
                    (Object[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg0);
            assertEquals(arg0.length, res0.length);
            for (int i = 0; i < arg0.length; i++) {
                assertEquals(arg0[i], res0[i]);
            }

            boolean[] arg1 = new boolean[] {true, false};
            Object[] res1 =
                    (Object[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg1);

            assertEquals(arg1.length, res1.length);
            for (int i = 0; i < arg1.length; i++) {
                assertEquals(arg1[i], res1[i]);
            }

            byte[] arg2 = new byte[] {-128, 127};
            byte[] res2 = (byte[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg2);
            assertArrayEquals(arg2, res2);

            short[] arg3 = new short[] {-32768, 32767};
            Object[] res3 =
                    (Object[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg3);
            assertEquals(arg3.length, res3.length);
            for (int i = 0; i < arg3.length; i++) {
                assertEquals(arg3[i], ((Long) res3[i]).shortValue());
            }

            int[] arg4 = new int[] {Integer.MIN_VALUE, Integer.MAX_VALUE};
            Object[] res4 =
                    (Object[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg4);
            assertEquals(arg4.length, res4.length);
            for (int i = 0; i < arg4.length; i++) {
                assertEquals(arg4[i], ((Long) res4[i]).intValue());
            }

            long[] arg5 = new long[] {Long.MIN_VALUE, Long.MAX_VALUE};
            Object[] res5 =
                    (Object[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg5);
            assertEquals(arg5.length, res5.length);
            for (int i = 0; i < arg5.length; i++) {
                assertEquals(arg5[i], ((Long) res5[i]).longValue());
            }

            Object[] arg6 = new Object[] {"中国", 123L};
            Object[] res6 =
                    (Object[]) interpreter.invoke("test_call.test_call_one_arg", (Object) arg6);
            assertEquals(arg6.length, res6.length);
            for (int i = 0; i < arg6.length; i++) {
                assertEquals(arg6[i], res6[i]);
            }

            Date arg7 = new Date(2021 - 1900, 9 - 1, 22);
            Object res7 = interpreter.invoke("test_call.test_call_one_arg", (Object) arg7);
            assertEquals(arg7, res7);

            Time arg8 = new Time(100000);
            Object res8 = interpreter.invoke("test_call.test_call_one_arg", (Object) arg8);
            assertEquals(arg8, res8);

            Timestamp arg9 = new Timestamp(500000010);
            Object res9 = interpreter.invoke("test_call.test_call_one_arg", (Object) arg9);
            assertEquals(arg9, res9);

            ArrayList<Object> arg10 = new ArrayList<>();
            arg10.add(1L);
            arg10.add("pemja");
            assertEquals(arg10, interpreter.invoke("test_call.test_call_one_arg", arg10));

            Map<Object, Object> arg11 = new HashMap<>();
            arg11.put(1L, "pemja");
            arg11.put("pemja", 2L);
            assertEquals(arg11, interpreter.invoke("test_call.test_call_one_arg", arg11));

            PyIterator iterators =
                    (PyIterator) interpreter.invoke("test_call.test_return_generator", 3);
            Object[] expected = new Object[] {0L, 1L, 2L, "haha", null};
            int index = 0;
            while (iterators.hasNext()) {
                assertEquals(expected[index], iterators.next());
                index++;
            }
            iterators.close();

            PyObject arg12 = (PyObject) interpreter.invoke("test_call.test_return_python_object");
            assertEquals(2L, arg12.getAttr("_a"));
            arg12.setAttr("_a", 4);
            assertEquals(4L, arg12.getAttr("_a"));
            arg12.invokeMethod("add", 12);
            assertEquals(16L, arg12.invokeMethod("get_value"));
            arg12.invokeMethod("add_all", 1, 2, 3);
            assertEquals(22L, arg12.getAttr("_a"));
            arg12.close();
        }
    }

    @Test
    public void testCallPyJObject() {
        PythonInterpreterConfig config =
                PythonInterpreterConfig.newBuilder().addPythonPaths(testDir).build();
        try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
            interpreter.exec("import test_pyjobject");

            TestObject arg = new TestObject();
            assertEquals(arg, interpreter.invoke("test_pyjobject.test_call_java_object", arg));

            Map<String, Long> map = new HashMap<>();
            map.put("pemja", 1L);
            map.put("java", 2L);
            map.put("python", 3L);
            assertEquals(
                    map.values(),
                    interpreter.invoke("test_pyjobject.test_call_collection", map.values()));

            Iterator<Long> valuesIter = map.values().iterator();
            assertEquals(
                    valuesIter,
                    interpreter.invoke("test_pyjobject.test_call_iterator", valuesIter));
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
                            interpreter1.exec("a = str(set(sys.path))");
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
                            interpreter2.exec("a = str(set(sys.path))");
                            path2.set(interpreter2.get("a", String.class));
                            interpreter2.close();
                        });

        threadA.start();
        threadB.start();
        threadA.join();
        threadB.join();
        String path1String = path1.get();
        String path2String = path2.get();

        assertEquals(path1String, path2String);
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
                                    interpreter.exec("import math");
                                    interpreter.exec(
                                            "in_array = (0, math.pi / 2, math.pi / 3, math.pi)");
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
                                    interpreter.exec("import math");
                                    interpreter.exec(
                                            "in_array = (0, math.pi / 2, math.pi / 3, math.pi)");
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
    public void testExec2() {
        PythonInterpreterConfig config = PythonInterpreterConfig.newBuilder().build();
        try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
            interpreter.exec("a = 123");
            interpreter.exec("def process():\n" + "   return a");
            interpreter.exec("a += 1");
            assertEquals(124L, interpreter.invoke("process"));
        }
    }

    @Test
    public void testCallbackJavaWithAllTypes() {
        try {
            PythonInterpreterConfig config =
                    PythonInterpreterConfig.newBuilder().addPythonPaths(testDir).build();
            try (PythonInterpreter interpreter = new PythonInterpreter(config)) {
                interpreter.exec("import test_callback_java");

                assertEquals(
                        "pemjajavapython7fffffff--Pemja-is-cool",
                        interpreter.invoke("test_callback_java.test_callback_java_basic"));

                TestObject object = new TestObject();
                interpreter.invoke("test_callback_java.test_callback_with_all_types", object);
                interpreter.invoke("test_callback_java.test_java_call_python", object, interpreter);
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed to call test_call_java in test_callback_java.py", e);
        }
    }

    @Test
    public void testCallbackJavaInMultiThread() throws InterruptedException {
        AtomicReference<Throwable> exceptionReference = new AtomicReference<>();
        Thread threadA =
                new Thread(
                        () -> {
                            try (PythonInterpreter interpreter =
                                    new PythonInterpreter(
                                            PythonInterpreterConfig.newBuilder()
                                                    .addPythonPaths(testDir)
                                                    .build())) {
                                try {
                                    interpreter.exec("import test_callback_java");
                                    assertEquals(
                                            "pemjajavapython7fffffff--Pemja-is-cool",
                                            interpreter.invoke(
                                                    "test_callback_java.test_callback_java_basic"));
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
                                                    .addPythonPaths(testDir)
                                                    .build())) {
                                try {
                                    interpreter.exec("import test_callback_java");
                                    assertEquals(
                                            "pemjajavapython7fffffff--Pemja-is-cool",
                                            interpreter.invoke(
                                                    "test_callback_java.test_callback_java_basic"));
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

    public static final class TestObject<T> {

        /* ----------------------------------- test boolean ------------------------------------- */

        public String testBoolean(boolean arg) {
            return "testBoolean_boolean";
        }

        public String testBoolean(Boolean arg) {
            return "testBoolean_Boolean";
        }

        public String testBoolean(T arg) {
            return "testBoolean_T";
        }

        public String testBooleanObj(Boolean arg) {
            return "testBooleanObj_arg";
        }

        public String testBooleanObj(T arg) {
            return "testBooleanObj_T";
        }

        public String testBooleanGeneric(T obj) {
            return "testBooleanGeneric_T";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test long ---------------------------------------- */

        public String testLong(int arg) {
            return "testLong_int";
        }

        public String testLong(long arg) {
            return "testLong_long";
        }

        public String testLong(Integer arg) {
            return "testLong_Integer";
        }

        public String testLong(Long arg) {
            return "testLong_Long";
        }

        public String testLong(Number arg) {
            return "testLong_Number";
        }

        public String testLong(T arg) {
            return "testLong_T";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test int ----------------------------------------- */

        public String testInt(int arg) {
            return "testInt_int";
        }

        public String testInt(Integer arg) {
            return "testInt_Integer";
        }

        public String testInt(Number arg) {
            return "testInt_Number";
        }

        public String testInt(T arg) {
            return "testInt_T";
        }
        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Number -------------------------------------- */

        public String testNumber(Number arg) {
            return "testNumber_Number";
        }

        public String testNumber(T arg) {
            return "testNumber_T";
        }

        public String testGeneric(T arg) {
            return "testGeneric_T";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Collection ---------------------------------- */

        public String testCollection(Collection collection) {
            return "testCollection";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Iterable ------------------------------------ */

        public String testIterable(Iterable iterable) {
            return "testIterable";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test String -------------------------------------- */

        public String testString(String arg) {
            return "testString_String";
        }

        public String testString(CharSequence arg) {
            return "testString_CharSequence";
        }

        public String testString(T arg) {
            return "testString_T";
        }

        public String testStringCharSequence(CharSequence arg) {
            return "testStringCharSequence_CharSequence";
        }

        public String testStringGeneric(T arg) {
            return "testStringGeneric_T";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test vargs --------------------------------------- */

        public String testVargs(String arg, String... otherArgs) {
            return "testVargs";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test bytes --------------------------------------- */

        public String testBytes(byte[] arg) {
            return "testBytes_byte[]";
        }

        public String testBytesGeneric(T arg) {
            return "testBytesGeneric_T";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test List ---------------------------------------- */

        public String testList(List list) {
            return "testList";
        }

        public String testGenericList(T list) {
            return "testGenericList_T";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Map ----------------------------------------- */

        public String testMap(Map map) {
            return "testMap";
        }

        public String testGenericMap(Map map) {
            return "testGenericMap";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Map ----------------------------------------- */

        public String testArray(String[] args) {
            return "testArray";
        }

        public String testIntArray(int[] args) {
            return "testIntArray";
        }

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Field --------------------------------------- */

        public static final String NAME = "TestObject";

        /* -------------------------------------------------------------------------------------- */

        /* ----------------------------------- test Java Call Python ---------------------------- */
        public String testJavaCallPython(Interpreter interpreter) {
            interpreter.exec("a = 'testJavaCallPython'");
            return interpreter.get("a", String.class);
        }
        /* -------------------------------------------------------------------------------------- */
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
