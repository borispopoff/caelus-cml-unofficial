/*---------------------------------------------------------------------------* \
Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "SRFVelocityFvPatchVectorField.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "volFields.hpp"

#include "SRFModel.hpp"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::SRFVelocityFvPatchVectorField::SRFVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(p, iF),
    relative_(0),
    inletValue_(p.size(), Zero)
{}


CML::SRFVelocityFvPatchVectorField::SRFVelocityFvPatchVectorField
(
    const SRFVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchVectorField(ptf, p, iF, mapper),
    relative_(ptf.relative_),
    inletValue_(ptf.inletValue_, mapper)
{}


CML::SRFVelocityFvPatchVectorField::SRFVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchVectorField(p, iF, dict),
    relative_(dict.lookup("relative")),
    inletValue_("inletValue", dict, p.size())
{}


CML::SRFVelocityFvPatchVectorField::SRFVelocityFvPatchVectorField
(
    const SRFVelocityFvPatchVectorField& srfvpvf
)
:
    fixedValueFvPatchVectorField(srfvpvf),
    relative_(srfvpvf.relative_),
    inletValue_(srfvpvf.inletValue_)
{}


CML::SRFVelocityFvPatchVectorField::SRFVelocityFvPatchVectorField
(
    const SRFVelocityFvPatchVectorField& srfvpvf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(srfvpvf, iF),
    relative_(srfvpvf.relative_),
    inletValue_(srfvpvf.inletValue_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::SRFVelocityFvPatchVectorField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    vectorField::autoMap(m);
    inletValue_.autoMap(m);
}


void CML::SRFVelocityFvPatchVectorField::rmap
(
    const fvPatchVectorField& ptf,
    const labelList& addr
)
{
    fixedValueFvPatchVectorField::rmap(ptf, addr);

    const SRFVelocityFvPatchVectorField& tiptf =
        refCast<const SRFVelocityFvPatchVectorField>(ptf);

    inletValue_.rmap(tiptf.inletValue_, addr);
}


void CML::SRFVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    // If not relative to the SRF include the effect of the SRF
    if (!relative_)
    {
        // Get reference to the SRF model
        const SRF::SRFModel& srf =
            db().lookupObject<SRF::SRFModel>("SRFProperties");

        // Determine patch velocity due to SRF
        const vectorField SRFVelocity(srf.velocity(patch().Cf()));

        operator==(-SRFVelocity + inletValue_);
    }
    // If already relative to the SRF simply supply the inlet value as a fixed
    // value
    else
    {
        operator==(inletValue_);
    }

    fixedValueFvPatchVectorField::updateCoeffs();
}


void CML::SRFVelocityFvPatchVectorField::write(Ostream& os) const
{
    fvPatchVectorField::write(os);
    os.writeKeyword("relative") << relative_ << token::END_STATEMENT << nl;
    inletValue_.writeEntry("inletValue", os);
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
    makePatchTypeField
    (
        fvPatchVectorField,
        SRFVelocityFvPatchVectorField
    );
}

// ************************************************************************* //
