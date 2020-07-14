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

Application
    waveDyMSolver

Description
    Solver for 2 incompressible, isothermal immiscible fluids using a VOF
    (volume of fluid) phase-fraction based interface capturing approach,
    with optional mesh motion and mesh topology changes including adaptive
    re-meshing.

    The momentum and other fluid properties are of the "mixture" and a single
    momentum equation is solved.

    Turbulence modelling is generic, i.e. laminar, RAS or LES may be selected.
---------------------------------------------------------------------------*/

#include "fvCFD.hpp"
#include "dynamicFvMesh.hpp"
#include "CMULES.hpp"
#include "EulerDdtScheme.hpp"
#include "localEulerDdtScheme.hpp"
#include "CrankNicolsonDdtScheme.hpp"
#include "subCycle.hpp"
#include "interfaceProperties.hpp"
#include "twoPhaseMixture.hpp"
#include "turbulenceModel.hpp"
#include "interpolationTable.hpp"
#include "pimpleControl.hpp"
#include "fvIOoptionList.hpp"
#include "CorrectPhi_.hpp"
#include "fvcSmooth.hpp"

#include "relaxationZone.hpp"
#include "externalWaveForcing.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.hpp"
    #include "createTime.hpp"
    #include "createDynamicFvMesh.hpp"

    pimpleControl pimple(mesh);

    #include "initContinuityErrs.hpp"
    #include "createDyMControls.hpp"
    #include "createFields.hpp"
    #include "createMRF.hpp"
    #include "createFvOptions.hpp"
    #include "initCorrectPhi.hpp"
    #include "createUfIfPresent.hpp"

    if (!LTS)
    {
        #include "readTimeControls.hpp"
        #include "CourantNo.hpp"
        #include "setInitialDeltaT.hpp"
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        #include "readDyMControls.hpp"

        if (LTS)
        {
            #include "setRDeltaTVoF.hpp"
        }
        else
        {
            #include "CourantNo.hpp"
            #include "alphaCourantNo.hpp"
            #include "setDeltaTVoF.hpp"
        }

        runTime++;

        Info<< "Time = " << runTime.timeName() << nl << endl;

        if (waves)
        {
            externalWave->step();
        }

        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
            if (pimple.firstIter() || moveMeshOuterCorrectors)
            {
                scalar timeBeforeMeshUpdate = runTime.elapsedCpuTime();

                mesh.update();

                if (mesh.changing())
                {
                    Info<< "Execution time for mesh.update() = "
                        << runTime.elapsedCpuTime() - timeBeforeMeshUpdate
                        << " s" << endl;

                    // Do not apply previous time-step mesh compression flux
                    // if the mesh topology changed
                    if (mesh.topoChanging())
                    {
                        talphaPhi1Corr0.clear();
                    }

                    gh = g & mesh.C();
                    ghf = g & mesh.Cf();

                    if (correctPhi)
                    {
                        // Calculate absolute flux
                        // from the mapped surface velocity
                        phi = mesh.Sf() & UfPtr();

                        #include "correctPhi.hpp"

                        // Make the flux relative to the mesh motion
                        fvc::makeRelative(phi, U);

                        mixture.correct();
                    }

                    if (checkMeshCourantNo)
                    {
                        #include "meshCourantNo.hpp"
                    }
                }
            }

            #include "alphaControls.hpp"
            #include "alphaEqnSubCycle.hpp"

            if (waves)
            {
                relaxing->correct();
            }

            mixture.correct();
            interface.correct();

            #include "UEqn.hpp"

            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "pEqn.hpp"
            }

            if (pimple.turbCorr())
            {
                turbulence->correct();
            }
        }

        runTime.write();

        #include "reportTimeStats.hpp" 
    }

    if (waves)
    {
        // Close down the external wave forcing in a nice manner
        externalWave->close();
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
