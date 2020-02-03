/*---------------------------------------------------------------------------*\
Copyright (C) 2015 OpenFOAM Foundation
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

#include "parFvFieldReconstructor.hpp"


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::parFvFieldReconstructor::createPatchFaceMaps()
{
    const fvBoundaryMesh& fvb = procMesh_.boundary();

    patchFaceMaps_.setSize(fvb.size());
    forAll(fvb, patchi)
    {
        if (!isA<processorFvPatch>(fvb[patchi]))
        {
            // Create map for patch faces only

            // Mark all used elements (i.e. destination patch faces)
            boolList faceIsUsed(distMap_.faceMap().constructSize(), false);
            const polyPatch& basePatch = baseMesh_.boundaryMesh()[patchi];
            forAll(basePatch, i)
            {
                faceIsUsed[basePatch.start()+i] = true;
            }

            // Copy face map
            patchFaceMaps_.set
            (
                patchi,
                new mapDistributeBase(distMap_.faceMap())
            );

            // Compact out unused elements
            labelList oldToNewSub;
            labelList oldToNewConstruct;
            patchFaceMaps_[patchi].compact
            (
                faceIsUsed,
                procMesh_.nFaces(),      // maximum index of subMap
                oldToNewSub,
                oldToNewConstruct,
                UPstream::msgType()
            );
            //Pout<< "patchMap:" << patchFaceMaps_[patchi] << endl;
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::parFvFieldReconstructor::parFvFieldReconstructor
(
    fvMesh& baseMesh,
    const fvMesh& procMesh,
    const mapDistributePolyMesh& distMap,
    const bool isWriteProc
)
:
    baseMesh_(baseMesh),
    procMesh_(procMesh),
    distMap_(distMap),
    isWriteProc_(isWriteProc)
{
    createPatchFaceMaps();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::parFvFieldReconstructor::reconstructPoints()
{
    // Reconstruct the points for moving mesh cases and write
    // them out
    distributedUnallocatedDirectFieldMapper mapper
    (
        labelUList::null(),
        distMap_.pointMap()
    );
    pointField basePoints(procMesh_.points(), mapper);
    baseMesh_.movePoints(basePoints);
    if (Pstream::master())
    {
        baseMesh_.write();
    }
}


// ************************************************************************* //
