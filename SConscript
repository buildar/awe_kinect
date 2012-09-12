import os.path

Import("*")
env = Environment()
if MODE == 'debug':
    env.Append(CPPFLAGS='-g')
env.Append(LIBS=['OpenNI','websockets','glog'])
env.Append(CPPPATH='-I/usr/include/ni')
sources = Split("""
constants.cpp
dump_writer.cpp
options.cpp
util.cpp
position.cpp
skeleton.cpp
server.cpp
awe_kinect.cpp
""")

sources = [os.path.join('src', p) for p in sources]

object_list = env.Object(source=sources)
if env['PLATFORM'] == 'posix':
    env.Append(CPPDEFINES=['PLATFORM_POSIX'])
elif env['PLATFORM'] == 'win32':
    env.Append(CPPDEFINES=['PLATFORM_WINDOWS'])

env.Program(target='awe_kinect', source=object_list)
