/*---------------------------------------------------------------------------*\
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

#include "USGSPreconditioner.hpp"
#include "restrict.hpp"

namespace CML
{
    defineTypeNameAndDebug(USGSPreconditioner, 0);

    lduMatrix::preconditioner::
        addasymMatrixConstructorToTable<USGSPreconditioner>
        addUSGSPreconditionerAsymMatrixConstructorToTable_;
}

CML::USGSPreconditioner::USGSPreconditioner
(
    lduMatrix::solver const& sol,
    dictionary const&
) :
    lduMatrix::preconditioner(sol),
    rD_(sol.matrix().diag())
{
    this->approximateInverse(rD_, sol.matrix());
}

void CML::USGSPreconditioner::approximateInverse
(
    scalarField& rD,
    lduMatrix const& matrix
)
{
    scalarField const D(matrix.diag());
    scalar* RESTRICT rDPtr = rD.begin();
    scalar const* RESTRICT DPtr = D.begin();

    const label* const RESTRICT uPtr = matrix.lduAddr().upperAddr().begin();
    const label* const RESTRICT lPtr = matrix.lduAddr().lowerAddr().begin();

    const scalar* const RESTRICT upperPtr = matrix.upper().begin();
    const scalar* const RESTRICT lowerPtr = matrix.lower().begin();

    label nFaces = matrix.upper().size();

    for (label face=0; face<nFaces; face++)
    {
        rDPtr[uPtr[face]] -=
            upperPtr[face]*lowerPtr[face]/(DPtr[lPtr[face]]+SMALL);
    }
 
    // Calculate the reciprocal of the preconditioned diagonal
    label nCells = rD.size();

    for (label cell=0; cell<nCells; cell++)
    {
        rDPtr[cell] = scalar(1)/rDPtr[cell];
    }
}

void CML::USGSPreconditioner::precondition
(
    scalarField& w,
    scalarField const& r,
    direction const
) const
{
    scalar* RESTRICT wPtr = w.begin();
    scalar const* RESTRICT rPtr = r.begin();
    scalar const* RESTRICT rDPtr = rD_.begin();

    label const* const RESTRICT uPtr =
        solver_.matrix().lduAddr().upperAddr().begin();
    label const* const RESTRICT lPtr =
        solver_.matrix().lduAddr().lowerAddr().begin();
    label const* const RESTRICT losortPtr =
        solver_.matrix().lduAddr().losortAddr().begin();

    const scalar* const RESTRICT upperPtr =
        solver_.matrix().upper().begin();
    const scalar* const RESTRICT lowerPtr =
        solver_.matrix().lower().begin();

    label nCells = w.size();
    label nFaces = solver_.matrix().upper().size();
    label nFacesM1 = nFaces - 1;

    for (label cell=0; cell<nCells; cell++)
    {
        wPtr[cell] = rDPtr[cell]*rPtr[cell];
    }

    label sface;

    for (label face=0; face<nFaces; face++)
    {
        sface = losortPtr[face];
        wPtr[uPtr[sface]] -=
            rDPtr[uPtr[sface]]*lowerPtr[sface]*wPtr[lPtr[sface]];
    }

    for (label face=nFacesM1; face>=0; face--)
    {
        wPtr[lPtr[face]] -=
            rDPtr[lPtr[face]]*upperPtr[face]*wPtr[uPtr[face]];
    }
}

void CML::USGSPreconditioner::preconditionT
(
    scalarField& wT,
    scalarField const& rT,
    direction const
) const
{
    scalar* RESTRICT wTPtr = wT.begin();
    scalar const* RESTRICT rTPtr = rT.begin();
    scalar const* RESTRICT rDPtr = rD_.begin();

    const label* const RESTRICT uPtr =
        solver_.matrix().lduAddr().upperAddr().begin();
    const label* const RESTRICT lPtr =
        solver_.matrix().lduAddr().lowerAddr().begin();
    const label* const RESTRICT losortPtr =
        solver_.matrix().lduAddr().losortAddr().begin();

    const scalar* const RESTRICT upperPtr =
        solver_.matrix().upper().begin();
    const scalar* const RESTRICT lowerPtr =
        solver_.matrix().lower().begin();

    label nCells = wT.size();
    label nFaces = solver_.matrix().upper().size();
    label nFacesM1 = nFaces - 1;

    for (label cell=0; cell<nCells; cell++)
    {
        wTPtr[cell] = rDPtr[cell]*rTPtr[cell];
    }

    for (label face=0; face<nFaces; face++)
    {
        wTPtr[uPtr[face]] -=
            rDPtr[uPtr[face]]*upperPtr[face]*wTPtr[lPtr[face]];
    }

    label sface;

    for (label face=nFacesM1; face>=0; face--)
    {
        sface = losortPtr[face];
        wTPtr[lPtr[sface]] -=
            rDPtr[lPtr[sface]]*lowerPtr[sface]*wTPtr[uPtr[sface]];
    }
}

