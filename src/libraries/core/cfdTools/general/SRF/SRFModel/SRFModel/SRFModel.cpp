/*---------------------------------------------------------------------------* \
Copyright (C) 2011-2018 OpenFOAM Foundation
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

#include "SRFModel.hpp"
#include "SRFVelocityFvPatchVectorField.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    namespace SRF
    {
        defineTypeNameAndDebug(SRFModel, 0);
        defineRunTimeSelectionTable(SRFModel, dictionary);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::SRF::SRFModel::SRFModel
(
    const word& type,
    const volVectorField& Urel
)
:
    IOdictionary
    (
        IOobject
        (
            "SRFProperties",
            Urel.time().constant(),
            Urel.db(),
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    ),
    Urel_(Urel),
    mesh_(Urel_.mesh()),
    origin_("origin", dimLength, lookup("origin")),
    axis_(lookup("axis")),
    SRFModelCoeffs_(subDict(type + "Coeffs")),
    omega_(dimensionedVector("omega", dimless/dimTime, Zero))
{
    // Normalise the axis
    axis_ /= mag(axis_);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::SRF::SRFModel::~SRFModel()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CML::SRF::SRFModel::read()
{
    if (regIOobject::read())
    {
        // Re-read origin
        lookup("origin") >> origin_;

        // Re-read axis
        lookup("axis") >> axis_;
        axis_ /= mag(axis_);

        // Re-read sub-model coeffs
        SRFModelCoeffs_ = subDict(type() + "Coeffs");

        return true;
    }
    else
    {
        return false;
    }
}


const CML::dimensionedVector& CML::SRF::SRFModel::origin() const
{
    return origin_;
}


const CML::vector& CML::SRF::SRFModel::axis() const
{
    return axis_;
}


const CML::dimensionedVector& CML::SRF::SRFModel::omega() const
{
    return omega_;
}


CML::tmp<CML::DimensionedField<CML::vector, CML::volMesh>>
CML::SRF::SRFModel::Fcoriolis() const
{
    return tmp<DimensionedField<vector, volMesh>>
    (
        new DimensionedField<vector, volMesh>
        (
            IOobject
            (
                "Fcoriolis",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            2.0*omega_ ^ Urel_
        )
    );
}


CML::tmp<CML::DimensionedField<CML::vector, CML::volMesh>>
CML::SRF::SRFModel::Fcentrifugal() const
{
    return tmp<DimensionedField<vector, volMesh>>
    (
        new DimensionedField<vector, volMesh>
        (
            IOobject
            (
                "Fcentrifugal",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            omega_ ^ (omega_ ^ (mesh_.C() - origin_))
        )
    );
}


CML::tmp<CML::DimensionedField<CML::vector, CML::volMesh>>
CML::SRF::SRFModel::Su() const
{
    return Fcoriolis() + Fcentrifugal();
}


CML::vectorField CML::SRF::SRFModel::velocity
(
    const vectorField& positions
) const
{
    tmp<vectorField> tfld =
        omega_.value()
      ^ (
            (positions - origin_.value())
          - axis_*(axis_ & (positions - origin_.value()))
        );

    return tfld();
}


CML::tmp<CML::volVectorField> CML::SRF::SRFModel::U() const
{
    return tmp<volVectorField>
    (
        new volVectorField
        (
            IOobject
            (
                "Usrf",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            omega_
          ^ ((mesh_.C() - origin_) - axis_*(axis_ & (mesh_.C() - origin_)))
        )
    );
}


CML::tmp<CML::volVectorField> CML::SRF::SRFModel::Uabs() const
{
    tmp<volVectorField> Usrf = U();

    tmp<volVectorField> tUabs
    (
        new volVectorField
        (
            IOobject
            (
                "Uabs",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            Usrf
        )
    );

    volVectorField& Uabs = tUabs.ref();

    // Add SRF contribution to internal field
    Uabs.internalField() += Urel_.internalField();

    // Add Urel boundary contributions
    volVectorField::GeometricBoundaryField& Uabsbf = Uabs.boundaryFieldRef();
    const volVectorField::GeometricBoundaryField& bvf = Urel_.boundaryField();

    forAll(bvf, i)
    {
        if (isA<SRFVelocityFvPatchVectorField>(bvf[i]))
        {
            // Only include relative contributions from
            // SRFVelocityFvPatchVectorField's
            const SRFVelocityFvPatchVectorField& UrelPatch =
                refCast<const SRFVelocityFvPatchVectorField>(bvf[i]);
            if (UrelPatch.relative())
            {
                Uabsbf[i] += Urel_.boundaryField()[i];
            }
        }
        else
        {
            Uabsbf[i] += Urel_.boundaryField()[i];
        }
    }

    return tUabs;
}


// ************************************************************************* //
