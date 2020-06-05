/*---------------------------------------------------------------------------*\
Copyright (C) 2018 OpenFOAM Foundation
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

#include "cyclicRepeatAMIGAMGInterfaceField.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "lduMatrix.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(cyclicRepeatAMIGAMGInterfaceField, 0);
    addToRunTimeSelectionTable
    (
        GAMGInterfaceField,
        cyclicRepeatAMIGAMGInterfaceField,
        lduInterface
    );
    addToRunTimeSelectionTable
    (
        GAMGInterfaceField,
        cyclicRepeatAMIGAMGInterfaceField,
        lduInterfaceField
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::cyclicRepeatAMIGAMGInterfaceField::cyclicRepeatAMIGAMGInterfaceField
(
    const GAMGInterface& GAMGCp,
    const lduInterfaceField& fineInterface
)
:
    cyclicAMIGAMGInterfaceField(GAMGCp, fineInterface)
{}


CML::cyclicRepeatAMIGAMGInterfaceField::cyclicRepeatAMIGAMGInterfaceField
(
    const GAMGInterface& GAMGCp,
    const bool doTransform,
    const int rank
)
:
    cyclicAMIGAMGInterfaceField(GAMGCp, doTransform, rank)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::cyclicRepeatAMIGAMGInterfaceField::~cyclicRepeatAMIGAMGInterfaceField()
{}


// ************************************************************************* //
