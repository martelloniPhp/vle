
/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <vle/devs/SimulatorAtomic.hpp>

#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <iostream>
namespace vle
{
namespace devs
{

SimulatorAtomic::SimulatorAtomic(vpz::AtomicModel *atomic)
    : m_atomicModel(atomic)
    , m_tn(negativeInfinity)
    , m_have_handle(false)
    , m_have_internal(false)
{
    assert(atomic && "Simulator: missing vpz::AtomicMOdel");

     m_atomicModel->m_simulator = this;
     //std::cout << "simulateur atomic créé"<< std::endl;
}

void SimulatorAtomic::updateSimulatorTargets(const std::string &port)
{
	//std::cout << "updateSimulatorTargets 1"<< std::endl;
    assert(m_atomicModel);
//std::cout << "updateSimulatorTargets 2"<< std::endl;
    mTargets.erase(port);
//std::cout << "updateSimulatorTargets 3"<< std::endl;
    vpz::ModelPortList result;
    m_atomicModel->getAtomicModelsTarget(port, result);
//std::cout << "updateSimulatorTargets 4"<< std::endl;
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
                                         elem.second)); }	
   else if(elem.first->isMulti()){
mTargets.emplace(port,
                         TargetSimulator(static_cast<vpz::MultiComponent *>(
                                             elem.first)->get_simulator(),
                                         elem.second)); }                                    						 
									 }
         //std::cout << "Atomic updateSimulatorTargets 7"<< std::endl;
}

std::pair<Simulator::iterator, Simulator::iterator>
SimulatorAtomic::targets(const std::string &port)
{ 
	//std::cout << "simAt->target 1"<< std::endl;
    auto x = mTargets.equal_range(port);
//std::cout << "sim->target 2"<< std::endl;
    // If the updateSimulatorTargets function was never call, we update
    // the simulator targets and try to retrieve the newest simulator
    // targets.
    if (x.first == x.second) {
        updateSimulatorTargets(port);
        x = mTargets.equal_range(port);
    }
//std::cout << "simAT->target 2"<< std::endl;
    if (x.first->second.first == nullptr)
        return {mTargets.end(), mTargets.end()};
//std::cout << "simAT->target 3"<< std::endl;
    return x;
}

void SimulatorAtomic::removeTargetPort(const std::string &port)
{
    auto it = mTargets.find(port);

    if (it != mTargets.end()) {
        mTargets.erase(it);
    }
}

void SimulatorAtomic::addTargetPort(const std::string &port)
{
    assert(mTargets.find(port) == mTargets.end());

    mTargets.insert(value_type(
        port, TargetSimulator((Simulator *)nullptr, std::string())));
}

void SimulatorAtomic::addDynamics(std::unique_ptr<Dynamics> dynamics) 
{
    m_dynamics = std::unique_ptr<Dynamics>(std::move(dynamics));
  //   std::cout << "add dynamics atomic" << std::endl;
}

void SimulatorAtomic::addDynamics(std::unique_ptr<DynamicsComp> /*dynamics*/)
{
   throw utils::InternalError(_("Simulator Atomic wrong model"));

}

const std::string &SimulatorAtomic::getName() const 
{
    if (not m_atomicModel)
        throw utils::InternalError(_("Simulator destroyed"));

    return m_atomicModel->getName();
}

void SimulatorAtomic::finish() { m_dynamics->finish(); }

void SimulatorAtomic::output(Time time)
{
    assert(m_result.empty());

    m_dynamics->output(time, m_result);
}

Time SimulatorAtomic::timeAdvance()
{
    Time tn = m_dynamics->timeAdvance();

    if (tn < 0.0)
        throw utils::ModellingError(
            (fmt(_("Negative time advance in '%1%' (%2%)")) % getName() % tn)
                .str());

    return tn;
}

Time SimulatorAtomic::init(Time time)
{
    Time tn = m_dynamics->init(time);

    if (tn < 0.0)
        throw utils::ModellingError(
            (fmt(_("Negative init function in '%1%' (%2%)")) % getName() % tn)
                .str());

    m_tn = tn + time;
    return m_tn;
}

Time SimulatorAtomic::confluentTransitions(Time time)
{
    assert(not m_external_events.empty() and "Simulator d-conf error");
    assert(m_have_internal == true and "Simulator d-conf error");
    m_dynamics->confluentTransitions(time, m_external_events);

    m_external_events.clear();
    m_have_internal = false;

    m_tn = timeAdvance() + time;
    return m_tn;
}

Time SimulatorAtomic::internalTransition(Time time)
{
    assert(m_have_internal == true and "Simulator d-int error");
    m_dynamics->internalTransition(time);

    m_have_internal = false;

    m_tn = timeAdvance() + time;
    return m_tn;
}

Time SimulatorAtomic::externalTransition(Time time)
{
    assert(not m_external_events.empty() and "Simulator d-ext error");
    m_dynamics->externalTransition(m_external_events, time);

    m_external_events.clear();

    m_tn = timeAdvance() + time;
    return m_tn;
}

std::unique_ptr<value::Value>
SimulatorAtomic::observation(const ObservationEvent &event) const
{
    return m_dynamics->observation(event);
}
}
} // namespace vle devs
