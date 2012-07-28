# Copyright 2002-2005 Vladimir Prus.
# Copyright 2002-2003 Dave Abrahams.
# Copyright 2006 Rene Rivera.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import TestCmd

import copy
import fnmatch
import glob
import math
import os
import os.path
import re
import shutil
import string
import StringIO
import sys
import tempfile
import time
import traceback
import tree
import types

from xml.sax.saxutils import escape


annotations = []


def print_annotation(name, value, xml):
    """Writes some named bits of information about the current test run."""
    if xml:
        print escape(name) + " {{{"
        print escape(value)
        print "}}}"
    else:
        print name + " {{{"
        print value
        print "}}}"


def flush_annotations(xml=0):
    global annotations
    for ann in annotations:
        print_annotation(ann[0], ann[1], xml)
    annotations = []


def clear_annotations():
    global annotations
    annotations = []


defer_annotations = 0

def set_defer_annotations(n):
    global defer_annotations
    defer_annotations = n


def annotate_stack_trace(tb=None):
    if tb:
        trace = TestCmd.caller(traceback.extract_tb(tb), 0)
    else:
        trace = TestCmd.caller(traceback.extract_stack(), 1)
    annotation("stacktrace", trace)


def annotation(name, value):
    """Records an annotation about the test run."""
    annotations.append((name, value))
    if not defer_annotations:
        flush_annotations()


def get_toolset():
    toolset = None
    for arg in sys.argv[1:]:
        if not arg.startswith("-"):
            toolset = arg
    return toolset or "gcc"


# Detect the host OS.
cygwin = hasattr(os, "uname") and os.uname()[0].lower().startswith("cygwin")
windows = cygwin or os.environ.get("OS", "").lower().startswith("windows")


def prepare_prefixes_and_suffixes(toolset):
    prepare_suffix_map(toolset)
    prepare_library_prefix(toolset)


def prepare_suffix_map(toolset):
    """
      Set up suffix translation performed by the Boost Build testing framework
    to accomodate different toolsets generating targets of the same type using
    different filename extensions (suffixes).

    """
    global suffixes
    suffixes = {}
    if windows:
        if toolset == "gcc":
            suffixes[".lib"] = ".a"  # mingw static libs use suffix ".a".
            suffixes[".obj"] = ".o"
        if cygwin:
            suffixes[".implib"] = ".lib.a"
        else:
            suffixes[".implib"] = ".lib"
    else:
        suffixes[".exe"] = ""
        suffixes[".dll"] = ".so"
        suffixes[".lib"] = ".a"
        suffixes[".obj"] = ".o"
        suffixes[".implib"] = ".no_implib_files_on_this_platform"

        if hasattr(os, "uname") and os.uname()[0] == "Darwin":
            suffixes[".dll"] = ".dylib"


def prepare_library_prefix(toolset):
    """
      Setup whether Boost Build is expected to automatically prepend prefixes
    to its built library targets.

    """
    global lib_prefix
    lib_prefix = "lib"

    global dll_prefix
    if cygwin:
        dll_prefix = "cyg"
    elif windows and toolset != "gcc":
        dll_prefix = None
    else:
        dll_prefix = "lib"


def re_remove(sequence, regex):
    me = re.compile(regex)
    result = filter(lambda x: me.match(x), sequence)
    if not result:
        raise ValueError()
    for r in result:
        sequence.remove(r)


def glob_remove(sequence, pattern):
    result = fnmatch.filter(sequence, pattern)
    if not result:
        raise ValueError()
    for r in result:
        sequence.remove(r)


class Tester(TestCmd.TestCmd):
    """Main tester class for Boost Build.

    Optional arguments:

    `arguments`                   - Arguments passed to the run executable.
    `executable`                  - Name of the executable to invoke.
    `match`                       - Function to use for compating actual and
                                    expected file contents.
    `boost_build_path`            - Boost build path to be passed to the run
                                    executable.
    `translate_suffixes`          - Whether to update suffixes on the the file
                                    names passed from the test script so they
                                    match those actually created by the current
                                    toolset. For example, static library files
                                    are specified by using the .lib suffix but
                                    when the "gcc" toolset is used it actually
                                    creates them using the .a suffix.
    `pass_toolset`                - Whether the test system should pass the
                                    specified toolset to the run executable.
    `use_test_config`             - Whether the test system should tell the run
                                    executable to read in the test_config.jam
                                    configuration file.
    `ignore_toolset_requirements` - Whether the test system should tell the run
                                    executable to ignore toolset requirements.
    `workdir`                     - Absolute directory where the test will be
                                    run from.
    `pass_d0`                     - If set, when tests are not explicitly run
                                    in verbose mode, they are run as silent
                                    (-d0 & --quiet Boost Jam options).

    Optional arguments inherited from the base class:

    `description`                 - Test description string displayed in case
                                    of a failed test.
    `subdir`                      - List of subdirectories to automatically
                                    create under the working directory. Each
                                    subdirectory needs to be specified
                                    separately, parent coming before its child.
    `verbose`                     - Flag that may be used to enable more
                                    verbose test system output. Note that it
                                    does not also enable more verbose build
                                    system output like the --verbose command
                                    line option does.
    """
    def __init__(self, arguments=None, executable="bjam",
        match=TestCmd.match_exact, boost_build_path=None,
        translate_suffixes=True, pass_toolset=True, use_test_config=True,
        ignore_toolset_requirements=True, workdir="", pass_d0=True, **keywords):

        assert arguments.__class__ is not str
        self.original_workdir = os.getcwd()
        if workdir and not os.path.isabs(workdir):
            raise ("Parameter workdir <%s> must point to an absolute "
                "directory: " % workdir)

        self.last_build_timestamp = 0
        self.translate_suffixes = translate_suffixes
        self.use_test_config = use_test_config

        self.toolset = get_toolset()
        self.pass_toolset = pass_toolset
        self.ignore_toolset_requirements = ignore_toolset_requirements

        prepare_prefixes_and_suffixes(pass_toolset and self.toolset or "gcc")

        use_default_bjam = "--default-bjam" in sys.argv

        if not use_default_bjam:
            jam_build_dir = ""
            if os.name == "nt":
                jam_build_dir = "bin.ntx86"
            elif (os.name == "posix") and os.__dict__.has_key("uname"):
                if os.uname()[0].lower().startswith("cygwin"):
                    jam_build_dir = "bin.cygwinx86"
                    if ("TMP" in os.environ and
                        os.environ["TMP"].find("~") != -1):
                        print("Setting $TMP to /tmp to get around problem "
                            "with short path names")
                        os.environ["TMP"] = "/tmp"
                elif os.uname()[0] == "Linux":
                    cpu = os.uname()[4]
                    if re.match("i.86", cpu):
                        jam_build_dir = "bin.linuxx86"
                    else:
                        jam_build_dir = "bin.linux" + os.uname()[4]
                elif os.uname()[0] == "SunOS":
                    jam_build_dir = "bin.solaris"
                elif os.uname()[0] == "Darwin":
                    if os.uname()[4] == "i386":
                        jam_build_dir = "bin.macosxx86"
                    else:
                        jam_build_dir = "bin.macosxppc"
                elif os.uname()[0] == "AIX":
                    jam_build_dir = "bin.aix"
                elif os.uname()[0] == "IRIX64":
                    jam_build_dir = "bin.irix"
                elif os.uname()[0] == "FreeBSD":
                    jam_build_dir = "bin.freebsd"
                elif os.uname()[0] == "OSF1":
                    jam_build_dir = "bin.osf"
                else:
                    raise ("Do not know directory where Jam is built for this "
                        "system: %s/%s" % (os.name, os.uname()[0]))
            else:
                raise ("Do not know directory where Jam is built for this "
                    "system: %s" % os.name)

            # Find where jam_src is located. Try for the debug version if it is
            # lying around.
            dirs = [os.path.join("../engine", jam_build_dir + ".debug"),
                    os.path.join("../engine", jam_build_dir)]
            for d in dirs:
                if os.path.exists(d):
                    jam_build_dir = d
                    break
            else:
                print("Cannot find built Boost.Jam")
                sys.exit(1)

        verbosity = ["-d0", "--quiet"]
        if not pass_d0:
            verbosity = []
        if "--verbose" in sys.argv:
            keywords["verbose"] = True
            verbosity = ["-d+2"]

        if boost_build_path is None:
            boost_build_path = self.original_workdir + "/.."

        program_list = []
        if use_default_bjam:
            program_list.append(executable)
        else:
            program_list.append(os.path.join(jam_build_dir, executable))
        program_list.append('-sBOOST_BUILD_PATH="' + boost_build_path + '"')
        if verbosity:
            program_list += verbosity
        if arguments:
            program_list += arguments

        TestCmd.TestCmd.__init__(self, program=program_list, match=match,
            workdir=workdir, inpath=use_default_bjam, **keywords)

        os.chdir(self.workdir)

    def cleanup(self):
        try:
            TestCmd.TestCmd.cleanup(self)
            os.chdir(self.original_workdir)
        except AttributeError:
            # When this is called during TestCmd.TestCmd.__del__ we can have
            # both 'TestCmd' and 'os' unavailable in our scope. Do nothing in
            # this case.
            pass

    #
    # Methods that change the working directory's content.
    #
    def set_tree(self, tree_location):
        # It is not possible to remove the current directory.
        d = os.getcwd()
        os.chdir(os.path.dirname(self.workdir))
        shutil.rmtree(self.workdir, ignore_errors=False)

        if not os.path.isabs(tree_location):
            tree_location = os.path.join(self.original_workdir, tree_location)
        shutil.copytree(tree_location, self.workdir)

        os.chdir(d)
        def make_writable(unused, dir, entries):
            for e in entries:
                name = os.path.join(dir, e)
                os.chmod(name, os.stat(name).st_mode | 0222)
        os.path.walk(".", make_writable, None)

    def write(self, file, content, wait=True):
        if wait:
            self.wait_for_time_change_since_last_build()
        nfile = self.native_file_name(file)
        try:
            os.makedirs(os.path.dirname(nfile))
        except Exception, e:
            pass
        f = open(nfile, "wb")
        f.write(content)
        f.close()

    def copy(self, src, dst):
        try:
            self.write(dst, self.read(src, 1))
        except:
            self.fail_test(1)

    def copy_preserving_timestamp(self, src, dst):
        src_name = self.native_file_name(src)
        dst_name = self.native_file_name(dst)
        stats = os.stat(src_name)
        self.write(dst, self.read(src, 1))
        os.utime(dst_name, (stats.st_atime, stats.st_mtime))

    def touch(self, names, wait=True):
        if wait:
            self.wait_for_time_change_since_last_build()
        for name in self.adjust_names(names):
            os.utime(self.native_file_name(name), None)

    def rm(self, names):
        if not type(names) == types.ListType:
            names = [names]

        if names == ["."]:
            # If we are deleting the entire workspace, there is no need to wait
            # for a clock tick.
            self.last_build_timestamp = 0

        # Avoid attempts to remove the current directory.
        os.chdir(self.original_workdir)
        for name in names:
            n = glob.glob(self.native_file_name(name))
            if n: n = n[0]
            if not n:
                n = self.glob_file(string.replace(name, "$toolset",
                    self.toolset + "*"))
            if n:
                if os.path.isdir(n):
                    shutil.rmtree(n, ignore_errors=False)
                else:
                    os.unlink(n)

        # Create working dir root again in case we removed it.
        if not os.path.exists(self.workdir):
            os.mkdir(self.workdir)
        os.chdir(self.workdir)

    def expand_toolset(self, name):
        """
          Expands $toolset placeholder in the given file to the name of the
        toolset currently being tested.

        """
        self.write(name, string.replace(self.read(name), "$toolset",
            self.toolset))

    def dump_stdio(self):
        annotation("STDOUT", self.stdout())
        annotation("STDERR", self.stderr())

    def run_build_system(self, extra_args=None, subdir="", stdout=None,
        stderr="", status=0, match=None, pass_toolset=None,
        use_test_config=None, ignore_toolset_requirements=None,
        expected_duration=None, **kw):

        assert extra_args.__class__ is not str

        if os.path.isabs(subdir):
            print("You must pass a relative directory to subdir <%s>." % subdir
                )
            return

        self.previous_tree, dummy = tree.build_tree(self.workdir)

        if match is None:
            match = self.match

        if pass_toolset is None:
            pass_toolset = self.pass_toolset

        if use_test_config is None:
            use_test_config = self.use_test_config

        if ignore_toolset_requirements is None:
            ignore_toolset_requirements = self.ignore_toolset_requirements

        try:
            kw["program"] = []
            kw["program"] += self.program
            if extra_args:
                kw["program"] += extra_args
            if pass_toolset:
                kw["program"].append("toolset=" + self.toolset)
            if use_test_config:
                kw["program"].append('--test-config="%s"' % os.path.join(
                    self.original_workdir, "test-config.jam"))
            if ignore_toolset_requirements:
                kw["program"].append("--ignore-toolset-requirements")
            if "--python" in sys.argv:
                kw["program"].append("--python")
            kw["chdir"] = subdir
            self.last_program_invocation = kw["program"]
            build_time_start = time.time()
            apply(TestCmd.TestCmd.run, [self], kw)
            build_time_finish = time.time()
        except:
            self.dump_stdio()
            raise

        old_last_build_timestamp = self.last_build_timestamp
        self.tree, self.last_build_timestamp = tree.build_tree(self.workdir)
        self.difference = tree.tree_difference(self.previous_tree, self.tree)
        if self.difference.empty():
            # If nothing has been changed by this build and sufficient time has
            # passed since the last build that actually changed something,
            # there is no need to wait for touched or newly created files to
            # start getting newer timestamps than the currently existing ones.
            self.last_build_timestamp = old_last_build_timestamp

        self.difference.ignore_directories()
        self.unexpected_difference = copy.deepcopy(self.difference)

        if (status and self.status) is not None and self.status != status:
            expect = ""
            if status != 0:
                expect = " (expected %d)" % status

            annotation("failure", '"%s" returned %d%s' % (kw["program"],
                self.status, expect))

            annotation("reason", "unexpected status returned by bjam")
            self.fail_test(1)

        if stdout is not None and not match(self.stdout(), stdout):
            annotation("failure", "Unexpected stdout")
            annotation("Expected STDOUT", stdout)
            annotation("Actual STDOUT", self.stdout())
            stderr = self.stderr()
            if stderr:
                annotation("STDERR", stderr)
            self.maybe_do_diff(self.stdout(), stdout)
            self.fail_test(1, dump_stdio=False)

        # Intel tends to produce some messages to stderr which make tests fail.
        intel_workaround = re.compile("^xi(link|lib): executing.*\n", re.M)
        actual_stderr = re.sub(intel_workaround, "", self.stderr())

        if stderr is not None and not match(actual_stderr, stderr):
            annotation("failure", "Unexpected stderr")
            annotation("Expected STDERR", stderr)
            annotation("Actual STDERR", self.stderr())
            annotation("STDOUT", self.stdout())
            self.maybe_do_diff(actual_stderr, stderr)
            self.fail_test(1, dump_stdio=False)

        if expected_duration is not None:
            actual_duration = build_time_finish - build_time_start
            if actual_duration > expected_duration:
                print("Test run lasted %f seconds while it was expected to "
                    "finish in under %f seconds." % (actual_duration,
                    expected_duration))
                self.fail_test(1, dump_stdio=False)

    def glob_file(self, name):
        result = None
        if hasattr(self, "difference"):
            for f in (self.difference.added_files +
                self.difference.modified_files +
                self.difference.touched_files):
                if fnmatch.fnmatch(f, name):
                    result = self.native_file_name(f)
                    break
        if not result:
            result = glob.glob(self.native_file_name(name))
            if result:
                result = result[0]
        return result

    def read(self, name, binary=False):
        try:
            if self.toolset:
                name = string.replace(name, "$toolset", self.toolset + "*")
            name = self.glob_file(name)
            openMode = "r"
            if binary:
                openMode += "b"
            else:
                openMode += "U"
            f = open(name, openMode)
            result = f.read()
            f.close()
            return result
        except:
            annotation("failure", "Could not open '%s'" % name)
            self.fail_test(1)
            return ""

    def read_and_strip(self, name):
        if not self.glob_file(name):
            return ""
        f = open(self.glob_file(name), "rb")
        lines = f.readlines()
        f.close()
        result = string.join(map(string.rstrip, lines), "\n")
        if lines and lines[-1][-1] != "\n":
            return result + "\n"
        return result

    def fail_test(self, condition, dump_difference=True, dump_stdio=True,
        dump_stack=True):
        if not condition:
            return

        if dump_difference and hasattr(self, "difference"):
            f = StringIO.StringIO()
            self.difference.pprint(f)
            annotation("changes caused by the last build command", f.getvalue())

        if dump_stdio:
            self.dump_stdio()

        if "--preserve" in sys.argv:
            print
            print "*** Copying the state of working dir into 'failed_test' ***"
            print
            path = os.path.join(self.original_workdir, "failed_test")
            if os.path.isdir(path):
                shutil.rmtree(path, ignore_errors=False)
            elif os.path.exists(path):
                raise "Path " + path + " already exists and is not a directory"
            shutil.copytree(self.workdir, path)
            print "The failed command was:"
            print " ".join(self.last_program_invocation)

        if dump_stack:
            annotate_stack_trace()
        sys.exit(1)

    # A number of methods below check expectations with actual difference
    # between directory trees before and after a build. All the 'expect*'
    # methods require exact names to be passed. All the 'ignore*' methods allow
    # wildcards.

    # All names can be either a string or a list of strings.
    def expect_addition(self, names):
        for name in self.adjust_names(names):
            try:
                glob_remove(self.unexpected_difference.added_files, name)
            except:
                annotation("failure", "File %s not added as expected" % name)
                self.fail_test(1)

    def ignore_addition(self, wildcard):
        self.ignore_elements(self.unexpected_difference.added_files, wildcard)

    def expect_removal(self, names):
        for name in self.adjust_names(names):
            try:
                glob_remove(self.unexpected_difference.removed_files, name)
            except:
                annotation("failure", "File %s not removed as expected" % name)
                self.fail_test(1)

    def ignore_removal(self, wildcard):
        self.ignore_elements(self.unexpected_difference.removed_files, wildcard)

    def expect_modification(self, names):
        for name in self.adjust_names(names):
            try:
                glob_remove(self.unexpected_difference.modified_files, name)
            except:
                annotation("failure", "File %s not modified as expected" % name)
                self.fail_test(1)

    def ignore_modification(self, wildcard):
        self.ignore_elements(self.unexpected_difference.modified_files, \
            wildcard)

    def expect_touch(self, names):
        d = self.unexpected_difference
        for name in self.adjust_names(names):
            # We need to check both touched and modified files. The reason is
            # that:
            #   (1) Windows binaries such as obj, exe or dll files have slight
            #       differences even with identical inputs due to Windows PE
            #       format headers containing an internal timestamp.
            #   (2) Intel's compiler for Linux has the same behaviour.
            filesets = [d.modified_files, d.touched_files]

            while filesets:
                try:
                    glob_remove(filesets[-1], name)
                    break
                except ValueError:
                    filesets.pop()

            if not filesets:
                annotation("failure", "File %s not touched as expected" % name)
                self.fail_test(1)

    def ignore_touch(self, wildcard):
        self.ignore_elements(self.unexpected_difference.touched_files, wildcard)

    def ignore(self, wildcard):
        self.ignore_elements(self.unexpected_difference.added_files, wildcard)
        self.ignore_elements(self.unexpected_difference.removed_files, wildcard)
        self.ignore_elements(self.unexpected_difference.modified_files, wildcard)
        self.ignore_elements(self.unexpected_difference.touched_files, wildcard)

    def expect_nothing(self, names):
        for name in self.adjust_names(names):
            if name in self.difference.added_files:
                annotation("failure",
                    "File %s added, but no action was expected" % name)
                self.fail_test(1)
            if name in self.difference.removed_files:
                annotation("failure",
                    "File %s removed, but no action was expected" % name)
                self.fail_test(1)
                pass
            if name in self.difference.modified_files:
                annotation("failure",
                    "File %s modified, but no action was expected" % name)
                self.fail_test(1)
            if name in self.difference.touched_files:
                annotation("failure",
                    "File %s touched, but no action was expected" % name)
                self.fail_test(1)

    def expect_nothing_more(self):
        # Not totally sure about this change, but I do not see a good
        # alternative.
        if windows:
            self.ignore("*.ilk")       # MSVC incremental linking files.
            self.ignore("*.pdb")       # MSVC program database files.
            self.ignore("*.rsp")       # Response files.
            self.ignore("*.tds")       # Borland debug symbols.
            self.ignore("*.manifest")  # MSVC DLL manifests.

        # Debug builds of bjam built with gcc produce this profiling data.
        self.ignore("gmon.out")
        self.ignore("*/gmon.out")

        # Boost Build's 'configure' functionality (unfinished at the time)
        # produces this file.
        self.ignore("bin/config.log")

        # Compiled Python files created when running Python based Boost Build.
        self.ignore("*.pyc")

        if not self.unexpected_difference.empty():
            annotation("failure", "Unexpected changes found")
            output = StringIO.StringIO()
            self.unexpected_difference.pprint(output)
            annotation("unexpected changes", output.getvalue())
            self.fail_test(1)

    def __expect_lines(self, data, lines, expected):
        # str.splitlines() trims at most one trailing newline while we want the
        # trailing newline to indicate that there should be an extra empty line
        # at the end.
        splitlines = lambda x : (x + "\n").splitlines()

        if data is None:
            data = []
        elif data.__class__ is str:
            data = splitlines(data)

        if lines.__class__ is str:
            lines = [splitlines(lines)]
        else:
            expanded = []
            for x in lines:
                if x.__class__ is str:
                    expanded.extend(splitlines(x))
                else:
                    expanded.append(x)
            lines = expanded

        if _contains_lines(data, lines) != bool(expected):
            output = []
            if expected:
                output = ["Did not find expected lines:"]
            else:
                output = ["Found unexpected lines:"]
            first = True
            for line_sequence in lines:
                if line_sequence:
                    if first:
                        first = False
                    else:
                        output.append("...")
                    output.extend("  > " + line for line in line_sequence)
            output.append("in output:")
            output.extend("  > " + line for line in data)
            annotation("failure", "\n".join(output))
            self.fail_test(1)

    def expect_output_lines(self, lines, expected=True):
        self.__expect_lines(self.stdout(), lines, expected)

    def expect_content_lines(self, filename, line, expected=True):
        self.__expect_lines(self.__read_file(filename), line, expected)

    def __read_file(self, name, exact=False):
        name = self.adjust_names(name)[0]
        result = ""
        try:
            if exact:
                result = self.read(name)
            else:
                result = string.replace(self.read_and_strip(name), "\\", "/")
        except (IOError, IndexError):
            print "Note: could not open file", name
            self.fail_test(1)
        return result

    def expect_content(self, name, content, exact=False):
        actual = self.__read_file(name, exact)
        content = string.replace(content, "$toolset", self.toolset + "*")

        matched = False
        if exact:
            matched = fnmatch.fnmatch(actual, content)
        else:
            def sorted_(x):
                x.sort()
                return x
            actual_ = map(lambda x: sorted_(x.split()), actual.splitlines())
            content_ = map(lambda x: sorted_(x.split()), content.splitlines())
            if len(actual_) == len(content_):
                matched = map(
                    lambda x, y: map(lambda n, p: fnmatch.fnmatch(n, p), x, y),
                    actual_, content_)
                matched = reduce(
                    lambda x, y: x and reduce(
                        lambda a, b: a and b,
                    y),
                    matched)

        if not matched:
            print "Expected:\n"
            print content
            print "Got:\n"
            print actual
            self.fail_test(1)

    def maybe_do_diff(self, actual, expected):
        if os.environ.get("DO_DIFF"):
            e = tempfile.mktemp("expected")
            a = tempfile.mktemp("actual")
            f = open(e, "w")
            f.write(expected)
            f.close()
            f = open(a, "w")
            f.write(actual)
            f.close()
            print("DIFFERENCE")
            # Current diff should return 1 to indicate 'different input files'
            # but some older diff versions may return 0 and depending on the
            # exact Python/OS platform version, os.system() call may gobble up
            # the external process's return code and return 0 itself.
            if os.system('diff -u "%s" "%s"' % (e, a)) not in [0, 1]:
                print('Unable to compute difference: diff -u "%s" "%s"' % (e, a
                    ))
            os.unlink(e)
            os.unlink(a)
        else:
            print("Set environmental variable 'DO_DIFF' to examine "
                "difference.")

    # Helpers.
    def mul(self, *arguments):
        if len(arguments) == 0:
            return None

        here = arguments[0]
        if type(here) == type(""):
            here = [here]

        if len(arguments) > 1:
            there = apply(self.mul, arguments[1:])
            result = []
            for i in here:
                for j in there:
                    result.append(i + j)
            return result

        return here

    # Internal methods.
    def ignore_elements(self, list, wildcard):
        """Removes in-place 'list' elements matching the given 'wildcard'."""
        list[:] = filter(lambda x, w=wildcard: not fnmatch.fnmatch(x, w), list)

    def adjust_lib_name(self, name):
        global lib_prefix
        global dll_prefix
        result = name

        pos = string.rfind(name, ".")
        if pos != -1:
            suffix = name[pos:]
            if suffix == ".lib":
                (head, tail) = os.path.split(name)
                if lib_prefix:
                    tail = lib_prefix + tail
                    result = os.path.join(head, tail)
            elif suffix == ".dll":
                (head, tail) = os.path.split(name)
                if dll_prefix:
                    tail = dll_prefix + tail
                    result = os.path.join(head, tail)
        # If we want to use this name in a Jamfile, we better convert \ to /,
        # as otherwise we would have to quote \.
        result = string.replace(result, "\\", "/")
        return result

    def adjust_suffix(self, name):
        if not self.translate_suffixes:
            return name
        pos = string.rfind(name, ".")
        if pos == -1:
            return name
        suffix = name[pos:]
        return name[:pos] + suffixes.get(suffix, suffix)

    # Acceps either a string or a list of strings and returns a list of
    # strings. Adjusts suffixes on all names.
    def adjust_names(self, names):
        if type(names) == types.StringType:
            names = [names]
        r = map(self.adjust_lib_name, names)
        r = map(self.adjust_suffix, r)
        r = map(lambda x, t=self.toolset: string.replace(x, "$toolset", t + "*"
            ), r)
        return r

    def native_file_name(self, name):
        name = self.adjust_names(name)[0]
        elements = string.split(name, "/")
        return os.path.normpath(apply(os.path.join, [self.workdir] + elements))

    def wait_for_time_change(self, original_timestamp=None):
        """
          Wait until newly assigned file system timestamps are large enough to
        be correctly recognized as newer than the given original timestamp.
        They need to be large enough for this to be recognizable by both this
        Python based testing framework and the Boost Jam executable being
        tested.

          Note: This code assumes that both the file system and the Boost Jam
        executable being tested use file modification timestamp resolution no
        coarser than 1 second. This for example does not hold on old FAT file
        systems that used a 2 second file modification timestamp resolution.

        """
        # In theory waiting until original_timestamp + 1 should be enough here
        # but due to rounding errors in Python's floating point timestamp
        # representation, file system's file modification timestamp caching and
        # possibly some other OS/Python/file-system layers as well it
        # occasionally happens that with just "+ 1" our 'touch' and other
        # commands still do not bump the file time enough to trigger a rebuild.
        # To work around this we add an extra tiny wait at the end in case
        # there is a chance we are too close to the problematic "+ 1" boundary.
        if original_timestamp is None:
            time.sleep(1.1)
            return
        wait_check = lambda x : (self.__get_current_file_timestamp() <
            original_timestamp + x)
        extra_wait_needed = wait_check(2)
        while wait_check(1):
            time.sleep(0.1)
        if extra_wait_needed:
            time.sleep(0.1)

    def wait_for_time_change_since_last_build(self):
        """
          Wait until newly assigned file system timestamps are large enough to
        be correctly recognized as newer than the ones assigned to files
        created by our previous build run. They need to be large enough for
        this to be recognizable by both this Python based testing framework and
        the Boost Jam executable being tested. Used to make subsequent builds
        correctly recognize newly created or touched files.

        """
        if self.last_build_timestamp:
            self.wait_for_time_change(self.last_build_timestamp)

    def __get_current_file_timestamp(self):
        fd, path = tempfile.mkstemp(prefix="__Boost_Build_timestamp_tester__")
        try:
            return os.fstat(fd).st_mtime
        finally:
            os.close(fd)
            os.unlink(path)


class List:

    def __init__(self, s=""):
        elements = []
        if isinstance(s, type("")):
            # Have to handle escaped spaces correctly.
            s = string.replace(s, "\ ", "\001")
            elements = string.split(s)
        else:
            elements = s
        self.l = [string.replace(e, "\001", " ") for e in elements]

    def __len__(self):
        return len(self.l)

    def __getitem__(self, key):
        return self.l[key]

    def __setitem__(self, key, value):
        self.l[key] = value

    def __delitem__(self, key):
        del self.l[key]

    def __str__(self):
        return str(self.l)

    def __repr__(self):
        return "%s.List(%r)" % (self.__module__, " ".join(self.l))

    def __mul__(self, other):
        result = List()
        if not isinstance(other, List):
            other = List(other)
        for f in self:
            for s in other:
                result.l.append(f + s)
        return result

    def __rmul__(self, other):
        if not isinstance(other, List):
            other = List(other)
        return List.__mul__(other, self)

    def __add__(self, other):
        result = List()
        result.l = self.l[:] + other.l[:]
        return result


def _contains_lines(data, lines):
    data_line_count = len(data)
    expected_line_count = reduce(lambda x, y: x + len(y), lines, 0)
    index = 0
    for expected in lines:
        if expected_line_count > data_line_count - index:
            return False
        expected_line_count -= len(expected)
        index = __match_line_sequence(data, index, data_line_count -
            expected_line_count, expected)
        if index < 0:
            return False
    return True


def __match_line_sequence(data, start, end, lines):
    if not lines:
        return start
    for index in xrange(start, end - len(lines) + 1):
        data_index = index
        for expected in lines:
            if not fnmatch.fnmatch(data[data_index], expected):
                break;
            data_index += 1
        else:
            return data_index
    return -1


###############################################################################
#
# Initialization.
#
###############################################################################

# Make os.stat() return file modification times as floats instead of integers
# to get the best possible file timestamp resolution available. The exact
# resolution depends on the underlying file system and the Python os.stat()
# implementation. The better the resolution we achieve, the shorter we need to
# wait for files we create to start getting new timestamps.
#
# Additional notes:
#  * os.stat_float_times() function first introduced in Python 2.3. and
#    suggested for deprecation in Python 3.3.
#  * On Python versions 2.5+ we do not need to do this as there os.stat()
#    returns floating point file modification times by default.
#  * Windows CPython implementations prior to version 2.5 do not support file
#    modification timestamp resolutions of less than 1 second no matter whether
#    these timestamps are returned as integer or floating point values.
#  * Python documentation states that this should be set in a program's
#    __main__ module to avoid affecting other libraries that might not be ready
#    to support floating point timestamps. Since we use no such external
#    libraries, we ignore this warning to make it easier to enable this feature
#    in both our single & multiple-test scripts.
if (2, 3) <= sys.version_info < (2, 5) and not os.stat_float_times():
    os.stat_float_times(True)


# Quickie tests. Should use doctest instead.
if __name__ == "__main__":
    assert str(List("foo bar") * "/baz") == "['foo/baz', 'bar/baz']"
    assert repr("foo/" * List("bar baz")) == "__main__.List('foo/bar foo/baz')"

    assert _contains_lines([], [])
    assert _contains_lines([], [[]])
    assert _contains_lines([], [[], []])
    assert _contains_lines([], [[], [], []])
    assert not _contains_lines([], [[""]])
    assert not _contains_lines([], [["a"]])

    assert _contains_lines([""], [])
    assert _contains_lines(["a"], [])
    assert _contains_lines(["a", "b"], [])
    assert _contains_lines(["a", "b"], [[], [], []])

    assert _contains_lines([""], [[""]])
    assert not _contains_lines([""], [["a"]])
    assert not _contains_lines(["a"], [[""]])
    assert _contains_lines(["a", "", "b", ""], [["a"]])
    assert _contains_lines(["a", "", "b", ""], [[""]])
    assert _contains_lines(["a", "", "b"], [["b"]])
    assert not _contains_lines(["a", "b"], [[""]])
    assert not _contains_lines(["a", "", "b", ""], [["c"]])
    assert _contains_lines(["a", "", "b", "x"], [["x"]])

    data = ["1", "2", "3", "4", "5", "6", "7", "8", "9"]
    assert _contains_lines(data, [["1", "2"]])
    assert not _contains_lines(data, [["2", "1"]])
    assert not _contains_lines(data, [["1", "3"]])
    assert not _contains_lines(data, [["1", "3"]])
    assert _contains_lines(data, [["1"], ["2"]])
    assert _contains_lines(data, [["1"], [], [], [], ["2"]])
    assert _contains_lines(data, [["1"], ["3"]])
    assert not _contains_lines(data, [["3"], ["1"]])
    assert _contains_lines(data, [["3"], ["7"], ["8"]])
    assert not _contains_lines(data, [["1"], ["3", "5"]])
    assert not _contains_lines(data, [["1"], [""], ["5"]])
    assert not _contains_lines(data, [["1"], ["5"], ["3"]])
    assert not _contains_lines(data, [["1"], ["5", "3"]])

    assert not _contains_lines(data, [[" 3"]])
    assert not _contains_lines(data, [["3 "]])
    assert not _contains_lines(data, [["3", ""]])
    assert not _contains_lines(data, [["", "3"]])

    print("tests passed")
