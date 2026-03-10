# CMake generated Testfile for 
# Source directory: C:/Users/lenovo/Desktop/Supermarket
# Build directory: C:/Users/lenovo/Desktop/Supermarket/build_qml
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[ModelTests]=] "C:/Users/lenovo/Desktop/Supermarket/build_qml/bin/Debug/supermarket_tests.exe")
  set_tests_properties([=[ModelTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;199;add_test;C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[ModelTests]=] "C:/Users/lenovo/Desktop/Supermarket/build_qml/bin/Release/supermarket_tests.exe")
  set_tests_properties([=[ModelTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;199;add_test;C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[ModelTests]=] "C:/Users/lenovo/Desktop/Supermarket/build_qml/bin/MinSizeRel/supermarket_tests.exe")
  set_tests_properties([=[ModelTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;199;add_test;C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[ModelTests]=] "C:/Users/lenovo/Desktop/Supermarket/build_qml/bin/RelWithDebInfo/supermarket_tests.exe")
  set_tests_properties([=[ModelTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;199;add_test;C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;0;")
else()
  add_test([=[ModelTests]=] NOT_AVAILABLE)
endif()
subdirs("_deps/nlohmann_json_header-build")
subdirs("_deps/sqlite3-build")
subdirs("_deps/crow-build")
