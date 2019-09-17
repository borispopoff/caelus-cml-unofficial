/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2017 OpenFOAM Foundation
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

#include "wallShearStress.hpp"
#include "volFields.hpp"
#include "surfaceFields.hpp"
#include "incompressible/turbulenceModel/turbulenceModel.hpp"
#include "compressible/turbulenceModel/compressibleTurbulenceModel.hpp"
#include "wallPolyPatch.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(wallShearStress, 0);
}


// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

void CML::wallShearStress::writeFileHeader(const label i)
{
    // Add headers to output data
    writeHeader(file(), "Wall shear stress");
    writeCommented(file(), "Time");
    writeTabbed(file(), "patch");
    writeTabbed(file(), "min");
    writeTabbed(file(), "max");
    file() << endl;
}


void CML::wallShearStress::calcShearStress
(
    const fvMesh& mesh,
    const volSymmTensorField& Reff,
    volVectorField& shearStress
)
{
    forAllConstIter(labelHashSet, patchSet_, iter)
    {
        label patchi = iter.key();
        const polyPatch& pp = mesh.boundaryMesh()[patchi];

        vectorField& ssp = shearStress.boundaryField()[patchi];
        const vectorField& Sfp = mesh.Sf().boundaryField()[patchi];
        const scalarField& magSfp = mesh.magSf().boundaryField()[patchi];
        const symmTensorField& Reffp = Reff.boundaryField()[patchi];

        ssp = (-Sfp/magSfp) & Reffp;

        vector minSsp = gMin(ssp);
        vector maxSsp = gMax(ssp);

        if (Pstream::master())
        {
            file() << mesh.time().value()
                << token::TAB << pp.name()
                << token::TAB << minSsp
                << token::TAB << maxSsp
                << endl;
        }

        Info(log_)<< "    min/max(" << pp.name() << ") = "
            << minSsp << ", " << maxSsp << endl;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::wallShearStress::wallShearStress
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    functionObjectFile(obr, name, typeName),
    name_(name),
    obr_(obr),
    active_(true),
    log_(true),
    patchSet_()
{
    // Check if the available mesh is an fvMesh, otherwise deactivate
    if (!isA<fvMesh>(obr_))
    {
        active_ = false;
        WarningInFunction
            << "No fvMesh available, deactivating " << name_ << nl
            << endl;
    }

    if (active_)
    {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        volVectorField* wallShearStressPtr
        (
            new volVectorField
            (
                IOobject
                (
                    type(),
                    mesh.time().timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh,
                dimensionedVector
                (
                    "0",
                    sqr(dimLength)/sqr(dimTime),
                    Zero
                )
            )
        );

        mesh.objectRegistry::store(wallShearStressPtr);
    }

    read(dict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::wallShearStress::~wallShearStress()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::wallShearStress::read(const dictionary& dict)
{
    if (active_)
    {
        log_ = dict.lookupOrDefault<Switch>("log", true);

        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        const polyBoundaryMesh& pbm = mesh.boundaryMesh();

        patchSet_ =
            mesh.boundaryMesh().patchSet
            (
                wordReList(dict.lookupOrDefault("patches", wordReList()))
            );

        Info<< type() << " " << name_ << ":" << nl;

        if (patchSet_.empty())
        {
            forAll(pbm, patchi)
            {
                if (isA<wallPolyPatch>(pbm[patchi]))
                {
                    patchSet_.insert(patchi);
                }
            }

            Info<< "    processing all wall patches" << nl << endl;
        }
        else
        {
            Info<< "    processing wall patches: " << nl;
            labelHashSet filteredPatchSet;
            forAllConstIter(labelHashSet, patchSet_, iter)
            {
                label patchi = iter.key();
                if (isA<wallPolyPatch>(pbm[patchi]))
                {
                    filteredPatchSet.insert(patchi);
                    Info<< "        " << pbm[patchi].name() << endl;
                }
                else
                {
                    WarningInFunction
                        << "Requested wall shear stress on non-wall boundary "
                        << "type patch: " << pbm[patchi].name() << endl;
                }
            }

            Info<< endl;

            patchSet_ = filteredPatchSet;
        }
    }
}


void CML::wallShearStress::execute()
{
    typedef compressible::turbulenceModel cmpModel;
    typedef incompressible::turbulenceModel icoModel;

    if (active_)
    {
        functionObjectFile::write();

        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        volVectorField& wallShearStress =
            mesh.lookupObjectRef<volVectorField>(type());

        Info(log_)<< type() << " " << name_ << " output:" << nl;


        tmp<volSymmTensorField> Reff;
        if (mesh.foundObject<cmpModel>("turbulenceModel"))
        {
            const cmpModel& model =
                mesh.lookupObject<cmpModel>("turbulenceModel");

            Reff = model.devRhoReff();
        }
        else if (mesh.foundObject<icoModel>("turbulenceModel"))
        {
            const icoModel& model =
                mesh.lookupObject<icoModel>("turbulenceModel");

            Reff = model.devReff();
        }
        else
        {
            FatalErrorInFunction
                << "Unable to find turbulence model in the "
                << "database" << exit(FatalError);
        }

        calcShearStress(mesh, Reff(), wallShearStress);
    }
}


void CML::wallShearStress::end()
{
    if (active_)
    {
        execute();
    }
}


void CML::wallShearStress::timeSet()
{
    // Do nothing
}


void CML::wallShearStress::write()
{
    if (active_)
    {
        functionObjectFile::write();

        const volVectorField& wallShearStress =
            obr_.lookupObject<volVectorField>(type());

        Info(log_)<< type() << " " << name_ << " output:" << nl
            << "    writing field " << wallShearStress.name() << nl
            << endl;

        wallShearStress.write();
    }
}


// ************************************************************************* //
