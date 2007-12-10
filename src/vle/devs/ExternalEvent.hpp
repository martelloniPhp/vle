/**
 * @file devs/ExternalEvent.hpp
 * @author The VLE Development Team.
 * @brief External event based on the devs::Event class and are build by
 * graph::Model when output function are called.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef VLE_DEVS_EXTERNALEVENT_HPP
#define VLE_DEVS_EXTERNALEVENT_HPP

#include <vle/devs/Event.hpp>
#include <string>

namespace vle { namespace devs {

    class Simulator;

    /**
     * @brief External event based on the devs::Event class and are build by
     * graph::Model when output function are called.
     *
     */
    class ExternalEvent : public Event
    {
    public:
        ExternalEvent(const std::string& sourcePortName) :
            Event(0),
            m_portName(sourcePortName),
            m_target(0)
        { }

        ExternalEvent(const std::string& sourcePortName,
                      Simulator* source) :
            Event(source),
            m_portName(sourcePortName),
            m_target(0)
        { }

	ExternalEvent(ExternalEvent* event,
		      Simulator* target,
		      const std::string& targetPortName) :
	    Event(*event),
	    m_portName(targetPortName),
	    m_target(target)
	{ }

	virtual ~ExternalEvent()
        { }

	inline const std::string& getPortName() const
        { return m_portName; }

	inline Simulator* getTarget()
        { return m_target; }

        const std::string& getTargetModelName() const;

	inline bool onPort(const std::string& portName) const
        { return m_portName == portName; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        
	virtual bool isExternal() const;

	virtual bool isInit() const;

	virtual bool isInternal() const;

	virtual bool isState() const;

        /**
         * A new virtual function to specify if this event is an
         * InstantaneousEvent or single ExternalEvent.
         *
         * @return true if this event is ExternalEvent, other return false.
         */
        virtual bool isInstantaneous() const;

    protected:
	std::string   m_portName;
	Simulator*    m_target;
    };

    inline std::ostream& operator<<(std::ostream& o, const ExternalEvent& evt)
    {
        return o << "from: '" << evt.getSourceModelName()
            << "' value: '" << evt.getAttributes()->toString()
            << "' to port: '" << evt.getPortName()
            << "'";
    }


}} // namespace vle devs

#endif
