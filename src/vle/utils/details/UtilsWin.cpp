/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <memory>
#include <vle/utils/details/UtilsWin.hpp>
#include <windows.h>

namespace vle {
namespace utils {

Path
UtilsWin::convertPathTo83Path(const Path& path)
{
    Path ret = path;
    std::string strpath(path.string());
    DWORD lenght;

    lenght = GetShortPathName(strpath.c_str(), NULL, 0);
    if (lenght > 0) {
        auto p = std::make_unique<TCHAR[]>(lenght);
        lenght = GetShortPathName(strpath.c_str(), p.get(), lenght);
        if (lenght > 0)
            ret = p.get();
    }

    return ret;
}
}
} // namespace vle utils
