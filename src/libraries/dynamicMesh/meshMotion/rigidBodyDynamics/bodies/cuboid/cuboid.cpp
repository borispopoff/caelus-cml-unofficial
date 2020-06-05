/*---------------------------------------------------------------------------*\
Copyright (C) 2016-2019 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "cuboid.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
namespace RBD
{
    defineTypeNameAndDebug(cuboid, 0);

    addToRunTimeSelectionTable
    (
        rigidBody,
        cuboid,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::autoPtr<CML::RBD::rigidBody> CML::RBD::cuboid::clone() const
{
    return autoPtr<rigidBody>(new cuboid(*this));
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::RBD::cuboid::~cuboid()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void CML::RBD::cuboid::write(Ostream& os) const
{
    writeEntry(os, "type", type());

    writeEntry(os, "mass", m());

    writeEntry(os, "L", L());
}


// ************************************************************************* //
