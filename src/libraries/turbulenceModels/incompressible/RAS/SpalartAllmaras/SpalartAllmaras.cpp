/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2012 OpenFOAM Foundation
Copyright (C) 2014-2015 Applied CCM
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

#include "SpalartAllmaras.hpp"
#include "fvOptions.hpp"
#include "addToRunTimeSelectionTable.hpp"

namespace CML
{
namespace incompressible
{
namespace RASModels
{

defineTypeNameAndDebug(SpalartAllmaras, 0);
addToRunTimeSelectionTable(RASModel, SpalartAllmaras, dictionary);

tmp<volScalarField> SpalartAllmaras::chi() const
{
    return nuTilda_/nu();
}


tmp<volScalarField> 
SpalartAllmaras::fv1(const volScalarField& chi) const
{
    const volScalarField chi3(pow3(chi));
    return chi3/(chi3 + pow3(Cv1_));
}


tmp<volScalarField> SpalartAllmaras::fv2
(
    const volScalarField& chi,
    const volScalarField& fv1
) const
{
    return (scalar(1) - chi/(scalar(1)+chi*fv1));
}


tmp<volScalarField> 
SpalartAllmaras::fw(const volScalarField& Stilda) const
{
    volScalarField r
    (
        min
        (
            nuTilda_
           /(
               max
               (
                   Stilda,
                   dimensionedScalar("SMALL", Stilda.dimensions(), SMALL)
               )
              *sqr(kappa_*d_)
            ),
            scalar(10.0)
        )
    );
    r.boundaryFieldRef() == 0.0;

    const volScalarField g(r + Cw2_*(pow6(r) - r));

    return g*pow((1.0 + pow6(Cw3_))/(pow6(g) + pow6(Cw3_)), 1.0/6.0);
}

SpalartAllmaras::SpalartAllmaras
(
    const volVectorField& U,
    const surfaceScalarField& phi,
    transportModel& transport,
    const word& turbulenceModelName,
    const word& modelName
)
:
    RASModel(modelName, U, phi, transport, turbulenceModelName),
    curvatureCorrection_(coeffDict_.lookupOrDefault<Switch>("curvatureCorrection", false)),
    outputfr1_
    (
        coeffDict_.lookupOrDefault<Switch>
        (
            "outputfr1",
            false
        )
    ),
    sigmaNut_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "sigmaNut",
            coeffDict_,
            0.66666
        )
    ),
    kappa_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "kappa",
            coeffDict_,
            0.41
        )
    ),

    Cb1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cb1",
            coeffDict_,
            0.1355
        )
    ),
    Cb2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cb2",
            coeffDict_,
            0.622
        )
    ),
    Cw1_(Cb1_/sqr(kappa_) + (1.0 + Cb2_)/sigmaNut_),
    Cw2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cw2",
            coeffDict_,
            0.3
        )
    ),
    Cw3_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cw3",
            coeffDict_,
            2.0
        )
    ),
    Cv1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cv1",
            coeffDict_,
            7.1
        )
    ),
    Cr1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cr1",
            coeffDict_,
            1.0
        )
    ),
    Cr2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cr2",
            coeffDict_,
            12.0
        )
    ),
    Cr3_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cr3",
            coeffDict_,
            1.0
        )
    ),
    nuTilda_
    (
        IOobject
        (
            "nuTilda",
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),
    nut_
    (
        IOobject
        (
            "nut",
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),
    fr1_
    (
        IOobject
        (
            "fr1",
            runTime_.timeName(),
            mesh_,
            IOobject::NO_READ,
            outputfr1_ ? IOobject::AUTO_WRITE : IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar("one", dimless, 1)
    ),
    d_(mesh_)
{
    if (curvatureCorrection_)
    {
        Info<<" Curvature correction modification enabled" << endl;
    }

    printCoeffs();
}

tmp<volScalarField> SpalartAllmaras::DnuTildaEff() const
{
    return tmp<volScalarField>
    (
        new volScalarField("DnuTildaEff", (nuTilda_ + nu())/sigmaNut_)
    );
}


tmp<volScalarField> SpalartAllmaras::k() const
{
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject
            (
                "k",
                runTime_.timeName(),
                mesh_
            ),
            (
                sqrt
                (
                    epsilon()*nut_/0.09
                    +
                    dimensionedScalar
                    (
                        "sk",
                        dimensionSet(0,4,-4,0,0,0,0),
                        scalar(VSMALL)
                    )
                )
            )
        )
    );
}


tmp<volScalarField> SpalartAllmaras::epsilon() const
{
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject
            (
                "epsilon",
                runTime_.timeName(),
                mesh_
            ),
            scalar(2)*nuEff()*magSqr(symm(fvc::grad(U())))
        )
    );
}


tmp<volSymmTensorField> SpalartAllmaras::R() const
{
    return tmp<volSymmTensorField>
    (
        new volSymmTensorField
        (
            IOobject
            (
                "R",
                runTime_.timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            ((2.0/3.0)*I)*k() - nut()*twoSymm(fvc::grad(U_))
        )
    );
}


tmp<volSymmTensorField> SpalartAllmaras::devReff() const
{
    return tmp<volSymmTensorField>
    (
        new volSymmTensorField
        (
            IOobject
            (
                "devRhoReff",
                runTime_.timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
           -nuEff()*dev(twoSymm(fvc::grad(U_)))
        )
    );
}


tmp<fvVectorMatrix> 
SpalartAllmaras::divDevReff(volVectorField& U) const
{
    const volScalarField nuEff_(nuEff());

    return
    (
      - fvm::laplacian(nuEff_, U)
      - fvc::div(nuEff_*dev(T(fvc::grad(U))))
    );
}


tmp<fvVectorMatrix> SpalartAllmaras::divDevRhoReff
(
    const volScalarField& rho,
    volVectorField& U
) const
{
    volScalarField muEff("muEff", rho*nuEff());

    return
    (
      - fvm::laplacian(muEff, U)
      - fvc::div(muEff*dev(T(fvc::grad(U))))
    );
}


bool SpalartAllmaras::read()
{
    if (RASModel::read())
    {
        curvatureCorrection_.readIfPresent("curvatureCorrection", coeffDict());
        sigmaNut_.readIfPresent(coeffDict());
        kappa_.readIfPresent(coeffDict());
        Cb1_.readIfPresent(coeffDict());
        Cb2_.readIfPresent(coeffDict());
        Cw1_ = Cb1_/sqr(kappa_) + (1.0 + Cb2_)/sigmaNut_;
        Cw2_.readIfPresent(coeffDict());
        Cw3_.readIfPresent(coeffDict());
        Cv1_.readIfPresent(coeffDict());
        Cr1_.readIfPresent(coeffDict());
        Cr2_.readIfPresent(coeffDict());
        Cr3_.readIfPresent(coeffDict());

        return true;
    }
    else
    {
        return false;
    }
}


void SpalartAllmaras::correct()
{
    fv::options& fvOptions(fv::options::New(this->mesh_));
    RASModel::correct();

    if (!turbulence_)
    {
        // Re-calculate viscosity
        nut_ = nuTilda_*fv1(this->chi());
        nut_.correctBoundaryConditions();

        return;
    }

    if (mesh_.changing())
    {
        d_.correct();
    }

    const volScalarField chi(this->chi());
    const volScalarField fv1(this->fv1(chi));
    const volTensorField Omegaij(skew(fvc::grad(this->U_)));
    const volScalarField sqrOmega(2*magSqr(Omegaij));

    // Curvature correction terms
    if (curvatureCorrection_)
    {
       const volSymmTensorField Sij(symm(fvc::grad(this->U_)));    
       const volScalarField sqrS(2*magSqr(Sij));
       const dimensionedScalar smallOmega("smallOmega", sqrOmega.dimensions(), SMALL);
       const volScalarField sqrD(0.5*(sqrS + sqrOmega + smallOmega));
       const volScalarField rStar(sqrt(sqrS)/sqrt(sqrOmega+smallOmega));
       const volSymmTensorField DSijDt(fvc::DDt(this->phi_,Sij));
       const volScalarField rTilda(  
           (scalar(2)/sqr(sqrD))*(Omegaij && (Sij & DSijDt)));
       fr1_ = 
           (scalar(1) + Cr1_)*scalar(2)*rStar/(scalar(1) + rStar)
            *(scalar(1)-Cr3_*atan(Cr2_*rTilda)) - Cr1_;
    }

    const volScalarField Stilda
    (
        sqrt(sqrOmega) + fv2(chi, fv1)*nuTilda_/sqr(kappa_*d_)
    );

    tmp<fvScalarMatrix> nuTildaEqn
    (
        fvm::ddt(nuTilda_)
      + fvm::div(phi_, nuTilda_)
      - fvm::laplacian(DnuTildaEff(), nuTilda_)
      - Cb2_/sigmaNut_*magSqr(fvc::grad(nuTilda_))
     ==
        fr1_*Cb1_*Stilda*nuTilda_
      - fvm::Sp(Cw1_*fw(Stilda)*nuTilda_/sqr(d_), nuTilda_)
      + fvOptions(nuTilda_)
    );

    nuTildaEqn.ref().relax();
    fvOptions.constrain(nuTildaEqn.ref());
    mesh_.updateFvMatrix(nuTildaEqn.ref());
    solve(nuTildaEqn);
    fvOptions.correct(nuTilda_);
    bound(nuTilda_, dimensionedScalar("0", nuTilda_.dimensions(), 0.0));
    nuTilda_.correctBoundaryConditions();

    // Re-calculate viscosity
    nut_.internalField() = fv1*nuTilda_.internalField();
    nut_.correctBoundaryConditions();
}


}
}
}


