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

#include <vle/vpz/MultiComponent.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <iostream>

namespace vle { namespace vpz {

MultiComponent::MultiComponent(const std::string& name,
                         CoupledModel* parent)
    : BaseModel(name, parent)
    , m_simulator(nullptr)
    , m_debug(false)
{
	//std::cout << "modele multi créé"<< std::endl;
}

MultiComponent::MultiComponent(const std::string& name,
                         CoupledModel* parent,
                         const std::string& condition,
                         //const std::vector < std::string > dynamic,
                         const std::string& observable)
    : BaseModel(name, parent)
    , m_simulator(nullptr)
    //, m_dynamics(dynamic)
    , m_observables(observable)
    , m_debug(false)
{
    std::string conditionList(condition);
    boost::trim(conditionList);

    if (not conditionList.empty()) {
        boost::split(m_conditions, conditionList, boost::is_any_of(","),
                     boost::algorithm::token_compress_on);
        if (m_conditions.front().empty()) {
            m_conditions.pop_back();
        }
    }
    
    std::cout << "modele multi créé"<< std::endl;
}

MultiComponent::MultiComponent(const MultiComponent& mdl)
    : BaseModel(mdl)
    , m_simulator(nullptr)
    //, m_dynamics(mdl.dynamics())
    , m_observables(mdl.observables())
    , m_debug(mdl.m_debug)
{
    m_conditions = mdl.m_conditions;
   // std::cout << "modele multi créé"<< std::endl;
}

MultiComponent& MultiComponent::operator=(const MultiComponent& mdl)
{
    MultiComponent m(mdl);
    swap(m);
    return *this;
}

void MultiComponent::delCondition(const std::string& str)
{
    auto itfind =
	std::find(m_conditions.begin(), m_conditions.end(), str);

    m_conditions.erase(itfind);
}

BaseModel* MultiComponent::findModel(const std::string& name) const
{
    return (getName() == name) ?
        const_cast < BaseModel* >(
            reinterpret_cast < const BaseModel* >(this)) : nullptr;
}

void MultiComponent::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName().c_str() << "\" type=\"multi\""
        << ">\n";
    writePortListXML(out);
    out << "</model>\n";
}

void MultiComponent::write(std::ostream& out) const
{
    out << "<model name=\"" << getName().c_str() << "\" "
        << "type=\"multi\" ";

    if (not conditions().empty()) {
	out << "conditions=\"";

        auto it =
            conditions().begin();
        while (it != conditions().end()) {
            out << it->c_str();
            ++it;
            if (it != conditions().end()) {
                out << ",";
            }
        }

        out << "\" ";
    }

    //out << "dynamics=\"" << dynamics().c_str() << "\" ";

    if (not observables().empty()) {
        out << "observables=\"" << observables().c_str() << "\" ";
    }

    writeGraphics(out);

    out << ">\n";

    writePort(out);

    out << "</model>\n";
}





void MultiComponent::updateObservable(const std::string& oldname,
                                        const std::string& newname)
{
    if (observables() == oldname) {
        setObservables(newname);
    }
}

void MultiComponent::purgeObservable(const std::set < std::string >&
                                       observablelist)
{
    if (observablelist.find(observables()) == observablelist.end()) {
        setObservables("");
    }
}

void MultiComponent::updateConditions(const std::string& oldname,
                                        const std::string& newname)
{
    std::replace(m_conditions.begin(),
                 m_conditions.end(), oldname, newname);
}

void MultiComponent::purgeConditions(const std::set < std::string >&
                                       conditionlist)
{
    for (int i = m_conditions.size() - 1; i >= 0; --i) {

        auto itfind =
            conditionlist.find(m_conditions[i]);

        if (itfind == conditionlist.end()) {
            m_conditions.erase(m_conditions.begin() + i);
        }
    }
}
void MultiComponent::addComponent(Component *c)
{
	ComponentList.push_back(c);
	if(!c->dynamics().empty())
	{
	m_dynamics.push_back(c->dynamics());
	}
	//std::cout<< "sim.addcomponent: " << c->dynamics() << std::endl;
}
	
}} // namespace vpz graph
