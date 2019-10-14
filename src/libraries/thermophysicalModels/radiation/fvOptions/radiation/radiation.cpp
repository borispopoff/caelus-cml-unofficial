/*---------------------------------------------------------------------------*\
Copyright (C) 2017 OpenFOAM Foundation
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

#include "radiation.hpp"
#include "fluidThermo.hpp"
#include "fvMatrices.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace CML
{
namespace fv
{
    defineTypeNameAndDebug(radiation, 0);

    addToRunTimeSelectionTable
    (
        option,
        radiation,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::radiation::radiation
(
    const word& sourceName,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    option(sourceName, modelType, dict, mesh)
{
    const basicThermo& thermo =
        mesh_.lookupObject<basicThermo>(basicThermo::dictName);

    fieldNames_.setSize(1);
    fieldNames_[0] = thermo.he().name();
    applied_.setSize(fieldNames_.size(), false);

    radiation_ = CML::radiation::radiationModel::New(thermo.T());
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CML::fv::radiation::read(const dictionary& dict)
{
    return option::read(dict);
}


void CML::fv::radiation::addSup
(
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldi
)
{
    const basicThermo& thermo =
        mesh_.lookupObject<basicThermo>(basicThermo::dictName);

    radiation_->correct();

    eqn += radiation_->Sh(thermo, eqn.psi());
}


// ************************************************************************* //
