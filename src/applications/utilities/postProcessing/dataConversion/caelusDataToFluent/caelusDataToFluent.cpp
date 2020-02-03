/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
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

Description
    Translates CAELUS data to Fluent format.

\*---------------------------------------------------------------------------*/

#include "fvCFD.hpp"
#include "writeFluentFields.hpp"
#include "OFstream.hpp"
#include "IOobjectList.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    argList::noParallel();
    timeSelector::addOptions(false);   // no constant

#   include "setRootCase.hpp"
#   include "createTime.hpp"

    instantList timeDirs = timeSelector::select0(runTime, args);

#   include "createMesh.hpp"

    // make a directory called proInterface in the case
    mkDir(runTime.rootPath()/runTime.caseName()/"fluentInterface");

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Info<< "Time = " << runTime.timeName() << endl;

        if (mesh.readUpdate())
        {
            Info<< "    Read new mesh" << endl;
        }

        // make a directory called proInterface in the case
        mkDir(runTime.rootPath()/runTime.caseName()/"fluentInterface");

        // open a file for the mesh
        OFstream fluentDataFile
        (
            runTime.rootPath()/
            runTime.caseName()/
            "fluentInterface"/
            runTime.caseName() + runTime.timeName() + ".dat"
        );

        fluentDataFile
            << "(0 \"FOAM to Fluent data File\")" << endl << endl;

        // Writing number of faces
        label nFaces = mesh.nFaces();

        forAll(mesh.boundary(), patchi)
        {
            nFaces += mesh.boundary()[patchi].size();
        }

        fluentDataFile
            << "(33 (" << mesh.nCells() << " " << nFaces << " "
            << mesh.nPoints() << "))" << endl;

        IOdictionary caelusDataToFluentDict
        (
            IOobject
            (
                "caelusDataToFluentDict",
                runTime.system(),
                mesh,
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE
            )
        );


        // Search for list of objects for this time
        IOobjectList objects(mesh, runTime.timeName());


        // Converting volScalarField
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        // Search list of objects for volScalarFields
        IOobjectList scalarFields(objects.lookupClass("volScalarField"));

        forAllIter(IOobjectList, scalarFields, iter)
        {
            // Read field
            volScalarField field(*iter(), mesh);

            // lookup field from dictionary and convert field
            label unitNumber;
            if
            (
                caelusDataToFluentDict.readIfPresent(field.name(), unitNumber)
             && unitNumber > 0
            )
            {
                Info<< "    Converting field " << field.name() << endl;
                writeFluentField(field, unitNumber, fluentDataFile);
            }
        }


        // Converting volVectorField
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        // Search list of objects for volVectorFields
        IOobjectList vectorFields(objects.lookupClass("volVectorField"));

        forAllIter(IOobjectList, vectorFields, iter)
        {
            // Read field
            volVectorField field(*iter(), mesh);

            // lookup field from dictionary and convert field
            label unitNumber;
            if
            (
                caelusDataToFluentDict.readIfPresent(field.name(), unitNumber)
             && unitNumber > 0
            )
            {
                Info<< "    Converting field " << field.name() << endl;
                writeFluentField(field, unitNumber, fluentDataFile);
            }
        }

        Info<< endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
