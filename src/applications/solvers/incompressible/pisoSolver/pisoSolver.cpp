/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
Copyright (C) 2014 Applied CCM
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
    pisoSolver

Description
    Transient solver for incompressible flow.

    Sub-models include:
    - turbulence modelling, i.e. laminar, RAS or LES
    - run-time selectable MRF and finite volume options, e.g. explicit porosity

\*---------------------------------------------------------------------------*/

#include "fvCFD.hpp"
#include "singlePhaseTransportModel.hpp"
#include "turbulenceModel.hpp"
#include "pisoControl.hpp"
#include "fvOptions.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.hpp"
    #include "createTime.hpp"
    #include "createMesh.hpp"

    pisoControl piso(mesh);

    #include "createFields.hpp"
    #include "createMRF.hpp"
    #include "createFvOptions.hpp"
    #include "initContinuityErrs.hpp"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        #include "CourantNo.hpp"

        // Pressure-velocity PISO corrector
        {
            #include "UEqn.hpp"

            // --- PISO loop
            while (piso.correct())
            {
                #include "pEqn.hpp"
            }
        }

        turbulence->correct();

        runTime.write();

        #include "reportTimeStats.hpp"

    }

    Info<< "End\n" << endl;

    return 0;
}

