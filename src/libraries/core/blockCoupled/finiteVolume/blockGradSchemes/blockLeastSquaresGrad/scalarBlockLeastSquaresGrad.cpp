/*---------------------------------------------------------------------------*\
Copyright (C) 2010-2015 H. Jasak
Copyright (C) 2014 V. Vukcevic
-------------------------------------------------------------------------------
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    Caelus is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

Description
    Specialisation of blockLeastSquaresGrad for scalars. Needed for implicit
    fvmGrad operator for block coupled systems.

    This file has been obtained from FOAM-Extend-4.0.
    https://sourceforge.net/p/foam-extend/foam-extend-4.0/

    Please report any omissions of authorship and/or copyright to
    info@appliedccm.com.au. Corrections will be made upon provision of proof.

\*---------------------------------------------------------------------------*/

#include "blockLeastSquaresGrad.hpp"
#include "surfaceMesh.hpp"
#include "fvsPatchFieldsFwd.hpp"
#include "fvsPatchField.hpp"


namespace CML
{
namespace fv
{

template<>
tmp<BlockLduSystem<vector, vector>> blockLeastSquaresGrad<scalar>::fvmGrad
(
    const GeometricField<scalar, fvPatchField, volMesh>& vf
) const
{
    const fvMesh& mesh = vf.mesh();

    const unallocLabelList& own = mesh.owner();
    const unallocLabelList& nei = mesh.neighbour();

    volScalarField cellV
    (
        IOobject
        (
            "cellV",
            vf.instance(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("zero", dimVolume, 0),
        zeroGradientFvPatchScalarField::typeName
    );
    cellV.primitiveFieldRef() = mesh.V();
    cellV.correctBoundaryConditions();
    const scalarField& cellVIn = cellV.primitiveField();

    const surfaceScalarField& w = mesh.weights();

    tmp<BlockLduSystem<vector, vector>> tbs
    (
        new BlockLduSystem<vector, vector>(mesh)
    );
    BlockLduSystem<vector, vector>& bs = tbs.ref();
    vectorField& source = bs.source();

    // Grab ldu parts of block matrix as linear always
    CoeffField<vector>::linearTypeField& d = bs.diag().asLinear();
    CoeffField<vector>::linearTypeField& u = bs.upper().asLinear();
    CoeffField<vector>::linearTypeField& l = bs.lower().asLinear();

    // Get references to least square vectors
    const leastSquaresVectors& lsv = leastSquaresVectors::New(mesh);

    const surfaceVectorField& ownLs = lsv.pVectors();
    const surfaceVectorField& neiLs = lsv.nVectors();

    // Internal field
    const vectorField& ownLsIn = ownLs.primitiveField();
    const vectorField& neiLsIn = neiLs.primitiveField();

    label owner, neighbour;

    forAll (nei, facei)
    {
        owner = own[facei];
        neighbour = nei[facei];

        u[facei] = cellVIn[owner]*ownLsIn[facei];
        l[facei] = cellVIn[neighbour]*neiLsIn[facei];

        // Caution - this is NOT negSumDiag(). VV, 17/July/2014.
        d[owner] -= u[facei];
        d[neighbour] -= l[facei];
    }

    // Boundary contributions
    forAll (vf.boundaryField(), patchi)
    {
        const fvPatchScalarField& pf = vf.boundaryField()[patchi];
        const fvPatch& patch = pf.patch();
        const vectorField& pownLs = ownLs.boundaryField()[patchi];
        const fvsPatchScalarField& pw = w.boundaryField()[patchi];
        const labelList& fc = patch.faceCells();

        // Part of diagonal contribution irrespective of the patch type
        forAll (pf, facei)
        {
            const label celli = fc[facei];
            d[celli] -= cellVIn[celli]*pownLs[facei];
        }

        if (patch.coupled())
        {
            const vectorField& pneiLs = neiLs.boundaryField()[patchi];
            const scalarField cellVInNei(
                cellV.boundaryField()[patchi].patchNeighbourField());

            CoeffField<vector>::linearTypeField& pcoupleUpper =
                bs.coupleUpper()[patchi].asLinear();
            CoeffField<vector>::linearTypeField& pcoupleLower =
                bs.coupleLower()[patchi].asLinear();

            // Coupling  and diagonal contributions
            forAll (pf, facei)
            {
                pcoupleUpper[facei] -= cellVIn[fc[facei]]*pownLs[facei];
                pcoupleLower[facei] -= cellVInNei[facei]*pneiLs[facei];
            }
        }
        else
        {
            const scalarField internalCoeffs(pf.valueInternalCoeffs(pw));
            const scalarField boundaryCoeffs(pf.valueBoundaryCoeffs(pw));

            // Diagonal and source contributions depending on the patch type
            forAll (pf, facei)
            {
                const label celli = fc[facei];

                d[celli] += cellVIn[celli]*pownLs[facei]*internalCoeffs[facei];

                source[celli] -= cellVIn[celli]*pownLs[facei]*
                    boundaryCoeffs[facei];
            }
        }
    }

    return tbs;
}


} // End namespace fv
} // End namespace CML
