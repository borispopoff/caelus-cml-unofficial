/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
Copyright (C) 2016 Applied CCM
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

#include "DILUSmoother.hpp"
#include "DILUPreconditioner.hpp"
#include "restrict.hpp"

namespace CML
{
    defineTypeNameAndDebug(DILUSmoother, 0);

    lduMatrix::smoother::addasymMatrixConstructorToTable<DILUSmoother>
        addDILUSmootherAsymMatrixConstructorToTable_;
}

CML::DILUSmoother::DILUSmoother
(
    word const& fieldName,
    lduMatrix const& matrix,
    FieldField<Field, scalar> const& interfaceBouCoeffs,
    FieldField<Field, scalar> const& interfaceIntCoeffs,
    lduInterfaceFieldPtrsList const& interfaces
) :
    lduMatrix::smoother
    (
        fieldName,
        matrix,
        interfaceBouCoeffs,
        interfaceIntCoeffs,
        interfaces
    ),
    rD_(matrix_.diag())
{
    DILUPreconditioner::approximateInverse(rD_, matrix_);
}

void CML::DILUSmoother::smooth
(
    scalarField& psi,
    scalarField const& source,
    direction const cmpt,
    label const nSweeps
) const
{
    const scalar* const RESTRICT rDPtr = rD_.begin();

    const label* const RESTRICT uPtr =
        matrix_.lduAddr().upperAddr().begin();
    const label* const RESTRICT lPtr =
        matrix_.lduAddr().lowerAddr().begin();

    const scalar* const RESTRICT upperPtr = matrix_.upper().begin();
    const scalar* const RESTRICT lowerPtr = matrix_.lower().begin();

    // Temporary storage for the residual
    scalarField rA(rD_.size());
    scalar* RESTRICT rAPtr = rA.begin();

    for (label sweep=0; sweep<nSweeps; sweep++)
    {
        matrix_.residual
        (
            rA,
            psi,
            source,
            interfaceBouCoeffs_,
            interfaces_,
            cmpt
        );

        rA *= rD_;

        label nFaces = matrix_.upper().size();
        for (label face=0; face<nFaces; face++)
        {
            label u = uPtr[face];
            rAPtr[u] -= rDPtr[u]*lowerPtr[face]*rAPtr[lPtr[face]];
        }

        label nFacesM1 = nFaces - 1;
        for (label face=nFacesM1; face>=0; face--)
        {
            label l = lPtr[face];
            rAPtr[l] -= rDPtr[l]*upperPtr[face]*rAPtr[uPtr[face]];
        }

        psi += rA;
    }
}

