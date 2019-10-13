/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
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

#include "fvOption.hpp"
#include "fvMesh.hpp"
#include "volFields.hpp"
#include "fvsPatchFields.hpp"
#include "ListOps.hpp"
#include "addToRunTimeSelectionTable.hpp"

namespace CML
{
    namespace fv
    {
        defineTypeNameAndDebug(option, 0);
        defineRunTimeSelectionTable(option, dictionary);
    }
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

bool CML::fv::option::alwaysApply() const
{
    return false;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::option::option
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    name_(name),
    modelType_(modelType),
    mesh_(mesh),
    dict_(dict),
    coeffs_(dict.subDict(modelType + "Coeffs")),
    active_(dict_.lookupOrDefault<Switch>("active", true)),
    fieldNames_(),
    applied_()
{
    Info<< incrIndent << indent << "Source: " << name_ << endl << decrIndent;
}


// * * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * //

CML::autoPtr<CML::fv::option> CML::fv::option::New
(
    const word& name,
    const dictionary& coeffs,
    const fvMesh& mesh
)
{
    word modelType(coeffs.lookup("type"));

    Info<< indent
        << "Selecting finite volume options model type " << modelType << endl;

    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(modelType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown Model type " << modelType << nl << nl
            << "Valid model types are:" << nl
            << dictionaryConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    return autoPtr<option>(cstrIter()(name, modelType, coeffs, mesh));
}


CML::fv::option::~option()
{}


bool CML::fv::option::isActive()
{
    return active_;
}


CML::label CML::fv::option::applyToField(const word& fieldName) const
{
    if (alwaysApply())
    {
        return 0;
    }

    return findIndex(fieldNames_, fieldName);
}


void CML::fv::option::checkApplied() const
{
    forAll(applied_, i)
    {
        if (!applied_[i])
        {
            WarningInFunction
                << "Source " << name_ << " defined for field "
                << fieldNames_[i] << " but never used" << endl;
        }
    }
}


void CML::fv::option::addSup
(
    fvMatrix<scalar>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    fvMatrix<sphericalTensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    fvMatrix<symmTensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    fvMatrix<tensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    const volScalarField& rho,
    fvMatrix<sphericalTensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    const volScalarField& rho,
    fvMatrix<symmTensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    const volScalarField& rho,
    fvMatrix<tensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldi
)
{
    addSup(alpha*rho, eqn, fieldi);
}


void CML::fv::option::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    addSup(alpha*rho, eqn, fieldi);
}


void CML::fv::option::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<sphericalTensor>& eqn,
    const label fieldi
)
{
    addSup(alpha*rho, eqn, fieldi);
}


void CML::fv::option::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<symmTensor>& eqn,
    const label fieldi
)
{
    addSup(alpha*rho, eqn, fieldi);
}


void CML::fv::option::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<tensor>& eqn,
    const label fieldi
)
{
    addSup(alpha*rho, eqn, fieldi);
}


void CML::fv::option::constrain(fvMatrix<scalar>& eqn, const label fieldi)
{}


void CML::fv::option::constrain(fvMatrix<vector>& eqn, const label fieldi)
{}


void CML::fv::option::constrain
(
    fvMatrix<sphericalTensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::constrain
(
    fvMatrix<symmTensor>& eqn,
    const label fieldi
)
{}


void CML::fv::option::constrain(fvMatrix<tensor>& eqn, const label fieldi)
{}


void CML::fv::option::correct(volScalarField& field)
{}


void CML::fv::option::correct(volVectorField& field)
{}


void CML::fv::option::correct(volSphericalTensorField& field)
{}


void CML::fv::option::correct(volSymmTensorField& field)
{}


void CML::fv::option::correct(volTensorField& field)
{}


// ************************************************************************* //
