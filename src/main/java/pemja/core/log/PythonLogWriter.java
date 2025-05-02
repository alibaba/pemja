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

package pemja.core.log;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/** The Logger Writer implementation for Python Logging. */
public class PythonLogWriter {
    public PythonLogWriter() {}

    public void log(String name, String severityName, String message, String trace) {
        Logger log = LoggerFactory.getLogger(name);
        Severity severity = Severity.valueOf(severityName);
        switch (severity) {
            case ERROR:
            case CRITICAL:
                if (trace == null) {
                    log.error(message);
                } else {
                    log.error("{} {}", message, trace);
                }
                break;
            case WARN:
                if (trace == null) {
                    log.warn(message);
                } else {
                    log.warn("{} {}", message, trace);
                }
                break;
            case INFO:
            case NOTICE:
                log.info(message);
                break;
            case DEBUG:
                log.debug(message);
                break;
            case UNSPECIFIED:
            case TRACE:
                log.trace(message);
                break;
            default:
                log.warn("Unknown message severity {}", severityName);
                log.info(message);
        }
    }

    private enum Severity {
        ERROR,
        CRITICAL,
        WARN,
        INFO,
        NOTICE,
        DEBUG,
        TRACE,
        UNSPECIFIED
    }
}
