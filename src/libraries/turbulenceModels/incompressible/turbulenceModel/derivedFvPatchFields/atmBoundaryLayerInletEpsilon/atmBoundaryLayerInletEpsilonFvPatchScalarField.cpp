/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of CAELUS.

    CAELUS is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    CAELUS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with CAELUS; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "atmBoundaryLayerInletEpsilonFvPatchScalarField.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "fvPatchFieldMapper.hpp"
#include "volFields.hpp"
#include "surfaceFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace incompressible
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

atmBoundaryLayerInletEpsilonFvPatchScalarField::
atmBoundaryLayerInletEpsilonFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    atmBoundaryLayer()
{}


atmBoundaryLayerInletEpsilonFvPatchScalarField::
atmBoundaryLayerInletEpsilonFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF, dict, false),
    atmBoundaryLayer(patch().Cf(), dict)
{
    scalarField::operator=(epsilon(patch().Cf()));
}


atmBoundaryLayerInletEpsilonFvPatchScalarField::
atmBoundaryLayerInletEpsilonFvPatchScalarField
(
    const atmBoundaryLayerInletEpsilonFvPatchScalarField& psf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(psf, p, iF, mapper),
    atmBoundaryLayer(psf, mapper)
{}


atmBoundaryLayerInletEpsilonFvPatchScalarField::
atmBoundaryLayerInletEpsilonFvPatchScalarField
(
    const atmBoundaryLayerInletEpsilonFvPatchScalarField& psf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(psf, iF),
    atmBoundaryLayer(psf)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void atmBoundaryLayerInletEpsilonFvPatchScalarField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    fixedValueFvPatchScalarField::autoMap(m);
    atmBoundaryLayer::autoMap(m);
}


void atmBoundaryLayerInletEpsilonFvPatchScalarField::rmap
(
    const fvPatchScalarField& psf,
    const labelList& addr
)
{
    fixedValueFvPatchScalarField::rmap(psf, addr);

    const atmBoundaryLayerInletEpsilonFvPatchScalarField& blpsf =
        refCast<const atmBoundaryLayerInletEpsilonFvPatchScalarField>(psf);

    atmBoundaryLayer::rmap(blpsf, addr);
}


void atmBoundaryLayerInletEpsilonFvPatchScalarField::write(Ostream& os) const
{
    fvPatchScalarField::write(os);
    atmBoundaryLayer::write(os);
    writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchScalarField,
    atmBoundaryLayerInletEpsilonFvPatchScalarField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace incompressible
} // End namespace CML

// ************************************************************************* //
