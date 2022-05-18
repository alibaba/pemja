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

import java.io.Serializable;
import java.util.Map;

/** An interface for working in non-java language interpreter. */
public interface Interpreter extends AutoCloseable, Serializable {
    /**
     * Sets the Java Object into the interpreter's global scope with the specified variable name.
     *
     * @param name the variable name
     * @param value the variable value
     */
    void set(String name, Object value);

    /**
     * Retrieves the value to the specified name from the interpreter. The return type of the value
     * is the Object.
     *
     * @param name the variable name
     * @return the value mapped to the specified name in the interpreter's global scope
     */
    Object get(String name);

    /**
     * Retrieves the value to the specified name from the interpreter. The return type of the value
     * is the T mapped to Class<T>.
     *
     * @param name the variable name
     * @param tClass the corresponding Java Class of the value.
     * @return the value mapped to the specified name in the interpreter's global scope
     */
    <T> T get(String name, Class<T> tClass);

    /**
     * Invokes a callable function with a variable number of arguments args.
     *
     * @param name the function name
     * @param args the variable number of arguments
     * @return the function result
     */
    Object invoke(String name, Object... args);

    /**
     * Invokes a callable function with a named arguments given by the dictionary kwargs.
     *
     * @param name the function name
     * @param kwargs the named arguments
     * @return the function result
     */
    Object invoke(String name, Map<String, Object> kwargs);

    /**
     * Invokes a callable function with a variable number of arguments args and a named arguments
     * given by the dictionary kwargs.
     *
     * @param name the function name
     * @param args the variable number of arguments
     * @param kwargs the named arguments
     * @return the function result
     */
    Object invoke(String name, Object[] args, Map<String, Object> kwargs);

    /**
     * Invokes the method of a called object with a variable number of arguments args.
     *
     * @param obj the called object
     * @param method the name of the method
     * @param args the variable number of arguments
     * @return the result of the method
     */
    Object invokeMethod(String obj, String method, Object... args);

    /** Execute an arbitrary number of statements in this interpreter. */
    void exec(String code);
}
