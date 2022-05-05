/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XPYT_STREAM_HPP
#define XPYT_STREAM_HPP

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace xpyt
{
    py::module get_stream_module();
}

#endif
