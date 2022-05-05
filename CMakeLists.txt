cmake_minimum_required(VERSION 3.11)

project(njp.cxx)

# Compiler flags for silencing warnings
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
endif ()

# uses xeus as a submodule as the one in vcpkg is super-outdated.
# fetches dependencies for xeus and compile by ourself instead
kls_vcpkg_package(xtl)
kls_vcpkg_package(cppzmq)
kls_vcpkg_package(openssl)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    kls_vcpkg_package(libuuid)
endif ()
if (DEBUG)
    set(PYTHON_IS_DEBUG CACHE BOOL ON)
endif()
cmake_policy(SET CMP0077 NEW)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Python3 COMPONENTS Development Interpreter REQUIRED)
set(PYBIND11_FINDPYTHON CACHE BOOL ON)
set(PYBIND11_NOPYTHON CACHE BOOL ON)
set(XEUS_BUILD_SHARED_LIBS CACHE BOOL OFF)
kls_define_modules(xeus pybind11 pybind11_json)

kls_add_executable_module(njp.kernel)
kls_module_source_directory(njp.kernel Module)
kls_public_source_directory(njp.kernel Published)
target_link_libraries(njp.kernel PRIVATE xeus-static njp.common)
set_target_properties(njp.kernel PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
target_compile_definitions(njp.kernel PUBLIC "XEUS_PYTHON_EXPORTS")

find_package(xtl REQUIRED)
target_link_libraries(njp.kernel PUBLIC xeus-static xtl PRIVATE pybind11::pybind11 pybind11::embed pybind11_json)
if (WIN32 OR CYGWIN)
    #target_link_libraries(njp.kernel PRIVATE ${PYTHON_LIBRARIES})
elseif (APPLE)
    target_link_libraries(njp.kernel PRIVATE "-undefined dynamic_lookup")
endif ()
