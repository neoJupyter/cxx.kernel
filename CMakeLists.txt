cmake_minimum_required(VERSION 3.11)

project(njp.cxx)

# uses xeus as a submodule as the one in vcpkg is super-outdated.
# fetches dependencies for xeus and compile by ourself instead
kls_vcpkg_package(xtl)
kls_vcpkg_package(cppzmq)
kls_vcpkg_package(openssl)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    kls_vcpkg_package(libuuid)
endif ()
kls_define_modules(xeus)
