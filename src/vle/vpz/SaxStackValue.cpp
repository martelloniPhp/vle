/**
 * @file vle/vpz/SaxStackValue.cpp
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


#include <vle/vpz/SaxStackValue.hpp>

namespace vle { namespace vpz {

bool ValueStackSax::isCompositeParent() const
{
    if (not m_valuestack.empty()) {
        const value::Value val = m_valuestack.top();

        return val->isMap() or val->isSet();
    }
    return false;
}

void ValueStackSax::pushInteger()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }
}

void ValueStackSax::pushBoolean()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }
}

void ValueStackSax::pushString()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }
}

void ValueStackSax::pushDouble()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }
}

void ValueStackSax::pushMap()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }

    pushOnVectorValue(value::MapFactory::create());
}

void ValueStackSax::pushMapKey(const Glib::ustring& key)
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, m_valuestack.top()->isMap());
    }

    m_lastkey.assign(key);
}

void ValueStackSax::pushSet()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }

    pushOnVectorValue(value::SetFactory::create());
}

void ValueStackSax::pushTuple()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }

    pushOnVectorValue(value::TupleFactory::create());
}

void ValueStackSax::pushTable(const size_t width, const size_t height)
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }

    pushOnVectorValue(value::TableFactory::create(width, height));
}

void ValueStackSax::pushXml()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }
}

void ValueStackSax::pushNull()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, isCompositeParent());
    }
}

void ValueStackSax::popValue()
{
    if (not m_valuestack.empty()) {
        m_valuestack.pop();
    }
}

const value::Value& ValueStackSax::topValue()
{
    Assert(utils::SaxParserError, not m_valuestack.empty(),
           "Empty sax parser value stack for the top operation");

    return m_valuestack.top();
}

void ValueStackSax::pushOnVectorValue(const value::Value& val)
{
    if (not m_valuestack.empty()) {
        if (m_valuestack.top()->isSet()) {
            value::toSetValue(m_valuestack.top())->addValue(val);
        } else if (m_valuestack.top()->isMap()) {
            value::toMapValue(m_valuestack.top())->addValue(
                m_lastkey, val);
        }
    } else {
        m_result.push_back(val);
    }

    if (val->isSet() or val->isMap() or val->isTuple() or val->isTable()) {
        m_valuestack.push(val);
    }
}

void ValueStackSax::clear()
{
    while (not m_valuestack.empty())
        m_valuestack.pop();

    m_result.clear();
}


}} // namespace vle vpz
