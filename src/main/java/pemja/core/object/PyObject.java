/*
 * Copyright 2022 Alibaba Group Holding Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package pemja.core.object;

/** A Java object that wraps a Python object. */
public class PyObject implements AutoCloseable {
    final long tState;
    public final long pyobject;

    protected PyObject(long tState, long pyobject) {
        this.tState = tState;
        this.pyobject = pyobject;
    }

    public Object getAttr(String attr) {
        return getAttr(attr, Object.class);
    }

    public <T> T getAttr(String attr, Class<T> clazz) {
        return clazz.cast(getAttr(tState, pyobject, attr, clazz));
    }

    public void setAttr(String attr, Object value) {
        setAttr(tState, pyobject, attr, value);
    }

    public Object invokeMethod(String name, Object... args) {
        if (args.length == 0) {
            return invokeMethodNoArgs(tState, pyobject, name);
        } else if (args.length == 1) {
            return invokeMethodOneArg(tState, pyobject, name, args[0]);
        } else {
            return invokeMethod(tState, pyobject, name, args);
        }
    }

    @Override
    public void close() throws Exception {
        decRef(tState, pyobject);
    }

    private native void decRef(long tsState, long pyobject);

    private native Object getAttr(long tState, long pyobject, String attr, Class<?> clazz);

    private native void setAttr(long tState, long pyobject, String attr, Object value);

    private native Object invokeMethodNoArgs(long tState, long pyobject, String name);

    private native Object invokeMethodOneArg(long tState, long pyobject, String name, Object arg);

    private native Object invokeMethod(long tState, long pyobject, String name, Object[] args);
}
