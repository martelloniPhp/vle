/*
 * @file vle/vpz/Experiment.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_VPZ_EXPERIMENT_HPP
#define VLE_VPZ_EXPERIMENT_HPP

#include <vle/vpz/DllDefines.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/Replicas.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Views.hpp>
#include <boost/cstdint.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The experiment class stores information about experimental
     * conditions (initials values), observations, replicas, etc.
     */
    class VLE_VPZ_EXPORT Experiment : public Base
    {
    public:
        /**
         * @brief Build an empty experiment with 1 unit duration an beginning date
         * and seed to 1.
         */
        Experiment()
            : m_duration(1.0), m_begin(0.0), m_seed(1)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Experiment()
        {}

        /**
         * @brief Write Experiment information under specified root node
         * including Replicas, ExperimentalCondition and Views.
         * @param out Output stream.
         * @code
         * <experiment name="exp1" duration="0.33" begin="0.0" seed="65431">
         *   [...]
         * </experiment>
         * @endcode
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return The Base::EXPERIMENT.
         */
        virtual Base::type getType() const
        { return VLE_VPZ_EXPERIMENT; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Conditions, Views functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Just delete the complete list of information from
         * vpz::Experiment.
         */
        void clear();

        /**
         * @brief Remove all no permanent value of the Experiement ie.: all
         * observable and condition that are no permanent. This function is use
         * to clean not usefull data for the devs::ModelFactory.
         */
        void cleanNoPermanent();

        /**
         * @brief Add the specific Conditions into this Experiment.
         * @param c The Conditions to add.
         */
        void addConditions(const Conditions& c);

        /**
         * @brief Add the specific Views into this Experiment.
         * @param m The Views to add.
         */
        void addViews(const Views& m);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get a constant reference to the Replicas.
         * @return Get a constant reference to the Replicas.
         */
        const Replicas& replicas() const
        { return m_replicas; }

        /**
         * @brief Get a reference to the Replicas.
         * @return Get a reference to the Replicas.
         */
        Replicas& replicas()
        { return m_replicas; }

        /**
         * @brief Get a constant reference to the Conditions.
         * @return Get a constant reference to the Conditions.
         */
        const Conditions& conditions() const
        { return m_conditions; }

        /**
         * @brief Get a reference to the Conditions.
         * @return Get a reference to the Conditions.
         */
        Conditions& conditions()
        { return m_conditions; }

        /**
         * @brief Get a constant reference to the Views.
         * @return Get a constant reference to the Views.
         */
        const Views& views() const
        { return m_views; }

        /**
         * @brief Get a reference to the Views.
         * @return Get a reference to the Views.
         */
        Views& views()
        { return m_views; }

        /**
         * @brief Set the Experiment name.
         * @param name the Experiment name.
         * @throw utils::ArgError if name is empty.
         */
        void setName(const std::string& name);

        /**
         * @brief Get the Experiment name.
         * @return the Experiment name.
         */
        const std::string& name() const
        { return m_name; }

        /**
         * @brief Set the duration of the Experiment file.
         * @param duration The duration to set Experiment file.
         * @throw utils::ArgError if duration is <= 0.
         */
        void setDuration(double duration);

        /**
         * @brief Get the duration of the Experiment file.
         * @return The duration.
         */
        double duration() const
        { return m_duration; }

        /**
         * @brief Assign a new beginning date to the simulation.
         * @param begin The new beginning date of the simulation.
         */
        void setBegin(const double& begin)
        { m_begin = begin; }

        /**
         * @brief Get the beginning date of the simulation.
         * @return A real [0.0..max double[
         */
        const double& begin() const
        { return m_begin; }

        /**
         * @brief Set the seed of the Experiment file.
         * @param seed The new seed to initialise the random generator.
         */
        void setSeed(boost::uint32_t seed)
        { m_seed = seed; }

        /**
         * @brief Get the seed of the Experiment file.
         * @return The seed to initialise to random generator.
         */
        boost::uint32_t seed() const
        { return m_seed; }

        /**
         * @brief Set the experimental design combination.
         * @param name The new name of experimental design combination.
         */
        void setCombination(const std::string& name);

        /**
         * @brief Set the experimental design combination.
         * @return the current name of experimental design combination.
         */
        const std::string& combination() const
        { return m_combination; }

    private:
        std::string         m_name;
        double              m_duration;
        double              m_begin;
        boost::uint32_t     m_seed;
        std::string         m_combination;
        Replicas            m_replicas;
        Conditions          m_conditions;
        Views               m_views;
    };

}} // namespace vle vpz

#endif
