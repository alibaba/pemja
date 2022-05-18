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

import sys
import logging

import redirection


class StdOutCatcher(object):
    """
    Redirects Python's sys.stdout to Java's System.out
    """

    def write(self, stuff):
        redirection.stdout_redirect(stuff)

    def flush(self):
        pass


class StdErrCatcher(object):
    """
    Redirects Python's sys.stderr to Java's System.err
    """

    def write(self, stuff):
        redirection.stderr_redirect(stuff)

    def flush(self):
        pass


sys.stdout = StdOutCatcher()
sys.stderr = StdErrCatcher()
logging.getLogger().info = sys.stdout.write
logging.getLogger().error = sys.stderr.write
