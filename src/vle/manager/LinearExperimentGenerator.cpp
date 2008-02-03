/**
 * @file vle/manager/LinearExperimentGenerator.cpp
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


#include <vle/manager/LinearExperimentGenerator.hpp>
#include <vle/value/Set.hpp>


namespace vle { namespace manager {

void LinearExperimentGenerator::buildCombination(size_t& nb)
{
    for(size_t i=0; i < mCondition.size(); ++i)
	mCondition[i].pos++;
    nb++;
}

size_t LinearExperimentGenerator::getCombinationNumber() const
{
    Assert(utils::InternalError, not mCondition.empty(),
           "Build a linear experimental frame with empty value?");

    const size_t nb = mCondition[0].sz;
    size_t cndnb = 0;

    std::vector < cond_t >::const_iterator it;
    for (it = mCondition.begin(); it != mCondition.end(); ++it) {
        Assert(utils::InternalError, nb == it->sz,
               boost::format("Build a linear experimental frame with bad"
                             " number value for condition %1%") % cndnb);
        cndnb++;
    }

    return nb;
}

}} // namespace vle manager
