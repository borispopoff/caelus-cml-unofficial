/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2016 OpenFOAM Foundation
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

using namespace CML;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Dump patch + weights to vtk file
void writeWeights
(
    const polyMesh& mesh,
    const scalarField& wghtSum,
    const primitivePatch& patch,
    const fileName& folder,
    const fileName& prefix,
    const word& timeName
)
{
    vtkSurfaceWriter writer;

    // Collect geometry
    labelList pointToGlobal;
    labelList uniqueMeshPointLabels;
    autoPtr<globalIndex> globalPoints;
    autoPtr<globalIndex> globalFaces;
    faceList mergedFaces;
    pointField mergedPoints;
    CML::PatchTools::gatherAndMerge
    (
        mesh,
        patch.localFaces(),
        patch.meshPoints(),
        patch.meshPointMap(),

        pointToGlobal,
        uniqueMeshPointLabels,
        globalPoints,
        globalFaces,

        mergedFaces,
        mergedPoints
    );
    // Collect field
    scalarField mergedWeights;
    globalFaces().gather
    (
        labelList(UPstream::procIDs()),
        wghtSum,
        mergedWeights
    );

    if (Pstream::master())
    {
        writer.write
        (
            folder,
            prefix + "_" + timeName,
            mergedPoints,
            mergedFaces,
            "weightsSum",
            mergedWeights,
            false
        );
    }
}


void writeWeights(const polyMesh& mesh)
{
    const polyBoundaryMesh& pbm = mesh.boundaryMesh();

    const word tmName(mesh.time().timeName());

    forAll(pbm, patchi)
    {
        if (isA<cyclicAMIPolyPatch>(pbm[patchi]))
        {
            const cyclicAMIPolyPatch& cpp =
                refCast<const cyclicAMIPolyPatch>(pbm[patchi]);

            if (cpp.owner())
            {
                Info<< "Calculating AMI weights between owner patch: "
                    << cpp.name() << " and neighbour patch: "
                    << cpp.neighbPatch().name() << endl;

                const AMIPatchToPatchInterpolation& ami =
                    cpp.AMI();
                writeWeights
                (
                    mesh,
                    ami.tgtWeightsSum(),
                    cpp.neighbPatch(),
                    "output",
                    "tgt",
                    tmName
                );
                writeWeights
                (
                    mesh,
                    ami.srcWeightsSum(),
                    cpp,
                    "output",
                    "src",
                    tmName
                );
            }
        }
    }
}



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
            writeWeights(mesh);
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
