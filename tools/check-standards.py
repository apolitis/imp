#!/usr/bin/env python

import sys
import os.path
import glob
from reindent import Reindenter
import re
import traceback
try:
    import python.cpp_format as cpp_format
except ImportError:
    cpp_format = None
    print "Cannot conduct additional C++ format checks without the Python "
    print "Pygments (http://pygments.org/) library: please install."
    print

def _check_do_not_commit(line, filename, num, errors):
    marker = 'DO NOT' + ' COMMIT'
    if line.find(marker) >= 0:
        errors.append('%s:%d: Line contains the string "%s"' \
                      % (filename, num+1, marker));


def check_c_file(filename, errors):
    """Check each modified C file to make sure it adheres to the standards"""
    fh = file(filename, "r").read().split("\n")
    srch = re.compile('\s+$')
    url = re.compile('https?://')
    configh=False
    blank = False
    file_line = False
    exported= filename.find("internal")==-1 and filename.endswith(".h")
    name = os.path.split(filename)[1]
    module = None
    # modules/name/include/header.h
    if os.path.split(os.path.split(os.path.split(os.path.split(filename)[0])[0])[0])[1] == "modules":
        module = os.path.split(os.path.split(os.path.split(filename)[0])[0])[1]
    for (num, line) in enumerate(fh):
        line = line.rstrip('\r\n')
        if line.find("\\file %s/%s"%(module, name)) != -1 or line.find("\\file IMP/%s/%s"%(module, name)) != -1:
            file_line = True
        # No way to split URLs, so let them exceed 80 characters:
        if line.startswith(">>>>>>> "):
            errors.append("%s:%d: error: Incomplete merge found."% (filename, num+1))
        if len(line) > 80 and not url.search(line):
            errors.append('%s:%d: error: Line is longer than 80 characters.' \
                          % (filename, num+1))
        if line.find('\t') >= 0:
            errors.append('%s:%d: error: Line contains tabs.' % (filename, num+1))
        _check_do_not_commit(line, filename, num, errors)
        if srch.search(line):
            errors.append('%s:%d: error: Line has trailing whitespace' \
                          % (filename, num+1))
        if not filename.endswith(".cpp") and line.startswith("#define ") \
           and not line.startswith("#define IMP") \
           and not line.startswith("#define EIGEN_YES_I_KNOW_SPARSE_"
                                   "MODULE_IS_NOT_STABLE_YET"):
            found=False
            fline=line.replace("#define", "#undef")
            for (onum, oline) in enumerate(fh):
                if onum > num and oline.startswith(fline):
                    found=True
            if not found:
                errors.append('%s:%d: error: Preprocessor symbols must start with IMP' \
                                  % (filename, num+1))
        blank = (len(line) == 0)
        if line.startswith('#include "'):
            configh=True;
        if blank and num == 0:
            errors.append('%s:1: File has leading blank line(s)' % filename)
    if len(fh)>0 and len(fh) > 2 and len(fh[-2])==0:
        errors.append('%s:%d: File has trailing blank line(s)' % (filename, len(fh)))
    if exported and filename.endswith(".h") and not file_line and module:
        errors.append('%s:2: Exported header must have a line \\file %s/%s'% (filename, module, name))

def check_python_file(filename, errors):
    """Check each modified Python file to make sure it adheres to the
       standards"""
    temptest = re.compile('\s+def\s+temp_hide_test.*')
    test= re.compile('\s+def\s+(test_[abcdefghijklmnopqrstuvwxyz0123456789_]*)\(')
    import_as= re.compile('[ ]*import [ ]*.* [ ]*as [ ]*.*')
    import_from = re.compile('[ ]*from [ ]*.* [ ]*import [ ]*.*')
    tests=[]
    for (num, line) in enumerate(file(filename, "r")):
        _check_do_not_commit(line, filename, num, errors)
        if temptest.match(line):
            errors.append('%s:%d: Test case has the temp_hide_ prefix' \
                          % (filename, num+1))
        m= test.match(line)
        if line.startswith(">>>>>>> "):
            errors.append("%s:%d: error: Incomplete merge found."% (filename, num+1))
        if m:
            g= m.group(1)
            if g in tests:
                errors.append('%s:%d: Test case has multiple tests with the same name %s' \
                          % (filename, num+1, g))
            tests.append(m.group(1))
        if filename.find("test") == -1 and filename.find("example") != -1:
            if import_as.match(line):
                errors.append('%s:%d: Examples should not rename types on import as that confuses doxygen: '%(filename, num+1) + line)
            if import_from.match(line):
                errors.append('%s:%d: Examples should not use import from as that confuses doxygen: '%(filename, num+1) + line)
    fh = file(filename, "r")
    r = Reindenter(fh)
    try:
        if 'compat_python' not in filename and r.run():
            errors.append('Python file ' + filename + ' has odd indentation; ' \
                          + 'please run through tools/reindent.py first.')
    except Exception:
        print >> sys.stderr, "reindent.py FAILED on %s:" % filename
        raise

def get_file(filename):
    return (file(filename, 'r'), filename)

def check_modified_file(filename, errors):
    """Check each modified file to make sure it adheres to the standards"""
    # skip code that isn't ours
    if filename.find("dependency") != -1:
        return
    # don't check header guard in template headers
    if filename.find("templates") != -1:
        return
    if filename.endswith('.h') or filename.endswith('.cpp') \
       or filename.endswith('.c'):
        check_c_file(filename, errors)
        # don't check header guard in template headers
        if cpp_format and filename.endswith('.h') and filename.find("templates") == -1:
            cpp_format.check_header_file(get_file(filename), errors)
        elif cpp_format and filename.endswith('.cpp'):
            cpp_format.check_cpp_file(get_file(filename), errors)
    elif filename.endswith('.py') or filename.endswith('SConscript') \
         or filename.endswith('SConstruct'):
        check_python_file(filename, errors)

def file_matches_re(pathname, excludes):
    for e in excludes:
        if e.match(pathname):
            return True
    return False

def get_all_files():
    modfiles = []
    excludes = []
    excludes = [re.compile(x) for x in excludes]
    for root, dirs, files in os.walk('.'):
        if '.sconf_temp' not in root and not root.startswith('./build/'):
            for f in files:
                if not f.startswith('.'):
                    pathname = os.path.join(root, f)
                    if not file_matches_re(pathname, excludes):
                        modfiles.append(pathname)
    return modfiles

def main():
    errors = []
    if len(sys.argv) == 1:
        modfiles= get_all_files()
        print "usage:", sys.argv[0], "file_patterns"
    else:
        modfiles = sys.argv[1:]
    for pattern in modfiles:
        expanded= glob.glob(pattern)
        #rint pattern, expanded
        for filename in expanded:
            try:
                check_modified_file(filename, errors)
            except:
                print "Exception processing file "+filename
                traceback.print_exc()
    if len(errors) > 0:
        sys.stderr.write("The following problems needed to be fixed:\n\n")
        sys.stderr.write("\n".join(errors))
        sys.stderr.write("\n")
        sys.exit(1)

if __name__ == '__main__':
    main()
