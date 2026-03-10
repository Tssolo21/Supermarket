# CMake generated Testfile for 
# Source directory: C:/Users/lenovo/Desktop/Supermarket
# Build directory: C:/Users/lenovo/Desktop/Supermarket/build_mingw_qt
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[ModelTests]=] "C:/Users/lenovo/Desktop/Supermarket/build_mingw_qt/bin/supermarket_tests.exe")
set_tests_properties([=[ModelTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;199;add_test;C:/Users/lenovo/Desktop/Supermarket/CMakeLists.txt;0;")
subdirs("_deps/nlohmann_json_header-build")
subdirs("_deps/sqlite3-build")
subdirs("_deps/crow-build")
