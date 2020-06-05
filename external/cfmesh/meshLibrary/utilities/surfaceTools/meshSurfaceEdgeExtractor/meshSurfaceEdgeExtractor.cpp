/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
-------------------------------------------------------------------------------
License
    This file is part of cfMesh.

    cfMesh is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    cfMesh is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with cfMesh.  If not, see <http://www.gnu.org/licenses/>.

Author: Franjo Juretic (franjo.juretic@c-fields.com)

\*---------------------------------------------------------------------------*/

#include "meshSurfaceEdgeExtractor.hpp"
#include "demandDrivenData.hpp"

// #define DEBUGSearch

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from mesh, octree, regions for boundary vertices
meshSurfaceEdgeExtractor::meshSurfaceEdgeExtractor
(
    polyMeshGen& mesh,
    const meshOctree& octree,
    const labelList& pointRegion
)
:
    mesh_(mesh),
    nPoints_(mesh.points().size()),
    boundaryCell_(mesh.cells().size(), false),
    nFacesInCell_(mesh.cells().size(), direction(0)),
    meshOctree_(octree),
    pointRegions_(pointRegion.size())
{
    forAll(pointRegion, pointI)
        if( pointRegion[pointI] != -1 )
            pointRegions_.append(pointI, pointRegion[pointI]);

    createEdgeVertices();

    removeOldBoundaryFaces();

    createBoundaryFaces();
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

meshSurfaceEdgeExtractor::~meshSurfaceEdgeExtractor()
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void meshSurfaceEdgeExtractor::removeOldBoundaryFaces()
{
    const labelList neighbour_ = mesh_.neighbour();
    polyMeshGenModifier meshModifier_(mesh_);
    cellListPMG& cells_ = meshModifier_.cellsAccess();

    forAll(cells_, celli)
    {
        const cell& c = cells_[celli];

        cell newC(c);

        forAll(c, fI)
            if( neighbour_[c[fI]] != -1 )
            {
                boundaryCell_[celli] = true;
                newC[nFacesInCell_[celli]++] = c[fI];
            }

        if( nFacesInCell_[celli] < direction(c.size()) )
        {
            newC.setSize(nFacesInCell_[celli]);

            cells_[celli] = newC;
        };
    }

    PtrList<boundaryPatch>& boundaries = meshModifier_.boundariesAccess();
    boundaries.setSize(1);
    boundaries[0].patchSize() = 0;
    meshModifier_.facesAccess().setSize(boundaries[0].patchStart());
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
