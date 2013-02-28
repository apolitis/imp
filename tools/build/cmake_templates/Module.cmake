# Autogenerated file, run tools/build/setup_cmake.py to regenerate

%(tests)s

imp_get_process_exit_code("setup_module %(name)s" status ${PROJECT_BINARY_DIR}
                          COMMAND ${PROJECT_SOURCE_DIR}/tools/build/setup_module.py
                          --name=%(name)s
                          --datapath=${IMP_DATAPATH}
                          --defines=${IMP_%(NAME)s_CONFIG}:%(defines)s
                           --source=${PROJECT_SOURCE_DIR})
if(NOT ${status})
message("Module IMP.%(name)s ok")
imp_execute_process("setup_swig_wrappers %(name)s" ${PROJECT_BINARY_DIR}
                    COMMAND ${PROJECT_SOURCE_DIR}/tools/build/setup_swig_wrappers.py
                          --module=%(name)s
                          --datapath=${IMP_DATAPATH}
                           --source=${PROJECT_SOURCE_DIR})
imp_execute_process("get_python_tests %(name)s" ${PROJECT_BINARY_DIR}
                    COMMAND ${PROJECT_SOURCE_DIR}/tools/build/get_python_tests.py
                          --module=%(name)s
                          ${PROJECT_SOURCE_DIR})

%(subdirs)s
else()
message("Module IMP.%(name)s disabled")
set(IMP_%(NAME)s_LIBRARY CACHE INTERNAL "" FORCE)
endif()
