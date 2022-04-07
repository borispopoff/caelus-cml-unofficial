/*---------------------------------------------------------------------------*\
Copyright (C) 2019 OpenFOAM Foundation
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

#include "isotropicDamping.hpp"
#include "fvMatrix.hpp"
#include "fvmSup.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{
    defineTypeNameAndDebug(isotropicDamping, 0);
    addToRunTimeSelectionTable(option, isotropicDamping, dictionary);
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::fv::isotropicDamping::add
(
    const volScalarField& forceCoeff,
    fvMatrix<vector>& eqn
)
{
    eqn -= fvm::Sp(forceCoeff, eqn.psi());
    eqn += forceCoeff*value_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::isotropicDamping::isotropicDamping
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    damping(name, modelType, dict, mesh),
    value_("value", dimVelocity, coeffs_.lookup("value"))
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::fv::isotropicDamping::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    add(this->forceCoeff(), eqn);
}


void CML::fv::isotropicDamping::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    add(rho*forceCoeff(), eqn);
}


void CML::fv::isotropicDamping::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    add(alpha*rho*this->forceCoeff(), eqn);
}


bool CML::fv::isotropicDamping::read(const dictionary& dict)
{
    if (damping::read(dict))
    {
        value_ =
            dimensionedVector
            (
                value_.name(),
                value_.dimensions(),
                coeffs_.lookup(value_.name())
            );

        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
