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

#include "pointToFace.hpp"
#include "polyMesh.hpp"
#include "pointSet.hpp"

#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(pointToFace, 0);
    addToRunTimeSelectionTable(topoSetSource, pointToFace, word);
    addToRunTimeSelectionTable(topoSetSource, pointToFace, istream);

    template<>
    const char* CML::NamedEnum
    <
        CML::pointToFace::pointAction,
        2
    >::names[] =
    {
        "any",
        "all"
    };
}


CML::topoSetSource::addToUsageTable CML::pointToFace::usage_
(
    pointToFace::typeName,
    "\n    Usage: pointToFace <pointSet> any|all\n\n"
    "    Select faces with\n"
    "    -any point in the pointSet\n"
    "    -all points in the pointSet\n\n"
);

const CML::NamedEnum<CML::pointToFace::pointAction, 2>
    CML::pointToFace::pointActionNames_;


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::pointToFace::combine(topoSet& set, const bool add) const
{
    // Load the set
    pointSet loadedSet(mesh_, setName_);

    if (option_ == ANY)
    {
        // Add faces with any point in loadedSet
        forAllConstIter(pointSet, loadedSet, iter)
        {
            const label pointi = iter.key();
            const labelList& pFaces = mesh_.pointFaces()[pointi];

            forAll(pFaces, pFaceI)
            {
                addOrDelete(set, pFaces[pFaceI], add);
            }
        }
    }
    else if (option_ == ALL)
    {
        // Add all faces whose points are all in set.

        // Count number of points using face.
        Map<label> numPoints(loadedSet.size());

        forAllConstIter(pointSet, loadedSet, iter)
        {
            const label pointi = iter.key();
            const labelList& pFaces = mesh_.pointFaces()[pointi];

            forAll(pFaces, pFaceI)
            {
                const label facei = pFaces[pFaceI];

                Map<label>::iterator fndFace = numPoints.find(facei);

                if (fndFace == numPoints.end())
                {
                    numPoints.insert(facei, 1);
                }
                else
                {
                    fndFace()++;
                }
            }
        }


        // Include faces that are referenced as many times as there are points
        // in face -> all points of face
        forAllConstIter(Map<label>, numPoints, iter)
        {
            const label facei = iter.key();

            if (iter() == mesh_.faces()[facei].size())
            {
                addOrDelete(set, facei, add);
            }
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
CML::pointToFace::pointToFace
(
    const polyMesh& mesh,
    const word& setName,
    const pointAction option
)
:
    topoSetSource(mesh),
    setName_(setName),
    option_(option)
{}


// Construct from dictionary
CML::pointToFace::pointToFace
(
    const polyMesh& mesh,
    const dictionary& dict
)
:
    topoSetSource(mesh),
    setName_(dict.lookup("set")),
    option_(pointActionNames_.read(dict.lookup("option")))
{}


// Construct from Istream
CML::pointToFace::pointToFace
(
    const polyMesh& mesh,
    Istream& is
)
:
    topoSetSource(mesh),
    setName_(checkIs(is)),
    option_(pointActionNames_.read(checkIs(is)))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::pointToFace::~pointToFace()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::pointToFace::applyToSet
(
    const topoSetSource::setAction action,
    topoSet& set
) const
{
    if ((action == topoSetSource::NEW) || (action == topoSetSource::ADD))
    {
        Info<< "    Adding faces according to pointSet " << setName_
            << " ..." << endl;

        combine(set, true);
    }
    else if (action == topoSetSource::DELETE)
    {
        Info<< "    Removing faces according to pointSet " << setName_
            << " ..." << endl;

        combine(set, false);
    }
}


// ************************************************************************* //
