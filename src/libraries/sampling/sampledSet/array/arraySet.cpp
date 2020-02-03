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

#include "arraySet.hpp"
#include "sampledSet.hpp"
#include "meshSearch.hpp"
#include "DynamicList.hpp"
#include "polyMesh.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "word.hpp"
#include "transform.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(arraySet, 0);
    addToRunTimeSelectionTable(sampledSet, arraySet, word);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::arraySet::calcSamples
(
    DynamicList<point>& samplingPts,
    DynamicList<label>& samplingCells,
    DynamicList<label>& samplingFaces,
    DynamicList<label>& samplingSegments,
    DynamicList<scalar>& samplingCurveDist
) const
{
    const meshSearch& queryMesh = searchEngine();

    label nTotalSamples
    (
        pointsDensity_.x()
       *pointsDensity_.y()
       *pointsDensity_.z()
    );

    List<point> sampleCoords(nTotalSamples);

    const scalar deltax = spanBox_.x()/(pointsDensity_.x() + 1);
    const scalar deltay = spanBox_.y()/(pointsDensity_.y() + 1);
    const scalar deltaz = spanBox_.z()/(pointsDensity_.z() + 1);

    label p(0);
    for (label k=1; k<=pointsDensity_.z(); k++)
    {
        for (label j=1; j<=pointsDensity_.y(); j++)
        {
            for (label i=1; i<=pointsDensity_.x(); i++)
            {
                vector t(deltax*i , deltay*j, deltaz*k);
                sampleCoords[p] = coordSys_.origin() + t;
                p++;
            }
        }
    }

    forAll(sampleCoords, i)
    {
        sampleCoords[i] = transform(coordSys_.R().R(), sampleCoords[i]);
    }

    forAll(sampleCoords, sampleI)
    {
        label celli = queryMesh.findCell(sampleCoords[sampleI]);

        if (celli != -1)
        {
            samplingPts.append(sampleCoords[sampleI]);
            samplingCells.append(celli);
            samplingFaces.append(-1);
            samplingSegments.append(0);
            samplingCurveDist.append(1.0 * sampleI);
        }
    }
}


void CML::arraySet::genSamples()
{
    // Storage for sample points
    DynamicList<point> samplingPts;
    DynamicList<label> samplingCells;
    DynamicList<label> samplingFaces;
    DynamicList<label> samplingSegments;
    DynamicList<scalar> samplingCurveDist;

    calcSamples
    (
        samplingPts,
        samplingCells,
        samplingFaces,
        samplingSegments,
        samplingCurveDist
    );

    samplingPts.shrink();
    samplingCells.shrink();
    samplingFaces.shrink();
    samplingSegments.shrink();
    samplingCurveDist.shrink();

    setSamples
    (
        samplingPts,
        samplingCells,
        samplingFaces,
        samplingSegments,
        samplingCurveDist
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::arraySet::arraySet
(
    const word& name,
    const polyMesh& mesh,
    const meshSearch& searchEngine,
    const word& axis,
    const coordinateSystem& origin,
    const Vector<label>& pointsDensity,
    const Vector<scalar>& spanBox
)
:
    sampledSet(name, mesh, searchEngine, axis),
    coordSys_(origin),
    pointsDensity_(pointsDensity),
    spanBox_(spanBox)
{
    genSamples();

    if (debug)
    {
        write(Info);
    }
}


CML::arraySet::arraySet
(
    const word& name,
    const polyMesh& mesh,
    const meshSearch& searchEngine,
    const dictionary& dict
)
:
    sampledSet(name, mesh, searchEngine, dict),
    coordSys_(dict),
    pointsDensity_(dict.lookup("pointsDensity")),
    spanBox_(dict.lookup("spanBox"))
{
    genSamples();

    if (debug)
    {
        write(Info);
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::arraySet::~arraySet()
{}


// ************************************************************************* //
