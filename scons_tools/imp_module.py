"""Tools and Builders for IMP modules. See `IMPModule` for more information."""

import os.path
import pyscanner

from SCons.Script import Builder, File, Action, Glob, Return, Alias, Dir
import hierarchy

def postprocess_lib(env, target, suffix):
    """    if env['PLATFORM'] == 'darwin':
       libdir= os.path.split(pyext[0].abspath)[0]
       env.AddPostAction (libinst, "install_name_tool -change %s/libimp%s.dylib %s/libimp%s.dylib %s" \
                                  % (libdir, module_suffix,
                                     env['libdir'], module_suffix,
                                     libinst[0].path))"""

def make_vars(env):
    """Make a map which can be used for all string substitutions"""
    module = env['IMP_MODULE']
    module_include_path = env['IMP_MODULE_INCLUDE_PATH']
    module_src_path = env['IMP_MODULE_SRC_PATH']
    module_preproc = env['IMP_MODULE_PREPROC']
    module_namespace = env['IMP_MODULE_NAMESPACE']
    author = env['IMP_MODULE_AUTHOR']#source[0].get_contents()
    version = env['IMP_MODULE_VERSION']#source[1].get_contents()
    vars={'module_include_path':module_include_path,
          'module_src_path':module_src_path, 'module':module,
          'PREPROC':module_preproc, 'author':author, 'version':version,
          'namespace':module_namespace}
    return vars


def action_config(target, source, env):
    """The IMPModuleConfig Builder generates a configuration header file
       used to mark classes and functions for export and to define namespaces,
       and a corresponding SWIG interface, e.g.
       env.IMPModuleConfig(('config.h', 'foo_config.i'), env.Value('foo'))
       generates a configuration header and interface for the 'foo' module."""
    vars= make_vars(env)
    h = file(target[0].abspath, 'w')
    i = file(target[1].abspath, 'w')
    vars['filename']=os.path.basename(target[0].abspath)
    print >> h, """/*
 * \\file %(filename)s
 * \\brief Provide macros to mark functions and classes as exported
 *        from a DLL/.so, and to set up namespaces
 *
 * When building the module, %(PREPROC)s_EXPORTS should be defined, and when
 * using the module externally, it should not be. Classes and functions
 * defined in the module's headers should then be marked with
 * %(PREPROC)sEXPORT if they are intended to be part of the API, or with
 * %(PREPROC)sLOCAL if they are not (the latter is the default).
 *
 * The Windows build environment requires applications to mark exports in
 * this way; we use the same markings to set the visibility of ELF symbols
 * if we have compiler support.
 *
 * All code in this module should live in the %(namespace)s namespace.
 * This is simply achieved by wrapping things with the
 * %(PREPROC)s_BEGIN_NAMESPACE and %(PREPROC)s_END_NAMESPACE macros.
 * There are similar macros for module code that is designed to be for
 * internal use only.
 *
 * This header is auto-generated by tools/imp-module.py; it should not be
 * edited manually.
 *
 * Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */

#ifndef %(PREPROC)s_CONFIG_H
#define %(PREPROC)s_CONFIG_H

#ifdef _MSC_VER
#ifdef %(PREPROC)s_EXPORTS
#define %(PREPROC)sEXPORT __declspec(dllexport)
#else
#define %(PREPROC)sEXPORT __declspec(dllimport)
#endif
#define %(PREPROC)sLOCAL
#else
#ifdef GCC_VISIBILITY
#define %(PREPROC)sEXPORT __attribute__ ((visibility("default")))
#define %(PREPROC)sLOCAL __attribute__ ((visibility("hidden")))
#else
#define %(PREPROC)sEXPORT
#define %(PREPROC)sLOCAL
#endif
#endif
""" % vars

    print >> i, """/* Ignore shared object exports macros */
#define %(PREPROC)sEXPORT
#define %(PREPROC)sLOCAL
""" % vars

    for out in (i, h):
        print >> out, "#define %(PREPROC)s_BEGIN_NAMESPACE \\"%vars
        for comp in vars['namespace'].split("::"):
            print >> out, "namespace %s {\\" %comp
        print >> out
        print >> out, "#define %(PREPROC)s_END_NAMESPACE \\"%vars
        for comp in vars['namespace'].split("::"):
            print >> out, "} /* namespace %s */ \\" %comp
        print >> out
        print >> out, """#define %(PREPROC)s_BEGIN_INTERNAL_NAMESPACE \\
%(PREPROC)s_BEGIN_NAMESPACE \\
namespace internal {
""" %vars
        print >> out
        print >> out, """#define %(PREPROC)s_END_INTERNAL_NAMESPACE \\
} /* namespace internal */ \\
%(PREPROC)s_END_NAMESPACE
""" %vars

    print >> h, """
#endif  /* %(PREPROC)s_CONFIG_H */""" % vars


def action_version_info(target, source, env):
    """The IMPModuleVersionInfo Builder generates a source file and header to
       return version information, e.g.
       env.IMPModuleVersionInfo(('src/internal/version_info.cpp',
                                 'include/internal/version_info.h'),
                                (env.Value('foo'), env.Value('Me'),
                                 env.Value('1.0')))
       generates version information for the 'foo' module."""
    vars= make_vars(env)

    cpp = file(target[0].abspath, 'w')
    h = file(target[1].abspath, 'w')

    for (f, ext) in ((cpp, 'cpp'), (h, 'h')):
        vars['ext']=ext
        print >> f, """/**
 *  \\file %(module_include_path)s/internal/version_info.%(ext)s
 *  \\brief %(module)s module version information.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */
""" % vars

    print >> h, """#ifndef %(PREPROC)s_INTERNAL_VERSION_INFO_H
#define %(PREPROC)s_INTERNAL_VERSION_INFO_H

#include "../config.h"

#include <IMP/VersionInfo.h>
""" % vars

    print >> cpp, '#include <%(module_include_path)s/internal/version_info.h>\n' \
                  % vars

    for f in (h, cpp):
        print >> f, "%(PREPROC)s_BEGIN_INTERNAL_NAMESPACE\n" % vars

    print >> h, """//! Version and authorship of the %(module)s module.
extern %(PREPROC)sEXPORT VersionInfo version_info;""" \
        % vars

    print >> cpp, 'VersionInfo version_info("%(author)s", "%(version)s");' \
              %vars

    for f in (h, cpp):
        print >> f, "\n%(PREPROC)s_END_INTERNAL_NAMESPACE" % vars

    print >> h, "\n#endif  /* %(PREPROC)s_INTERNAL_VERSION_INFO_H */" % vars

def action_link_test(target, source, env):
    """The IMPModuleLinkTesto Builder generates a source file. By linking in two
    of these, any functions which are defined in headers but not declared inline are detected"""
    cpp = file(target[0].abspath, 'w')
    vars= make_vars(env)
    vars['fname']=os.path.split(target[0].abspath)[1]
    print >> cpp, """/**
 *  \\file %(module_include_path)s/internal/%(fname)s
 *  \\brief Test linking for non-inlined functions.
 *
 *  This file is auto-generated, do not edit.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */
""" % vars

    print >> cpp, """
#ifndef NDEBUG
#include "%(module_include_path)s.h"
#endif""" % vars


def IMPSharedLibrary(env, files, install=True):
    """Build, and optionally also install, an IMP module's C++
       shared library. This is only available from within an environment
       created by `IMPSharedLibraryEnvironment`."""
    module = env['IMP_MODULE']
    module_suffix = env['IMP_MODULE_SUFFIX']
    vars= make_vars(env)
    if env['build']=='profile' and env['CC'] == 'gcc':
        build = env.StaticLibrary('#/build/lib/imp%s' % module_suffix,
                                      list(files) + [env['VER_CPP'], \
                                                     env['LINK_0_CPP'],\
                                                     env['LINK_1_CPP']])
    else:
        build = env.SharedLibrary('#/build/lib/imp%s' % module_suffix,
                           list(files) + [env['VER_CPP'], \
                                              env['LINK_0_CPP'],\
                                              env['LINK_1_CPP']])
        postprocess_lib(env, build, "something")
    install = env.Install(env.GetInstallDirectory('libdir'), build)
    postprocess_lib(env, install, "something")
    env.Alias(vars['module']+"-lib", build)
    env.Alias(vars['module']+"-install-lib", install)
    env.Alias("install", install)


def IMPSharedLibraryEnvironment(env):
    """Create a customized environment suitable for building IMP module C++
       shared libraries. Use the resulting object's `IMPSharedLibrary` pseudo
       builder to actually build the shared library."""
    from scons_tools import get_sharedlib_environment
    vars= make_vars(env)
    env = get_sharedlib_environment(env, '%(PREPROC)s_EXPORTS' % vars,
                                    cplusplus=True)
    env.AddMethod(IMPSharedLibrary)
    return env

def IMPHeaders(env, files):
    """Install the given header files, plus any auto-generated files for this
       IMP module."""
    vars=make_vars(env)
    includedir = env.GetInstallDirectory('includedir')
    install = hierarchy.InstallHierarchy(env, includedir+"/"+vars['module_include_path'],
                            list(files) + [env['CONFIG_H'], env['VER_H']])
    build=hierarchy.InstallHierarchy(env, "#/build/include/"+vars['module_include_path'],
                            list(files) + [env['CONFIG_H'], env['VER_H']], True)
    env.Alias(vars['module']+"-include", build)
    env.Alias(vars['module']+"-install-include", install)
    env.Alias("install", install)

def IMPData(env, files):
    """Install the given data files for this IMP module."""
    vars=make_vars(env)
    datadir = env.GetInstallDirectory('datadir')
    install = hierarchy.InstallDataHierarchy(env, datadir+"/"+vars['module_include_path'], files, False)
    build = hierarchy.InstallDataHierarchy(env, "#/build/data/"+vars['module_include_path'], files, True)
    env.Alias(vars['module']+"-data", build)
    env.Alias(vars['module']+"-install-data", install)
    env.Alias("install", install)


def IMPPythonExtension(envi, swig_interface):
    """Build and install an IMP module's Python extension and the associated
       wrapper file from a SWIG interface file. This is only available from
       within an environment created by `IMPPythonExtensionEnvironment`."""
    env= IMPPythonExtensionEnvironment(envi)
    module = env['IMP_MODULE']
    module_suffix = env['IMP_MODULE_SUFFIX']
    vars=make_vars(env)
    build=[]
    install=[]
    if env['IMP_MODULE_CPP']:
        swigcom = env['SWIGCOM']
        if isinstance(swigcom, list) and isinstance(swigcom[0], str):
            repl = '$SWIG -interface _IMP%s ' % module_suffix
            swigcom[0] = swigcom[0].replace('$SWIG ', repl)
        buildlib = env.LoadableModule('#/build/lib/_IMP%s' % module_suffix,
                                      swig_interface,
                                      SWIGCOM=swigcom)
        # Place the generated Python wrapper in lib directory:
        gen_pymod = File('IMP%s.py' % module_suffix.replace("_","."))
        buildinit = env.LinkInstallAs('#/build/lib/%s/__init__.py'
                                      % vars['module_include_path'],
                                      gen_pymod)
        installinit = env.InstallAs(env.GetInstallDirectory('pythondir',
                                                            vars['module_include_path'],
                                                            ' __init__.py'),
                                    gen_pymod)
        installlib = env.Install(env.GetInstallDirectory('pyextdir'), buildlib)
        postprocess_lib(env, buildlib, module_suffix)
        build.append(buildlib)
        build.append(buildinit)
        install.append(installinit)
        install.append(installlib)
    files = Glob('src/*.py')
    #print [x.path for x in Glob("*")]
    #print Dir("src").path
    #print [x.path for x in Glob("src/*")]
    #print [x.path for x in Glob("src/*.py")]
    for f in Glob("src/*.py"):
        #print f
        nm= os.path.split(f.path)[1]
        #print ('#/build/lib/%s/'+nm) % vars['module_include_path']
        build.append(env.LinkInstallAs(('#/build/lib/%s/'+nm) % vars['module_include_path'],
                                       f))
        install.append(env.InstallAs(env.GetInstallDirectory('pythondir',
                                                             vars['module_include_path'],
                                                             nm),f))
    # Install the Python extension and module:
    #buildlib = env.Install("#/build/lib", pyext)

    env.Alias(vars['module']+"-python", build)
    env.Alias(vars['module']+"-install-python", install)
    env.Alias("install", install)

def IMPPythonExtensionEnvironment(env):
    """Create a customized environment suitable for building IMP module Python
       extensions. Use the resulting object's `IMPPythonExtension` pseudo
       builder to actually build the extension."""
    from scons_tools import get_pyext_environment
    module = env['IMP_MODULE']
    module_suffix= env['IMP_MODULE_SUFFIX']
    env = get_pyext_environment(env, module.upper(), cplusplus=True)
    env.Append(LIBS=['imp%s' % module_suffix])
    env['SWIGPATH'] = [env['CPPPATH'], '#']
    env.Append(SWIGFLAGS='-python -c++ -naturalvar')
    env.AddMethod(IMPPythonExtension)
    return env

def _action_unit_test(target, source, env):
    #app = "cd %s; %s %s %s -v > /dev/null" \
    app = "%s %s %s %s > /dev/null" \
          % (#os.path.split(target[0].path)[0],
             source[0].abspath, env['PYTHON'],
             source[1].abspath,
             " ".join([x.abspath for x in source[2:]]))
    if env.Execute(app) == 0:
        file(str(target[0]), 'w').write('PASSED\n')
    else:
        print "IMP.%s unit tests FAILED" % env['IMP_MODULE']
        return 1

def IMPModuleGetHeaders(env):
    vars = make_vars(env)
    raw_files=Glob("*.h")+Glob("*/*.h")
    files=[]
    for f in raw_files:
        s= str(f)
        #print s
        fname= os.path.split(s)[1]
        if fname.startswith("."):
            continue
        if s== "internal/version_info.h":
            continue
        if s=="config.h":
            continue
        files.append(f)
    return files

def IMPModuleGetSource(env):
    vars = make_vars(env)
    raw_files=Glob("*.cpp")+Glob("*/*.cpp")
    files=[]
    for f in raw_files:
        s= str(f)
        #print s
        fname= os.path.split(s)[1]
        if fname.startswith("."):
            continue
        if s== "internal/link_0.cpp":
            continue
        if s== "internal/link_1.cpp":
            continue
        if s=="internal/version_info.cpp":
            continue
        files.append(f)
    return files

#   files= ["#/bin/imppy.sh", "#/tools/run_all_tests.py"]+\
#        [x.abspath for x in Glob("test_*.py")+ Glob("*/test_*.py")]

def IMPModuleTest(env, dependencies=[], **keys):
    """Pseudo-builder to run tests for an IMP module. The single target is
       generally a simple output file, e.g. 'test.passed', while the single
       source is a Python script to run (usually run-all-tests.py).
       Right now, the assumption is made that run-all-tests.py executes
       all files called test_*.py in the current directory and subdirectories.
       If the TEST_ENVSCRIPT construction variable is set, it is a shell
       script to run to set up the environment to run the test script.
       A convenience alias for the tests is added, and they are always run."""
    files= ["#/bin/imppy.sh", "#/tools/run-all-tests.py"]+\
        [x.abspath for x in Glob("test_*.py")+ Glob("*/test_*.py")]
    #print files
    test = env._IMPModuleTest("test.passed", files, **keys)
    env.AlwaysBuild("test.passed")
    vars=make_vars(env)
    ta=env.Alias(vars['module']+"-test", test)
    env.Alias('test', vars['module']+"-test")
    for d in dependencies:
        env.Depends(ta, Alias(d+"-python"))

def invalidate(env, fail_action):
    """'Break' an environment, so that any builds with it use the fail_action
       function (which should be an Action which terminates the build)"""
    for var in ('SHLINKCOM', 'CCCOM', 'CXXCOM', 'SHCCCOM', 'SHCXXCOM',
                'SWIGCOM'):
        env[var] = fail_action
    env.Append(BUILDERS={'_IMPModuleTest': Builder(action=fail_action)})
    env['VALIDATED'] = False

def validate(env):
    """Confirm that a module's environment is OK for builds."""
    module = env['IMP_MODULE']
    env['VALIDATED'] = True

def IMPModuleBuild(env, author, version, description, required_modules=[]):
    env['IMP_MODULE_DESCRIPTION'] = description
    env['IMP_MODULE_VERSION'] = version
    env['IMP_MODULE_AUTHOR'] = author
    vars=make_vars(env)
    env.validate()
    #env.SConscript('doc/SConscript', exports='env')
    nnl=False
    print "Configuring module " + vars['module'],
    if os.path.exists(env.Dir("data").abspath):
        env.SConscript('data/SConscript', exports='env')
    else:
        print " (no data)",
    if not env['IMP_MODULE_CPP']:
        print " (python only)",
    print
    if env['IMP_MODULE_CPP']:
        if required_modules is not None:
            env.Prepend(LIBS=['imp'])
            for x in required_modules:
                if x.startswith("imp_"):
                    print "Required modules should have the name of the module (eg 'algebra'), not the name of the library."
                if x=='kernel':
                    print "You do not need to list the kernel as a required module"
            env.Prepend(LIBS=['imp_'+x for x in required_modules])
        env.SConscript('include/SConscript', exports='env')
        env.SConscript('src/SConscript', exports='env')
        env.Depends(env.Alias(vars['module']+"-src"), [Alias(vars['module']+"-include")])
    if env.get('python', True):
        env.SConscript('pyext/SConscript', exports='env')
        env.SConscript('test/SConscript', exports='env')
        env.Depends(env.Alias(vars['module']+"-test"), [Alias(vars['module']+"-python"),
                                                        Alias(vars['module']+"-data")])
        env.Depends(env.Alias(vars['module']+"-python"),
                    [Alias(vars['module']+"-src")])
        env.Default([env.Alias(vars['module']+"-python")])
        env.Alias('test', [env.Alias(vars['module']+"-test")])
        env.Alias('pythonlibs', [env.Alias(vars['module']+"-python")])
    else:
        env.Default([env.Alias(vars['module']+"-src")])
        env.Alias('libs', [env.Alias(vars['module']+"-src")])



def IMPModuleSetup(env, module, cpp=True, module_suffix=None,
                   module_include_path=None, module_src_path=None, module_preproc=None,
                   module_namespace=None):
    """Set up an IMP module. The module's SConscript gets its own
       customized environment ('env') in which the following pseudo-builders
       or methods are available: IMPPython, IMPModuleTest, validate
       and invalidate. If `cpp` is True, necessary C++ headers are also
       automatically generated, and these additional methods are available:
       IMPSharedLibraryEnvironment, IMPPythonExtensionEnvironment, IMPHeaders,
       IMPData.
       Either validate or invalidate must be called in the module's top-level
       SConscript before setting up any builders, to indicate whether the
       module's necessary dependencies have been met.
    """
    if module_suffix is None:
        module_suffix="_"+module
    if module_src_path is None:
        module_src_path="modules/"+module
    if module_include_path is None:
        module_include_path="IMP/"+module
    if module_preproc is None:
        module_preproc="IMP"+module.upper()
    if module_namespace is None:
        module_namespace="IMP::"+module
    #print module_suffix
    #print module_src_path
    #print module_include_path
    #print module_preproc
    #print module_namespace
    env['IMP_MODULES_ALL'].append(module)
    env = env.Clone()
    config = Builder(action=action_config)
    version_info = Builder(action=action_version_info)
    link_test = Builder(action=action_link_test)
    env.Append(BUILDERS = {'IMPModuleConfig': config,
                           'IMPModuleVersionInfo': version_info,
                           'IMPModuleLinkTest': link_test})

    env['IMP_MODULE'] = module
    env['IMP_MODULE_SUFFIX'] = module_suffix
    env['IMP_MODULE_INCLUDE_PATH'] = module_include_path
    env['IMP_MODULE_SRC_PATH'] = module_src_path
    env['IMP_MODULE_PREPROC'] = module_preproc
    env['IMP_MODULE_NAMESPACE'] = module_namespace
    env['IMP_MODULE_DESCRIPTION'] = "An IMP Module"
    env['IMP_MODULE_VERSION'] = "SVN"
    env['IMP_MODULE_AUTHOR'] = "A. Biologist"
    env['IMP_MODULE_CPP']= cpp
    env.Prepend(CPPPATH=['#/build/include'])
    env.Prepend(LIBPATH=['#/build/lib'])
    vars=make_vars(env)
    if cpp:
        # Generate version information
        env['VER_CPP'], env['VER_H'] = \
            env.IMPModuleVersionInfo(
                 ('%s/src/internal/version_info.cpp' % (module),
                  '%s/include/internal/version_info.h' % (module)),
                 ())
        # Generate config header and SWIG equivalent
        env['CONFIG_H'] = env.IMPModuleConfig(('%s/include/config.h' % module,
                                               '%s/pyext/%s_config.i' \
                                               % (module, module)),
                                              [])[0]
        env['LINK_0_CPP']=env.IMPModuleLinkTest('#/%(module_src_path)s/src/internal/link_0.cpp'%vars,
                                                [])[0]
        env['LINK_1_CPP']=env.IMPModuleLinkTest('#/%(module_src_path)s/src/internal/link_1.cpp'%vars,
                                                [])[0]
        env.AddMethod(IMPSharedLibraryEnvironment)
        env.AddMethod(IMPHeaders)
        env.AddMethod(IMPData)
    env.AddMethod(IMPPythonExtension)
    env.AddMethod(IMPModuleTest)
    env.AddMethod(IMPModuleBuild)
    env.AddMethod(IMPModuleGetHeaders)
    env.AddMethod(IMPModuleGetSource)
    env.AddMethod(validate)
    env.AddMethod(invalidate)
    env.Append(BUILDERS={'_IMPModuleTest': \
                         Builder(action=Action(_action_unit_test),
                                 source_scanner=pyscanner.PythonScanner)})
    env['TEST_ENVSCRIPT'] = None
    env['VALIDATED'] = None
    env.SConscript('%s/SConscript' % module, exports='env')

def generate(env):
    """Add builders and construction variables for the IMP module tool."""
    env['IMP_MODULES_ALL'] = []
    env.AddMethod(IMPModuleSetup)

def exists(env):
    """Right now no external programs are needed"""
    return True
