/**
 * @file src/examples/petrinet/Logical.cpp
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


#include <vle/extension.hpp>
#include <vle/devs.hpp>

namespace vle { namespace examples { namespace petrinet {

    /**
     * @brief A DEVS generator which send an empty message on its "out" port
     * every 1.0 time. The generator start from the StartTime parameter from
     * VPZ.
     */
    class PetrinetBeep : public devs::Dynamics
    {
    public:
        PetrinetBeep(const graph::AtomicModel& model,
                     const devs::InitEventList& events) :
            devs::Dynamics(model, events)
        {
            mStartTime = value::toDouble(events.get("start"));
            mTimeStep = value::toDouble(events.get("timestep"));
        }

        virtual ~PetrinetBeep()
        { }

        virtual devs::Time init(const devs::Time& /* time */)
        { return mStartTime; }

        virtual void output(const devs::Time& /* time */,
                            devs::ExternalEventList& output) const
        { output.addEvent(new devs::ExternalEvent("out")); }

        virtual devs::Time timeAdvance() const
        { return mTimeStep; }

    private:
        devs::Time mStartTime;
        devs::Time mTimeStep;
    };

    /**
     * @brief A DEVS counter which store the date of the latest event.
     */
    class PetrinetCounter : public devs::Dynamics
    {
    public:
        PetrinetCounter(const graph::AtomicModel& model,
                        const devs::InitEventList& events) :
            devs::Dynamics(model, events),
            mDate(0),
            mNumber(0)
        { }

        virtual ~PetrinetCounter()
        { }

        virtual void externalTransition(const devs::ExternalEventList& evts,
                                        const devs::Time& time)
        { mDate = time; mNumber += evts.size(); }

        virtual value::Value observation(const devs::ObservationEvent& e) const
        { return e.onPort("date") ?
            value::DoubleFactory::create(mDate.getValue()) :
                value::DoubleFactory::create(mNumber); }

    private:
        devs::Time mDate;
        int mNumber;
    };

    /**
     * @brief A DEVS implementation of the extension::PetriNet.
     */
    class PetrinetOrdinary : public extension::PetriNet
    {
    public:
        PetrinetOrdinary(const graph::AtomicModel& model,
                         const devs::InitEventList& events) :
            extension::PetriNet(model, events)
        { }

        virtual ~PetrinetOrdinary()
        { }

    };

    /**
     * @brief A DEVS model which simulate a Meteo. Each time step, it build an
     * event with true or false.
     */
    class Meteo : public devs::Dynamics
    {
    public:
        Meteo(const graph::AtomicModel& model,
              const devs::InitEventList& events) :
            devs::Dynamics(model, events)
        {
            mInit = value::toBoolean(events.get("init"));
            mMin = value::toInteger(events.get("min"));
            mMax = value::toInteger(events.get("max"));
        }

        virtual ~Meteo()
        { }

        virtual void output(const devs::Time& /* time */,
                            devs::ExternalEventList& output) const
        {
            if (mActive) {
                output.addEvent(buildEvent("yes"));
            } else {
                output.addEvent(buildEvent("no"));
            }
        }

        virtual devs::Time timeAdvance() const
        {
            return devs::Time(1);
        }

        virtual devs::Time init(const devs::Time& /* time */)
        {
            mActive = mInit;
            mNextTime = rand().get_int_range(mMin, mMax);
            return devs::Time(0);
        }

        virtual void internalTransition(const devs::Time& /* event */)
        {
            if (mNextTime == 0) {
                mActive = not mActive;
                mNextTime = rand().get_int_range(mMin, mMax);
            } else {
                --mNextTime;
            }
        }

        virtual value::Value observation(
            const devs::ObservationEvent& event) const
        {
            if (event.onPort("state")) {
                return buildBoolean(mActive);
            }
            return value::Value();
        }

    private:
	bool mInit;
        unsigned int mMin;
        unsigned int mMax;
	unsigned int mNextTime;
        bool mActive;
    };

    /**
     * @brief Build an Petri Net model which build an output when it receives
     * mDayNumber of event.
     */
    class Trigger : public extension::PetriNet
    {
    public:
        Trigger(const graph::AtomicModel& model,
                const devs::InitEventList& events) :
            extension::PetriNet(model, events)
        {
            mDayNumber = value::toInteger(events.get("day"));
        }

        virtual ~Trigger()
        { }

        virtual void build()
        {
            addPlace("P1");
            addPlace("P2");
            addPlace("P3");
            addPlace("P4");
            addPlace("P5");
            addOutputPlace("P6", "out");

            addInputTransition("T1", "yes");
            addInputTransition("T2", "no");
            addTransition("T3");
            addTransition("T4");
            addTransition("T5");
            addTransition("T6");
            addTransition("T7");
            addTransition("T8");

            addArc("T1", "P1");
            addArc("P1", "T3");
            addArc("P1", "T4");
            addArc("T3", "P3");
            addArc("P3", "T3");
            addArc("T4", "P3");
            addArc("P3", "T7");
            addArc("T7", "P3");
            addArc("P3", "T5");

            addArc("T2", "P2");
            addArc("P2", "T5");
            addArc("P2", "T6");
            addArc("T6", "P4");
            addArc("P4", "T6");
            addArc("P4", "T4");
            addArc("T5", "P4");

            addArc("T5", "P5");
            addArc("T6", "P5");
            addArc("P5", "T7");
            addArc("P5", "T8", mDayNumber);
            addArc("T8", "P6");

            addInitialMarking("P4", 1);
        }

    private:
        unsigned int mDayNumber;
    };

}}} // namespace vle examples petrinet

DECLARE_NAMED_DYNAMICS(Beep, vle::examples::petrinet::PetrinetBeep)
DECLARE_NAMED_DYNAMICS(Ordinary, vle::examples::petrinet::PetrinetOrdinary)
DECLARE_NAMED_DYNAMICS(Counter, vle::examples::petrinet::PetrinetCounter)
DECLARE_NAMED_DYNAMICS(Meteo, vle::examples::petrinet::Meteo)
DECLARE_NAMED_DYNAMICS(Trigger, vle::examples::petrinet::Trigger)