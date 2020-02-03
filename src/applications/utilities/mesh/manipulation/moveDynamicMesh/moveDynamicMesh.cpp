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
    moveDynamicMesh

Description
    Mesh motion and topological mesh changes utility.

\*---------------------------------------------------------------------------*/

#include "argList.hpp"
#include "Time.hpp"
#include "dynamicFvMesh.hpp"
#include "vtkSurfaceWriter.hpp"
#include "cyclicAMIPolyPatch.hpp"
#include "PatchTools.hpp"
#include "checkGeometry.hpp"

using namespace CML;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "addRegionOption.hpp"
    argList::addBoolOption
    (
        "checkAMI",
        "check AMI weights"
    );

    #include "setRootCase.hpp"
    #include "createTime.hpp"
    #include "createNamedDynamicFvMesh.hpp"

    const bool checkAMI  = args.optionFound("checkAMI");

    if (checkAMI)
    {
        Info<< "Writing VTK files with weights of AMI patches." << nl << endl;
    }

    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << endl;

        mesh.update();
        mesh.checkMesh(true);

        if (checkAMI)
        {
            writeAMIWeightsSums(mesh);
        }

        runTime.write();

#ifdef _WIN32
        Info<< "ExecutionTime = " << static_cast<scalar>(runTime.elapsedCpuTime()) << " s"
            << "  ClockTime = " << static_cast<scalar>(runTime.elapsedClockTime()) << " s"
#else
        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
#endif
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
