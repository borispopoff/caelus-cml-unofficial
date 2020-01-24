/*---------------------------------------------------------------------------*\
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

Namespace
    CML::parsing::evalStringToScalar

Description
    Parsing encapsulation for stringOps::toScalar

\*---------------------------------------------------------------------------*/
#ifndef evalStringToScalar_HPP
#define evalStringToScalar_HPP

#include "scalar.hpp"
#include "string.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace stringOps
{

    //- A simple string to scalar evaluation that handles various basic
    //- expressions. For trivial input, use readScalar instead (faster).
    //
    //  The evaluation supports the following:
    //  - operations:  - + * /
    //  - functions:  exp, log, log10, pow, sqrt, cbrt, sqr, mag, magSqr
    //  - trigonometric:  sin, cos, tan, asin, acos, atan, atan2, hypot
    //  - hyperbolic:  sinh, cosh, tanh
    //  - conversions:  degToRad, radToDeg
    //  - constants:  pi()
    //  - misc: rand(), rand(seed)
    //
    //  \note The rand() function returns a uniform scalar on [0-1] interval
    scalar toScalar(const std::string& s);

} // End namespace stringOps
} // End namespace CML


#endif
