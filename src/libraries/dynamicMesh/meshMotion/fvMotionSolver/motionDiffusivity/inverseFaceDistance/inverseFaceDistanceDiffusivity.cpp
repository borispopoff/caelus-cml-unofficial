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

#include "inverseFaceDistanceDiffusivity.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "HashSet.hpp"
#include "wallPoint.hpp"
#include "MeshWave.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(inverseFaceDistanceDiffusivity, 0);

    addToRunTimeSelectionTable
    (
        motionDiffusivity,
        inverseFaceDistanceDiffusivity,
        Istream
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::inverseFaceDistanceDiffusivity::inverseFaceDistanceDiffusivity
(
    const fvMesh& mesh,
    Istream& mdData
)
:
    uniformDiffusivity(mesh, mdData),
    patchNames_(mdData)
{
    correct();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::inverseFaceDistanceDiffusivity::~inverseFaceDistanceDiffusivity()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::inverseFaceDistanceDiffusivity::correct()
{
    const polyBoundaryMesh& bdry = mesh().boundaryMesh();

    labelHashSet patchSet(bdry.size());

    label nPatchFaces = 0;

    forAll(patchNames_, i)
    {
        const label pID = bdry.findPatchID(patchNames_[i]);

        if (pID > -1)
        {
            patchSet.insert(pID);
            nPatchFaces += bdry[pID].size();
        }
    }

    List<wallPoint> faceDist(nPatchFaces);
    labelList changedFaces(nPatchFaces);

    nPatchFaces = 0;

    forAllConstIter(labelHashSet, patchSet, iter)
    {
        const polyPatch& patch = bdry[iter.key()];

        const vectorField::subField fc(patch.faceCentres());

        forAll(fc, patchFacei)
        {
            changedFaces[nPatchFaces] = patch.start() + patchFacei;

            faceDist[nPatchFaces] = wallPoint(fc[patchFacei], 0);

            nPatchFaces++;
        }
    }
    faceDist.setSize(nPatchFaces);
    changedFaces.setSize(nPatchFaces);

    MeshWave<wallPoint> waveInfo
    (
        mesh(),
        changedFaces,
        faceDist,
        mesh().globalData().nTotalCells()+1   // max iterations
    );

    const List<wallPoint>& faceInfo = waveInfo.allFaceInfo();
    const List<wallPoint>& cellInfo = waveInfo.allCellInfo();

    for (label facei=0; facei<mesh().nInternalFaces(); facei++)
    {
        scalar dist = faceInfo[facei].distSqr();

        faceDiffusivity_[facei] = 1.0/sqrt(dist);
    }

    forAll(faceDiffusivity_.boundaryField(), patchi)
    {
        fvsPatchScalarField& bfld = faceDiffusivity_.boundaryField()[patchi];

        const labelUList& faceCells = bfld.patch().faceCells();

        if (patchSet.found(patchi))
        {
            forAll(bfld, i)
            {
                scalar dist = cellInfo[faceCells[i]].distSqr();
                bfld[i] = 1.0/sqrt(dist);
            }
        }
        else
        {
            const label start = bfld.patch().start();

            forAll(bfld, i)
            {
                scalar dist = faceInfo[start+i].distSqr();
                bfld[i] = 1.0/sqrt(dist);
            }
        }
    }
}


// ************************************************************************* //
