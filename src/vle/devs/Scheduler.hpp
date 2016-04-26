/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef VLE_DEVS_SCHEDULER_HPP
#define VLE_DEVS_SCHEDULER_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ViewEvent.hpp>
#include <map>
#include <vector>
#include <boost/heap/fibonacci_heap.hpp>

namespace vle { namespace devs {

class Simulator;

/**
 * Compare two events with the \e getTime() function as comparator.
 *
 * \param e1 first event to compare.
 * \param e2 second event to compare.
 * \return true if Time e1 is more recent than devs::Time e2.
 */
template <typename EventT>
bool
EventCompare(const EventT& e1, const EventT& e2) noexcept
{
    return e1.getTime() > e2.getTime();
}

/**
 * Pop an element from the \e scheduler.
 *
 * \param scheduler where to remove element.
 */
template <typename SchedulerT>
void pop(SchedulerT &scheduler) noexcept
{
    using event_type = typename SchedulerT::value_type;

    if (scheduler.empty())
        return;

    std::pop_heap(scheduler.begin(), scheduler.end(),
                  EventCompare<event_type>);

    scheduler.pop_back();
}

/**
 * Push a new element build inplace in the \e scheduler.
 *
 * \param scheduler where to push element.
 * \param args list of argument to build inplace the element to push into
 * the scheduler.
 */
template <typename SchedulerT, typename... Args>
void push(SchedulerT &scheduler, Args&&... args)
{
    using event_type = typename SchedulerT::value_type;

    scheduler.emplace_back(std::forward<Args>(args)...);

    std::push_heap(scheduler.begin(), scheduler.end(),
                   EventCompare<event_type>);
}

struct HeapElementCompare {

    template <typename HeapElementT>
    bool
    operator()(const HeapElementT& lhs, const HeapElementT& rhs) const noexcept
    {
        return lhs.m_time >= rhs.m_time;
    }
};

struct HeapElement {

    HeapElement(Time time, Simulator *Simulator)
        : m_time(time)
        , m_simulator(Simulator)
    {}

    Time       m_time;
    Simulator *m_simulator;
};

using Heap = boost::heap::fibonacci_heap<
    HeapElement,
    boost::heap::compare<HeapElementCompare>>;

using HandleT = Heap::handle_type;

struct BagModel
{
    BagModel()
        : internal_event(false)
    {}

    ExternalEventList external_events;
    bool internal_event;
};

using Bag = std::vector<std::pair<Simulator*, BagModel>>;

class VLE_LOCAL Scheduler
{
public:
    Scheduler() = default;
    ~Scheduler() = default;

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;


    void addInternal(Simulator *simulator, Time time);
    void addObservation(View *ptr, Time time);
    void addExternal(Simulator *simulator,
                     std::shared_ptr<value::Value> values,
                     const std::string& portname);
    void delSimulator(Simulator *simulator);

    Bag& getCurrentBag() noexcept;

    /**
     * Check if the next bag will be at the current date or at an
     * another. This function is useful to check observation.
     *
     * \return true if the next scheduler date is the same date as the
     * current date.
     */
    bool haveNextBagAtTime() noexcept;

    Time getCurrentTime() const noexcept
    {
        return m_current_time;
    }

    bool haveObservationEventAtTime() noexcept;

    /**
     * Builds a list of \e ViewEvent for scheduler observation events.
     *
     * \return List of ViewEvent.
     */
    std::vector<ViewEvent> getCurrentObservationBag() noexcept;

    /**
     * Builds the next bag: at the same time with external event or/and
     * scheduler internal events.
     *
     */
    void makeNextBag();

private:
    Bag m_current_bag;
    Bag m_next_bag;
    Heap m_scheduler;
    std::vector<ViewEvent> m_observation;
    Time m_current_time;

    Time getNextTime() noexcept;
};

}} // namespace vle devs

#endif
