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

import java.io.File;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * A configuration object for constructing PythonInterpreter instance.
 *
 * <p>For common or important configuration options, this class provides getters and setters methods
 * * with detailed inline documentation.
 *
 * <p>For example:
 *
 * <pre>{@code
 * PythonInterpreterConfig config =
 *       PythonInterpreterConfig.newBuilder()
 *               .setPythonExec("/path/venv/bin/python")
 *               .addPythonPaths("/path/package")
 *               .build();
 *
 * }</pre>
 */
public final class PythonInterpreterConfig {

    /** Defines the search paths for module files. */
    private final String[] paths;

    /** Defines the path of python interpreter. */
    private final String pythonExec;

    /** Defines the execution type of python interpreter. */
    private final ExecType execType;

    private PythonInterpreterConfig(String[] paths, String pythonExec, ExecType execType) {
        this.paths = paths;
        this.pythonExec = pythonExec;
        this.execType = execType;
    }

    /** Returns the search paths. */
    public String[] getPaths() {
        return paths;
    }

    /** Returns the path of python interpreter. */
    public String getPythonExec() {
        return pythonExec;
    }

    /** Returns the execType of python interpreter. */
    public ExecType getExecType() {
        return execType;
    }

    /** A builder for configuring the {@link PythonInterpreterConfig}. */
    public static PythonInterpreterConfigBuilder newBuilder() {
        return new PythonInterpreterConfigBuilder();
    }

    public static class PythonInterpreterConfigBuilder {
        private Set<String> paths = new LinkedHashSet<>();

        private String pythonExec = null;

        private ExecType execType = ExecType.MULTI_THREAD;

        /**
         * Adds the search paths for module files. One or more directory path names separated by
         * <code>{@link File#pathSeparator}</code>
         *
         * <pre>
         *  PythonInterpreterConfig config =
         *      PythonInterpreterConfig.newBuilder()
         *          .addPythonPaths("path1:path2:path3")
         *          .build();
         * </pre>
         *
         * @param paths One or more directory path names separated by {@link File#pathSeparator}.
         */
        public PythonInterpreterConfigBuilder addPythonPaths(String paths) {
            return addPythonPaths(paths.split(File.pathSeparator));
        }

        /**
         * Adds multiple search paths for module files.
         *
         * <pre>
         * PythonInterpreterConfig config =
         *      PythonInterpreterConfig.newBuilder()
         *          .addPythonPaths("path1", "path2", "path3")
         *          .build();
         * </pre>
         *
         * @param paths Multiple search paths.
         */
        public PythonInterpreterConfigBuilder addPythonPaths(String... paths) {
            this.paths.addAll(Arrays.asList(paths));
            return this;
        }

        /**
         * Configures the path of python interpreter.
         *
         * <pre>
         *  PythonInterpreterConfig config =
         *      PythonInterpreterConfig.newBuilder()
         *          .setPythonExec("/path/venv/bin/python")
         *          .build();
         * </pre>
         *
         * @param pythonExec The path of python interpreter.
         */
        public PythonInterpreterConfigBuilder setPythonExec(String pythonExec) {
            this.pythonExec = pythonExec;
            return this;
        }

        /** Configures the exec type of python interpreter. */
        public PythonInterpreterConfigBuilder setExcType(ExecType excType) {
            this.execType = excType;
            return this;
        }

        /** Creates the actual {@link PythonInterpreterConfig}. */
        public PythonInterpreterConfig build() {
            return new PythonInterpreterConfig(paths.toArray(new String[0]), pythonExec, execType);
        }
    }

    /** The Execution type specifies how to execute the Python function in concurrency. */
    public enum ExecType {

        /** Python supports execute the python function in multi thread. */
        MULTI_THREAD,

        /**
         * Python Sub Interpreter has better performance than multi thread because it can better
         * overcome the performance impact of GIL, but some Python libraries including CPython
         * extensions may not support sub interpreter.
         */
        SUB_INTERPRETER
    }
}
