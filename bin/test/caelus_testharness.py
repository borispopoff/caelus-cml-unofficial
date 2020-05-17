#!/usr/bin/env python
# This code was based off code provided with the fluidity project.
import sys
import os
import os.path
import glob
import time
import traceback
import multiprocessing as mp
import queue as Queue
import xml.parsers.expat
import xml.etree.ElementTree as eTree
from io import StringIO

# Make sure we use regressiontest provided with caelus
sys.path.insert(0, os.getcwd())
import caelus_regressiontest as regressiontest

# Need scons. Use system or local to caelus
try:
    import SCons.Script
except ImportError:
    # try again by using scons provided with caelus
    head, tail = os.path.split(sys.argv[0])
    scons_path = os.path.abspath(
        os.path.join(head, "../../external/scons-local-3.0.3/scons-local-3.0.3")
    )
    sys.path.append(scons_path)
    import SCons.Script

# imports from Caelus site-scons
head, tail = os.path.split(sys.argv[0])
site_scons_path = os.path.abspath(os.path.join(head, "../..", "site_scons"))
sys.path.insert(0, site_scons_path)

from environment import caelus_env

# imports from CPL
from caelus.config.cmlenv import cml_get_version


class TestHarness:
    def __init__(
        self,
        length="any",
        parallel="any",
        exclude_tags=None,
        tags=None,
        file="",
        ref_path=None,
        from_file=None,
        verbose=True,
        justtest=False,
        exit_fails=False,
        xml_outfile="",
    ):
        self.tests = []
        self.verbose = verbose
        self.length = length
        self.parallel = parallel
        self.passcount = 0
        self.failcount = 0
        self.warncount = 0
        self.teststatus = []
        self.completed_tests = []
        self.justtest = justtest
        self.xml_parser = regressiontest.TestSuite("Caelus-TestHarness", [])
        self.cwd = os.getcwd()
        self.iolock = mp.Lock()
        self.xml_outfile = xml_outfile
        self.exit_fails = exit_fails

        if file == "":
            print("Test criteria:")
            print("-" * 80)
            print("length: ", length)
            print("parallel: ", parallel)
            print("tags to include: ", tags)
            print("tags to exclude: ", exclude_tags)
            print("-" * 80)
            print()

        # Base SCons environment
        env = caelus_env()

        # step 1. form a list of all the xml files to be considered.
        xml_files = []
        projectdir = env["PROJECT_DIR"]
        dirnames = []
        testpaths = ["tests", "tutorials"]  # Releative to Caelus PROJECT_DIR
        for directory in testpaths:
            if os.path.exists(os.path.join(projectdir, directory)):
                dirnames.append(directory)
        testdirs = [os.path.join(projectdir, x) for x in dirnames]

        for rootDir in testdirs:
            for dirName, subdirList, fileList in os.walk(rootDir):
                g = glob.glob1(dirName, "*.xml")
                for xml_file in g:
                    try:
                        p = eTree.parse(os.path.join(dirName, xml_file))
                        x = p.getroot()
                        if x.tag == "testproblem":
                            xml_files.append(os.path.join(dirName, xml_file))
                    except xml.parsers.expat.ExpatError:
                        print(("Warning: %s mal-formed" % xml_file))
                        traceback.print_exc()

        # step 2. if the user has specified a particular file, let's use that.
        if file != "":
            files = [file]
        elif from_file:
            try:
                f = open(from_file, "r")
                files = [line[:-1] for line in f.readlines()]
            except IOError as e:
                sys.stderr.write(
                    "Unable to read tests from file %s: %s" % (from_file, e)
                )
                sys.exit(1)
            f.close()
        else:
            files = None

        if files:
            for (subdir, xml_file) in [os.path.split(x) for x in xml_files]:
                temp_files = files
                for file in temp_files:
                    if xml_file == file:
                        p = eTree.parse(os.path.join(subdir, xml_file))
                        prob_defn = p.findall("problem_definition")[0]
                        prob_nprocs = int(prob_defn.attrib["nprocs"])
                        testprob = regressiontest.TestProblem(
                            os.path.join(subdir, xml_file),
                            projectdir,
                            ref_path,
                            prob_nprocs,
                            verbose=self.verbose,
                        )
                        self.tests.append((subdir, testprob))
                        files.remove(xml_file)
            if files != []:
                print("Could not find the following specified test files:")
                for f in files:
                    print(f)
                sys.exit(1)
            return

        # step 3. form a cut-down list of the xml files matching the correct length and the correct parallelism.
        working_set = []
        for xml_file in xml_files:
            p = eTree.parse(xml_file)
            prob_defn = p.findall("problem_definition")[0]
            prob_length = prob_defn.attrib["length"]
            prob_nprocs = int(prob_defn.attrib["nprocs"])
            if prob_length == length or (
                length == "any" and prob_length not in ["special"]
            ):
                if self.parallel == "parallel":
                    if prob_nprocs > 1:
                        working_set.append(xml_file)
                elif self.parallel == "serial":
                    if prob_nprocs == 1:
                        working_set.append(xml_file)
                elif self.parallel == "any":
                    working_set.append(xml_file)

        def get_xml_file_tags(xml_file):
            p_tags = eTree.parse(xml_file).findall("tags")
            if len(p_tags) > 0 and not p_tags[0].text is None:
                xml_tags = p_tags[0].text.split()
            else:
                xml_tags = []

            return xml_tags

        # step 4. if there are any excluded tags, let's exclude tests that have
        # them
        if exclude_tags is not None:
            to_remove = []
            for xml_file in working_set:
                prob_tags = get_xml_file_tags(xml_file)
                include = True
                for tag in exclude_tags:
                    if tag in prob_tags:
                        include = False
                        break
                if not include:
                    to_remove.append(xml_file)
            for xml_file in to_remove:
                working_set.remove(xml_file)

        # step 5. if there are any tags, let's use them
        if tags is not None:
            tagged_set = []
            for xml_file in working_set:
                prob_tags = get_xml_file_tags(xml_file)

                include = True
                for tag in tags:
                    if tag not in prob_tags:
                        include = False

                if include is True:
                    tagged_set.append(xml_file)
        else:
            tagged_set = working_set

        for (subdir, xml_file) in [os.path.split(x) for x in tagged_set]:
            p = eTree.parse(os.path.join(subdir, xml_file))
            prob_defn = p.findall("problem_definition")[0]
            prob_nprocs = int(prob_defn.attrib["nprocs"])
            testprob = regressiontest.TestProblem(
                os.path.join(subdir, xml_file),
                projectdir,
                ref_path,
                prob_nprocs,
                verbose=self.verbose,
            )
            self.tests.append((subdir, testprob))

        if len(self.tests) == 0:
            print("Warning: no matching tests.")

    def length_matches(self, filelength):
        if self.length == filelength:
            return True
        if self.length == "medium" and filelength == "short":
            return True
        return False

    def log(self, string):
        if self.verbose:
            print(string)

    def clean(self):
        self.log(" ")
        for t in self.tests:
            os.chdir(t[0])
            t[1].clean()

        return

    def run(self):
        self.log(" ")
        if not self.justtest:
            tests_by_nprocs = {}
            for test_id in range(len(self.tests)):
                # sort tests by number of processes requested
                tests_by_nprocs.setdefault(self.tests[test_id][1].nprocs, []).append(
                    test_id
                )

            m = mp.Manager()
            test_exception_ids = mp.Queue()
            items = mp.Queue()
            rets = mp.Queue()
            spawn = mp.Process
            workerlist = []

            available_procs = mp.cpu_count()
            sim_max_procs = sorted(list(tests_by_nprocs.keys()), reverse=True)[0]
            requested_procs = options.processor_count
            print("Maximum processors required by any case:", sim_max_procs)
            possible_procs = available_procs // sim_max_procs 
            nprocs = requested_procs
            if requested_procs > possible_procs:
                nprocs = possible_procs
                print("Requested processor count exceeds available when parallel simulations are running, reducing to:", nprocs)

            # spin up enough new workers to fully subscribe processor count
            for i in range(nprocs):
                worker = spawn(target=self.processor_run, args=(items, test_exception_ids, rets))
                worker.start()
                workerlist.append(worker)

            # fill the queue
            for test_id in range(len(self.tests)):
                items.put(test_id)

            # wait until all workers finish
            for worker in workerlist:
                worker.join()

            # get and return exceptions
            exceptions = []
            while not test_exception_ids.empty():
                test_id, lines = test_exception_ids.get(timeout=0.1)
                exceptions.append((self.tests[test_id], lines))

            for e, lines in exceptions:
                tc = regressiontest.TestCase(e[1].name, "%s.%s" % (e[1].length, e[1].filename[:-4]))
                tc.add_failure_info("Failure", lines)
                self.xml_parser.test_cases += [tc]
                self.tests.remove(e)
                self.completed_tests += [e[1]]

            # get and return results as a list
            ret = []
            while not rets.empty():
                test_id, testprob = rets.get(timeout=0.1)
                ret.append((test_id, testprob))

            for r in ret:
                test_id = r[0]
                testprob = r[1]
                self.tests[test_id][1].xml_reports.append(testprob.xml_reports[0])
            
            # run tests
            count = len(self.tests)
            while True:
                for t in self.tests:
                    if t is None:
                        continue
                    test = t[1]
                    os.chdir(t[0])
                    if test.is_finished():
                        if test.length == "long":
                            test.cml_logs(nloglines=20)
                        else:
                            test.cml_logs(nloglines=0)
                        try:
                            self.teststatus += test.test()
                        except:
                            self.log(
                                "Error: %s raised an exception while testing:"
                                % test.filename
                            )
                            lines = traceback.format_exception(
                                sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]
                            )
                            for line in lines:
                                self.log(line)
                            self.teststatus += ["F"]
                            test.pass_status = ["F"]
                        self.completed_tests += [test]
                        self.xml_parser.test_cases += test.xml_reports
                        t = None
                        count -= 1

                if count == 0:
                    break
                time.sleep(60)
        else:
            for t in self.tests:
                test = t[1]
                os.chdir(t[0])
                if self.length == "long":
                    test.cml_logs(nloglines=20)
                else:
                    test.cml_logs(nloglines=0)
                self.teststatus += test.test()
                self.completed_tests += [test]

                self.xml_parser.test_cases += test.xml_reports

        self.passcount = self.teststatus.count("P")
        self.failcount = self.teststatus.count("F")
        self.warncount = self.teststatus.count("W")

        if self.failcount + self.warncount > 0:
            print()
            print("Summary of test problems with failures or warnings:")
            for t in self.completed_tests:
                if t.pass_status.count("F") + t.warn_status.count("W") > 0:
                    print(t.filename + ":", "".join(t.pass_status + t.warn_status))
            print()

        if self.passcount + self.failcount + self.warncount > 0:
            print("Passes:   %d" % self.passcount)
            print("Failures: %d" % self.failcount)
            print("Warnings: %d" % self.warncount)

        if self.xml_outfile != "":
            fd = open(self.cwd + "/" + self.xml_outfile, "w")
            self.xml_parser.to_file(fd, [self.xml_parser])
            fd.close()

        if self.exit_fails:
            sys.exit(self.failcount)

    def processor_run(self, items, test_exception_ids, rets):
        """This is the portion of the loop which actually runs the
        tests. Each processor runs tests from the queue until it is exhausted."""

        # We use IO locking to attempt to keep output understandable
        # That means writing to a buffer to minimise interactions
        main_stdout = sys.stdout

        while True:
            buf = StringIO()
            sys.stdout = buf
            try:
                # pull a test number from the queue
                test_id = items.get(timeout=0.1)
                (directory, testprob) = self.tests[test_id]
            except Queue.Empty:
                # If the queue is empty, we're done.
                sys.stdout = main_stdout
                buf.seek(0)
                with self.iolock:
                    print(buf.read())
                break

            try:
                runtime = testprob.run(directory)
                rets.put((test_id, testprob))
                if self.length == "short" and runtime > 30.0:
                    self.log(
                        "Warning: short test ran for %f seconds which"
                        + " is longer than the permitted 30s run time" % runtime
                    )
                    self.teststatus += ["W"]
                    testprob.pass_status = ["W"]

            except:
                self.log("Error: %s raised an exception while running:" % testprob.filename)
                lines = traceback.format_exception(
                    sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]
                )
                for line in lines:
                    self.log(line)
                testprob.pass_status = ["F"]
                test_exception_ids.put((test_id, lines))
            finally:
                sys.stdout = main_stdout
                buf.seek(0)
                with self.iolock:
                    print(buf.read())

    def list(self):
        print(len(self.tests), "tests found")
        print("List of tests:")
        for (subdir, test) in self.tests:
            print(os.path.join(subdir, test.filename))


if __name__ == "__main__":
    import optparse

    def tag_callback(option, opt, value, parser):
      setattr(parser.values, option.dest, value.split(','))

    parser = optparse.OptionParser()
    parser.add_option(
        "-l",
        "--length",
        dest="length",
        help="length of problem (default=any)",
        default="any",
    )
    parser.add_option(
        "-p",
        "--parallelism",
        dest="parallel",
        help="parallelism of problem: options are serial, parallel or any (default=any)",
        default="any",
    )
    parser.add_option(
        "-e",
        "--exclude-tags",
        type="string",
        dest="exclude_tags",
        help="run only tests that do not have specific tags (takes precidence over -t)",
        default=[],
        action="callback",
        callback=tag_callback,
    )
    parser.add_option(
        "-t",
        "--tags",
        type="string",
        dest="tags",
        help="run tests with specific tags",
        default=[],
        action="callback",
        callback=tag_callback,
    )
    parser.add_option(
        "-f",
        "--file",
        dest="file",
        help="specific test case to run (by filename)",
        default="",
    )
    parser.add_option(
        "--from-file",
        dest="from_file",
        default=None,
        help="run tests listed in FROM_FILE (one test per line)",
    )
    parser.add_option(
        "-n",
        "--nprocs",
        dest="processor_count",
        type="int",
        help="number of tests to run at the same time",
        default=1,
    )
    parser.add_option("-c", "--clean", action="store_true", dest="clean", default=False)
    parser.add_option(
        "-r",
        "--reference-path",
        dest="ref_path",
        default=None,
        help="path to reference case data",
    )
    parser.add_option(
        "--just-test", action="store_true", dest="justtest", default=False
    )
    parser.add_option("--just-list", action="store_true", dest="justlist")
    parser.add_option(
        "-x",
        "--xml-output",
        dest="xml_outfile",
        default="",
        help="filename for xml output",
    )
    parser.add_option(
        "--exit-failure-count",
        action="store_true",
        dest="exit_fails",
        help="Return failure count on exit",
    )
    (options, args) = parser.parse_args()

    if len(args) > 0:
        parser.error("Too many arguments.")

    if options.parallel not in ["serial", "parallel", "any"]:
        parser.error("Specify parallelism as either serial, parallel or any.")

    try:
        os.mkdir(os.environ["HOME"] + os.sep + "lock")
    except OSError:
        pass

    if len(options.exclude_tags) == 0:
        exclude_tags = None
    else:
        exclude_tags = options.exclude_tags

    if len(options.tags) == 0:
        tags = None
    else:
        tags = options.tags

    testharness = TestHarness(
        length=options.length,
        parallel=options.parallel,
        exclude_tags=exclude_tags,
        tags=tags,
        file=options.file,
        verbose=True,
        justtest=options.justtest,
        from_file=options.from_file,
        ref_path=options.ref_path,
        exit_fails=options.exit_fails,
        xml_outfile=options.xml_outfile,
    )

    if options.justlist:
        testharness.list()
    elif options.clean:
        testharness.clean()
    else:
        print("-" * 80)
        print("Searching for default caelus version...")
        cenv_default = cml_get_version()

        cenv = cenv_default
        print("Using Caelus version: " + cenv.version)
        print("Caelus path: " + cenv.project_dir)
        print("-" * 80)

        testharness.run()
