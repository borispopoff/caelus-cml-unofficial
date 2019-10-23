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

#include "cellZone.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "cellZoneMesh.hpp"
#include "polyMesh.hpp"
#include "primitiveMesh.hpp"
#include "IOstream.hpp"
#include "demandDrivenData.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(cellZone, 0);
    defineRunTimeSelectionTable(cellZone, dictionary);
    addToRunTimeSelectionTable(cellZone, cellZone, dictionary);
}

const char * const CML::cellZone::labelsName = "cellLabels";

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::cellZone::cellZone
(
    const word& name,
    const labelUList& addr,
    const label index,
    const cellZoneMesh& zm
)
:
    zone(name, addr, index),
    zoneMesh_(zm)
{}


CML::cellZone::cellZone
(
    const word& name,
    const Xfer<labelList>& addr,
    const label index,
    const cellZoneMesh& zm
)
:
    zone(name, addr, index),
    zoneMesh_(zm)
{}


CML::cellZone::cellZone
(
    const word& name,
    const dictionary& dict,
    const label index,
    const cellZoneMesh& zm
)
:
    zone(name, dict, this->labelsName, index),
    zoneMesh_(zm)
{}


CML::cellZone::cellZone
(
    const cellZone& cz,
    const labelUList& addr,
    const label index,
    const cellZoneMesh& zm
)
:
    zone(cz, addr, index),
    zoneMesh_(zm)
{}

CML::cellZone::cellZone
(
    const cellZone& cz,
    const Xfer<labelList>& addr,
    const label index,
    const cellZoneMesh& zm
)
:
    zone(cz, addr, index),
    zoneMesh_(zm)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::cellZone::~cellZone()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::label CML::cellZone::whichCell(const label globalCellID) const
{
    return zone::localID(globalCellID);
}


const CML::cellZoneMesh& CML::cellZone::zoneMesh() const
{
    return zoneMesh_;
}


bool CML::cellZone::checkDefinition(const bool report) const
{
    return zone::checkDefinition(zoneMesh_.mesh().nCells(), report);
}


void CML::cellZone::writeDict(Ostream& os) const
{
    os  << nl << name() << nl << token::BEGIN_BLOCK << nl
        << "    type " << type() << token::END_STATEMENT << nl;

    writeEntry(os, this->labelsName, *this);

    os  << token::END_BLOCK << endl;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void CML::cellZone::operator=(const cellZone& zn)
{
    clearAddressing();
    labelList::operator=(zn);
}


void CML::cellZone::operator=(const labelUList& addr)
{
    clearAddressing();
    labelList::operator=(addr);
}


void CML::cellZone::operator=(const Xfer<labelList>& addr)
{
    clearAddressing();
    labelList::operator=(addr);
}


// * * * * * * * * * * * * * * * Ostream Operator  * * * * * * * * * * * * * //

CML::Ostream& CML::operator<<(Ostream& os, const cellZone& zn)
{
    zn.write(os);
    os.check("Ostream& operator<<(Ostream&, const cellZone&");
    return os;
}


// ************************************************************************* //
