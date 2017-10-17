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

#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/AtomicModel.hpp>

namespace vle {
namespace devs {


void Simulator::updateSimulatorTargets(const std::string& port)
{
    assert(m_atomicModel);

    mTargets.erase(port);

    vpz::ModelPortList result;
    m_atomicModel->getAtomicModelsTarget(port, result);

    if (result.begin() == result.end()) {
        mTargets.emplace(port, TargetSimulator(nullptr, std::string()));
        return;
    }

    for (auto &elem : result)
    {
		//std::cout << "updateSimulatorTargets 5 "<< elem.first->isAtomic() << std::endl;
		if(elem.first->isAtomic()){
        mTargets.emplace(port,
                         TargetSimulator(static_cast<vpz::AtomicModel *>(
                                             elem.first)->get_simulator(),
                                         elem.second));
									 }
		if(elem.first->isMulti()){
		//	std::cout << "updateSimulatorTargets 6 " << std::endl;
        mTargets.emplace(port,
                         TargetSimulator((static_cast<vpz::MultiComponent *>(
                                             elem.first)) ->get_simulator(),
                                         elem.second));
									 }
	}
        
}



}
} // namespace vle devs
