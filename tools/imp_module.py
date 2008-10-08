"""Tools and Builders for IMP modules. See `IMPModule` for more information."""

import os.path
from SCons.Script import Builder, File, Action

def action_exports(target, source, env):
    """The IMPModuleExports Builder generates a header file used to mark
       classes and functions for export and a corresponding SWIG interface, e.g.
       env.IMPModuleExports(('foo_exports.h', 'foo_exports.i'),
                            env.Value('foo'))
       generates exports macros for the 'foo' module."""
    name = source[0].get_contents()
    h = file(target[0].abspath, 'w')
    i = file(target[1].abspath, 'w')
    vars = {'name':name, 'NAME':name.upper(),
            'filename':os.path.basename(target[0].abspath),
            'namespace':'IMP::' + name, 'NAMESPACE':'IMP' + name.upper()}
    print >> h, """/*
 * \\file %(filename)s
 * \\brief Provide macros to mark functions and classes as exported
 *        from a DLL/.so, and to set up namespaces
 *
 * When building the module, IMP%(NAME)s_EXPORTS should be defined, and when
 * using the module externally, it should not be. Classes and functions
 * defined in the module's headers should then be marked with
 * IMP%(NAME)sEXPORT if they are intended to be part of the API, or with
 * IMP%(NAME)sLOCAL if they are not (the latter is the default).
 *
 * The Windows build environment requires applications to mark exports in
 * this way; we use the same markings to set the visibility of ELF symbols
 * if we have compiler support.
 *
 * All code in this module should live in the %(namespace)s namespace.
 * This is simply achieved by wrapping things with the
 * %(NAMESPACE)s_BEGIN_NAMESPACE and %(NAMESPACE)s_END_NAMESPACE macros.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */

#ifndef IMP%(NAME)s_EXPORTS_H
#define IMP%(NAME)s_EXPORTS_H

#ifdef _MSC_VER
#ifdef IMP%(NAME)s_EXPORTS
#define IMP%(NAME)sEXPORT __declspec(dllexport)
#else
#define IMP%(NAME)sEXPORT __declspec(dllimport)
#endif
#define IMP%(NAME)sLOCAL
#else
#ifdef GCC_VISIBILITY
#define IMP%(NAME)sEXPORT __attribute__ ((visibility("default")))
#define IMP%(NAME)sLOCAL __attribute__ ((visibility("hidden")))
#else
#define IMP%(NAME)sEXPORT
#define IMP%(NAME)sLOCAL
#endif
#endif
""" % vars

    print >> i, """/* Ignore shared object exports macros */
#define IMP%(NAME)sEXPORT
#define IMP%(NAME)sLOCAL
""" % vars

    for out in (i, h):
        print >> out, """#define %(NAMESPACE)s_BEGIN_NAMESPACE \\
namespace IMP \\
{ \\
\\
namespace %(name)s \\
{

#define %(NAMESPACE)s_END_NAMESPACE \\
} /* namespace %(name)s */ \\
\\
} /* namespace IMP */""" % vars

    print >> h, """
#endif  /* IMP%(NAME)s_EXPORTS_H */""" % vars


def action_version_info(target, source, env):
    """The IMPModuleVersionInfo Builder generates a source file and header to
       return version information, e.g.
       env.IMPModuleVersionInfo(('src/internal/foo_version_info.cpp',
                                 'include/internal/foo_version_info.h'),
                                (env.Value('foo'), env.Value('Me'),
                                 env.Value('1.0')))
       generates version information for the 'foo' module."""
    module = source[0].get_contents()
    author = source[1].get_contents()
    version = source[2].get_contents()
    cpp = file(target[0].abspath, 'w')
    h = file(target[1].abspath, 'w')

    for (f, ext) in ((cpp, 'cpp'), (h, 'h')):
        print >> f, """/**
 *  \\file %(module)s_version_info.%(ext)s    \\brief %(module)s module version information.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */
""" % {'module':module, 'ext':ext}

    print >> h, """#ifndef IMP%(MODULE)s_VERSION_INFO_H
#define IMP%(MODULE)s_VERSION_INFO_H

#include "../%(module)s_exports.h"

#include <IMP/VersionInfo.h>
""" % {'module':module, 'MODULE':module.upper()}

    print >> cpp, '#include <IMP/%s/internal/%s_version_info.h>\n' \
                  % (module, module)

    for f in (h, cpp):
        print >> f, "IMP%s_BEGIN_NAMESPACE" % module.upper()
        print >> f, "\nnamespace internal\n{\n"

    print >> h, """//! Version and authorship of the %(module)s module.
extern IMP%(MODULE)sEXPORT VersionInfo %(module)s_version_info;""" \
        % {'module':module, 'MODULE':module.upper()}

    print >> cpp, 'VersionInfo %s_version_info("%s", "%s");' \
              % (module, author, version)

    for f in (h, cpp):
        print >> f, "\n} // namespace internal\n"
        print >> f, "IMP%s_END_NAMESPACE" % module.upper()

    print >> h, "\n#endif  /* IMP%s_VERSION_INFO_H */" % module.upper()

def _add_all_alias(aliases, env, name):
    """Add an 'all' alias `name` to the list of aliases, but only if the
       environment has been validated (i.e. this module is OK to build)."""
    if env['VALIDATED'] is None:
        print "Module environment not correctly set up - you must call\n" + \
              "either env.validate() or env.invalidate() in the " + \
              "module SConscript"
        env.Exit(1)
    if env['VALIDATED']:
        aliases.append(name)

def _get_module_install_aliases(env):
    """Get a list of all 'install' aliases for this module"""
    aliases = ['%s-install' % env['IMP_MODULE'], 'modules-install']
    _add_all_alias(aliases, env, 'install')
    return aliases

def _get_module_test_aliases(env):
    """Get a list of all 'test' aliases for this module"""
    aliases = ['%s-test' % env['IMP_MODULE'], 'modules-test']
    _add_all_alias(aliases, env, 'test')
    return aliases

def _add_module_default_alias(env, targets):
    """Add the default alias for this module, to build the given targets."""
    if env['VALIDATED']:
        env.Default(targets)

def IMPSharedLibrary(env, files, install=True):
    """Build, and optionally also install, an IMP module's C++
       shared library. This is only available from within an environment
       created by `IMPSharedLibraryEnvironment`."""
    module = env['IMP_MODULE']
    lib = env.SharedLibrary('#/build/lib/imp_%s' % module,
                            list(files) + [env['VER_CPP']])
    if install:
        libinst = env.Install(env['libdir'], lib)
        for alias in _get_module_install_aliases(env):
            env.Alias(alias, [libinst])
        return lib, libinst
    else:
        return lib

def IMPSharedLibraryEnvironment(env):
    """Create a customized environment suitable for building IMP module C++
       shared libraries. Use the resulting object's `IMPSharedLibrary` pseudo
       builder to actually build the shared library."""
    from tools import get_sharedlib_environment
    module = env['IMP_MODULE']
    env = get_sharedlib_environment(env, 'IMP%s_EXPORTS' % module.upper(),
                                    cplusplus=True)
    env.AddMethod(IMPSharedLibrary)
    return env

def IMPHeaders(env, files):
    """Install the given header files, plus any auto-generated files for this
       IMP module."""
    from tools.hierarchy import InstallHierarchy
    includedir = os.path.join(env['includedir'], 'IMP')
    inst = InstallHierarchy(env, includedir, env['IMP_MODULE'],
                            env['IMP_MODULE_DESCRIPTION'],
                            list(files) + [env['EXP_H'], env['VER_H']])
    for alias in _get_module_install_aliases(env):
        env.Alias(alias, inst)
    return inst

def IMPPython(env, files):
    """Install the given Python files for this IMP module."""
    from tools.hierarchy import InstallPythonHierarchy
    pydir = os.path.join(env['pythondir'], 'IMP')
    inst, lib = InstallPythonHierarchy(env, pydir, env['IMP_MODULE'], files)
    for alias in _get_module_install_aliases(env):
        env.Alias(alias, inst)
    _add_module_default_alias(env, lib)
    return lib

def IMPPythonExtension(env, swig_interface):
    """Build and install an IMP module's Python extension and the associated
       wrapper file from a SWIG interface file. This is only available from
       within an environment created by `IMPPythonExtensionEnvironment`."""
    module = env['IMP_MODULE']
    pyext = env.LoadableModule('#/build/lib/_IMP_%s' % module, swig_interface)
    # .py file should also be generated:
    gen_pymod = File('IMP_%s.py' % module)
    env.Depends(gen_pymod, pyext)
    # Place in lib directory:
    pymod = env.LinkInstallAs('#/build/lib/IMP/%s/__init__.py' % module,
                              gen_pymod)

    # Install the Python extension and module:
    libinst = env.Install(env['pyextdir'], pyext)
    pyinst = env.Install(os.path.join(env['pythondir'], 'IMP', module), pymod)
    for alias in _get_module_install_aliases(env):
        env.Alias(alias, [libinst, pyinst])
    _add_module_default_alias(env, [pyext, pymod])
    return pyext, pymod

def IMPPythonExtensionEnvironment(env):
    """Create a customized environment suitable for building IMP module Python
       extensions. Use the resulting object's `IMPPythonExtension` pseudo
       builder to actually build the extension."""
    from tools import get_pyext_environment
    module = env['IMP_MODULE']
    env = get_pyext_environment(env, 'IMP' + module.upper(), cplusplus=True)
    env.Append(LIBS=['imp_%s' % module])
    env.Append(SWIGPATH='#/build/include')
    env.Append(SWIGFLAGS='-python -c++ -naturalvar')
    env.AddMethod(IMPPythonExtension)
    return env

def _action_unit_test(target, source, env):
    (dir, script) = os.path.split(source[0].path)
    scripts = [File('#/bin/imppy.sh')]
    if env['TEST_ENVSCRIPT']:
        scripts.append(File(env['TEST_ENVSCRIPT']))
    app = "cd %s && %s %s %s -v > /dev/null" \
          % (dir, " ".join([x.abspath for x in scripts]),
             env['PYTHON'], script)
    if env.Execute(app) == 0:
        file(str(target[0]), 'w').write('PASSED\n')
    else:
        print "unit tests FAILED"
        return 1

def _print_unit_test(target, source, env):
    return "IMPModuleTest('%s')" % source[0]

def _emit_unit_test(target, source, env):
    # Add all test_*.py scripts to sources
    for dirpath, dirnames, filenames in os.walk('.'):
        for f in filenames:
            if f.startswith('test_') and f.endswith('.py'):
                source.append(os.path.join(dirpath, f))
    # Add environment scripts to sources
    source.append('#/bin/imppy.sh')
    if env['TEST_ENVSCRIPT']:
        source.append(env['TEST_ENVSCRIPT'])
    return target, source

def IMPModuleTest(env, target, source, **keys):
    """Pseudo-builder to run tests for an IMP module. The single target is
       generally a simple output file, e.g. 'test.passed', while the single
       source is a Python script to run (usually run-all-tests.py).
       Right now, the assumption is made that run-all-tests.py executes
       all files called test_*.py in the current directory and subdirectories.
       If the TEST_ENVSCRIPT construction variable is set, it is a shell
       script to run to set up the environment to run the test script.
       A convenience alias for the tests is added, and they are always run."""
    test = env._IMPModuleTest(target, source, **keys)
    for alias in _get_module_test_aliases(env):
        env.Alias(alias, test)
    env.AlwaysBuild(target)
    return test

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

def IMPModule(env, module, author, version, description, cpp=True):
    """Set up an IMP module. The module's SConscript gets its own
       customized environment ('env') in which the following pseudo-builders
       or methods are available: IMPPython, IMPModuleTest, validate
       and invalidate. If `cpp` is True, necessary C++ headers are also
       automatically generated, and these additional methods are available:
       IMPSharedLibraryEnvironment, IMPPythonExtensionEnvironment, IMPHeaders.
       Either validate or invalidate must be called in the module's top-level
       SConscript before setting up any builders, to indicate whether the
       module's necessary dependencies have been met.
    """
    env = env.Clone()
    exports = Builder(action=action_exports)
    version_info = Builder(action=action_version_info)
    env.Append(BUILDERS = {'IMPModuleExports': exports,
                           'IMPModuleVersionInfo': version_info})

    env['IMP_MODULE'] = module
    env['IMP_MODULE_DESCRIPTION'] = description
    env.Append(CPPPATH=['#/build/include', env['BOOST_CPPPATH']])
    env.Append(LIBPATH=['#/build/lib'], LIBS=['imp'])

    if cpp:
        # Generate version information
        env['VER_CPP'], env['VER_H'] = \
            env.IMPModuleVersionInfo(
                 ('%s/src/internal/%s_version_info.cpp' % (module, module),
                  '%s/include/internal/%s_version_info.h' % (module, module)),
                 (env.Value(module), env.Value(author), env.Value(version)))
        # Generate exports header and SWIG equivalent
        env['EXP_H'] = env.IMPModuleExports(('%s/include/%s_exports.h' \
                                             % (module, module),
                                             '%s/pyext/%s_exports.i' \
                                             % (module, module)),
                                            env.Value(module))[0]
        env.AddMethod(IMPSharedLibraryEnvironment)
        env.AddMethod(IMPPythonExtensionEnvironment)
        env.AddMethod(IMPHeaders)

    env.AddMethod(IMPPython)
    env.AddMethod(IMPModuleTest)
    env.AddMethod(validate)
    env.AddMethod(invalidate)
    env.Append(BUILDERS={'_IMPModuleTest': \
                         Builder(action=Action(_action_unit_test,
                                               _print_unit_test),
                                 emitter=_emit_unit_test)})
    env['TEST_ENVSCRIPT'] = None
    env['VALIDATED'] = None
    return env.SConscript('%s/SConscript' % module, exports='env')

def generate(env):
    """Add builders and construction variables for the IMP module tool."""
    env.AddMethod(IMPModule)

def exists(env):
    """Right now no external programs are needed"""
    return True
