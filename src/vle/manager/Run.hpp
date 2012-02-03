/*
 * @file vle/manager/Run.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#ifndef VLE_MANAGER_RUN_HPP
#define VLE_MANAGER_RUN_HPP

#include <vle/DllDefines.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>

namespace vle { namespace manager {

    /**
     * @brief Run is a base class for launch simulation from specified vpz::Vpz
     * object in a verbose mode or a quiet mode. It provides an access to
     * RootCoordinator builded for this simulation.
     */
    class VLE_EXPORT Run
    {
    public:
        /**
         * @brief Default constructor.
         */
        Run(const utils::ModuleManager& modulemgr);

        /**
         * @brief Return true if the simulation reports error.
         * @return true if the simulation reports error, false otherwise.
         */
        bool haveError() const;

        /**
         * @brief Return a constant reference to the RootCoordinator.
         * @return Return a constant reference to the RootCoordinator.
         */
        const devs::RootCoordinator& rootCoordinator() const;

        /**
         * @brief Return a reference to the RootCoordinator.
         * @return Return a reference to the RootCoordinator.
         */
        devs::RootCoordinator& rootCoordinator();

        /**
         * @brief Return a constant list of the dictionary viewname,
         * oov::Plugin.
         * @return Return a constant list of the dictionary viewname.
         */
        const oov::OutputMatrixViewList& outputs() const;

        /**
         * @brief Return the list of the dictionary viewname, oov::Plugin.
         * @return Return the list of the dictionary viewname.
         */
        oov::OutputMatrixViewList& outputs();

    protected:
        Run(const Run& other);
        Run& operator=(const Run& other);

        devs::RootCoordinator   m_root;
        bool                    m_error;
    };



    /**
     * @brief RunVerbose is a specific Run class to launch simulation from
     * specified vpz::Vpz object with verbose mode to standard error. This
     * class is a functor to be use with std::for_each.
     */
    class VLE_EXPORT RunVerbose : public Run
    {
    public:
        RunVerbose(const utils::ModuleManager& modulemgr, std::ostream& out);

        /**
         * @brief Launch the simulation of the specified vpz::Vpz object. Be
         * carrefull, the object is destroyed in this function.
         * @param vpz a reference to the vpz::Vpz to simulate. Be carreful,
         * destroy by this function.
         */
        void operator()(vpz::Vpz* vpz);

        /**
         * @brief Launch the simulation on the specified filename.
         * @param filename the filename of the vpz::Vpz to simulate.
         */
        void operator()(const std::string& filename);

        /**
         * @brief Launch the simulation of the specified vpz::Vpz object. A
         * clone of this vpz::Vpz is build to allow simulation.
         * @param a reference to the vpz::vpz to simulate.
         */
        void operator()(const vpz::Vpz& vpz);

        /**
         * @brief Launch the simulation of the specifieed vpz::Vpz object. Be
         * carrefull, the object is destroyed in this function.
         * @param vpz a reference to the vpz::Vpz to simulate. Be carreful,
         * destroy by this function.
         */
        void start(vpz::Vpz* vpz);

        /**
         * @brief Launch the simulation on the specified filename.
         * @param filename The filename to simulate.
         */
        void start(const std::string& filename);

        /**
         * @brief Launch the simulation of the specified vpz::Vpz object. A
         * clone of this vpz::Vpz is build to allow simulation.
         * @param a reference to the vpz::vpz to simulate.
         */
        void start(const vpz::Vpz& vpz);

    private:
        RunVerbose(const RunVerbose& other);
        RunVerbose& operator=(const RunVerbose& other);

        std::ostream&       m_out;
    };



    /**
     * @brief RunQuiet is a specific Run class to launch simulation from
     * specified vpz::Vpz object with verbose mode into std::string and a member
     * to access it. This class is a functor to be use with std::for_each.
     */
    class VLE_EXPORT RunQuiet : public Run
    {
    public:
        RunQuiet(const utils::ModuleManager& modulemgr);

        /**
         * @brief Launch the simulation of the specified vpz::Vpz object. Be
         * carrefull, the object is destroyed in this function.
         * @param vpz a reference to the vpz::Vpz to simulate. Be carreful,
         * destroy by this function.
         */
        void operator()(vpz::Vpz* vpz);

        /**
         * @brief Launch the simulation on the specified filename.
         * @param filename the filename of the vpz::Vpz to simulate.
         */
        void operator()(const std::string& filename);

        /**
         * @brief Launch the simulation of the specified vpz::Vpz object. A
         * clone of this vpz::Vpz is build to allow simulation.
         * @param a reference to the vpz::vpz to simulate.
         */
        void operator()(const vpz::Vpz& vpz);

        /**
         * @brief Launch the simulation of the specifieed vpz::Vpz object. Be
         * carrefull, the object is destroyed in this function.
         * @param vpz a reference to the vpz::Vpz to simulate. Be carreful,
         * destroy by this function.
         */
        void start(vpz::Vpz* vpz);

        /**
         * @brief Launch the simulation on the specified filename.
         * @param filename The filename to simulate.
         */
        void start(const std::string& filename);

        /**
         * @brief Launch the simulation of the specified vpz::Vpz object. A
         * clone of this vpz::Vpz is build to allow simulation.
         * @param a reference to the vpz::vpz to simulate.
         */
        void start(const vpz::Vpz& vpz);

        /**
         * @brief Return a string description of the error. use Run::haveError()
         * before otherwise, this function returns an empty string.
         * @return A description of the error if the simulation failed,
         * otherwise an empty std::string.
         */
        const std::string& error() const;

    private:
        RunQuiet(const RunQuiet& other);
        RunQuiet& operator=(const RunQuiet& other);

        std::string m_stringerror;
    };

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    inline Run::Run(const utils::ModuleManager& modulemgr)
        : m_root(modulemgr), m_error(false)
    {
    }

    inline bool Run::haveError() const
    {
        return m_error;
    }

    inline devs::RootCoordinator& Run::rootCoordinator()
    {
        return m_root;
    }

    inline const devs::RootCoordinator& Run::rootCoordinator() const
    {
        return m_root;
    }

    inline const oov::OutputMatrixViewList& Run::outputs() const
    {
        return m_root.outputs();
    }

    inline oov::OutputMatrixViewList& Run::outputs()
    {
        return m_root.outputs();
    }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    inline RunVerbose::RunVerbose(const utils::ModuleManager& modulemgr,
                                  std::ostream& out)
        : Run(modulemgr), m_out(out)
    {
    }

    inline void RunVerbose::start(vpz::Vpz* vpz)
    {
        operator()(vpz);
    }

    inline void RunVerbose::start(const std::string& filename)
    {
        operator()(filename);
    }

    inline void RunVerbose::start(const vpz::Vpz& vpz)
    {
        operator()(vpz);
    }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    inline RunQuiet::RunQuiet(const utils::ModuleManager& modulemgr)
        : Run(modulemgr)
    {
    }

    inline const std::string& RunQuiet::error() const
    {
        return m_stringerror;
    }

    inline void RunQuiet::start(vpz::Vpz* vpz)
    {
        operator()(vpz);
    }

    inline void RunQuiet::start(const std::string& filename)
    {
        operator()(filename);
    }

    inline void RunQuiet::start(const vpz::Vpz& vpz)
    {
        operator()(vpz);
    }

}} // namespace vle manager

#endif
