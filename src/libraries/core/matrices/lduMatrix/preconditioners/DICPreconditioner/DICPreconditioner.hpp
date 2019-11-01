/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
Copyright (C) 2014 - 2019 Applied CCM
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

Class

    CML::DICPreconditioner

Description

    Simplified diagonal-based incomplete Cholesky preconditioner for symmetric
    matrices (symmetric equivalent of DILU).  The reciprocal of the
    preconditioned diagonal is calculated and stored. Guarding against zero
    pivots is added to prevent FPE.

SourceFiles

    DICPreconditioner.cpp

Modifications

    Aleksandar Jemcov

References

    [1] Templates for the Solution of Linear Systems: Building Blocks
        for Iterative Methods, R. Barrett, M. Barry, T.F. Chan, J. Demmel,
        J. Donato, J. Dongarra, V. Eijkhout, R. Pozo, C. Romine, and
        Van der Vorst, SIAM, 1994, Philadephia, PA, 2nd edition

    [2] Iterative Methods for Sparse Linear Systems, Y. Saad, SIAM, 2003,
        Philadephia, PA, 2nd edition

\*---------------------------------------------------------------------------*/

#ifndef DICPreconditioner_HPP
#define DICPreconditioner_HPP

#include "lduMatrix.hpp"

namespace CML
{

class DICPreconditioner : public lduMatrix::preconditioner
{
    scalarField rD_;
    scalarField rDuUpper_;
    scalarField rDlUpper_;


public:

    //- Runtime type information
    TypeName("DIC");


    // Constructors

    //- Construct from matrix components and preconditioner solver controls
    DICPreconditioner
    (
        lduMatrix::solver const&,
        dictionary const& solverControlsUnused
    );

    //- Disallow default bitwise copy construct
    DICPreconditioner(const DICPreconditioner&) = delete;


    //- Destructor
    virtual ~DICPreconditioner()
    {}


    // Member Functions

    void approximateInverse();

    //- Return w the preconditioned form of residual r
    virtual void precondition
    (
        scalarField& w,
        scalarField const& r,
        direction const cmpt=0
    ) const;

    // Member Operators

    //- Disallow default bitwise assignment
    void operator=(const DICPreconditioner&) = delete;
};

}

#endif

