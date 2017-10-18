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

#ifndef VLE_DEVS_SIMULATORATOMIC_HPP
#define VLE_DEVS_SIMULATORATOMIC_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/Scheduler.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/View.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/MultiComponent.hpp>


namespace vle {
namespace devs {

class Dynamics;
class Simulator;

/**
 * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
 * represent the DEVS simulator.
 *
 */
class VLE_LOCAL SimulatorAtomic : public Simulator
{
public:
     /**
     * @brief Build a new devs::Simulator with an empty devs::Dynamics, a
     * null last time but a vpz::AtomicModel node.
     * @param a The atomic model.
     * @throw utils::InternalError if the atomic model does not exist.
     */
    SimulatorAtomic(vpz::AtomicModel* a);

    /**
     * @brief Delete the attached devs::Dynamics user's model.
     */
    ~SimulatorAtomic() = default;

    /**
     * @brief Assign a new dynamics to the current Simulator. If a dynamic
     * already exists, it will be delete.
     * @param dynamics The new dynamics.
     */
    void addDynamics(std::unique_ptr<Dynamics> dynamics) override;
	void addDynamics(std::unique_ptr<DynamicsComp> dynamics) override;
    /**
     * @brief Get the name of the vpz::AtomicModel node.
     * @return the name of the vpz::AtomicModel.
     * @throw utils::InternalEvent if the model is destroyed.
     */
    const std::string& getName() const override;

    /**
     * @brief Get the atomic model attached to the Simulator.
     * @return A reference.
     */
    vpz::AtomicModel* getStructure() const 
    {
        return m_atomicModel;
    }
	vpz::MultiComponent* getStruc() const 
    {
        return nullptr;
    }

    /**
     * @brief Return a constant reference to the devs::Dynamics.
     * @return
     */
    const std::unique_ptr<Dynamics>& dynamics() const override
    {
        return m_dynamics;
    }

    /*-*-*-*-*-*-*-*-*-*/

    /**
     * Browse model's structure to find Simulator connected to the
     * specified output port.
     *
     * \param port The output port used to build simulators' target list.
     */
    void updateSimulatorTargets(const std::string& port) override;

    /**
     * Get begin and end iterators to find Simulator connected to the
     * specified output port.
     *
     * \param port The output port to get the simulators' target list.
     *
     * \return Two iterators.
     */
    std::pair<iterator, iterator> targets(const std::string& port) override;

    /**
     * @brief Add an empty target port.
     * @param port Name of the port.
     */
    void removeTargetPort(const std::string& port) override;

    /**
     * @brief Remove a target port.
     * @param port Name of the port to remove.
     */
    void addTargetPort(const std::string& port) override;

    /*-*-*-*-*-*-*-*-*-*/

    Time init(Time time) override;
    Time timeAdvance() override;
    void finish() override;
    void output(Time time) override;
    Time internalTransition(Time time) override;
    Time externalTransition(Time time) override;
    Time confluentTransitions(Time time) override;
    std::unique_ptr<value::Value> observation(
      const ObservationEvent& event) const override;

    inline const ExternalEventList& result() const noexcept override
    {
        return m_result;
    }

    inline void clear_result() noexcept override
    {
        m_result.clear();
    }

    inline Time getTn() const noexcept override
    {
        return m_tn;
    }

    inline HandleT handle() const noexcept override
    {
        assert(m_have_handle && "Simulator: handle is not defined");
        return m_handle;
    }

    inline bool haveHandle() const noexcept override
    {
        return m_have_handle;
    }

    inline void setHandle(HandleT handle) noexcept override
    {
        m_have_handle = true;
        m_handle = handle;
    }

    inline void resetHandle() noexcept override
    {
        m_have_handle = false;
    }

    inline bool haveExternalEvents() const noexcept override
    {
        return not m_external_events.empty();
    }

    inline void addExternalEvents(std::shared_ptr<value::Value> values,
                                  const std::string& portname) override
    {
        m_external_events.emplace_back(values, portname);
    }

    inline void setInternalEvent() noexcept override
    {
        m_have_internal = true;
    }

    inline bool haveInternalEvent() const noexcept override
    {
        return m_have_internal;
    }

    inline void resetInternalEvent() noexcept override
    {
        m_have_internal = false;
    }

    std::vector<Observation>& getObservations() noexcept override
    {
        return m_observations;
    }

private:
    std::unique_ptr<Dynamics> m_dynamics;
    vpz::AtomicModel* m_atomicModel;
    TargetSimulatorList mTargets;
    ExternalEventList m_external_events;
    ExternalEventList m_result;
    std::vector<Observation> m_observations;
    std::string m_parents;
    Time m_tn;
    HandleT m_handle;
    bool m_have_handle;
    bool m_have_internal;
};
}
} // namespace vle devs

#endif
