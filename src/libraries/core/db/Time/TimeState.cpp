/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2016 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "TimeState.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::TimeState::TimeState()
:
    dimensionedScalar(Time::timeName(0), dimTime, 0),
    timeIndex_(0),
    deltaT_(0),
    deltaTSave_(0),
    deltaT0_(0),
    deltaTchanged_(false),
    writeTimeIndex_(0),
    writeTime_(false)
{}


// * * * * * * * * * * * * * * * * Destructor * * * * * * * * * * * * * * * * //

CML::TimeState::~TimeState()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::scalar CML::TimeState::userTimeToTime(const scalar theta) const
{
    return theta;
}


CML::scalar CML::TimeState::timeToUserTime(const scalar t) const
{
    return t;
}


// ************************************************************************* //
