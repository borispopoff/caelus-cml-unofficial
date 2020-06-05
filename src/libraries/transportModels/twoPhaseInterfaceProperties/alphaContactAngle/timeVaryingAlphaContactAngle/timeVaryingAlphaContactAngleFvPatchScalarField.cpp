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

#include "timeVaryingAlphaContactAngleFvPatchScalarField.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "fvPatchFieldMapper.hpp"
#include "volMesh.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::timeVaryingAlphaContactAngleFvPatchScalarField::
timeVaryingAlphaContactAngleFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    alphaContactAngleFvPatchScalarField(p, iF),
    t0_(0.0),
    thetaT0_(0.0),
    te_(0.0),
    thetaTe_(0.0)
{}


CML::timeVaryingAlphaContactAngleFvPatchScalarField::
timeVaryingAlphaContactAngleFvPatchScalarField
(
    const timeVaryingAlphaContactAngleFvPatchScalarField& gcpsf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    alphaContactAngleFvPatchScalarField(gcpsf, p, iF, mapper),
    t0_(gcpsf.t0_),
    thetaT0_(gcpsf.thetaT0_),
    te_(gcpsf.te_),
    thetaTe_(gcpsf.te_)
{}


CML::timeVaryingAlphaContactAngleFvPatchScalarField::
timeVaryingAlphaContactAngleFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    alphaContactAngleFvPatchScalarField(p, iF, dict),
    t0_(readScalar(dict.lookup("t0"))),
    thetaT0_(readScalar(dict.lookup("thetaT0"))),
    te_(readScalar(dict.lookup("te"))),
    thetaTe_(readScalar(dict.lookup("thetaTe")))
{
    evaluate();
}


CML::timeVaryingAlphaContactAngleFvPatchScalarField::
timeVaryingAlphaContactAngleFvPatchScalarField
(
    const timeVaryingAlphaContactAngleFvPatchScalarField& gcpsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    alphaContactAngleFvPatchScalarField(gcpsf, iF),
    t0_(gcpsf.t0_),
    thetaT0_(gcpsf.thetaT0_),
    te_(gcpsf.te_),
    thetaTe_(gcpsf.thetaTe_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::tmp<CML::scalarField>
CML::timeVaryingAlphaContactAngleFvPatchScalarField::theta
(
    const fvPatchVectorField&,
    const fvsPatchVectorField&
) const
{
    scalar t = patch().boundaryMesh().mesh().time().value();
    scalar theta0 = thetaT0_;

    if (t < t0_)
    {
        theta0 = thetaT0_;
    }
    else if (t > te_)
    {
        theta0 = thetaTe_;
    }
    else
    {
        theta0 = thetaT0_ + (t - t0_)*(thetaTe_ - thetaT0_)/(te_ - t0_);
    }

    return tmp<scalarField>(new scalarField(size(), theta0));
}


void CML::timeVaryingAlphaContactAngleFvPatchScalarField::write
(
    Ostream& os
) const
{
    alphaContactAngleFvPatchScalarField::write(os);
    writeEntry(os, "t0", t0_);
    writeEntry(os, "thetaT0", thetaT0_);
    writeEntry(os, "te", te_);
    writeEntry(os, "thetaTe", thetaTe_);
    writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
    makePatchTypeField
    (
        fvPatchScalarField,
        timeVaryingAlphaContactAngleFvPatchScalarField
    );
}

// ************************************************************************* //
