# PemJa

## What is it?

PemJa is an open source cross language call framework based on FFI. It aims to provide a high-performance

framework of calling between different languages.

## Where to get it

Python binary installers for the latest released version are available at the [Python package index](https://pypi.org/project/pemja)

```bash
pip install pemja
```

Java Maven Dependency
```
<dependency>
    <groupId>com.alibaba</groupId>
    <artifactId>pemja</artifactId>
    <version>{version}</version>
</dependency>
```

## Dependencies

- [NumPy - Adds support for large, multi-dimensional arrays, matrices and high-level mathematical functions to operate on these arrays](https://www.numpy.org)

## Installation from sources

Prerequisites for building PemJa:

* Unix-like environment (we use Linux, Mac OS X)
* Git
* Maven (we recommend version 3.2.5 and require at least 3.1.1)
* Java 8 or 11 (Java 9 or 10 may work)
* Python >= 3.8 (we recommend version 3.8, 3.9, 3.10, 3.11)

```
git clone https://github.com/alibaba/pemja.git
cd pemja
mvn clean install -DskipTests
pip install -r dev/dev-requirements.txt
python setup.py sdist
pip install dist/*.tar.gz
```

## Usage

```java
String path = ...;
PythonInterpreterConfig config = PythonInterpreterConfig
    .newBuilder()
    .setPythonExec("python3") // specify python exec
    .addPythonPaths(path) // add path to search path
    .build();

PythonInterpreter interpreter = new PythonInterpreter(config);

// set & get
interpreter.set("a", 12345);
interpreter.get("a"); // Object
interpreter.get("a", Integer.class); // Integer

// exec & eval
interpreter.exec("print(a)");

// invoke functions
interpreter.exec("import str_upper");
String result = interpreter.invoke("str_upper.upper", "abcd");
// Object invoke(String name, Object... args);
// Object invoke(String name, Object[] args, Map<String, Object> kwargs);

// invoke object methods
/*
// invoke.py
class A:
       def __init__(self):
           self._a = 0
   
       def get_value(self):
           return self._a
   
       def add(self, n):
           self._a += n
   
       def add_all(self, *args):
           for item in args:
               self._a += item
               return self._a
   
       def minus(self, n):
           self._a -= n
           return self._a
*/

interpreter.exec("import invoke");
interpreter.exec("a = invoke.A()");
interpreter.invokeMethod("a", "add", 3);
interpreter.invokeMethod("a", "minus", 2);
interpreter.invokeMethod("a", "add_all", 1, 2, 3);


// python callback java methods
/*
// invoke_callback.py
from pemja import findClass

StringBuilder = findClass('java.lang.StringBuilder')
Integer = findClass('java.lang.Integer')

def callback_java():
    sb = StringBuilder()
    sb.append('pemja')
    sb.append('java')
    sb.append('python')
    sb.append(Integer.toHexString(Integer.MAX_VALUE))
    return sb.toString()
*/
interpreter.exec("import call_back")
print(interpreter.invoke("call_back.callback_java"))
```

## Documentation
