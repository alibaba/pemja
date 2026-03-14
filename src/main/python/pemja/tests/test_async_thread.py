################################################################################
#
#  Copyright 2022 Alibaba Group Holding Limited.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

import asyncio
import threading


# ---------------------------------------------------------------------------
#  asyncio tests — the event loop runs on a non-pemja thread
# ---------------------------------------------------------------------------

def test_return_custom_object_in_asyncio(java_obj):
    """
    Test that calling a Java method which returns a custom Java object
    from an asyncio event loop (running on a non-pemja thread) does not
    crash the JVM.

    Before the fix: SIGSEGV in JcpPyJObject_New (NULL deref on JcpThread).
    After the fix: works normally.
    """
    result = [None]
    error = [None]

    async def coro():
        # This runs on the event loop thread (a non-pemja thread).
        # returnSelf() returns a custom Java object (TestObject), which
        # goes through JcpPyJObject_New -> pyjobject_init -> JcpThread_Get().
        obj = java_obj.returnSelf()
        return str(obj)

    def run_loop():
        try:
            loop = asyncio.new_event_loop()
            asyncio.set_event_loop(loop)
            result[0] = loop.run_until_complete(coro())
            loop.close()
        except Exception as e:
            error[0] = str(e)

    t = threading.Thread(target=run_loop)
    t.start()
    t.join(timeout=10)

    if error[0] is not None:
        raise RuntimeError("Error in asyncio thread: " + error[0])

    return result[0]


def test_return_string_in_asyncio(java_obj):
    """
    Control: returning a String (built-in type) from asyncio should always
    work, even without the fix.
    """
    result = [None]
    error = [None]

    async def coro():
        return java_obj.returnString()

    def run_loop():
        try:
            loop = asyncio.new_event_loop()
            asyncio.set_event_loop(loop)
            result[0] = loop.run_until_complete(coro())
            loop.close()
        except Exception as e:
            error[0] = str(e)

    t = threading.Thread(target=run_loop)
    t.start()
    t.join(timeout=10)

    if error[0] is not None:
        raise RuntimeError("Error in asyncio thread: " + error[0])

    return result[0]


# ---------------------------------------------------------------------------
#  Same-thread control test
# ---------------------------------------------------------------------------

def test_return_custom_object_in_same_thread(java_obj):
    """
    Control: calling on the pemja thread itself should always work.
    """
    obj = java_obj.returnSelf()
    return str(obj)
