/**
 * @file vle/utils/test/test_template.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_library_test_template
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <numeric>
#include <vle/utils/Template.hpp>
#include <vle/utils/Tools.hpp>

using namespace vle;

struct F
{
    F() { utils::init(); }
    ~F() { utils::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

const char* tpl1 = "a) My name is {{name}} and I am {{year}} old\n" \
                   "b) My name is {{name}} and I am {{year}} old\n" \
                   "c) My name is {{name}} and I am {{year}} olds\n";

const char* tpl2 = "class {{name}} : public DifferenceEquation::Multiple\n" \
                    "{\n" \
                    "public:\n" \
                    "    {{name}}(\n" \
                    "       const graph::AtomicModel& atom,\n" \
                    "       const devs::InitEventList& evts)\n" \
                    "        : DifferenceEquation::Multiple(atom, evts)\n" \
                    "    {\n" \
                    "{{for i in list}}" \
                    "        {{list^i}} = createVar(\"{{list2^i}}\");\n" \
                    "        {{list2^i}} = createVar(\"{{list^i}}\");\n" \
                    "{{end for}}" \
                    "    }\n" \
                    "\n" \
                    "    virtual ~{{name}}()\n" \
                    "    {}\n" \
                    "\n" \
                    "private:\n" \
                    "{{for i in list}}" \
                    "    /* avant {{name}}:{{list^i}} */\n" \
                    "    Var {{list^i}};\n" \
                    "    /* milieu c'est merveilleux */\n" \
                    "    Var {{list2^i}};\n" \
                    "    /* après*/\n" \
                    "{{end for}}" \
                    "\n" \
                    "};\n";

const char* tpl3 = "class {{name}} : public DifferenceEquation::Multiple\n" \
                    "{\n" \
                    "public:\n" \
                    "    {{name}}(\n" \
                    "       const graph::AtomicModel& atom,\n" \
                    "       const devs::InitEventList& evts)\n" \
                    "        : DifferenceEquation::Multiple(atom, evts)\n" \
                    "    {\n" \
                    "{{for i in list}}" \
                    "{{if isabc}}" \
                    "        {{list^i}} = createVar(\"{{list2^i}}\");\n" \
                    "{{end if}}" \
                    "{{ifnot isabc}}" \
                    "        {{list2^i}} = createVar(\"{{list^i}}\");\n" \
                    "{{end ifnot}}" \
                    "{{end for}}" \
                    "    }\n" \
                    "\n" \
                    "    virtual ~{{name}}()\n" \
                    "    {}\n" \
                    "\n" \
                    "private:\n" \
                    "{{for i in list}}" \
                    "    /* avant {{name}}:{{list^i}} */\n" \
                    "{{if isabc}}" \
                    "    Var {{list^i}};\n" \
                    "{{end if}}" \
                    "{{ifnot isabc}}" \
                    "    Var {{list2^i}};\n" \
                    "    /* après*/\n" \
                    "{{end ifnot}}" \
                    "{{end for}}" \
                    "\n" \
                    "};\n";

BOOST_AUTO_TEST_CASE(test_template_simple)
{
    vle::utils::Template tpl(tpl1);
    tpl.stringSymbol().append("name", "toto");
    tpl.stringSymbol().append("year", "74");

    std::ostringstream out;
    tpl.process(out);

    std::string str = out.str();

    BOOST_REQUIRE_EQUAL(str,
                        "a) My name is toto and I am 74 old\n"
                        "b) My name is toto and I am 74 old\n"
                        "c) My name is toto and I am 74 olds\n");
}

BOOST_AUTO_TEST_CASE(test_template_for)
{
    vle::utils::Template tpl(tpl2);

    tpl.stringSymbol().append("name", "meteo");

    tpl.listSymbol().append("list", "X");
    tpl.listSymbol().append("list", "Y");
    tpl.listSymbol().append("list", "Z");

    tpl.listSymbol().append("list2", "A");
    tpl.listSymbol().append("list2", "B");
    tpl.listSymbol().append("list2", "C");

    std::stringstream out;
    tpl.process(out);

    BOOST_REQUIRE_EQUAL(out.str(),
                        "class meteo : public DifferenceEquation::Multiple\n"
                        "{\n"
                        "public:\n"
                        "    meteo(\n"
                        "       const graph::AtomicModel& atom,\n"
                        "       const devs::InitEventList& evts)\n"
                        "        : DifferenceEquation::Multiple(atom, evts)\n"
                        "    {\n"
                        "        X = createVar(\"A\");\n"
                        "        A = createVar(\"X\");\n"
                        "        Y = createVar(\"B\");\n"
                        "        B = createVar(\"Y\");\n"
                        "        Z = createVar(\"C\");\n"
                        "        C = createVar(\"Z\");\n"
                        "    }\n"
                        "\n"
                        "    virtual ~meteo()\n"
                        "    {}\n"
                        "\n"
                        "private:\n"
                        "    /* avant meteo:X */\n"
                        "    Var X;\n"
                        "    /* milieu c'est merveilleux */\n"
                        "    Var A;\n"
                        "    /* après*/\n"
                        "    /* avant meteo:Y */\n"
                        "    Var Y;\n"
                        "    /* milieu c'est merveilleux */\n"
                        "    Var B;\n"
                        "    /* après*/\n"
                        "    /* avant meteo:Z */\n"
                        "    Var Z;\n"
                        "    /* milieu c'est merveilleux */\n"
                        "    Var C;\n"
                        "    /* après*/\n"
                        "\n"
                        "};\n");
}

BOOST_AUTO_TEST_CASE(test_template_for_if)
{
    vle::utils::Template tpl(tpl3);

    tpl.stringSymbol().append("name", "meteo");

    tpl.listSymbol().append("list", "X");
    tpl.listSymbol().append("list", "Y");
    tpl.listSymbol().append("list", "Z");

    tpl.listSymbol().append("list2", "A");
    tpl.listSymbol().append("list2", "B");
    tpl.listSymbol().append("list2", "C");

    tpl.boolSymbol().append("isabc", true);

    std::stringstream out;
    tpl.process(out);

    BOOST_REQUIRE_EQUAL(out.str(),
                        "class meteo : public DifferenceEquation::Multiple\n"
                        "{\n"
                        "public:\n"
                        "    meteo(\n"
                        "       const graph::AtomicModel& atom,\n"
                        "       const devs::InitEventList& evts)\n"
                        "        : DifferenceEquation::Multiple(atom, evts)\n"
                        "    {\n"
                        "        X = createVar(\"A\");\n"
                        "        Y = createVar(\"B\");\n"
                        "        Z = createVar(\"C\");\n"
                        "    }\n"
                        "\n"
                        "    virtual ~meteo()\n"
                        "    {}\n"
                        "\n"
                        "private:\n"
                        "    /* avant meteo:X */\n"
                        "    Var X;\n"
                        "    /* avant meteo:Y */\n"
                        "    Var Y;\n"
                        "    /* avant meteo:Z */\n"
                        "    Var Z;\n"
                        "\n"
                        "};\n");
}

BOOST_AUTO_TEST_CASE(test_template_for_ifnot)
{
    vle::utils::Template tpl(tpl3);

    tpl.stringSymbol().append("name", "meteo");

    tpl.listSymbol().append("list", "X");
    tpl.listSymbol().append("list", "Y");
    tpl.listSymbol().append("list", "Z");

    tpl.listSymbol().append("list2", "A");
    tpl.listSymbol().append("list2", "B");
    tpl.listSymbol().append("list2", "C");

    tpl.boolSymbol().append("isabc", false);

    std::stringstream out;
    tpl.process(out);

    BOOST_REQUIRE_EQUAL(out.str(),
                        "class meteo : public DifferenceEquation::Multiple\n"
                        "{\n"
                        "public:\n"
                        "    meteo(\n"
                        "       const graph::AtomicModel& atom,\n"
                        "       const devs::InitEventList& evts)\n"
                        "        : DifferenceEquation::Multiple(atom, evts)\n"
                        "    {\n"
                        "        A = createVar(\"X\");\n"
                        "        B = createVar(\"Y\");\n"
                        "        C = createVar(\"Z\");\n"
                        "    }\n"
                        "\n"
                        "    virtual ~meteo()\n"
                        "    {}\n"
                        "\n"
                        "private:\n"
                        "    /* avant meteo:X */\n"
                        "    Var A;\n"
                        "    /* après*/\n"
                        "    /* avant meteo:Y */\n"
                        "    Var B;\n"
                        "    /* après*/\n"
                        "    /* avant meteo:Z */\n"
                        "    Var C;\n"
                        "    /* après*/\n"
                        "\n"
                        "};\n");
}