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

#include "maxwellSlipUFvPatchVectorField.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "mathematicalConstants.hpp"
#include "fvPatchFieldMapper.hpp"
#include "volFields.hpp"
#include "fvcGrad.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

maxwellSlipUFvPatchVectorField::maxwellSlipUFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    mixedFixedValueSlipFvPatchVectorField(p, iF),
    accommodationCoeff_(1.0),
    Uwall_(p.size(), vector(0.0, 0.0, 0.0)),
    thermalCreep_(true),
    curvature_(true)
{}


maxwellSlipUFvPatchVectorField::maxwellSlipUFvPatchVectorField
(
    const maxwellSlipUFvPatchVectorField& mspvf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mixedFixedValueSlipFvPatchVectorField(mspvf, p, iF, mapper),
    accommodationCoeff_(mspvf.accommodationCoeff_),
    Uwall_(mspvf.Uwall_),
    thermalCreep_(mspvf.thermalCreep_),
    curvature_(mspvf.curvature_)
{}


maxwellSlipUFvPatchVectorField::maxwellSlipUFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    mixedFixedValueSlipFvPatchVectorField(p, iF),
    accommodationCoeff_(readScalar(dict.lookup("accommodationCoeff"))),
    Uwall_("Uwall", dict, p.size()),
    thermalCreep_(dict.lookupOrDefault("thermalCreep", true)),
    curvature_(dict.lookupOrDefault("curvature", true))
{
    if
    (
        mag(accommodationCoeff_) < SMALL
     || mag(accommodationCoeff_) > 2.0
    )
    {
        FatalIOErrorInFunction
        (
            dict
        )   << "unphysical accommodationCoeff_ specified"
            << "(0 < accommodationCoeff_ <= 1)" << endl
            << exit(FatalIOError);
    }

    if (dict.found("value"))
    {
        fvPatchField<vector>::operator=
        (
            vectorField("value", dict, p.size())
        );

        if (dict.found("refValue") && dict.found("valueFraction"))
        {
            this->refValue() = vectorField("refValue", dict, p.size());
            this->valueFraction() =
                scalarField("valueFraction", dict, p.size());
        }
        else
        {
            this->refValue() = *this;
            this->valueFraction() = scalar(1);
        }
    }
}


maxwellSlipUFvPatchVectorField::maxwellSlipUFvPatchVectorField
(
    const maxwellSlipUFvPatchVectorField& mspvf,
    const DimensionedField<vector, volMesh>& iF
)
:
    mixedFixedValueSlipFvPatchVectorField(mspvf, iF),
    accommodationCoeff_(mspvf.accommodationCoeff_),
    Uwall_(mspvf.Uwall_),
    thermalCreep_(mspvf.thermalCreep_),
    curvature_(mspvf.curvature_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void maxwellSlipUFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const fvPatchScalarField& pmu =
        patch().lookupPatchField<volScalarField, scalar>("mu");
    const fvPatchScalarField& prho =
        patch().lookupPatchField<volScalarField, scalar>("rho");
    const fvPatchField<scalar>& ppsi =
        patch().lookupPatchField<volScalarField, scalar>("psi");

    Field<scalar> C1
    (
        sqrt(ppsi*constant::mathematical::piByTwo)
      * (2.0 - accommodationCoeff_)/accommodationCoeff_
    );

    Field<scalar> pnu(pmu/prho);
    valueFraction() = (1.0/(1.0 + patch().deltaCoeffs()*C1*pnu));

    refValue() = Uwall_;

    if (thermalCreep_)
    {
        const volScalarField& vsfT =
            this->db().objectRegistry::lookupObject<volScalarField>("T");
        label patchi = this->patch().index();
        const fvPatchScalarField& pT = vsfT.boundaryField()[patchi];
        Field<vector> gradpT(fvc::grad(vsfT)().boundaryField()[patchi]);
        vectorField n(patch().nf());

        refValue() -= 3.0*pnu/(4.0*pT)*transform(I - n*n, gradpT);
    }

    if (curvature_)
    {
        const fvPatchTensorField& ptauMC =
            patch().lookupPatchField<volTensorField, tensor>("tauMC");
        vectorField n(patch().nf());

        refValue() -= C1/prho*transform(I - n*n, (n & ptauMC));
    }

    mixedFixedValueSlipFvPatchVectorField::updateCoeffs();
}


void maxwellSlipUFvPatchVectorField::write(Ostream& os) const
{
    fvPatchVectorField::write(os);
    writeEntry(os, "accommodationCoeff", accommodationCoeff_);
    writeEntry(os, "Uwall", Uwall_);
    writeEntry(os, "thermalCreep", thermalCreep_);
    writeEntry(os, "curvature", curvature_);

    writeEntry(os, "refValue", refValue());
    writeEntry(os, "valueFraction", valueFraction());

    writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchVectorField,
    maxwellSlipUFvPatchVectorField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
