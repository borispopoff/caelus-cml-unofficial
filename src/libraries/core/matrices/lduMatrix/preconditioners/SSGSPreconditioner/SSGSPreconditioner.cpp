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

#include "SSGSPreconditioner.hpp"
#include "restrict.hpp"

namespace CML
{
    defineTypeNameAndDebug(SSGSPreconditioner, 0);

    lduMatrix::preconditioner::
        addsymMatrixConstructorToTable<SSGSPreconditioner>
        addSSGSPreconditionerSymMatrixConstructorToTable_;
}

CML::SSGSPreconditioner::SSGSPreconditioner
(
    lduMatrix::solver const& sol,
    dictionary const&
) :
    lduMatrix::preconditioner(sol),
    rD_(sol.matrix().diag()),
    rDuUpper_(sol.matrix().upper().size()),
    rDlUpper_(sol.matrix().upper().size())
{
    this->approximateInverse();
}

void CML::SSGSPreconditioner::approximateInverse()
{
    scalarField const D(solver_.matrix().diag());
    scalar* RESTRICT rDPtr = rD_.begin();
    const scalar* const RESTRICT DPtr = D.begin();
    scalar* RESTRICT rDuUpperPtr = rDuUpper_.begin();
    scalar* RESTRICT rDlUpperPtr = rDlUpper_.begin();

    const label* const RESTRICT uPtr =
        solver_.matrix().lduAddr().upperAddr().begin();
    const label* const RESTRICT lPtr =
        solver_.matrix().lduAddr().lowerAddr().begin();
    const scalar* const RESTRICT upperPtr =
        solver_.matrix().upper().begin();

    label nCells = rD_.size();
    label nFaces = solver_.matrix().upper().size();

    for (label face=0; face<nFaces; face++)
    {
        rDPtr[uPtr[face]] -= sqr(upperPtr[face])/(DPtr[lPtr[face]]+SMALL);
    }

    // Generate reciprocal diagonal
    for (label cell=0; cell<nCells; cell++)
    {
        rDPtr[cell] = scalar(1)/rDPtr[cell];
    }

    for (label face=0; face<nFaces; face++)
    {
        rDuUpperPtr[face] = rDPtr[uPtr[face]]*upperPtr[face];
        rDlUpperPtr[face] = rDPtr[lPtr[face]]*upperPtr[face];
    }
}

void CML::SSGSPreconditioner::precondition
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

    const scalar* const RESTRICT rDuUpperPtr = rDuUpper_.begin();
    const scalar* const RESTRICT rDlUpperPtr = rDlUpper_.begin();

    label nCells = w.size();
    label nFaces = solver_.matrix().upper().size();
    label nFacesM1 = nFaces - 1;

    for (label cell=0; cell<nCells; cell++)
    {
        wPtr[cell] = rDPtr[cell]*rPtr[cell];
    }

    for (label face=0; face<nFaces; face++)
    {
        wPtr[uPtr[face]] -= rDuUpperPtr[face]*wPtr[lPtr[face]];
    }

    for (label face=nFacesM1; face>=0; face--)
    {
        wPtr[lPtr[face]] -= rDlUpperPtr[face]*wPtr[uPtr[face]];
    }
}

