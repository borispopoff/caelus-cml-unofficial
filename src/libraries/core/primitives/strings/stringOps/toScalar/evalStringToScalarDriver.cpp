/*--------------------------------*- C++ -*----------------------------------*\
Copyright (C) 2019 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Caelus is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

Description
    Ragel lexer interface for lemon grammar of a simple string to
    scalar evaluation

\*---------------------------------------------------------------------------*/

#include "evalStringToScalar.hpp"
#include "evalStringToScalarDriver.hpp"
#include "evalStringToScalarScanner.hpp"
#include "error.hpp"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::parsing::evalStringToScalar::parseDriver::parseDriver()
:
    genericRagelLemonDriver(),
    value_(0)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::scalar CML::parsing::evalStringToScalar::parseDriver::execute
(
    const std::string& s
)
{
    // scanner::debug = 1;

    scanner().process(s, *this);

    return value_;
}


// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

CML::scalar CML::stringOps::toScalar(const std::string& s)
{
    CML::parsing::evalStringToScalar::parseDriver driver;

    scalar val = driver.execute(s);
    // val = driver.value();

    return val;
}
