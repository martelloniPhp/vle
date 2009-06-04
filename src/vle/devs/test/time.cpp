/**
 * @file vle/devs/test/time.cpp
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE devstime_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vle/devs/Simulator.hpp>

using namespace vle;


BOOST_AUTO_TEST_CASE(compare)
{
    devs::Time a(1.0);
    devs::Time b(2.0);

    BOOST_REQUIRE(b > a);
    BOOST_REQUIRE(b > 1.0);
    BOOST_REQUIRE(2.0 > a);
    BOOST_REQUIRE(b > 1);
    BOOST_REQUIRE(2 > a);

    devs::Time c(devs::Time::infinity);

    BOOST_REQUIRE(c > a);
    BOOST_REQUIRE(c > b);
    BOOST_REQUIRE(c > 2.0);
    BOOST_REQUIRE(c > 0.0);

    BOOST_REQUIRE_EQUAL(a, devs::Time(1.0));
    BOOST_REQUIRE_EQUAL(a, 1.0);
    BOOST_REQUIRE_EQUAL(a, 1);

    BOOST_REQUIRE_EQUAL(b, devs::Time(2.0));
    BOOST_REQUIRE_EQUAL(b, 2.0);
    BOOST_REQUIRE_EQUAL(b, 2);

    BOOST_REQUIRE(a == devs::Time(1.0));
    BOOST_REQUIRE(a == 1.0);
    BOOST_REQUIRE(a == 1);

    BOOST_REQUIRE(a < b);
    BOOST_REQUIRE(a < 2.0);
    BOOST_REQUIRE(a < 2);

    BOOST_REQUIRE(a <= devs::Time(1.0));
    BOOST_REQUIRE(a <= 1.0);
    BOOST_REQUIRE(a <= 1);

    BOOST_REQUIRE(a <= devs::Time(devs::Time::infinity));
    BOOST_REQUIRE(a <= -1.0);
}

BOOST_AUTO_TEST_CASE(modify)
{
    devs::Time a(1.0);
    devs::Time b(2.0);
    devs::Time c(3.0);

    double x;

    x = c - b - a;
    BOOST_REQUIRE_EQUAL(x, 0.0);
    BOOST_REQUIRE_EQUAL(a + b, c);
    BOOST_REQUIRE(c >= a + b);
    BOOST_REQUIRE_EQUAL(1.0 + b, c);
    BOOST_REQUIRE(c >= a + 2.0);

    a += b;
    BOOST_REQUIRE_EQUAL(a, c);

    a -= b;
    BOOST_REQUIRE_EQUAL(a + b, c);

    devs::Time z = a;
    ++z;
    BOOST_REQUIRE_EQUAL(z, a + 1);
    --z;
    BOOST_REQUIRE_EQUAL(z, a);
}

BOOST_AUTO_TEST_CASE(modify_and_infinity)
{
    devs::Time a(1.0);

    BOOST_REQUIRE_EQUAL(a + devs::Time::infinity, devs::Time::infinity);
    BOOST_REQUIRE_EQUAL(devs::Time::infinity + a, devs::Time::infinity);
    BOOST_REQUIRE_EQUAL(devs::Time::infinity - a, devs::Time::infinity);

    {
        devs::Time a(1.0);
        devs::Time b(devs::Time::infinity);
        a += b;
        BOOST_REQUIRE_EQUAL(a, devs::Time::infinity);
    }

    {
        devs::Time a(devs::Time::infinity);
        devs::Time b(1.0);
        a += b;
        BOOST_REQUIRE_EQUAL(a, devs::Time::infinity);
        BOOST_REQUIRE_EQUAL(b, 1.0);
    }

    {
        devs::Time a(1.0);
        devs::Time b(devs::Time::infinity);
        BOOST_REQUIRE_EQUAL(b, devs::Time::infinity);
    }

    {
        devs::Time a(devs::Time::infinity);
        devs::Time b(1.0);
        a -= b;
        BOOST_REQUIRE_EQUAL(a, devs::Time::infinity);
        BOOST_REQUIRE_EQUAL(b, 1.0);
    }

    {
        devs::Time a(devs::Time::infinity);
        devs::Time b(1.0);
        a += b;
        BOOST_REQUIRE_EQUAL(a, devs::Time::infinity);
        BOOST_REQUIRE_EQUAL(b, 1.0);
    }

}