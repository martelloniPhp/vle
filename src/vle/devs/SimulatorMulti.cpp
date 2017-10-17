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

#include <vle/devs/SimulatorMulti.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Time.hpp>
#include <vle/vpz/MultiComponent.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <iostream>
//#include <boost/bind.hpp>
//#include <boost/thread.hpp>
#include <thread>



namespace vle
{
namespace devs
{

SimulatorMulti::SimulatorMulti(vpz::MultiComponent *atomic)
    : m_multiComponent(atomic)
    , m_tn(negativeInfinity)
    , m_have_handle(false)
    , m_have_internal(false)
{
    assert(atomic && "Simulator: missing vpz::MultiComponent");

    m_multiComponent->m_simulator = this;
   
}

void SimulatorMulti::updateSimulatorTargets(const std::string &port)
{
    assert(m_multiComponent);

    mTargets.erase(port);

    vpz::ModelPortList result;
    m_multiComponent->getAtomicModelsTarget(port, result);

    if (result.begin() == result.end()) {
        mTargets.emplace(port, TargetSimulator(nullptr, std::string()));
        return;
    }

    for (auto &elem : result)
    {
		
                                         
		if(elem.first->isMulti()){
			//std::cout << "updateSimulatorTargets 6 " << std::endl;
        mTargets.emplace(port,
                         TargetSimulator(static_cast<vpz::MultiComponent *>(
                                             elem.first)->get_simulator(),
                                         elem.second));
									 }											 
									 
									 }
}

std::pair<SimulatorMulti::iterator, SimulatorMulti::iterator>
SimulatorMulti::targets(const std::string &port)
{
//	std::cout << "simMul->target 1"<< std::endl;
    auto x = mTargets.equal_range(port);
//std::cout << "simMul->target 2"<< std::endl;
    // If the updateSimulatorTargets function was never call, we update
    // the simulator targets and try to retrieve the newest simulator
    // targets.
    if (x.first == x.second) {
        updateSimulatorTargets(port);
        x = mTargets.equal_range(port);
    }
//std::cout << "simMul->target 3"<< std::endl;
    if (x.first->second.first == nullptr)
        return {mTargets.end(), mTargets.end()};
//std::cout << "simMul->target 4"<< std::endl;
    return x;
}

void SimulatorMulti::removeTargetPort(const std::string &port)
{
    auto it = mTargets.find(port);

    if (it != mTargets.end()) {
        mTargets.erase(it);
    }
}

void SimulatorMulti::addTargetPort(const std::string &port)
{
    assert(mTargets.find(port) == mTargets.end());

    mTargets.insert(value_type(
        port, TargetSimulator((SimulatorMulti *)nullptr, std::string())));
}

void SimulatorMulti::addDynamics(std::unique_ptr<Dynamics> /*dynamics*/)
{
   throw utils::InternalError(_("Simulator Multi wrong model"));
   //auto *dyn =  dynamic_cast<std::unique_ptr<DynamicsComp>>(std::move(dynamics));  
   //std::unique_ptr<DynamicsComp>  dyn = dynamics;
    //m_dynamics->push_back(std::move(dynamics));
}
void SimulatorMulti::addDynamics(std::unique_ptr<DynamicsComp> dynamics)
{
    
    
    m_dynamics.push_back(std::move(dynamics));
}
void SimulatorMulti::addDynamics(std::vector<std::unique_ptr<DynamicsComp>>* /* *dynamics*/)
{
    //m_dynamics = dynamics;
}

const std::string &SimulatorMulti::getName() const
{
    if (not m_multiComponent)
        throw utils::InternalError(_("Simulator destroyed"));

    return m_multiComponent->getName();
}

void SimulatorMulti::finish() 
{
	 //m_dynamics->finish(); 
	  for (auto &dyn : m_dynamics)
    {
		
		//if(dyn->getTn() == time){
		dyn->finish(); 
	//}
		
	}
}

void SimulatorMulti::output(Time time)
{
    assert(m_result.empty());
//std::cout  << time << " output simulator" << std::endl;
    //m_dynamics->output(time, m_result);
     for (auto &dyn : m_dynamics)
    {
		
		if(dyn->getTn() == time){
		dyn->output(time, m_result);
	}
		
	}
}

Time SimulatorMulti::timeAdvance()
{
    Time tn = infinity;//= m_dynamics->timeAdvance();
    Time temp = infinity;;
	for (auto &dyn : m_dynamics)
    {
		 /* if((dyn->timeAdvance()<dyn->getTn()))
				temp = dyn->timeAdvance();
		  else
			temp = dyn->getTn();*/
      //m_eventTable.init(temp);
      temp = dyn->timeAdvance();
     
      if(!(temp>tn))
      {
		  tn = temp;
	  }
    }

    if (tn < 0.0)
        throw utils::ModellingError(
            (fmt(_("Negative time advance in '%1%' (%2%)")) % getName() % tn)
                .str());
//std::cout  << " time advance: " << tn << std::endl;

    return tn;
    
}


void SimulatorMulti::initThread(Time time, int t,Time temp,Time *tn)
{
	int taille = m_dynamics.size()/4;
	for(int i = t*taille; i<(t*taille+taille);i++)
	//for(unsigned int i = t; i<=t*m_dynamics.size();i++)
	{
		 temp = m_dynamics[i]->init(time);
     
      //m_eventTable.init(temp);
     //dyn->setTn(dyn->init(time));
    // std::cout << "dynamique init " << dyn->getName() << "tn: " << temp << std::endl;
     m_dynamics[i]->setTn(temp);
      if(temp < *tn)
      {
		  *tn = temp;
	  }
	  
	 // std::cout<< i << std::endl;
	}
}

Time SimulatorMulti::init(Time time)
{
	
	std::cout << "debut init" << std::endl;
    Time tn1 = infinity;
    Time tn2 = infinity;
    Time tn3 = infinity;
    Time tn4 = infinity;//= m_dynamics.front()->init(time);
    Time temp;
    
   /* for (auto &dyn : m_dynamics)
    {
      temp = dyn->init(time);
     
      //m_eventTable.init(temp);
     //dyn->setTn(dyn->init(time));
    // std::cout << "dynamique init " << dyn->getName() << "tn: " << temp << std::endl;
     dyn->setTn(temp);
      if(temp < tn1)
      {
		  tn1 = temp;
	  }
	 // std::cout << "dynamique init " << dyn->getName() << "tn: " << dyn->getTn() << " - " << dyn->init(time)<< std::endl;
    }
*/

 std::thread t1(&SimulatorMulti::initThread,this,time,0,temp,&tn1);
 std::thread t2(&SimulatorMulti::initThread,this,time,1,temp,&tn2);
 std::thread t3(&SimulatorMulti::initThread,this,time,2,temp,&tn3);
 std::thread t4(&SimulatorMulti::initThread,this,time,3,temp,&tn4);
 
 t1.join();
 t2.join();
 t3.join();
 t4.join();
 
    if ((tn1 < 0.0)||(tn2 < 0.0)||(tn3 < 0.0)||(tn4 < 0.0))
        throw utils::ModellingError(
            (fmt(_("Negative init function in '%1%' (%2%)")) % getName() % tn1)
                .str());

   // m_tn = tn + time;
    m_tn = mintime(tn1,tn2,tn3,tn4)+time;
   // std::cout  << " time init " << m_tn << std::endl;
   std::cout << "fin init" << std::endl;
    return m_tn;
   // return 0;
}

Time SimulatorMulti::confluentTransitions(Time time)
{
    assert(not m_external_events.empty() and "Simulator d-conf error");
    assert(m_have_internal == true and "Simulator d-conf error");
   // m_dynamics->confluentTransitions(time, m_external_events);
    Time temp = infinity;
for (auto &dyn : m_dynamics)
    {
		if(dyn->getTn() == time)
		{
		dyn->confluentTransitions(time, m_external_events);		
		
		}else
		{
		//dyn->externalTransition(time, m_external_events);
		
		dyn->externalTransition(m_external_events, time);
		}
		dyn->setTn(dyn->timeAdvance()+time);
		if(temp>dyn->timeAdvance())
		{
			temp=dyn->timeAdvance();
		}
	}
    m_external_events.clear();
    m_have_internal = false;

   m_tn = timeAdvance() + time;
    return m_tn;
}



void SimulatorMulti::internalTransitionThread(Time time,int t,Time temp,Time *tn)
{
	int taille = m_dynamics.size()/4;
    
	//for(auto &dyn : m_dynamics)
	for(int i = t*taille; i<(t*taille+taille);i++)
	//for(unsigned int i = (3*m_dynamics.size()/4); i<m_dynamics.size();i++)
	{
		//if(m_dynamics[i]->getActivity())
		//{
			if(m_dynamics[i]->getTn() == time)
			{
				m_dynamics[i]->internalTransition(time);
				m_dynamics[i]->setTn(m_dynamics[i]->timeAdvance());
			}
			temp = m_dynamics[i]->getTn();
			if(temp < *tn)
			{
				*tn = temp;
			}
		//}
	}
	
}
Time SimulatorMulti::internalTransition(Time time)
{
	
	
    assert(m_have_internal == true and "Simulator d-int error");
    Time tn1 = infinity;
    Time tn2 = infinity;
    Time tn3 = infinity;
    Time tn4 = infinity;
    Time temp;
  /*      for (auto &dyn : m_dynamics)
    {
     if(dyn->getActivity())
		{
		if(dyn->getTn() == time)
		{
			dyn->internalTransition(time);
			dyn->setTn(dyn->timeAdvance());
		}
	temp = dyn->getTn();
		 if(temp < tn1)
      {
		  tn1 = temp;
	  }
	  }
	 
	}*/
	  //boost::thread t{SimulatorMulti::thread};
	 // boost::thread t( boost::bind( &SimulatorMulti::thread,this ) );
	  //std::thread t (SimulatorMulti::thread);
	  
	    std::thread t1(&SimulatorMulti::internalTransitionThread,this,time,0,temp,&tn1);
	    std::thread t2(&SimulatorMulti::internalTransitionThread,this,time,1,temp,&tn2);
	    std::thread t3(&SimulatorMulti::internalTransitionThread,this,time,2,temp,&tn3);
	    std::thread t4(&SimulatorMulti::internalTransitionThread,this,time,3,temp,&tn4);
	    t1.join();
	    t2.join();
	    t3.join();
	    t4.join();
	    
	//
	
	for (auto &dyn : m_dynamics)
    {
	dyn->majstate();
	}
	m_have_internal = false;    
    m_tn = mintime(tn1,tn2,tn3,tn4);
   
    
    return m_tn;
}

Time SimulatorMulti::externalTransition(Time time)
{
    assert(not m_external_events.empty() and "Simulator d-ext error");
     Time temp = infinity;
    
for (auto &dyn : m_dynamics)
    {
		
		dyn->externalTransition(m_external_events, time);
		dyn->setTn(dyn->timeAdvance()+time);
		if(temp>dyn->timeAdvance())
		{
			temp=dyn->timeAdvance();
		}
	}
    m_external_events.clear();

    m_tn = timeAdvance() + time;
    return m_tn;
}

std::unique_ptr<value::Value> SimulatorMulti::observation(const ObservationEvent& /*&event*/) const
{
    return nullptr; //m_dynamics->observation(event);
}
}
} // namespace vle devs
