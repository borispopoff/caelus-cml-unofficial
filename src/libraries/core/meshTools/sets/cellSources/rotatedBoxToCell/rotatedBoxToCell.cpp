/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2018 OpenFOAM Foundation
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

#include "rotatedBoxToCell.hpp"
#include "polyMesh.hpp"
#include "cellModeller.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(rotatedBoxToCell, 0);
    addToRunTimeSelectionTable(topoSetSource, rotatedBoxToCell, word);
    addToRunTimeSelectionTable(topoSetSource, rotatedBoxToCell, istream);
}


CML::topoSetSource::addToUsageTable CML::rotatedBoxToCell::usage_
(
    rotatedBoxToCell::typeName,
    "\n    Usage: rotatedBoxToCell (originx originy originz)"
    " (ix iy iz) (jx jy jz) (kx ky kz)\n\n"
    "    Select all cells with cellCentre within parallelopiped\n\n"
);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::rotatedBoxToCell::combine(topoSet& set, const bool add) const
{
    // Define a cell for the box
    pointField boxPoints(8);
    boxPoints[0] = origin_;
    boxPoints[1] = origin_ + i_;
    boxPoints[2] = origin_ + i_ + j_;
    boxPoints[3] = origin_ + j_;
    boxPoints[4] = origin_ + k_;
    boxPoints[5] = origin_ + k_ + i_;
    boxPoints[6] = origin_ + k_ + i_ + j_;
    boxPoints[7] = origin_ + k_ + j_;

    labelList boxVerts(8);
    forAll(boxVerts, i)
    {
        boxVerts[i] = i;
    }

    const cellModel& hex = *(cellModeller::lookup("hex"));

    // Get outwards pointing faces.
    faceList boxFaces(cellShape(hex, boxVerts).faces());

    // Precalculate normals
    vectorField boxFaceNormals(boxFaces.size());
    forAll(boxFaces, i)
    {
        boxFaceNormals[i] = boxFaces[i].area(boxPoints);
    }

    // Check whether cell centre is inside all faces of box.

    const pointField& ctrs = mesh_.cellCentres();

    forAll(ctrs, celli)
    {
        bool inside = true;

        forAll(boxFaces, i)
        {
            const face& f = boxFaces[i];

            if (((ctrs[celli] - boxPoints[f[0]]) & boxFaceNormals[i]) > 0)
            {
                inside = false;
                break;
            }
        }

        if (inside)
        {
            addOrDelete(set, celli, add);
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
CML::rotatedBoxToCell::rotatedBoxToCell
(
    const polyMesh& mesh,
    const vector& origin,
    const vector& i,
    const vector& j,
    const vector& k
)
:
    topoSetSource(mesh),
    origin_(origin),
    i_(i),
    j_(j),
    k_(k)
{}


// Construct from dictionary
CML::rotatedBoxToCell::rotatedBoxToCell
(
    const polyMesh& mesh,
    const dictionary& dict
)
:
    topoSetSource(mesh),
    origin_(dict.lookup("origin")),
    i_(dict.lookup("i")),
    j_(dict.lookup("j")),
    k_(dict.lookup("k"))
{}


// Construct from Istream
CML::rotatedBoxToCell::rotatedBoxToCell(const polyMesh& mesh, Istream& is)
:
    topoSetSource(mesh),
    origin_(is),
    i_(is),
    j_(is),
    k_(is)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::rotatedBoxToCell::~rotatedBoxToCell()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::rotatedBoxToCell::applyToSet
(
    const topoSetSource::setAction action,
    topoSet& set
) const
{
    if ((action == topoSetSource::NEW) || (action == topoSetSource::ADD))
    {
        Info<< "    Adding cells with center within rotated box " << endl;

        combine(set, true);
    }
    else if (action == topoSetSource::DELETE)
    {
        Info<< "    Removing cells with center within rotated box " << endl;

        combine(set, false);
    }
}


// ************************************************************************* //
