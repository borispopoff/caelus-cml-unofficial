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

#include "cyclicFvPatch.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "fvMesh.hpp"
#include "transform.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(cyclicFvPatch, 0);
    addToRunTimeSelectionTable(fvPatch, cyclicFvPatch, polyPatch);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::cyclicFvPatch::makeWeights(scalarField& w) const
{
    const cyclicFvPatch& nbrPatch = neighbFvPatch();

    const vectorField delta(coupledFvPatch::delta());
    const vectorField nbrDelta(nbrPatch.coupledFvPatch::delta());

    const scalarField nfDelta(nf() & delta);
    const scalarField nbrNfDelta(nbrPatch.nf() & nbrDelta);

    forAll(delta, facei)
    {
        const scalar ndoi = nfDelta[facei];
        const scalar ndni = nbrNfDelta[facei];
        const scalar ndi = ndoi + ndni;

        if (ndni/VGREAT < ndi)
        {
            w[facei] = ndni/ndi;
        }
        else
        {
            const scalar doi = mag(delta[facei]);
            const scalar dni = mag(nbrDelta[facei]);
            const scalar di = doi + dni;

            w[facei] = dni/di;
        }
    }
}


CML::tmp<CML::vectorField> CML::cyclicFvPatch::delta() const
{
    const vectorField patchD(coupledFvPatch::delta());
    const vectorField nbrPatchD(neighbFvPatch().coupledFvPatch::delta());

    tmp<vectorField> tpdv(new vectorField(patchD.size()));
    vectorField& pdv = tpdv.ref();

    // To the transformation if necessary
    if (parallel())
    {
        forAll(patchD, facei)
        {
            vector ddi = patchD[facei];
            vector dni = nbrPatchD[facei];

            pdv[facei] = ddi - dni;
        }
    }
    else
    {
        forAll(patchD, facei)
        {
            vector ddi = patchD[facei];
            vector dni = nbrPatchD[facei];

            pdv[facei] = ddi - transform(forwardT()[0], dni);
        }
    }

    return tpdv;
}


CML::tmp<CML::vectorField> CML::cyclicFvPatch::deltaFull() const
{
    return cyclicFvPatch::delta();
}


CML::tmp<CML::labelField> CML::cyclicFvPatch::interfaceInternalField
(
    const labelUList& internalData
) const
{
    return patchInternalField(internalData);
}


CML::tmp<CML::labelField> CML::cyclicFvPatch::internalFieldTransfer
(
    const Pstream::commsTypes commsType,
    const labelUList& iF
) const
{
    return neighbFvPatch().patchInternalField(iF);
}


// ************************************************************************* //
