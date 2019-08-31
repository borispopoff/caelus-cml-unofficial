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

\*---------------------------------------------------------------------------*/

#include "boxToCell.hpp"
#include "polyMesh.hpp"

#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{

defineTypeNameAndDebug(boxToCell, 0);

addToRunTimeSelectionTable(topoSetSource, boxToCell, word);

addToRunTimeSelectionTable(topoSetSource, boxToCell, istream);

}


CML::topoSetSource::addToUsageTable CML::boxToCell::usage_
(
    boxToCell::typeName,
    "\n    Usage: boxToCell (minx miny minz) (maxx maxy maxz)\n\n"
    "    Select all cells with cellCentre within bounding box\n\n"
);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::boxToCell::combine(topoSet& set, const bool add) const
{
    const pointField& ctrs = mesh_.cellCentres();

    forAll(ctrs, celli)
    {
        if (bb_.contains(ctrs[celli]))
        {
            addOrDelete(set, celli, add);
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
CML::boxToCell::boxToCell
(
    const polyMesh& mesh,
    const treeBoundBox& bb
)
:
    topoSetSource(mesh),
    bb_(bb)
{}


// Construct from dictionary
CML::boxToCell::boxToCell
(
    const polyMesh& mesh,
    const dictionary& dict
)
:
    topoSetSource(mesh),
    bb_(dict.lookup("box"))
{}


// Construct from Istream
CML::boxToCell::boxToCell
(
    const polyMesh& mesh,
    Istream& is
)
:
    topoSetSource(mesh),
    bb_(checkIs(is))
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::boxToCell::~boxToCell()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::boxToCell::applyToSet
(
    const topoSetSource::setAction action,
    topoSet& set
) const
{
    if ((action == topoSetSource::NEW) || (action == topoSetSource::ADD))
    {
        Info<< "    Adding cells with center within box " << bb_ << endl;

        combine(set, true);
    }
    else if (action == topoSetSource::DELETE)
    {
        Info<< "    Removing cells with center within box " << bb_ << endl;

        combine(set, false);
    }
}


// ************************************************************************* //
