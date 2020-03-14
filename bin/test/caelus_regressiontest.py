#!/usr/bin/env python
import sys
import os
import copy
import random
import xml.dom.minidom
import time
import glob
import threading
import traceback

from io import StringIO
from junit_xml import TestCase # TODO need to extend TestCase to record warnings correctly.


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
