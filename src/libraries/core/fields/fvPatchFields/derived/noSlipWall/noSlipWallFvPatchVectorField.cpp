/*---------------------------------------------------------------------------*\
Copyright (C) 2016-2019 Applied CCM
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

#include "noSlipWallFvPatchVectorField.hpp"
#include "addToRunTimeSelectionTable.hpp"

CML::noSlipWallFvPatchVectorField::noSlipWallFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(p, iF)
{
    operator==(Zero);
}


CML::noSlipWallFvPatchVectorField::noSlipWallFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchVectorField(p, iF, dict, false)
{
    operator==(Zero);
}


CML::noSlipWallFvPatchVectorField::noSlipWallFvPatchVectorField
(
    const noSlipWallFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchVectorField(ptf, p, iF, mapper, false) // Don't map
{
    operator==(Zero);
}


CML::noSlipWallFvPatchVectorField::noSlipWallFvPatchVectorField
(
    const noSlipWallFvPatchVectorField& ptf
)
:
    fixedValueFvPatchVectorField(ptf)
{}


CML::noSlipWallFvPatchVectorField::noSlipWallFvPatchVectorField
(
    const noSlipWallFvPatchVectorField& ptf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(ptf, iF)
{}

CML::tmp<CML::Field<CML::vector>>
CML::noSlipWallFvPatchVectorField::valueInternalCoeffs
(
    tmp<scalarField> const&
) const
{
    return CML::tmp<CML::Field<vector>>
    (
        new CML::Field<vector>(this->size(), pTraits<vector>::zero)
    );
}

CML::tmp<CML::Field<CML::vector>>
CML::noSlipWallFvPatchVectorField::valueBoundaryCoeffs
(
    tmp<scalarField> const&
) const
{
    return *this;
}

CML::tmp<CML::Field<CML::vector>>
CML::noSlipWallFvPatchVectorField::gradientInternalCoeffs() const
{
    // Field of normal vectors
    vectorField const n = (this->patch().nf())();

    // Initialize implicit coefficients
    vectorField coeffs(this->size(),pTraits<vector>::zero);
 
    forAll(coeffs,f)
    {
        coeffs[f][0] = scalar(1) - n[f][0]*n[f][0];
        coeffs[f][1] = scalar(1) - n[f][1]*n[f][1];
        coeffs[f][2] = scalar(1) - n[f][2]*n[f][2];
    }

    return -coeffs*this->patch().deltaCoeffs();
}

CML::tmp<CML::Field<CML::vector>>
CML::noSlipWallFvPatchVectorField::gradientBoundaryCoeffs() const
{
    // Field of normal vectors
    vectorField const n = (this->patch().nf())();

    // Initialize explicit coefficients
    vectorField coeffs(this->size(), pTraits<vector>::zero);

    // Field of tangent vectors
    vectorField const t = (*this - ((*this) & n)*n)();
    vectorField const iF = (this->patchInternalField())();

    forAll(coeffs,f)
    {
        coeffs[f][0] = t[f][0]+n[f][0]*n[f][1]*iF[f][1]
                     + n[f][2]*n[f][0]*iF[f][2];
        coeffs[f][1] = t[f][1]+n[f][1]*n[f][0]*iF[f][0]
                     + n[f][2]*n[f][1]*iF[f][2];
        coeffs[f][2] = t[f][2]+n[f][2]*n[f][0]*iF[f][0]
                     + n[f][1]*n[f][2]*iF[f][1];
    }
  
    return coeffs*this->patch().deltaCoeffs();
}

void CML::noSlipWallFvPatchVectorField::write(Ostream& os) const
{
    fvPatchVectorField::write(os);
    writeEntry(os, "value", *this);
}

namespace CML
{
    makePatchTypeField
    (
        fvPatchVectorField,
        noSlipWallFvPatchVectorField
    );
}




