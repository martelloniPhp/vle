/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
<<<<<<< HEAD
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
=======
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
>>>>>>> 7a80e7dfda3b977bb00912260315670db65be4f1
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

#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/devs/DynamicsComp.hpp>
#include <vle/devs/DynamicsInit.hpp>
#include <vle/devs/DynamicsObserver.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/MultiComponent.hpp>
#include <vle/vpz/Component.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <iostream>

namespace vle {
namespace devs {

ModelFactory::ModelFactory(utils::ContextPtr context,
                           std::map<std::string, View>& eventviews,
                           const vpz::Dynamics& dyn,
                           const vpz::Classes& cls,
                           const vpz::Experiment& exp)
  : mContext(context)
  , mEventViews(eventviews)
  , mDynamics(dyn)
  , mClasses(cls)
  , mExperiment(exp)
{
}

void
ModelFactory::createModel(Coordinator& coordinator,
                          const vpz::Conditions& experiment_conditions,
                          vpz::AtomicModel* model,
                          const std::string& dynamics,
                          const std::vector<std::string>& conditions,
                          const std::string& observable)
{
    const vpz::Dynamic& dyn = mDynamics.get(dynamics);
    auto sim = coordinator.addAtomicModel(model);

    InitEventList initValues;

    if (not conditions.empty()) {
        for (const auto& elem : conditions) {
            const auto& cnd = experiment_conditions.get(elem);
            auto vl = cnd.fillWithFirstValues();

            for (auto& elem : vl) {
                if (initValues.exist(elem.first))
                    throw utils::InternalError(
                      (fmt(_("Multiples condition with the same init port "
                             "name '%1%'")) %
                       elem.first)
                        .str());


                initValues.add(elem.first, elem.second);
            }
        }
    }

    sim->addDynamics(
      attachDynamics(coordinator, sim, dyn, initValues, observable));

    if (not observable.empty()) {
        vpz::Observable& ob(mExperiment.views().observables().get(observable));
        const vpz::ObservablePortList& lst(ob.observableportlist());

        for (const auto& elem : lst) {
            const vpz::ViewNameList& vnlst(elem.second.viewnamelist());
            for (const auto& viewname : vnlst)
                coordinator.addObservableToView(model, elem.first, viewname);
        }
    }

    coordinator.processInit(sim);
}

void ModelFactory::createMulticomponent(Coordinator &coordinator,
                               const vpz::Conditions &experiment_conditions,
                               vpz::MultiComponent *model,
                               //const std::string &dynamics,
                               const std::vector<std::string> &conditions,
                               const std::string &observable)
{
    

    auto sim = coordinator.addMulticomponent(model);

    InitEventList initValues;

    if (not conditions.empty()) {
        for (const auto &elem : conditions) {
            const auto &cnd = experiment_conditions.get(elem);
            auto vl = cnd.fillWithFirstValues();

            for (auto &elem : vl) {
                if (initValues.exist(elem.first))
                    throw utils::InternalError(
                        (fmt(_("Multiples condition with the same init port "
                               "name '%1%'")) %
                         elem.first)
                            .str());

                initValues.add(elem.first, elem.second);
            }
        }
    }
 if( model->dynamics().size() >0){ 
for(auto &elem : model->dynamics()){
	const vpz::Dynamic &dyn = mDynamics.get(elem);
 sim->addDynamics(
        attacheDynamics(coordinator, sim, dyn, initValues, observable));        
	}

	//std::cout << "attach dynamics nb" << sim->dynamics() << std::endl;

    /*if (not observable.empty()) {
        vpz::Observable &ob(mExperiment.views().observables().get(observable));
        const vpz::ObservablePortList &lst(ob.observableportlist());

        for (const auto &elem : lst) {
            const vpz::ViewNameList &vnlst(elem.second.viewnamelist());
            for (const auto &viewname : vnlst)
                coordinator.addObservableToView(model, elem.first, viewname);
        }
    }*/
    
   

    
    }
    coordinator.processInit(sim);
}





void ModelFactory::createModels(Coordinator &coordinator,
                                const vpz::Model &model)
{
    vpz::AtomicModelVector atomicmodellist;
    vpz::ComponentVector componentlist;
    vpz::MulticomponentVector multicomponentlist;
    vpz::BaseModel *mdl = model.node();
	//std::cout << " create models"<< std::endl;
    if (mdl) {
        if (mdl->isMulti()) {			
            multicomponentlist.push_back((vpz::MultiComponent *)mdl);
            
            }
        else if(mdl->isAtomic()){
			
			atomicmodellist.push_back((vpz::AtomicModel *)mdl);
		}
		 else if(mdl->isComponent()){
			
			componentlist.push_back((vpz::Component *)mdl);
		}        
        else {
			//std::cout << " is coupled"<< std::endl;
            vpz::BaseModel::getModelList(mdl, atomicmodellist, multicomponentlist, componentlist);
            
        }
 //std::cout << "atomicModelList: "<< atomicmodellist.size()<< "  multicomponentlist: "<< multicomponentlist.size() << "  Componentlist: "<< componentlist.size()<< std::endl;
        
         for (auto &elem : multicomponentlist) {
            createMulticomponent(coordinator,
                        mExperiment.conditions(),
                        elem,
                        //(elem)->dynamics(),
                        (elem)->conditions(),
                        (elem)->observables());
                        
        
      
		
        }
        
        for (auto &elem : atomicmodellist) {

            createModel(coordinator,
                        mExperiment.conditions(),
                        elem,
                        (elem)->dynamics(),
                        (elem)->conditions(),
                        (elem)->observables());

        }
    }
}

vpz::BaseModel*
ModelFactory::createModelFromClass(Coordinator& coordinator,
                                   vpz::CoupledModel* parent,
                                   const std::string& classname,
                                   const std::string& modelname,
                                   const vpz::Conditions& conditions)
{
    vpz::Class& classe(mClasses.get(classname));
    vpz::BaseModel* mdl(classe.node()->clone());
    vpz::AtomicModelVector atomicmodellist;
    vpz::BaseModel::getAtomicModelList(mdl, atomicmodellist);
    parent->addModel(mdl, modelname);

    for (auto& elem : atomicmodellist) {
        createModel(coordinator,
                    conditions,
                    elem,
                    (elem)->dynamics(),
                    (elem)->conditions(),
                    (elem)->observables());
    }

    return mdl;
}

std::unique_ptr<Dynamics> buildNewDynamicsWrapper(utils::ContextPtr context,
												devs::Simulator* atom,
												const vpz::Dynamic& dyn,
												const InitEventList& events,
												void* symbol)
{
    typedef Dynamics* (*fctdw)(const DynamicsWrapperInit&,
                               const InitEventList&);


    fctdw fct = utils::functionCast<fctdw>(symbol);

    try {
        utils::PackageTable pkg_table;

        return std::unique_ptr<Dynamics>(

          fct(DynamicsWrapperInit{ dyn.library(),
                                   context,
                                   *atom->getStructure(),
                                   pkg_table.get(dyn.package()) },
              events));
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          (fmt(_("Atomic model wrapper `%1%:%2%' (from dynamics `%3%'"
                 " library `%4%' package `%5%') throws error in"
                 " constructor: `%6%'")) %
           atom->getStructure()->getParentName() %
           atom->getStructure()->getName() % dyn.name() % dyn.library() %
           dyn.package() % e.what())
            .str());
    }
}

//
// Compute the dynamics observer options from the observable. If observable is
// empty or equal to Tined, the model can be observed with a TimedView
// otherwise, we compute which functions to observe.
//

bool haveEventView(const vpz::Views& views, const std::string& observable)
{
    if (observable.empty())
        return false;

    const auto& obs(views.observables().get(observable));
    const auto& lst(obs.observableportlist());

    for (const auto& elem : lst) {
        const auto& viewnamelist(elem.second.viewnamelist());
        for (const auto& viewname : viewnamelist) {
            const auto& view = views.get(viewname);
            if (view.is_enable() and not(view.type() & vpz::View::TIMED))
                return true;
        }
    }

    return false;
}
void
assignEventView(std::map<std::string, View>& views,
                const vpz::Views& vpzviews,
                const std::string& observable,
                const std::unique_ptr<DynamicsObserver>& dynamics)
{
    const auto& obs(vpzviews.observables().get(observable));
    const auto& lst(obs.observableportlist());

    for (const auto& elem : lst) {
        const auto& viewnamelist(elem.second.viewnamelist());
        for (const auto& viewname : viewnamelist) {
			 const auto& v = vpzviews.get(viewname);
			 
            if (v.type() == vpz::View::TIMED)
                continue;

            auto it = views.find(viewname);
            if (it == views.end())
                continue;

            auto& vv = it->second;
            if (v.type() & vpz::View::OUTPUT)
                dynamics->ppOutput.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::INTERNAL)
                dynamics->ppInternal.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::EXTERNAL)
                dynamics->ppExternal.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::CONFLUENT)
                dynamics->ppConfluent.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::FINISH)
                dynamics->ppFinish.emplace_back(&vv, elem.first);
        }
    }
}

std::unique_ptr<Dynamics> buildNewDynamics(utils::ContextPtr context,
                                           std::map<std::string, View> &views,
                                           const vpz::Views &vpzviews,
                                           const std::string &observable,
                                           devs::Simulator *atom,
                                           const vpz::Dynamic &dyn,
                                           const InitEventList &events,
                                           void *symbol)
{
    typedef Dynamics *(*fctdyn)(const DynamicsInit &, const InitEventList &);

    fctdyn fct = utils::functionCast<fctdyn>(symbol);
      if(symbol == nullptr){
		//	std::cout << "symbol null" << std::endl;
		}else{
			//std::cout << "symbol non null" << std::endl;
		}
	
    try {
        utils::PackageTable pkg_table;

        DynamicsInit init{
            context, *atom->getStructure(), pkg_table.get(dyn.package())};
           
        auto dynamics = std::unique_ptr<Dynamics>(fct(init, events));

        if (haveEventView(vpzviews, observable)) {
            auto observation = std::make_unique<DynamicsObserver>(
              init, events, atom->getObservations());

            if (atom->getStructure()->needDebug()) {
                auto debug = std::make_unique<DynamicsDbg>(init, events);
                debug->set(std::move(dynamics));
                observation->set(std::move(debug));
            } else {
                observation->set(std::move(dynamics));
            }

            assignEventView(views, vpzviews, observable, observation);

            return std::move(observation);
        }

       if (atom->getStructure()->needDebug()) {
            auto debug = std::make_unique<DynamicsDbg>(init, events);
            debug->set(std::move(dynamics));
            return std::move(debug);
        }
        else {
            return dynamics;
        }
    }
    catch (const std::exception &e) {
        throw utils::ModellingError(
            (fmt(_("Atomic model `%1%:%2%' (from dynamics `%3%' library"
                   " `%4%' package `%5%') throws error in constructor:"
                   " `%6%'")) %
             atom->getStructure()->getParentName() %
             atom->getStructure()->getName() % dyn.name() % dyn.library() %
             dyn.package() % e.what())
                .str());
    }
}


std::unique_ptr<DynamicsComp> buildDynamics(utils::ContextPtr context,
                                          // std::map<std::string, View> &views,
                                          // const vpz::Views &vpzviews,
                                          // const std::string &observable,
                                           devs::Simulator *atom,
                                           const vpz::Dynamic &dyn,
                                           const InitEventList &events,
                                           void *symbol)
{
    typedef DynamicsComp *(*fctdyn)(const DynamicsCompInit &, const InitEventList &);
   

    fctdyn fct = utils::functionCast<fctdyn>(symbol);
 

     
    
    try {
        utils::PackageTable pkg_table;
const InitEventList ev;
        const DynamicsCompInit& init{
            context, *atom->getStruc(), pkg_table.get(dyn.package())};
            
  
           
      
       auto dynamics = std::unique_ptr<DynamicsComp>(fct(init, events));
      
            return dynamics;
        
    }
    catch (const std::exception &e) {
        throw utils::ModellingError(
            (fmt(_("MultiComponent model `%1%:%2%' (from dynamics `%3%' library"
                   " `%4%' package `%5%') throws error in constructor:"
                   " `%6%'")) %
             atom->getStructure()->getParentName() %
             atom->getStructure()->getName() % dyn.name() % dyn.library() %
             dyn.package() % e.what())
                .str());
    }
}


std::unique_ptr<Dynamics> buildNewExecutive(utils::ContextPtr context,
                                            std::map<std::string, View> &views,
                                            const vpz::Views &vpzviews,
                                            const std::string &observable,
                                            Coordinator &coordinator,
                                            devs::Simulator *atom,
                                            const vpz::Dynamic &dyn,
                                            const InitEventList &events,
                                            void *symbol)
{
    typedef Dynamics *(*fctexe)(const ExecutiveInit &, const InitEventList &);

    fctexe fct = utils::functionCast<fctexe>(symbol);

    try {
        utils::PackageTable pkg_table;


        ExecutiveInit executiveinit{ coordinator,
                                     context,
                                     *atom->getStructure(),
                                     pkg_table.get(dyn.package()) };

        DynamicsInit init{ context,
                           *atom->getStructure(),
                           pkg_table.get(dyn.package()) };

        auto executive = std::unique_ptr<Dynamics>(fct(executiveinit, events));

        if (haveEventView(vpzviews, observable)) {
            auto observation = std::make_unique<DynamicsObserver>(
              init, events, atom->getObservations());

            if (atom->getStructure()->needDebug()) {
                auto debug = std::make_unique<DynamicsDbg>(init, events);
                debug->set(std::move(executive));
                observation->set(std::move(debug));
            } else {
                observation->set(std::move(executive));
            }

            assignEventView(views, vpzviews, observable, observation);

            return std::move(observation);
        }

        if (atom->getStructure()->needDebug()) {
            auto debug = std::make_unique<DynamicsDbg>(init, events);
            debug->set(std::move(executive));
            return std::move(debug);
        } else {
            return executive;
        }
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          (fmt(_("Executive model `%1%:%2%' (from dynamics `%3%'"
                 " library `%4%' package `%5%') throws error in"
                 " constructor: `%6%'")) %
           atom->getStructure()->getParentName() %
           atom->getStructure()->getName() % dyn.name() % dyn.library() %
           dyn.package() % e.what())
            .str());
    }
}

std::unique_ptr<Dynamics>
ModelFactory::attachDynamics(Coordinator& coordinator,
                             devs::Simulator* atom,
                             const vpz::Dynamic& dyn,
                             const InitEventList& events,
                             const std::string& observable)
{
    void* symbol = nullptr;
    auto type = utils::Context::ModuleType::MODULE_DYNAMICS;

    try {
        /* If \e package is not empty we assume that library is the shared
         * library. Otherwise, we load the global symbol stores in \e
         * library/executable and we cast it into a \e
         * vle::devs::Dynamics... Only useful for unit test or to build
         * executable with dynamics.
         */
        if (not dyn.package().empty()) {
            symbol =
              mContext->get_symbol(dyn.package(),
                                   dyn.library(),
                                   utils::Context::ModuleType::MODULE_DYNAMICS,
                                   &type);
        } else {
            symbol = mContext->get_symbol(dyn.library());

            if (dyn.library().length() >= 4) {
                if (dyn.library().compare(0, 4, "exe_") == 0)
                    type =
                      utils::Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE;
                else if (dyn.library().compare(0, 4, "wra_") == 0)
                    type = utils::Context::ModuleType::MODULE_DYNAMICS_WRAPPER;
            }
        }
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          (fmt(_("Dynamic library loading problem: cannot get any"
                 " dynamics, executive or wrapper '%1%' in library"
                 " '%2%' package '%3%'\n:%4%")) %
           dyn.name() % dyn.library() % dyn.package() % e.what())
            .str());
    }
//std::cout << "attach dynamics" << std::endl;
    switch (type) {
    case utils::Context::ModuleType::MODULE_DYNAMICS:
        return buildNewDynamics(mContext,
                                mEventViews,
                                mExperiment.views(),
                                observable,
                                atom,
                                dyn,
                                events,
                                symbol);
    case utils::Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
        return buildNewExecutive(mContext,
                                 mEventViews,
                                 mExperiment.views(),
                                 observable,
                                 coordinator,
                                 atom,
                                 dyn,
                                 events,
                                 symbol);
    case utils::Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
        return buildNewDynamicsWrapper(mContext, atom, dyn, events, symbol);
    default:
        throw utils::InternalError("Missing type");
    }
}

std::unique_ptr<DynamicsComp>
ModelFactory::attacheDynamics(Coordinator &coordinator,
                             devs::Simulator *atom,
                             const vpz::Dynamic &dyn,
                             const InitEventList &events,
                             const std::string &observable)
{
   // std::cout << "attach dynamicsComp" << std::endl;
void *symbol = nullptr;
 auto type = utils::Context::ModuleType::MODULE_DYNAMICS;
	    try {
        /* If \e package is not empty we assume that library is the shared
         * library. Otherwise, we load the global symbol stores in \e
         * library/executable and we cast it into a \e
         * vle::devs::Dynamics... Only useful for unit test or to build
         * executable with dynamics.
         */
        if (not dyn.package().empty()) {
            symbol = mContext->get_symbol(
                dyn.package(),
                dyn.library(),
                utils::Context::ModuleType::MODULE_DYNAMICS,
                &type);
        }
       /* else {
            symbol = mContext->get_symbol(dyn.library());

            if (dyn.library().length() >= 4) {
                if (dyn.library().compare(0, 4, "exe_") == 0)
                    type =
                        utils::Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE;
                else if (dyn.library().compare(0, 4, "wra_") == 0)
                    type = utils::Context::ModuleType::MODULE_DYNAMICS_WRAPPER;
            }
        }*/
    }
    catch (const std::exception &e) {
        throw utils::ModellingError(
            (fmt(_("Dynamic library loading problem: cannot get any"
                   " dynamics, executive or wrapper '%1%' in library"
                   " '%2%' package '%3%'\n:%4%")) %
             dyn.name() % dyn.library() % dyn.package() % e.what())
                .str());
    }
 
   
  std::unique_ptr<DynamicsComp> result = buildDynamics(mContext,
                               // mEventViews,
                               // mExperiment.views(),
                               // observable,
                                atom,
                                dyn,
                                events,
                                symbol);
							
                                
                  
                                
	return result;
   
}


}} // namespace vle devs
