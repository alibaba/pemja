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

def test_callback_java_basic():
    from pemja import findClass

    StringBuilder = findClass('java.lang.StringBuilder')
    Integer = findClass('java.lang.Integer')
    String = findClass('java.lang.String')

    sb = StringBuilder()
    sb.append('pemja')
    sb.append('java')
    sb.append('python')
    sb.append(Integer.toHexString(Integer.MAX_VALUE))
    sb.append(String.join("-", "Pemja", "is", "cool"))
    return sb.toString()


def test_callback_with_all_types(self):
    # bool
    assert_equals(self.testBoolean(True), "testBoolean_boolean")
    assert_equals(self.testBooleanObj(True), "testBooleanObj_arg")
    assert_equals(self.testBooleanGeneric(True), "testBooleanGeneric_T")

    # int/long/number
    assert_equals(self.testLong(123), "testLong_long")
    assert_equals(self.testInt(123), "testInt_int")
    assert_equals(self.testNumber(123), "testNumber_Number")
    assert_equals(self.testGeneric(123), "testGeneric_T")

    # collection
    assert_equals(self.testCollection([1, 2, 3]), "testCollection")

    # iterable
    assert_equals(self.testIterable([1, 2, 3]), "testIterable")

    # String
    assert_equals(self.testString("abc"), "testString_String")
    assert_equals(self.testStringCharSequence("abc"), "testStringCharSequence_CharSequence")
    assert_equals(self.testStringGeneric("abc"), "testStringGeneric_T")

    # bytes
    assert_equals(self.testBytes(b'123'), "testBytes_byte[]")
    assert_equals(self.testBytesGeneric(b'123'), "testBytesGeneric_T")

    # List
    assert_equals(self.testList([1, 2, 3]), "testList")
    assert_equals(self.testGenericList([1, 2, 3]), "testGenericList_T")

    # Map
    assert_equals(self.testMap({1: 2, 2: 3}), "testMap")
    assert_equals(self.testGenericMap({1: "str", 2: 3}), "testGenericMap")

    # Array
    assert_equals(self.testArray(("pemja", "java")), "testArray")
    assert_equals(self.testIntArray((1, 2)), "testIntArray")

    # field
    assert_equals(self.NAME, "TestObject")


def test_java_call_python(self, interpreter):
    assert_equals(self.testJavaCallPython(interpreter), 'testJavaCallPython')


def assert_equals(actual, expected):
    assert actual == expected, "{0} is different expected {1}".format(actual, expected)
