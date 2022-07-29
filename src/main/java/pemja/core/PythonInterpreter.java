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

import pemja.utils.CommonUtils;

import java.io.Serializable;
import java.lang.reflect.Field;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.CountDownLatch;

/** The Interpreter implementation for Python Interpreter. */
public final class PythonInterpreter implements Interpreter {

    private static final long serialVersionUID = 1L;

    private final MainInterpreter mainInterpreter = MainInterpreter.instance;

    /**
     * Mapping to the only `PyThreadState` in Python Interpreter (see
     * https://docs.python.org/3/c-api/init.html#c.PyThreadState).
     */
    private long tState = 0;

    /**
     * Creates a new PythonInterpreter instance.
     *
     * @param config the PythonInterpreterConfig object for constructing a Python Interpreter.
     */
    public PythonInterpreter(PythonInterpreterConfig config) {
        initialize(config);
    }

    @Override
    public void set(String name, Object value) {
        checkPythonInterpreterRunning();
        if (value instanceof String) {
            set(tState, name, (String) value);
        } else if (value instanceof Boolean) {
            set(tState, name, ((Boolean) value).booleanValue());
        } else if (value instanceof Byte) {
            set(tState, name, ((Byte) value).byteValue());
        } else if (value instanceof Character) {
            set(tState, name, new String(new char[] {(char) value}));
        } else if (value instanceof Short) {
            set(tState, name, ((Short) value).shortValue());
        } else if (value instanceof Integer) {
            set(tState, name, ((Integer) value).intValue());
        } else if (value instanceof Long) {
            set(tState, name, ((Long) value).longValue());
        } else if (value instanceof Float) {
            set(tState, name, ((Float) value).floatValue());
        } else if (value instanceof Double) {
            set(tState, name, ((Double) value).doubleValue());
        } else {
            set(tState, name, value);
        }
    }

    @Override
    public Object get(String name) {
        return get(name, Object.class);
    }

    @Override
    public <T> T get(String name, Class<T> clazz) {
        checkPythonInterpreterRunning();
        return clazz.cast(get(tState, name, clazz));
    }

    @Override
    public Object invoke(String name, Object... args) {
        checkPythonInterpreterRunning();
        int argNum = args.length;
        if (argNum == 0) {
            return invokeNoArgs(tState, name);
        } else if (argNum == 1) {
            return invokeOneArg(name, args[0]);
        }
        return invoke(name, args, null);
    }

    @Override
    public Object invoke(String name, Map<String, Object> kwargs) {
        return invoke(name, null, kwargs);
    }

    @Override
    public Object invoke(String name, Object[] args, Map<String, Object> kwargs) {
        checkPythonInterpreterRunning();
        return invoke(tState, name, args, kwargs);
    }

    @Override
    public Object invokeMethod(String obj, String name, Object... args) {
        checkPythonInterpreterRunning();
        int argNum = args.length;
        if (argNum == 0) {
            return invokeMethodNoArgs(tState, obj, name);
        } else if (argNum == 1) {
            return invokeMethodOneArg(obj, name, args[0]);
        } else {
            return invokeMethod(tState, obj, name, args);
        }
    }

    @Override
    public void exec(String str) {
        checkPythonInterpreterRunning();
        exec(tState, str);
    }

    @Override
    public void close() {
        if (tState > 0) {
            try {
                finalize(tState);
            } finally {
                tState = 0;
            }
        }
    }

    /**
     * Initializes the {@link PythonInterpreter} with the provided {@link PythonInterpreterConfig}.
     *
     * @param config the specified {@link PythonInterpreterConfig}.
     */
    private void initialize(PythonInterpreterConfig config) {
        mainInterpreter.initialize(config.getPythonExec());
        this.tState = init(config.getExecType().ordinal());

        synchronized (PythonInterpreter.class) {
            configSearchPaths(config.getPaths());
        }
    }

    /** Config Search Paths in the current {@link PythonInterpreter} instance */
    private void configSearchPaths(String[] paths) {
        if (paths != null) {
            exec("import sys");
            for (int i = paths.length - 1; i >= 0; i--) {
                exec(String.format("sys.path.insert(0, '%s')", paths[i]));
            }
        }
    }

    /** Checks if the python interpreter is running. */
    private void checkPythonInterpreterRunning() {
        if (tState == 0) {
            throw new RuntimeException(
                    "The python interpreter has not been started. You need to call the `open` method before.");
        }
    }

    /**
     * Invokes a callable function with exactly 1 positional argument arg.
     *
     * @param name the function name
     * @param arg the only positional argument
     * @return the function result
     */
    private Object invokeOneArg(String name, Object arg) {
        if (arg instanceof String) {
            return invokeOneArgString(tState, name, (String) arg);
        } else if (arg instanceof Boolean) {
            return invokeOneArgBoolean(tState, name, (Boolean) arg);
        } else if (arg instanceof Integer) {
            return invokeOneArgInt(tState, name, (Integer) arg);
        } else if (arg instanceof Float) {
            return invokeOneArgDouble(tState, name, (Float) arg);
        } else if (arg instanceof Double) {
            return invokeOneArgDouble(tState, name, (Double) arg);
        } else if (arg instanceof Long) {
            return invokeOneArgLong(tState, name, (Long) arg);
        } else if (arg instanceof Character) {
            return invokeOneArgString(tState, name, new String(new char[] {(char) arg}));
        } else if (arg instanceof Byte) {
            return invokeOneArgInt(tState, name, (Byte) arg);
        } else if (arg instanceof Short) {
            return invokeOneArgInt(tState, name, (Short) arg);
        } else {
            return invokeOneArgObject(tState, name, arg);
        }
    }

    /**
     * Invokes the method of a called object with exactly 1 positional argument arg.
     *
     * @param obj the called object
     * @param method the method name
     * @param arg the only positional argument
     * @return the result of the method
     */
    private Object invokeMethodOneArg(String obj, String method, Object arg) {
        if (arg instanceof String) {
            return invokeMethodOneArgString(tState, obj, method, (String) arg);
        } else if (arg instanceof Boolean) {
            return invokeMethodOneArgBoolean(tState, obj, method, (Boolean) arg);
        } else if (arg instanceof Integer) {
            return invokeMethodOneArgInt(tState, obj, method, (Integer) arg);
        } else if (arg instanceof Float) {
            return invokeMethodOneArgDouble(tState, obj, method, (Float) arg);
        } else if (arg instanceof Double) {
            return invokeMethodOneArgDouble(tState, obj, method, (Double) arg);
        } else if (arg instanceof Long) {
            return invokeMethodOneArgLong(tState, obj, method, (Long) arg);
        } else if (arg instanceof Character) {
            return invokeMethodOneArgString(
                    tState, obj, method, new String(new char[] {(char) arg}));
        } else if (arg instanceof Byte) {
            return invokeMethodOneArgInt(tState, obj, method, (Byte) arg);
        } else if (arg instanceof Short) {
            return invokeMethodOneArgInt(tState, obj, method, (Short) arg);
        } else {
            return invokeMethodOneArgObject(tState, obj, method, arg);
        }
    }

    /**
     * Inits a JcpThread runs in an Independent PyThread.
     *
     * @return the JcpThread structure pointer.
     */
    private native long init(int execType);

    /**
     * Finalize the JcpThread and free the resources.
     *
     * @param tState the JcpThread
     */
    private native void finalize(long tState);

    /*--------- Set/Get the Java Object into JcpThread variable tables -------------*/

    private native void set(long tState, String name, boolean value);

    private native void set(long tState, String name, int value);

    private native void set(long tState, String name, long value);

    private native void set(long tState, String name, double value);

    private native void set(long tState, String name, String value);

    private native void set(long tState, String name, Object value);

    private native Object get(long tState, String name, Class<?> clazz);

    /*---------------------------------------------------------------------------------------*/

    /*------------------------------ Invokes a callable function ----------------------------*/

    private native Object invokeNoArgs(long tState, String name);

    private native Object invokeOneArgBoolean(long tState, String name, boolean arg);

    private native Object invokeOneArgInt(long tState, String name, int arg);

    private native Object invokeOneArgLong(long tState, String name, long arg);

    private native Object invokeOneArgDouble(long tState, String name, double arg);

    private native Object invokeOneArgString(long tState, String name, String arg);

    private native Object invokeOneArgObject(long tState, String name, Object arg);

    private native Object invoke(
            long tState, String name, Object[] args, Map<String, Object> kwargs);

    /*---------------------------------------------------------------------------------------*/

    /*------------------------- Invokes the method of a called object -----------------------*/

    private native Object invokeMethodNoArgs(long tState, String obj, String name);

    private native Object invokeMethodOneArgBoolean(
            long tState, String obj, String name, boolean arg);

    private native Object invokeMethodOneArgInt(long tState, String obj, String name, int arg);

    private native Object invokeMethodOneArgLong(long tState, String obj, String name, long arg);

    private native Object invokeMethodOneArgDouble(
            long tState, String obj, String name, double arg);

    private native Object invokeMethodOneArgString(
            long tState, String obj, String name, String arg);

    private native Object invokeMethodOneArgObject(
            long tState, String obj, String name, Object arg);

    private native Object invokeMethod(long tState, String obj, String name, Object[] args);

    /*---------------------------------------------------------------------------------------*/

    /**
     * Execute an arbitrary number of statements in the JcpThread.
     *
     * @param tState the JcpThread
     * @param code the python code
     */
    private native void exec(long tState, String code);

    /**
     * The main Python interpreter that all sub-interpreters will be created from. The
     * MainInterpreter is used to avoid potential deadlocks. Python can deadlock when trying to
     * acquire the GIL through methods such as <a href=
     * "https://docs.python.org/3/c-api/init.html#c.PyGILState_Ensure">PyGILState_*</a>.
     *
     * <p>While PemJa does not use those methods, CPython extensions such as numpy do. The deadlock
     * can occur if there is more than one PyThreadState per thread. To get around this, the
     * MainInterpreter creates a unique thread that initializes Python and keeps this thread around
     * forever. This ensures that any new sub-interpreters cannot be created on the same thread as
     * the main Python interpreter.
     */
    private static class MainInterpreter implements Serializable, AutoCloseable {

        private static final long serialVersionUID = 1L;

        private static final MainInterpreter instance = new MainInterpreter();

        private final CountDownLatch damonThreadStart = new CountDownLatch(1);

        private final CountDownLatch damonThreadFinish = new CountDownLatch(1);

        /** The flag whether the main interpreter has been started. */
        private boolean isStarted = false;

        private Thread thread;

        private Throwable error;

        private MainInterpreter() {}

        /** Initializes CPython. */
        @SuppressWarnings("unchecked")
        synchronized void initialize(String pythonExec) {
            if (!isStarted) {
                String pemjaLibPath =
                        CommonUtils.INSTANCE.getLibraryPathWithPattern(
                                pythonExec, "^pemja_core\\.cpython-.*\\.so$");
                String pythonLibPath = CommonUtils.INSTANCE.getPythonLibrary(pythonExec);
                String pemjaModulePath = CommonUtils.INSTANCE.getPemJaModulePath(pythonExec);

                try {
                    System.load(pythonLibPath);
                    if (CommonUtils.INSTANCE.isLinuxOs()) {
                        // We need to load libpython in unix globally.
                        CommonUtils.INSTANCE.loadLibrary(pythonExec, pythonLibPath);
                    }
                } catch (UnsatisfiedLinkError error) {
                    // ignore
                }

                try {
                    System.load(pemjaLibPath);
                } catch (UnsatisfiedLinkError error) {
                    try {
                        Field field = ClassLoader.class.getDeclaredField("loadedLibraryNames");
                        field.setAccessible(true);
                        Vector<String> libs = (Vector<String>) field.get(null);
                        synchronized (libs) {
                            int size = libs.size();
                            for (int i = 0; i < size; i++) {
                                String element = libs.elementAt(i);
                                if (element.contains("pemja_core")) {
                                    libs.removeElementAt(i);
                                }
                            }
                        }
                        System.load(pemjaLibPath);
                    } catch (Throwable throwable) {
                        // ignore
                    }
                }

                // We load on a separate thread to try and avoid GIL issues that come about from a
                // being on the same thread as the main interpreter.
                thread =
                        new Thread("PemJaMainInterpreter") {
                            @Override
                            public void run() {
                                try {
                                    initialize();
                                    // add shared modules
                                    addToPath(pemjaModulePath);
                                    importModule("redirect_stream");
                                } catch (Throwable t) {
                                    error = t;
                                } finally {
                                    damonThreadStart.countDown();
                                }

                                try {
                                    damonThreadFinish.await();
                                } catch (InterruptedException e) {
                                    throw new Error(e);
                                }
                            }
                        };
                thread.setDaemon(true);
                thread.start();

                try {
                    damonThreadStart.await();
                } catch (InterruptedException e) {
                    if (error == null) {
                        error = e;
                    }
                }

                if (error != null) {
                    throw new Error(error);
                }
                isStarted = true;
            }
        }

        @Override
        public void close() {
            if (thread != null) {
                damonThreadFinish.countDown();
            }
        }

        /** Initialize Python Interpreter. */
        private native void initialize();

        /** Adds the search path to the main interpreter. */
        private native void addToPath(String path);

        /** Imports the shared modules to the main interpreter. */
        private native void importModule(String module);
    }
}
