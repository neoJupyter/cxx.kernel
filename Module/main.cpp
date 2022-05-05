/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <signal.h>

#ifdef __GNUC__
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_shell_main.hpp"
#include "xeus/xinterpreter.hpp"

#include "pybind11/embed.h"
#include "pybind11/pybind11.h"

#include "xeus-python/xinterpreter.hpp"
#include "xeus-python/xdebugger.hpp"
#include "xeus-python/xpaths.hpp"
#include "xeus-python/xeus_python_config.hpp"
#include "xeus-python/xutils.hpp"

namespace py = pybind11;

void register_signal_handlers() {
#ifdef __GNUC__
    std::clog << "registering handler for SIGSEGV" << std::endl;
        signal(SIGSEGV, xpyt::sigsegv_handler);

        // Registering SIGINT and SIGKILL handlers
        signal(SIGKILL, xpyt::sigkill_handler);
#endif
    signal(SIGINT, xpyt::sigkill_handler);
}

static void set_python_argv(int argc, char *const *argv) {
    auto **argw = new wchar_t *[size_t(argc)];
    for (auto i = 0; i < argc; ++i) argw[i] = Py_DecodeLocale(argv[i], nullptr);
    PySys_SetArgvEx(argc, argw, 0);
    for (auto i = 0; i < argc; ++i) PyMem_RawFree(argw[i]);
    delete[] argw;
}

static void print_directives(const xeus::xconfiguration &config) {
    nl::json directives{};
    directives["ip"] = config.m_ip;
    directives["transport"] = config.m_transport;
    directives["control_port"] = config.m_control_port;
    directives["shell_port"] = config.m_shell_port;
    directives["stdin_port"] = config.m_stdin_port;
    directives["iopub_port"] = config.m_iopub_port;
    directives["hb_port"] = config.m_hb_port;
    directives["key"] = config.m_key;
    directives["signature_scheme"] = config.m_signature_scheme;
    std::cout << directives.dump() << std::endl;
}

std::string set_program_name_python() {
    std::string executable(xpyt::get_python_path());
    std::wstring executable_wide(executable.cbegin(), executable.cend());
    // On windows, sys.executable is not properly set with Py_SetProgramName
    // Cf. https://bugs.python.org/issue34725
    // A private undocumented API was added as a workaround in Python 3.7.2.
    // _Py_SetProgramFullPath(const_cast<wchar_t*>(executable_wide.c_str()));
    Py_SetProgramName(const_cast<wchar_t *>(executable_wide.c_str()));
    return executable;
}

int main(int argc, char *argv[]) {
    register_signal_handlers();
    const auto executable = set_program_name_python();
    // Instantiating the Python interpreter
    py::scoped_interpreter guard;
    set_python_argv(argc, argv);
    using context_type = xeus::xcontext_impl<zmq::context_t>;
    std::unique_ptr<context_type> context = std::make_unique<context_type>();
    // Instantiating the xeus xinterpreter
    auto interpreter = std::make_unique<xpyt::interpreter>();
    auto history = xeus::make_in_memory_history_manager();
    nl::json debugger_config;
    debugger_config["python"] = executable;
    xeus::xkernel kernel(
            xeus::xconfiguration{.m_ip = "0.0.0.0"}, xeus::get_user_name(), std::move(context), std::move(interpreter),
            xeus::make_xserver_shell_main, std::move(history), nullptr, xpyt::make_python_debugger, debugger_config
    );
    print_directives(kernel.get_config());
    kernel.start();
    return 0;
}