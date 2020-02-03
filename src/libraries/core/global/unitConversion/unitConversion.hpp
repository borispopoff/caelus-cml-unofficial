/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of CAELUS.

    CAELUS is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CAELUS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with CAELUS.  If not, see <http://www.gnu.org/licenses/>.

InNamespace
    CML

Description
    Unit conversion functions

\*---------------------------------------------------------------------------*/

#ifndef unitConversion_H
#define unitConversion_H

#include "mathematicalConstants.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//- Conversion from degrees to radians
inline scalar degToRad(const scalar deg)
{
    return (deg*constant::mathematical::pi/180.0);
}

//- Conversion from radians to degrees
inline scalar radToDeg(const scalar rad)
{
    return (rad*180.0/constant::mathematical::pi);
}

//- Multiplication factor for degrees to radians conversion
inline scalar degToRad()
{
    return (M_PI/180.0);
}

//- Multiplication factor for radians to degrees conversion
inline scalar radToDeg()
{
    return (180.0/M_PI);
}

//- Conversion from atm to Pa
inline scalar atmToPa(const scalar atm)
{
    return (atm*101325.0);
}

//- Conversion from atm to Pa
inline scalar paToAtm(const scalar pa)
{
    return (pa/101325.0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
