/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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

#include "word.hpp"
#include "debug.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const char* const CML::word::typeName = "word";
int CML::word::debug(CML::debug::debugSwitch(word::typeName, 0));
const CML::word CML::word::null;
// * * * * * * * * * * * * * * * IOstream Functions  * * * * * * * * * * * * //

void CML::writeEntry(Ostream& os, const word& value)
{
    os << value;
}


// ************************************************************************* //
