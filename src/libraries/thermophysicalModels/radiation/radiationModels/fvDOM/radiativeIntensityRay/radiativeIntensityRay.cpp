/*---------------------------------------------------------------------------*\
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

#include "radiativeIntensityRay.hpp"
#include "fvm.hpp"
#include "fvDOM.hpp"
#include "constants.hpp"

using namespace CML::constant;

const CML::word CML::radiation::radiativeIntensityRay::intensityPrefix
(
    "ILambda"
);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::radiation::radiativeIntensityRay::radiativeIntensityRay
(
    const fvDOM& dom,
    const fvMesh& mesh,
    const scalar phi,
    const scalar theta,
    const scalar deltaPhi,
    const scalar deltaTheta,
    const label nLambda,
    const absorptionEmissionModel& absorptionEmission,
    const blackBodyEmission& blackBody,
    const label rayId
)
:
    dom_(dom),
    mesh_(mesh),
    absorptionEmission_(absorptionEmission),
    blackBody_(blackBody),
    I_
    (
        IOobject
        (
            "I" + name(rayId),
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar("I", dimMass/pow3(dimTime), 0.0)
    ),
    qr_
    (
        IOobject
        (
            "qr" + name(rayId),
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar("qr", dimMass/pow3(dimTime), 0.0)
    ),
    qin_
    (
        IOobject
        (
            "qin" + name(rayId),
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar("qin", dimMass/pow3(dimTime), 0.0)
    ),
    qem_
    (
        IOobject
        (
            "qem" + name(rayId),
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar("qem", dimMass/pow3(dimTime), 0.0)
    ),
    d_(vector::zero),
    dAve_(vector::zero),
    theta_(theta),
    phi_(phi),
    omega_(0.0),
    nLambda_(nLambda),
    ILambda_(nLambda),
    myRayId_(rayId)
{
    scalar sinTheta = CML::sin(theta);
    scalar cosTheta = CML::cos(theta);
    scalar sinPhi = CML::sin(phi);
    scalar cosPhi = CML::cos(phi);

    omega_ = 2.0*sinTheta*CML::sin(deltaTheta/2.0)*deltaPhi;
    d_ = vector(sinTheta*sinPhi, sinTheta*cosPhi, cosTheta);
    dAve_ = vector
    (
        sinPhi
       *CML::sin(0.5*deltaPhi)
       *(deltaTheta - CML::cos(2.0*theta)
       *CML::sin(deltaTheta)),
        cosPhi
       *CML::sin(0.5*deltaPhi)
       *(deltaTheta - CML::cos(2.0*theta)
       *CML::sin(deltaTheta)),
        0.5*deltaPhi*CML::sin(2.0*theta)*CML::sin(deltaTheta)
    );

    // Transform directions so that they fall inside the bounds of reduced
    // dimension cases
    if (mesh_.nSolutionD() == 2)
    {
        vector meshDir(vector::zero);
        for (direction cmpt=0; cmpt<vector::nComponents; cmpt++)
        {
            if (mesh_.geometricD()[cmpt] == -1)
            {
                meshDir[cmpt] = 1;
            }
        }
        const vector normal(vector(0, 0, 1));

        const tensor coordRot = rotationTensor(normal, meshDir);

        dAve_ = coordRot & dAve_;
        d_ = coordRot & d_;
    }
    else if (mesh_.nSolutionD() == 1)
    {
        vector meshDir(vector::zero);
        for (direction cmpt=0; cmpt<vector::nComponents; cmpt++)
        {
            if (mesh_.geometricD()[cmpt] == 1)
            {
                meshDir[cmpt] = 1;
            }
        }
        const vector normal(vector(1, 0, 0));

        dAve_ = (dAve_ & normal)*meshDir;
        d_ = (d_ & normal)*meshDir;
    }


    autoPtr<volScalarField> IDefaultPtr;

    forAll(ILambda_, lambdaI)
    {
        IOobject IHeader
        (
            intensityPrefix + "_" + name(rayId) + "_" + name(lambdaI),
            mesh_.time().timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        );

        // Check if field exists and can be read
        if (IHeader.headerOk())
        {
            ILambda_.set
            (
                lambdaI,
                new volScalarField(IHeader, mesh_)
            );
        }
        else
        {
            // Demand driven load the IDefault field
            if (!IDefaultPtr.valid())
            {
                IDefaultPtr.reset
                (
                    new volScalarField
                    (
                        IOobject
                        (
                            "IDefault",
                            mesh_.time().timeName(),
                            mesh_,
                            IOobject::MUST_READ,
                            IOobject::NO_WRITE
                        ),
                        mesh_
                    )
                );
            }

            // Reset the MUST_READ flag
            IOobject noReadHeader(IHeader);
            noReadHeader.readOpt() = IOobject::NO_READ;

            ILambda_.set
            (
                lambdaI,
                new volScalarField(noReadHeader, IDefaultPtr())
            );
        }
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::scalar CML::radiation::radiativeIntensityRay::correct()
{
    // Reset boundary heat flux to zero
    qr_.boundaryField() = 0.0;

    scalar maxResidual = -GREAT;

    const surfaceScalarField Ji(dAve_ & mesh_.Sf());

    forAll(ILambda_, lambdaI)
    {
        const volScalarField& k = dom_.aLambda(lambdaI);

        fvScalarMatrix IiEq
        (
            fvm::div(Ji, ILambda_[lambdaI], "div(Ji,Ii_h)")
          + fvm::Sp(k*omega_, ILambda_[lambdaI])
        ==
            1.0/constant::mathematical::pi*omega_
           *(
                // Remove aDisp from k
                (k - absorptionEmission_.aDisp(lambdaI))
               *blackBody_.bLambda(lambdaI)

              + absorptionEmission_.E(lambdaI)/4
            )
        );

        IiEq.relax();

        scalar eqnResidual = solve
        (
            IiEq,
            mesh_.solver("Ii")
        ).initialResidual();

        const scalar initialRes = eqnResidual*omega_/dom_.omegaMax();

        maxResidual = max(initialRes, maxResidual);
    }

    return maxResidual;
}


void CML::radiation::radiativeIntensityRay::addIntensity()
{
    I_ = dimensionedScalar("zero", dimMass/pow3(dimTime), 0.0);

    forAll(ILambda_, lambdaI)
    {
        I_ += ILambda_[lambdaI];
    }
}
