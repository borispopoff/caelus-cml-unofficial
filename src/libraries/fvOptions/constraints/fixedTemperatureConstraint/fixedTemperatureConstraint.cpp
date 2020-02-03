/*---------------------------------------------------------------------------*\
Copyright (C) 2012-2016 OpenFOAM Foundation
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

#include "fixedTemperatureConstraint.hpp"
#include "fvMesh.hpp"
#include "fvMatrices.hpp"
#include "basicThermo.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    namespace fv
    {
        defineTypeNameAndDebug(fixedTemperatureConstraint, 0);
        addToRunTimeSelectionTable
        (
            option,
            fixedTemperatureConstraint,
            dictionary
        );
    }

    template<>
    const char* NamedEnum<fv::fixedTemperatureConstraint::temperatureMode, 2>::
    names[] =
    {
        "uniform",
        "lookup"
    };
}

const CML::NamedEnum<CML::fv::fixedTemperatureConstraint::temperatureMode, 2>
    CML::fv::fixedTemperatureConstraint::temperatureModeNames_;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::fixedTemperatureConstraint::fixedTemperatureConstraint
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    cellSetOption(name, modelType, dict, mesh),
    mode_(temperatureModeNames_.read(coeffs_.lookup("mode"))),
    Tuniform_(nullptr),
    TName_("T")
{
    switch (mode_)
    {
        case tmUniform:
        {
            Tuniform_.reset
            (
                DataEntry<scalar>::New("temperature", coeffs_).ptr()
            );
            break;
        }
        case tmLookup:
        {
            TName_ = coeffs_.lookupOrDefault<word>("T", "T");
            break;
        }
        default:
        {
            // error handling done by NamedEnum
        }
    }


    // Set the field name to that of the energy field from which the temperature
    // is obtained

    const basicThermo& thermo =
        mesh_.lookupObject<basicThermo>(basicThermo::dictName);

    fieldNames_.setSize(1, thermo.he().name());

    applied_.setSize(1, false);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::fv::fixedTemperatureConstraint::constrain
(
    fvMatrix<scalar>& eqn,
    const label
)
{
    const basicThermo& thermo =
        mesh_.lookupObject<basicThermo>(basicThermo::dictName);

    switch (mode_)
    {
        case tmUniform:
        {
            const scalar t = mesh_.time().value();
            scalarField Tuni(cells_.size(), Tuniform_->value(t));
            eqn.setValues(cells_, thermo.he(thermo.p(), Tuni, cells_));

            break;
        }
        case tmLookup:
        {
            const volScalarField& T =
                mesh().lookupObject<volScalarField>(TName_);

            scalarField Tlkp(T, cells_);
            eqn.setValues(cells_, thermo.he(thermo.p(), Tlkp, cells_));

            break;
        }
        default:
        {
            // error handling done by NamedEnum
        }
    }
}


bool CML::fv::fixedTemperatureConstraint::read(const dictionary& dict)
{
    if (cellSetOption::read(dict))
    {
        if (coeffs_.found(Tuniform_->name()))
        {
            Tuniform_.reset
            (
                DataEntry<scalar>::New(Tuniform_->name(), dict).ptr()
            );
        }

        coeffs_.readIfPresent("T", TName_);

        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
