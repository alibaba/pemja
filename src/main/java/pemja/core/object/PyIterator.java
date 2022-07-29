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

package pemja.core.object;

import java.util.Iterator;
import java.util.NoSuchElementException;

/** The Iterator implementation for Python Iterator. */
public class PyIterator extends PyObject implements Iterator {

    private boolean needFetch = true;

    private boolean stopIteration = false;

    private Object element;

    private PyIterator(long tsState, long pyIter) {
        super(tsState, pyIter);
    }

    @Override
    public boolean hasNext() {
        if (!stopIteration) {
            try {
                if (needFetch) {
                    element = next(tState, pyobject);
                    needFetch = false;
                }
            } catch (NoSuchElementException e) {
                stopIteration = true;
            }
        }
        return !stopIteration;
    }

    @Override
    public Object next() {
        if (needFetch) {
            element = next(tState, pyobject);
        }
        needFetch = true;
        return element;
    }

    private native Object next(long tsState, long pyIter);
}
