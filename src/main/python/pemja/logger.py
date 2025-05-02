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

import logging
import traceback

LOG_LEVEL_TO_LOGENTRY_MAP = {
    logging.FATAL: "CRITICAL",
    logging.ERROR: "ERROR",
    logging.WARNING: "WARN",
    logging.INFO: "INFO",
    logging.DEBUG: "DEBUG",
    logging.NOTSET: "UNSPECIFIED",
    -float('inf'): "DEBUG",
}


def _map_log_level(
        level: int) -> str:
    try:
        return LOG_LEVEL_TO_LOGENTRY_MAP[level]
    except KeyError:
        return max(
            beam_level for python_level,
            beam_level in LOG_LEVEL_TO_LOGENTRY_MAP.items()
            if python_level <= level)


class PythonLogHandler(logging.Handler):
    def __init__(self):
        super().__init__()
        self._logger_writer = None

    def emit(self, record: logging.LogRecord):
        if self._logger_writer is None:
            from pemja import findClass
            PythonLogWriter = findClass('pemja.core.log.PythonLogWriter')
            self._logger_writer = PythonLogWriter()

        message = self.format(record)
        name = '%s:%s' % (
            record.pathname or record.module, record.lineno or record.funcName)
        trace = None
        if record.exc_info:
            trace = ''.join(traceback.format_exception(*record.exc_info))
        severity = _map_log_level(record.levelno)
        self._logger_writer.log(name, severity, message, trace)


logger = logging.getLogger()
logger.setLevel(logging.DEBUG)
python_log_handler = PythonLogHandler()

logger.addHandler(python_log_handler)
