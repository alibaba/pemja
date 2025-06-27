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


def test_call_collection(collection):
    assert len(collection) == 3
    assert 1 in collection
    assert 2 in collection
    assert 3 in collection

    iterator = iter(collection)
    values = [value for value in iterator]
    values = sorted(values, reverse=True)

    assert values == [3, 2, 1]

    return collection


def test_call_iterator(iterator):
    values = [value for value in iterator]
    values = sorted(values, reverse=True)

    assert values == [3, 2, 1]

    return iterator


def test_call_java_object(o):
    return o


def test_map(o):
    assert list(o.keys()) == ["python", "java", "pemja"]
    for i, (k, v) in enumerate(o.items()):
        if i == 0:
            assert k == "python"
            assert v == 3
        if i == 1:
            assert k == "java"
            assert v == 2
        if i == 2:
            assert k == "pemja"
            assert v == 1
    return o.values()


def test_list(o):
    assert len(o) == 3
    assert o[0] == "python"
    assert o[1] == "java"
    assert o[2] == "pemja"
    return o
