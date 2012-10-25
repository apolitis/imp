"""
protoc.py: Protoc Builder for SCons

This Builder invokes protoc to generate C++ and Python
from a .proto file.

NOTE: Java is not currently supported."""

__author__ = "Scott Stafford"

import SCons.Action
import SCons.Builder
import SCons.Defaults
import SCons.Node.FS
import SCons.Util

from SCons.Script import File, Dir

import os.path

protocs = 'protoc'

# Stock protoc unfortunately has no way (currently) to insert a custom #include
# into the generated header, so we have to postprocess the file:
def _fix_protoc_includes(env, target, source):
    if env['PROTOCINCLUDE']:
        for t in target:
            if t.abspath.endswith('.h'):
                lines = open(t.abspath, 'r').readlines()
                f = open(t.abspath, 'w')
                inserted = False
                for line in lines:
                    if not inserted and line.startswith('#include'):
                        print >> f, '#include <%s>' % env['PROTOCINCLUDE']
                        inserted = True
                    f.write(line)

ProtocActions = [SCons.Action.Action('$PROTOCCOM', '$PROTOCCOMSTR'),
                 SCons.Action.Action(_fix_protoc_includes,
                                     'Fixing up protoc includes')]

def ProtocEmitter(target, source, env):
    dirOfCallingSConscript = Dir('.').srcnode()
    env.Prepend(PROTOCPROTOPATH = dirOfCallingSConscript.path)

    source_with_corrected_path = []
    for src in source:
        source_with_corrected_path.append( src.srcnode().abspath )

    source = source_with_corrected_path

    for src in source:
        modulename = os.path.split(os.path.splitext(src)[0])[1]

        if env['PROTOCOUTDIR']:
            base = os.path.join(env['PROTOCOUTDIR'] , modulename)
            target.extend( [ base + '.pb.cc', base + '.pb.h' ] )

        if env['PROTOCPYTHONOUTDIR']:
            base = os.path.join(env['PROTOCPYTHONOUTDIR'] , modulename)
            target.append( base + '_pb2.py' )

    try:
        target.append(env['PROTOCFDSOUT'])
    except KeyError:
        pass

    #~ print "PROTOC SOURCE:", [str(s) for s in source]
    #~ print "PROTOC TARGET:", [str(s) for s in target]

    return target, source

ProtocBuilder = SCons.Builder.Builder(action = ProtocActions,
                                   emitter = ProtocEmitter,
                                   srcsuffix = '$PROTOCSRCSUFFIX')

def generate(env):
    """Add Builders and construction variables for protoc to an Environment."""
    try:
        bld = env['BUILDERS']['Protoc']
    except KeyError:
        bld = ProtocBuilder
        env['BUILDERS']['Protoc'] = bld

    env['PROTOC']        = env.Detect(protocs) or 'protoc'
    env['PROTOCFLAGS']   = SCons.Util.CLVar('')
    env['PROTOCPROTOPATH'] = SCons.Util.CLVar('')
    env['PROTOCCOM']     = '$PROTOC ${["-I%s"%x for x in PROTOCPROTOPATH]} $PROTOCFLAGS --cpp_out=$PROTOCCPPOUTFLAGS$PROTOCOUTDIR ${PROTOCPYTHONOUTDIR and ("--python_out="+PROTOCPYTHONOUTDIR) or ""} ${PROTOCFDSOUT and ("-o"+PROTOCFDSOUT) or ""} ${[x.abspath for x in SOURCES]}'
    env['PROTOCOUTDIR'] = '${SOURCE.dir}'
    env['PROTOCPYTHONOUTDIR'] = "python"
    env['PROTOCSRCSUFFIX']  = '.proto'
    env['PROTOCINCLUDE']  = ''

def exists(env):

    return env.Detect(protocs)