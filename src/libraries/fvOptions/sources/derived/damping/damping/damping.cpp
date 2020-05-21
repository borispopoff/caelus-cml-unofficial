/*---------------------------------------------------------------------------*\
Copyright (C) 2017-2019 OpenFOAM Foundation
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

#include "damping.hpp"
#include "fvMatrix.hpp"
#include "zeroGradientFvPatchField.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{
    defineTypeNameAndDebug(damping, 1);
}
}


// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

CML::tmp<CML::volScalarField> CML::fv::damping::forceCoeff() const
{
    tmp<volScalarField> tforceCoeff
    (
        new volScalarField
        (
            IOobject
            (
                type() + ":forceCoeff",
                mesh_.time().timeName(),
                mesh_
            ),
            mesh_,
            dimensionedScalar("forceCoeff",lambda_.dimensions(), scale_.valid() ? 0 : 1)
        )
    );
    scalarField& forceCoeff = tforceCoeff();

    forAll(origins_, i)
    {
        const vectorField& c = mesh_.cellCentres();
        const scalarField x((c - origins_[i]) & directions_[i]);
        forceCoeff = max(forceCoeff, scale_->value(x));
    }

    forceCoeff *= lambda_.value();

    // Write out the force coefficient for debugging
    if (debug && mesh_.time().outputTime())
    {
        volScalarField vForceCoeff
        (
            IOobject
            (
                type() + ":forceCoeff",
                mesh_.time().timeName(),
                mesh_
            ),
            mesh_,
            lambda_.dimensions(),
            zeroGradientFvPatchField<scalar>::typeName
        );
        vForceCoeff.internalField() = forceCoeff;
        vForceCoeff.correctBoundaryConditions();
        vForceCoeff.write();
    }

    return tforceCoeff;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::damping::damping
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    option(name, modelType, dict, mesh),
    lambda_("lambda", dimless/dimTime, coeffs_.lookup("lambda")),
    scale_(),
    origins_(),
    directions_()
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CML::fv::damping::read(const dictionary& dict)
{
    if (option::read(dict))
    {
        lambda_ =
            dimensionedScalar
            (
                lambda_.name(),
                lambda_.dimensions(),
                coeffs_.lookup(lambda_.name())
            );

        const bool foundScale = coeffs_.found("scale");
        const bool foundOgn = coeffs_.found("origin");
        const bool foundDir = coeffs_.found("direction");
        const bool foundOgns = coeffs_.found("origins");
        const bool foundDirs = coeffs_.found("directions");
        const bool foundAll =
            foundScale
         && (
                (foundOgn && foundDir && !foundOgns && !foundDirs)
             || (!foundOgn && !foundDir && foundOgns && foundDirs)
            );
         const bool foundAny =
            foundScale || foundOgn || foundDir || foundOgns || foundDirs;

        if (!foundAll)
        {
            scale_ = autoPtr<DataEntry<scalar>>();
            origins_.clear();
            directions_.clear();
        }

        if (foundAll)
        {
            scale_ = DataEntry<scalar>::New("scale", coeffs_);
            if (foundOgn)
            {
                origins_.setSize(1);
                directions_.setSize(1);
                coeffs_.lookup("origin") >> origins_.last();
                coeffs_.lookup("direction") >> directions_.last();
            }
            else
            {
                coeffs_.lookup("origins") >> origins_;
                coeffs_.lookup("directions") >> directions_;

                if
                (
                    origins_.size() == 0
                 || directions_.size() == 0
                 || origins_.size() != directions_.size()
                )
                {
                    FatalErrorInFunction
                        << "The same, non-zero number of origins and "
                        << "directions must be provided" << exit(FatalError);
                }
            }
            forAll(directions_, i)
            {
                directions_[i] /= mag(directions_[i]);
            }
        }

        if (!foundAll && foundAny)
        {
            WarningInFunction
                << "The scaling specification is incomplete. \"scale\", "
                << "\"origin\" and \"direction\" (or \"origins\" and "
                << "\"directions\"), must all be specified in order to scale "
                << "the damping. The damping will be applied uniformly across "
                << "the cell set." << endl << endl;
        }

        fieldNames_ = wordList(1, coeffs_.lookupOrDefault<word>("U", "U"));

        applied_.setSize(1, false);

        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
