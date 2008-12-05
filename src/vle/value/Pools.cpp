/**
 * @file vle/value/Pools.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/value/Pools.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Matrix.hpp>

namespace vle { namespace value {


#ifdef VLE_HAVE_POOL
Pools* Pools::m_pool = 0;

Pools::Pools() :
    m_pools(
        std::max(sizeof(Boolean), std::max(sizeof(Integer),
        std::max(sizeof(Map), std::max(sizeof(Matrix),
        std::max(sizeof(Null), std::max(sizeof(Set),
        std::max(sizeof(String), std::max(sizeof(Table),
        std::max(sizeof(Tuple), sizeof(Xml)))))))))) + 1)
{
}

Pools::Pools(const Pools& /* other */) :
    m_pools(
        std::max(sizeof(Boolean), std::max(sizeof(Integer),
        std::max(sizeof(Map), std::max(sizeof(Matrix),
        std::max(sizeof(Null), std::max(sizeof(Set),
        std::max(sizeof(String), std::max(sizeof(Table),
        std::max(sizeof(Tuple), sizeof(Xml)))))))))) + 1)
{
}
#endif

}} // namespace vle value

