################################################################################
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

class A(object):
    def __init__(self):
        self._a = 0

    def get_value(self):
        return self._a

    def add(self, n):
        self._a += n
        return self._a

    def add_all(self, *args):
        for item in args:
            self._a += item
        return self._a

    def minus(self, n):
        self._a -= n
        return self._a


def test_call_no_args():
    return 'no arg'


def test_call_one_arg(arg):
    return arg


def test_call_variable_args(*args):
    return args[0]


def test_call_keywords_args(**kwargs):
    return kwargs['a']


def test_call_all_args(*args, **kwargs):
    return args[0] + kwargs['a']


def test_callback_java():
    from pemja import findClass

    StringBuilder = findClass('java.lang.StringBuilder')
    Integer = findClass('java.lang.Integer')

    sb = StringBuilder()
    sb.append('pemja')
    sb.append('java')
    sb.append('python')
    sb.append(Integer.toHexString(Integer.MAX_VALUE))
    return sb.toString()


def test_return_generator(num: int):
    for i in range(num):
        yield i
    yield "haha"
    yield None
