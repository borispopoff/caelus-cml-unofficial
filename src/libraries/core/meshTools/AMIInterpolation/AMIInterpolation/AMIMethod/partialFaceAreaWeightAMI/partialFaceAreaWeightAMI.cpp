/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2018 OpenFOAM Foundation
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

#include "partialFaceAreaWeightAMI.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(partialFaceAreaWeightAMI, 0);
    addToRunTimeSelectionTable(AMIMethod, partialFaceAreaWeightAMI, components);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::partialFaceAreaWeightAMI::setNextFaces
(
    label& startSeedI,
    label& srcFacei,
    label& tgtFacei,
    const boolList& mapFlag,
    labelList& seedFaces,
    const DynamicList<label>& visitedFaces,
    const bool errorOnNotFound
) const
{
    faceAreaWeightAMI::setNextFaces
    (
        startSeedI,
        srcFacei,
        tgtFacei,
        mapFlag,
        seedFaces,
        visitedFaces,
        false // no error on not found
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::partialFaceAreaWeightAMI::partialFaceAreaWeightAMI
(
    const primitivePatch& srcPatch,
    const primitivePatch& tgtPatch,
    const scalarField& srcMagSf,
    const scalarField& tgtMagSf,
    const faceAreaIntersect::triangulationMode& triMode,
    const bool reverseTarget,
    const bool requireMatch
)
:
    faceAreaWeightAMI
    (
        srcPatch,
        tgtPatch,
        srcMagSf,
        tgtMagSf,
        triMode,
        reverseTarget,
        requireMatch
    )
{}


// * * * * * * * * * * * * * * * * Destructor * * * * * * * * * * * * * * * //

CML::partialFaceAreaWeightAMI::~partialFaceAreaWeightAMI()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CML::partialFaceAreaWeightAMI::conformal() const
{
    return false;
}


void CML::partialFaceAreaWeightAMI::calculate
(
    labelListList& srcAddress,
    scalarListList& srcWeights,
    labelListList& tgtAddress,
    scalarListList& tgtWeights,
    label srcFacei,
    label tgtFacei
)
{
    bool ok =
        this->initialise
        (
            srcAddress,
            srcWeights,
            tgtAddress,
            tgtWeights,
            srcFacei,
            tgtFacei
        );

    if (!ok)
    {
        return;
    }

    // temporary storage for addressing and weights
    List<DynamicList<label>> srcAddr(this->srcPatch_.size());
    List<DynamicList<scalar>> srcWght(srcAddr.size());
    List<DynamicList<label>> tgtAddr(this->tgtPatch_.size());
    List<DynamicList<scalar>> tgtWght(tgtAddr.size());

    faceAreaWeightAMI::calcAddressing
    (
        srcAddr,
        srcWght,
        tgtAddr,
        tgtWght,
        srcFacei,
        tgtFacei
    );

    // transfer data to persistent storage
    forAll(srcAddr, i)
    {
        srcAddress[i].transfer(srcAddr[i]);
        srcWeights[i].transfer(srcWght[i]);
    }
    forAll(tgtAddr, i)
    {
        tgtAddress[i].transfer(tgtAddr[i]);
        tgtWeights[i].transfer(tgtWght[i]);
    }
}


// ************************************************************************* //
