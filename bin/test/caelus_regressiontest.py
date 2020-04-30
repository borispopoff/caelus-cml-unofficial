#!/usr/bin/env python
import warnings
import sys
import os
import re
import xml.etree.ElementTree as eTree
import copy
import random
import xml.dom.minidom
import time
import glob
import threading
import traceback

from collections import defaultdict
from io import StringIO

"""
decode, TestSuite and TestCase come from Junit_xml https://github.com/kyrus/python-junit-xml

The MIT License

Copyright (c) 2013 Kyrus Tech, Inc., Brian Beyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
"""


def decode(var, encoding):
    """
    If not already unicode, decode it.
    """
    ret = str(var)
    return ret


class TestSuite(object):
    """
    Suite of test cases.
    Can handle unicode strings or binary strings if their encoding is provided.
    """

    def __init__(
        self,
        name,
        test_cases=None,
        hostname=None,
        id=None,
        package=None,
        timestamp=None,
        properties=None,
        file=None,
        log=None,
        url=None,
        stdout=None,
        stderr=None,
    ):
        self.name = name
        if not test_cases:
            test_cases = []
        try:
            iter(test_cases)
        except TypeError:
            raise TypeError("test_cases must be a list of test cases")
        self.test_cases = test_cases
        self.timestamp = timestamp
        self.hostname = hostname
        self.id = id
        self.package = package
        self.file = file
        self.log = log
        self.url = url
        self.stdout = stdout
        self.stderr = stderr
        self.properties = properties

    def build_xml_doc(self, encoding=None):
        """
        Builds the XML document for the JUnit test suite.
        Produces clean unicode strings and decodes non-unicode with the help of encoding.
        @param encoding: Used to decode encoded strings.
        @return: XML document with unicode string elements
        """

        # build the test suite element
        test_suite_attributes = dict()
        if any(c.assertions for c in self.test_cases):
            test_suite_attributes["assertions"] = str(sum([int(c.assertions) for c in self.test_cases if c.assertions]))
        test_suite_attributes["disabled"] = str(len([c for c in self.test_cases if not c.is_enabled]))
        test_suite_attributes["errors"] = str(len([c for c in self.test_cases if c.is_error()]))
        test_suite_attributes["failures"] = str(len([c for c in self.test_cases if c.is_failure()]))
        test_suite_attributes["name"] = decode(self.name, encoding)
        test_suite_attributes["skipped"] = str(len([c for c in self.test_cases if c.is_skipped()]))
        test_suite_attributes["tests"] = str(len(self.test_cases))
        test_suite_attributes["time"] = str(sum(c.elapsed_sec for c in self.test_cases if c.elapsed_sec))

        if self.hostname:
            test_suite_attributes["hostname"] = decode(self.hostname, encoding)
        if self.id:
            test_suite_attributes["id"] = decode(self.id, encoding)
        if self.package:
            test_suite_attributes["package"] = decode(self.package, encoding)
        if self.timestamp:
            test_suite_attributes["timestamp"] = decode(self.timestamp, encoding)
        if self.file:
            test_suite_attributes["file"] = decode(self.file, encoding)
        if self.log:
            test_suite_attributes["log"] = decode(self.log, encoding)
        if self.url:
            test_suite_attributes["url"] = decode(self.url, encoding)

        xml_element = eTree.Element("testsuite", test_suite_attributes)

        # add any properties
        if self.properties:
            props_element = eTree.SubElement(xml_element, "properties")
            for k, v in self.properties.items():
                attrs = {"name": decode(k, encoding), "value": decode(v, encoding)}
                eTree.SubElement(props_element, "property", attrs)

        # add test suite stdout
        if self.stdout:
            stdout_element = eTree.SubElement(xml_element, "system-out")
            stdout_element.text = decode(self.stdout, encoding)

        # add test suite stderr
        if self.stderr:
            stderr_element = eTree.SubElement(xml_element, "system-err")
            stderr_element.text = decode(self.stderr, encoding)

        # test cases
        for case in self.test_cases:
            test_case_attributes = dict()
            test_case_attributes["name"] = decode(case.name, encoding)
            if case.assertions:
                # Number of assertions in the test case
                test_case_attributes["assertions"] = "%d" % case.assertions
            if case.elapsed_sec:
                test_case_attributes["time"] = "%f" % case.elapsed_sec
            if case.timestamp:
                test_case_attributes["timestamp"] = decode(case.timestamp, encoding)
            if case.classname:
                test_case_attributes["classname"] = decode(case.classname, encoding)
            if case.status:
                test_case_attributes["status"] = decode(case.status, encoding)
            if case.category:
                test_case_attributes["class"] = decode(case.category, encoding)
            if case.file:
                test_case_attributes["file"] = decode(case.file, encoding)
            if case.line:
                test_case_attributes["line"] = decode(case.line, encoding)
            if case.log:
                test_case_attributes["log"] = decode(case.log, encoding)
            if case.url:
                test_case_attributes["url"] = decode(case.url, encoding)

            test_case_element = eTree.SubElement(xml_element, "testcase", test_case_attributes)

            # failures
            for failure in case.failures:
                if failure["output"] or failure["message"]:
                    attrs = {"type": "failure"}
                    if failure["message"]:
                        attrs["message"] = decode(failure["message"], encoding)
                    if failure["type"]:
                        attrs["type"] = decode(failure["type"], encoding)
                    failure_element = eTree.Element("failure", attrs)
                    if failure["output"]:
                        failure_element.text = decode(failure["output"], encoding)
                    test_case_element.append(failure_element)

            # errors
            for error in case.errors:
                if error["message"] or error["output"]:
                    attrs = {"type": "error"}
                    if error["message"]:
                        attrs["message"] = decode(error["message"], encoding)
                    if error["type"]:
                        attrs["type"] = decode(error["type"], encoding)
                    error_element = eTree.Element("error", attrs)
                    if error["output"]:
                        error_element.text = decode(error["output"], encoding)
                    test_case_element.append(error_element)

            # skippeds
            for skipped in case.skipped:
                attrs = {"type": "skipped"}
                if skipped["message"]:
                    attrs["message"] = decode(skipped["message"], encoding)
                skipped_element = eTree.Element("skipped", attrs)
                if skipped["output"]:
                    skipped_element.text = decode(skipped["output"], encoding)
                test_case_element.append(skipped_element)

            # test stdout
            if case.stdout:
                stdout_element = eTree.Element("system-out")
                stdout_element.text = decode(case.stdout, encoding)
                test_case_element.append(stdout_element)

            # test stderr
            if case.stderr:
                stderr_element = eTree.Element("system-err")
                stderr_element.text = decode(case.stderr, encoding)
                test_case_element.append(stderr_element)

        return xml_element

    @staticmethod
    def to_xml_string(test_suites, prettyprint=True, encoding=None):
        """
        Returns the string representation of the JUnit XML document.
        @param encoding: The encoding of the input.
        @return: unicode string
        """
        warnings.warn(
            "Testsuite.to_xml_string is deprecated. It will be removed in version 2.0.0. "
            "Use function to_xml_report_string",
            DeprecationWarning,
        )
        return to_xml_report_string(test_suites, prettyprint, encoding)

    @staticmethod
    def to_file(file_descriptor, test_suites, prettyprint=True, encoding=None):
        """
        Writes the JUnit XML document to a file.
        """
        warnings.warn(
            "Testsuite.to_file is deprecated. It will be removed in version 2.0.0. Use function to_xml_report_file",
            DeprecationWarning,
        )
        to_xml_report_file(file_descriptor, test_suites, prettyprint, encoding)


def to_xml_report_string(test_suites, prettyprint=True, encoding=None):
    """
    Returns the string representation of the JUnit XML document.
    @param encoding: The encoding of the input.
    @return: unicode string
    """

    try:
        iter(test_suites)
    except TypeError:
        raise TypeError("test_suites must be a list of test suites")

    xml_element = eTree.Element("testsuites")
    attributes = defaultdict(int)
    for ts in test_suites:
        ts_xml = ts.build_xml_doc(encoding=encoding)
        for key in ["disabled", "errors", "failures", "tests"]:
            attributes[key] += int(ts_xml.get(key, 0))
        for key in ["time"]:
            attributes[key] += float(ts_xml.get(key, 0))
        xml_element.append(ts_xml)
    for key, value in attributes.items():
        xml_element.set(key, str(value))

    xml_string = eTree.tostring(xml_element, encoding=encoding)
    # is encoded now
    xml_string = _clean_illegal_xml_chars(xml_string.decode(encoding or "utf-8"))
    # is unicode now

    if prettyprint:
        # minidom.parseString() works just on correctly encoded binary strings
        xml_string = xml_string.encode(encoding or "utf-8")
        xml_string = xml.dom.minidom.parseString(xml_string)
        # toprettyxml() produces unicode if no encoding is being passed or binary string with an encoding
        xml_string = xml_string.toprettyxml(encoding=encoding)
        if encoding:
            xml_string = xml_string.decode(encoding)
        # is unicode now
    return xml_string


def to_xml_report_file(file_descriptor, test_suites, prettyprint=True, encoding=None):
    """
    Writes the JUnit XML document to a file.
    """
    xml_string = to_xml_report_string(test_suites, prettyprint=prettyprint, encoding=encoding)
    # has problems with encoded str with non-ASCII (non-default-encoding) characters!
    file_descriptor.write(xml_string)


def _clean_illegal_xml_chars(string_to_clean):
    """
    Removes any illegal unicode characters from the given XML string.

    @see: http://stackoverflow.com/questions/1707890/fast-way-to-filter-illegal-xml-unicode-chars-in-python
    """

    illegal_unichrs = [
        (0x00, 0x08),
        (0x0B, 0x1F),
        (0x7F, 0x84),
        (0x86, 0x9F),
        (0xD800, 0xDFFF),
        (0xFDD0, 0xFDDF),
        (0xFFFE, 0xFFFF),
        (0x1FFFE, 0x1FFFF),
        (0x2FFFE, 0x2FFFF),
        (0x3FFFE, 0x3FFFF),
        (0x4FFFE, 0x4FFFF),
        (0x5FFFE, 0x5FFFF),
        (0x6FFFE, 0x6FFFF),
        (0x7FFFE, 0x7FFFF),
        (0x8FFFE, 0x8FFFF),
        (0x9FFFE, 0x9FFFF),
        (0xAFFFE, 0xAFFFF),
        (0xBFFFE, 0xBFFFF),
        (0xCFFFE, 0xCFFFF),
        (0xDFFFE, 0xDFFFF),
        (0xEFFFE, 0xEFFFF),
        (0xFFFFE, 0xFFFFF),
        (0x10FFFE, 0x10FFFF),
    ]

    illegal_ranges = ["%s-%s" % (chr(low), chr(high)) for (low, high) in illegal_unichrs if low < sys.maxunicode]

    illegal_xml_re = re.compile("[%s]" % "".join(illegal_ranges))
    return illegal_xml_re.sub("", string_to_clean)


class TestCase(object):
    """A JUnit test case with a result and possibly some stdout or stderr"""

    def __init__(
        self,
        name,
        classname=None,
        elapsed_sec=None,
        stdout=None,
        stderr=None,
        assertions=None,
        timestamp=None,
        status=None,
        category=None,
        file=None,
        line=None,
        log=None,
        url=None,
        allow_multiple_subelements=False,
    ):
        self.name = name
        self.assertions = assertions
        self.elapsed_sec = elapsed_sec
        self.timestamp = timestamp
        self.classname = classname
        self.status = status
        self.category = category
        self.file = file
        self.line = line
        self.log = log
        self.url = url
        self.stdout = stdout
        self.stderr = stderr

        self.is_enabled = True
        self.errors = []
        self.failures = []
        self.skipped = []
        self.allow_multiple_subalements = allow_multiple_subelements

    def add_error_info(self, message=None, output=None, error_type=None):
        """Adds an error message, output, or both to the test case"""
        error = {}
        error["message"] = message
        error["output"] = output
        error["type"] = error_type
        if self.allow_multiple_subalements:
            if message or output:
                self.errors.append(error)
        elif not len(self.errors):
            self.errors.append(error)
        else:
            if message:
                self.errors[0]["message"] = message
            if output:
                self.errors[0]["output"] = output
            if error_type:
                self.errors[0]["type"] = error_type

    def add_failure_info(self, message=None, output=None, failure_type=None):
        """Adds a failure message, output, or both to the test case"""
        failure = {}
        failure["message"] = message
        failure["output"] = output
        failure["type"] = failure_type
        if self.allow_multiple_subalements:
            if message or output:
                self.failures.append(failure)
        elif not len(self.failures):
            self.failures.append(failure)
        else:
            if message:
                self.failures[0]["message"] = message
            if output:
                self.failures[0]["output"] = output
            if failure_type:
                self.failures[0]["type"] = failure_type

    def add_skipped_info(self, message=None, output=None):
        """Adds a skipped message, output, or both to the test case"""
        skipped = {}
        skipped["message"] = message
        skipped["output"] = output
        if self.allow_multiple_subalements:
            if message or output:
                self.skipped.append(skipped)
        elif not len(self.skipped):
            self.skipped.append(skipped)
        else:
            if message:
                self.skipped[0]["message"] = message
            if output:
                self.skipped[0]["output"] = output

    def is_failure(self):
        """returns true if this test case is a failure"""
        return sum(1 for f in self.failures if f["message"] or f["output"]) > 0

    def is_error(self):
        """returns true if this test case is an error"""
        return sum(1 for e in self.errors if e["message"] or e["output"]) > 0

    def is_skipped(self):
        """returns true if this test case has been skipped"""
        return len(self.skipped) > 0














class TestProblem:
    """A test records input information as well as tests for the output."""

    def __init__(self, filename, project_dir, ref_path, nprocs=1, verbose=False):
        """Read a regression test from filename and record its details."""
        self.name = ""
        self.command_line = ""
        self.clean_command_line = ""
        self.length = ""
        self.nprocs = nprocs
        self.verbose = verbose
        self.variables = []
        self.pass_tests = []
        self.warn_tests = []
        self.pass_status = []
        self.warn_status = []
        self.filename = filename.split("/")[-1]
        self.xml_reports = []
        self.project_dir = project_dir
        self.ref_path = ref_path

        # add dir to import path
        sys.path.insert(0, os.path.dirname(filename))

        dom = xml.dom.minidom.parse(filename)

        probtag = dom.getElementsByTagName("testproblem")[0]

        for child in probtag.childNodes:
            try:
                tag = child.tagName
            except AttributeError:
                continue

            if tag == "name":
                self.name = child.childNodes[0].nodeValue
            elif tag == "problem_definition":
                self.length = child.getAttribute("length")
                self.nprocs = int(child.getAttribute("nprocs"))
                self.command_line = (
                    child.getElementsByTagName("command_line")[0]
                    .childNodes[0]
                    .nodeValue
                )
                if child.getElementsByTagName("clean_command_line"):
                    self.clean_command_line = (
                        child.getElementsByTagName("clean_command_line")[0]
                        .childNodes[0]
                        .nodeValue
                    )
            elif tag == "variables":
                for var in child.childNodes:
                    try:
                        self.variables.append(
                            Variable(
                                name=var.getAttribute("name"),
                                language=var.getAttribute("language"),
                                code=var.childNodes[0].nodeValue.strip(),
                                varlist=var.getAttribute("variable_list"),
                            )
                        )
                    except AttributeError:
                        continue
            elif tag == "pass_tests":
                for test in child.childNodes:
                    try:
                        self.pass_tests.append(
                            Test(
                                name=test.getAttribute("name"),
                                language=test.getAttribute("language"),
                                code=test.childNodes[0].nodeValue.strip(),
                            )
                        )
                    except AttributeError:
                        continue
            elif tag == "warn_tests":
                for test in child.childNodes:
                    try:
                        self.warn_tests.append(
                            Test(
                                name=test.getAttribute("name"),
                                language=test.getAttribute("language"),
                                code=test.childNodes[0].nodeValue.strip(),
                            )
                        )
                    except AttributeError:
                        continue

        self.random_string()

    def log(self, string):
        if self.verbose:
            print(self.filename[:-4] + ": " + string)

    def random_string(self):
        letters = "abcdefghijklmnopqrstuvwxyz"
        letters += letters.upper() + "0123456789"

        string = self.filename[:-4]
        for i in range(10):
            string += random.choice(letters)

        self.random = string

    def is_finished(self):
        # TODO is using scheduler this function should check job is finished
        return True

    def clean(self):
        self.log("Cleaning")

        if self.clean_command_line == "":
            # Get a list of all the file paths that ends with .txt from in specified directory
            filelist = glob.glob("*.log")

            # Iterate over the list of filepaths & remove each file.
            for filePath in filelist:
                try:
                    os.remove(filePath)
                except OSError:
                    print("Error while deleting file : ", filePath)
        else:
            self.log(self.clean_command_line)
            os.system(self.clean_command_line)

    def run(self, directory):
        self.log("Running")

        start_time = time.process_time()
        wall_time = time.time()

        self.log(self.command_line)
        os.system("cd " + directory + "; " + self.command_line)
        run_time = time.process_time()  - start_time

        self.xml_reports.append(
            TestCase(
                self.name,
                "%s.%s" % (self.length, self.filename[:-4]),
                elapsed_sec=time.time() - wall_time,
            )
        )

        return run_time

    def cml_logs(self, nloglines=None):
        logs = glob.glob("*.log")

        if nloglines is None or nloglines > 0:
            for filename in logs:
                log = open(filename, "r").read().split("\n")
                if nloglines is not None:
                    log = log[-nloglines:]
                self.log("Log: " + filename)
                for line in log:
                    self.log(line)

        return

    def test(self):
        def trim(string):
            if len(string) > 4096:
                return string[:4096] + " ..."
            else:
                return string

        varsdict = {}
        self.log("Assigning variables:")
        for var in self.variables:
            tmpdict = {"ref_path": self.ref_path, "project_dir": self.project_dir}

            try:
                var.run(tmpdict)
            except:
                self.log("failure.")
                self.pass_status.append("F")
                tc = TestCase(self.name, "%s.%s" % (self.length, self.filename[:-4]))
                tc.add_failure_info("Failure")
                self.xml_reports.append(tc)
                return self.pass_status

            for variablename in var.varlist:
                varsdict[variablename] = tmpdict[variablename]
                self.log(
                    "Assigning %s = %s"
                    % (str(variablename), trim(str(varsdict[variablename])))
                )

        if len(self.pass_tests) != 0:
            self.log("Running failure tests: ")
            for test in self.pass_tests:
                self.log("Running %s:" % test.name)
                log = StringIO()
                original_stdout = sys.stdout
                sys.stdout = log
                status = test.run(varsdict)
                tc = TestCase(test.name, "%s.%s" % (self.length, self.filename[:-4]))
                if status:
                    self.log("success.")
                    self.pass_status.append("P")
                elif not status:
                    self.log("failure.")
                    self.pass_status.append("F")
                    tc.add_failure_info("Failure")
                else:
                    self.log("failure (info == %s)." % status)
                    self.pass_status.append("F")
                    tc.add_failure_info("Failure", status)
                self.xml_reports.append(tc)
                sys.stdout = original_stdout
                log.seek(0)
                tc.stdout = log.read()
                print(tc.stdout)

        if len(self.warn_tests) != 0:
            self.log("Running warning tests: ")
            for test in self.warn_tests:
                self.log("Running %s:" % test.name)
                log = StringIO()
                original_stdout = sys.stdout
                sys.stdout = log
                status = test.run(varsdict)
                tc = TestCase(test.name, "%s.%s" % (self.length, self.filename[:-4]))
                if status:
                    self.log("success.")
                    self.warn_status.append("P")
                elif not status:
                    self.log("warning.")
                    self.warn_status.append("W")
                    tc.add_failure_info("Warning")
                else:
                    self.log("warning (info == %s)." % status)
                    self.warn_status.append("W")
                    tc.add_failure_info("Warning", status)
                self.xml_reports.append(tc)
                sys.stdout = original_stdout
                log.seek(0)
                tc.stdout = log.read()
                print(tc.stdout)
        self.log("".join(self.pass_status + self.warn_status))
        return self.pass_status + self.warn_status


class TestOrVariable:
    """Tests and variables have a lot in common. This code unifies the commonalities."""

    def __init__(self, name, language, code):
        self.name = name
        self.language = language
        self.code = code

    def run(self, varsdict):
        func = getattr(self, "run_" + self.language)
        return func(varsdict)


class Test(TestOrVariable):
    """A test for the model output"""

    def run_bash(self, varsdict):
        varstr = ""
        for var in varsdict.keys():
            varstr = varstr + ('export %s="%s"; ' % (var, varsdict[var]))

        retcode = os.system(varstr + self.code)
        if retcode == 0:
            return True
        else:
            return False

    def run_python(self, varsdict):
        tmpdict = copy.copy(varsdict)
        try:
            exec(self.code, tmpdict)
            return True
        except AssertionError:
            # in case of an AssertionError, we assume the test has just failed
            return False
        except:
            # tell us what else went wrong:
            traceback.print_exc()
            return False


class Variable(TestOrVariable):
    """A variable definition for use in tests"""

    def __init__(self, name, language, code, varlist):
        self.varlist = varlist.split(",")
        super().__init__(name, language, code)

    def run_bash(self, varsdict):
        cmd = 'bash -c "%s"' % self.code
        fd = os.popen(cmd, "r")
        exec(self.name + "=" + fd.read(), varsdict)
        if self.name not in varsdict.keys():
            raise Exception

    def run_python(self, varsdict):
        try:
            print(self.code)
            exec(self.code, varsdict)
        except:
            print("Variable computation raised an exception")
            print("-" * 80)
            for (lineno, line) in enumerate(self.code.split("\n")):
                print("%3d  %s" % (lineno + 1, line))
            print("-" * 80)
            traceback.print_exc()
            print("-" * 80)
            raise Exception

        for varname in self.varlist:
            if varname not in varsdict:
                print("varname == ", varname)
                print("varsdict.keys() == ", varsdict.keys())
                print(varname, "varname not found: did you set all variables?")
                raise Exception


class ThreadIterator(list):
    """A thread-safe iterator over a list."""

    def __init__(self, seq):
        super().__init__()
        self.list = list(seq)

        self.lock = threading.Lock()

    def __iter__(self):
        return self

    def __next__(self):
        return self.next()

    def next(self):
        if len(self.list) == 0:
            raise StopIteration

        self.lock.acquire()
        ans = self.list.pop()
        self.lock.release()

        return ans
