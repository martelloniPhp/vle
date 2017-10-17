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

#ifndef VLE_DEVS_SIMULATOR_HPP
#define VLE_DEVS_SIMULATOR_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/Scheduler.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsComp.hpp>
#include <vle/devs/View.hpp>
#include <vle/vpz/AtomicModel.hpp>


namespace vle
{
namespace devs
{

class Dynamics;


class VLE_LOCAL Simulator 
{
public:
    typedef std::pair<Simulator *, std::string> TargetSimulator;
    typedef std::multimap<std::string, TargetSimulator> TargetSimulatorList;
    typedef TargetSimulatorList::const_iterator const_iterator;
    typedef TargetSimulatorList::iterator iterator;
    typedef TargetSimulatorList::size_type size_type;
    typedef TargetSimulatorList::value_type value_type;

    
   // Simulator(vpz::AtomicModel *a);

    
     virtual ~Simulator() = default;

  
     virtual void addDynamics(std::unique_ptr<Dynamics> dynamics) =0;
	 virtual void addDynamics(std::unique_ptr<DynamicsComp> dynamics) =0;
   
     virtual const std::string &getName()const =0;

   
     virtual const vpz::AtomicModel  *getStructure() const =0; 
     
     virtual const vpz::MultiComponent *getStruc() const =0;
     
     
     virtual const std::unique_ptr<Dynamics> &dynamics() const=0;

   
     virtual void updateSimulatorTargets(const std::string &port)=0;

    
     virtual std::pair<iterator, iterator> targets(const std::string &port) =0;

   
     virtual void removeTargetPort(const std::string &port)=0;

    
     virtual void addTargetPort(const std::string &port)=0;

 

     virtual Time init(Time time)=0;
     virtual Time timeAdvance()=0;
     virtual void finish()=0;
     virtual void output(Time time)=0;
     virtual Time internalTransition(Time time)=0;
     virtual Time externalTransition(Time time)=0;
     virtual Time confluentTransitions(Time time)=0;
     virtual  std::unique_ptr<value::Value> observation(const ObservationEvent &event) const =0;

     virtual const ExternalEventList &result() const noexcept =0;
    

     virtual void clear_result() =0;
    

     virtual Time getTn() const =0;
   
     virtual HandleT handle() const =0;
   
      virtual bool haveHandle() const =0; 


     virtual void setHandle(HandleT handle) =0;

     virtual void resetHandle()=0; 
    

     virtual bool haveExternalEvents() const =0;
   
     virtual void addExternalEvents(std::shared_ptr<value::Value> values,
                                  const std::string &portname)=0;
    

     virtual void setInternalEvent()=0;
   

     virtual bool haveInternalEvent()const noexcept =0;

     virtual void resetInternalEvent()=0;

     virtual std::vector<Observation> &getObservations()=0;
     
      virtual bool isAtomic() const
        { return false; }
		virtual bool isMulti() const
        { 
			return false; }

private:
    /*std::unique_ptr<Dynamics> m_dynamics;
    vpz::AtomicModel *m_atomicModel;
    TargetSimulatorList mTargets;
    ExternalEventList m_external_events;
    ExternalEventList m_result;
    std::vector<Observation> m_observations;
    std::string m_parents;
    Time m_tn;
    HandleT m_handle;
    bool m_have_handle;
    bool m_have_internal;*/
};
}
} // namespace vle devs

#endif
