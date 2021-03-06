# Autogenerated file, run tools/build/setup_cmake.py to regenerate

include_directories(%(includepath)s)
link_directories(%(libpath)s)

set(pytests %(pyexamples)s)

math(EXPR timeout "${IMP_TIMEOUT_FACTOR} * 180")

if (${IMP_MAX_CHECKS} MATCHES "INTERNAL")
set(testarg "--run_quick_test" "--deprecation_exceptions" "--log_level=2")
else()
set(testarg "--deprecation_exceptions")
endif()

foreach (test ${pytests})
 GET_FILENAME_COMPONENT(name ${test} NAME)
 add_test("IMP.%(name)s-${name}" ${IMP_TEST_SETUP} python ${test} ${testarg})
 set_tests_properties("IMP.%(name)s-${name}" PROPERTIES LABELS "IMP.%(name)s;example")
 set_tests_properties("IMP.%(name)s-${name}" PROPERTIES TIMEOUT ${timeout})
 set_tests_properties("IMP.%(name)s-${name}" PROPERTIES COST 3)
  #add_dependencies(${name} RMFPython)
endforeach(test)

set(cpp_tests %(cppexamples)s)

foreach (test ${cpp_tests})
   GET_FILENAME_COMPONENT(name ${test} NAME)
   GET_FILENAME_COMPONENT(name_we ${test} NAME_WE)
   add_executable("IMP.%(name)s-${name_we}" ${test})
   target_link_libraries("IMP.%(name)s-${name_we}"
    IMP.%(name)s-lib
    %(modules)s
    %(dependencies)s)
   set_target_properties("IMP.%(name)s-${name_we}" PROPERTIES
                         RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc/examples/%(name)s/"
                         OUTPUT_NAME "${name_we}")
  set_property(TARGET "IMP.%(name)s-${name_we}" PROPERTY FOLDER "IMP.%(name)s")

   add_test("IMP.%(name)s-${name_we}" ${IMP_TEST_SETUP}
            "${CMAKE_BINARY_DIR}/doc/examples/%(name)s/${name_we}${CMAKE_EXECUTABLE_SUFFIX}" ${testarg})
   set_tests_properties("IMP.%(name)s-${name_we}" PROPERTIES LABELS "IMP.%(name)s;example")
   set_tests_properties("IMP.%(name)s-${name_we}" PROPERTIES TIMEOUT ${timeout})
   set_tests_properties("IMP.%(name)s-${name_we}" PROPERTIES COST 3)
   set(executables ${executables} "IMP.%(name)s-${name_we}")
endforeach(test)

set(IMP_%(name)s_EXAMPLES ${executables} CACHE INTERNAL "" FORCE)
