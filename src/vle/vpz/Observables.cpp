/**
 * @file vle/vpz/Observables.cpp
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


#include <vle/vpz/Observables.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

void Observables::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<observables>\n";
        for (ObservableList::const_iterator it = m_list.begin();
             it != m_list.end(); ++it) {
            out << it->second;
        }
        out << "</observables>\n";
    }
}

void Observables::add(const Observables& obs)
{
    std::for_each(obs.observablelist().begin(),
                  obs.observablelist().end(),
                  AddObservable(*this));
}

Observable& Observables::add(const Observable& obs)
{
    Assert(utils::SaxParserError, not exist(obs.name()),
           (boost::format("Observable %1% already exist") % obs.name()));

    return (*m_list.insert(std::make_pair < std::string, Observable >(
                obs.name(), obs)).first).second;
}

Observable& Observables::get(const std::string& name)
{
    ObservableList::iterator it = m_list.find(name);
    Assert(utils::SaxParserError, it != m_list.end(),
           (boost::format("Observable %1% does not exist") % name));

    return it->second;
}

const Observable& Observables::get(const std::string& name) const
{
    ObservableList::const_iterator it = m_list.find(name);
    Assert(utils::SaxParserError, it != m_list.end(),
           (boost::format("Observable %1% doest not exist") % name));

    return it->second;
}

void Observables::cleanNoPermanent()
{
    ObservableList::iterator previous = m_list.begin();
    ObservableList::iterator it = m_list.begin();
    
    while (it != m_list.end()) {
        if (not it->second.isPermanent()) {
            if (it == previous) {
                m_list.erase(it);
                previous = m_list.begin();
                it = m_list.begin();
            } else {
                m_list.erase(it);
                it = previous;
            }
        } else {
            ++previous;
            ++it;
        }
    }
}

}} // namespace vle vpz
