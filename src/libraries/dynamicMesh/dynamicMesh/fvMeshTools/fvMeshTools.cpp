/*---------------------------------------------------------------------------*\
Copyright (C) 2012 OpenFOAM Foundation
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

#include "fvMeshTools.hpp"
#include "processorCyclicPolyPatch.hpp"
#include "polyBoundaryMeshEntries.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Adds patch if not yet there. Returns patchID.
CML::label CML::fvMeshTools::addPatch
(
    fvMesh& mesh,
    const polyPatch& patch,
    const dictionary& patchFieldDict,
    const word& defaultPatchFieldType,
    const bool validBoundary
)
{
    polyBoundaryMesh& polyPatches =
        const_cast<polyBoundaryMesh&>(mesh.boundaryMesh());

    label patchi = polyPatches.findPatchID(patch.name());
    if (patchi != -1)
    {
        // Already there
        return patchi;
    }


    // Append at end unless there are processor patches
    label insertPatchI = polyPatches.size();
    label startFacei = mesh.nFaces();

    if (!isA<processorPolyPatch>(patch))
    {
        forAll(polyPatches, patchi)
        {
            const polyPatch& pp = polyPatches[patchi];

            if (isA<processorPolyPatch>(pp))
            {
                insertPatchI = patchi;
                startFacei = pp.start();
                break;
            }
        }
    }


    // Below is all quite a hack. Feel free to change once there is a better
    // mechanism to insert and reorder patches.

    // Clear local fields and e.g. polyMesh parallelInfo.
    mesh.clearOut();

    label sz = polyPatches.size();

    fvBoundaryMesh& fvPatches = const_cast<fvBoundaryMesh&>(mesh.boundary());

    // Add polyPatch at the end
    polyPatches.setSize(sz+1);
    polyPatches.set
    (
        sz,
        patch.clone
        (
            polyPatches,
            insertPatchI,   //index
            0,              //size
            startFacei      //start
        )
    );
    fvPatches.setSize(sz+1);
    fvPatches.set
    (
        sz,
        fvPatch::New
        (
            polyPatches[sz],  // point to newly added polyPatch
            mesh.boundary()
        )
    );

    addPatchFields<volScalarField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<scalar>::zero
    );
    addPatchFields<volVectorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<vector>::zero
    );
    addPatchFields<volSphericalTensorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<sphericalTensor>::zero
    );
    addPatchFields<volSymmTensorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<symmTensor>::zero
    );
    addPatchFields<volTensorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<tensor>::zero
    );

    // Surface fields

    addPatchFields<surfaceScalarField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<scalar>::zero
    );
    addPatchFields<surfaceVectorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<vector>::zero
    );
    addPatchFields<surfaceSphericalTensorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<sphericalTensor>::zero
    );
    addPatchFields<surfaceSymmTensorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<symmTensor>::zero
    );
    addPatchFields<surfaceTensorField>
    (
        mesh,
        patchFieldDict,
        defaultPatchFieldType,
        pTraits<tensor>::zero
    );

    // Create reordering list
    // patches before insert position stay as is
    labelList oldToNew(sz+1);
    for (label i = 0; i < insertPatchI; i++)
    {
        oldToNew[i] = i;
    }
    // patches after insert position move one up
    for (label i = insertPatchI; i < sz; i++)
    {
        oldToNew[i] = i+1;
    }
    // appended patch gets moved to insert position
    oldToNew[sz] = insertPatchI;

    // Shuffle into place
    polyPatches.reorder(oldToNew, validBoundary);
    fvPatches.reorder(oldToNew);

    reorderPatchFields<volScalarField>(mesh, oldToNew);
    reorderPatchFields<volVectorField>(mesh, oldToNew);
    reorderPatchFields<volSphericalTensorField>(mesh, oldToNew);
    reorderPatchFields<volSymmTensorField>(mesh, oldToNew);
    reorderPatchFields<volTensorField>(mesh, oldToNew);
    reorderPatchFields<surfaceScalarField>(mesh, oldToNew);
    reorderPatchFields<surfaceVectorField>(mesh, oldToNew);
    reorderPatchFields<surfaceSphericalTensorField>(mesh, oldToNew);
    reorderPatchFields<surfaceSymmTensorField>(mesh, oldToNew);
    reorderPatchFields<surfaceTensorField>(mesh, oldToNew);

    return insertPatchI;
}


void CML::fvMeshTools::setPatchFields
(
    fvMesh& mesh,
    const label patchi,
    const dictionary& patchFieldDict
)
{
    setPatchFields<volScalarField>(mesh, patchi, patchFieldDict);
    setPatchFields<volVectorField>(mesh, patchi, patchFieldDict);
    setPatchFields<volSphericalTensorField>(mesh, patchi, patchFieldDict);
    setPatchFields<volSymmTensorField>(mesh, patchi, patchFieldDict);
    setPatchFields<volTensorField>(mesh, patchi, patchFieldDict);
    setPatchFields<surfaceScalarField>(mesh, patchi, patchFieldDict);
    setPatchFields<surfaceVectorField>(mesh, patchi, patchFieldDict);
    setPatchFields<surfaceSphericalTensorField>
    (
        mesh,
        patchi,
        patchFieldDict
    );
    setPatchFields<surfaceSymmTensorField>(mesh, patchi, patchFieldDict);
    setPatchFields<surfaceTensorField>(mesh, patchi, patchFieldDict);
}


void CML::fvMeshTools::zeroPatchFields(fvMesh& mesh, const label patchi)
{
    setPatchFields<volScalarField>(mesh, patchi, pTraits<scalar>::zero);
    setPatchFields<volVectorField>(mesh, patchi, pTraits<vector>::zero);
    setPatchFields<volSphericalTensorField>
    (
        mesh,
        patchi,
        pTraits<sphericalTensor>::zero
    );
    setPatchFields<volSymmTensorField>
    (
        mesh,
        patchi,
        pTraits<symmTensor>::zero
    );
    setPatchFields<volTensorField>(mesh, patchi, pTraits<tensor>::zero);
    setPatchFields<surfaceScalarField>(mesh, patchi, pTraits<scalar>::zero);
    setPatchFields<surfaceVectorField>(mesh, patchi, pTraits<vector>::zero);
    setPatchFields<surfaceSphericalTensorField>
    (
        mesh,
        patchi,
        pTraits<sphericalTensor>::zero
    );
    setPatchFields<surfaceSymmTensorField>
    (
        mesh,
        patchi,
        pTraits<symmTensor>::zero
    );
    setPatchFields<surfaceTensorField>(mesh, patchi, pTraits<tensor>::zero);
}


// Deletes last patch
void CML::fvMeshTools::trimPatches(fvMesh& mesh, const label nPatches)
{
    // Clear local fields and e.g. polyMesh globalMeshData.
    mesh.clearOut();

    polyBoundaryMesh& polyPatches =
        const_cast<polyBoundaryMesh&>(mesh.boundaryMesh());
    fvBoundaryMesh& fvPatches = const_cast<fvBoundaryMesh&>(mesh.boundary());

    if (polyPatches.empty())
    {
        FatalErrorInFunction
            << "No patches in mesh"
            << abort(FatalError);
    }

    label nFaces = 0;
    for (label patchi = nPatches; patchi < polyPatches.size(); patchi++)
    {
        nFaces += polyPatches[patchi].size();
    }
    reduce(nFaces, sumOp<label>());

    if (nFaces)
    {
        FatalErrorInFunction
            << "There are still " << nFaces
            << " faces in " << polyPatches.size()-nPatches
            << " patches to be deleted" << abort(FatalError);
    }

    // Remove actual patches
    polyPatches.setSize(nPatches);
    fvPatches.setSize(nPatches);

    trimPatchFields<volScalarField>(mesh, nPatches);
    trimPatchFields<volVectorField>(mesh, nPatches);
    trimPatchFields<volSphericalTensorField>(mesh, nPatches);
    trimPatchFields<volSymmTensorField>(mesh, nPatches);
    trimPatchFields<volTensorField>(mesh, nPatches);

    trimPatchFields<surfaceScalarField>(mesh, nPatches);
    trimPatchFields<surfaceVectorField>(mesh, nPatches);
    trimPatchFields<surfaceSphericalTensorField>(mesh, nPatches);
    trimPatchFields<surfaceSymmTensorField>(mesh, nPatches);
    trimPatchFields<surfaceTensorField>(mesh, nPatches);
}


void CML::fvMeshTools::reorderPatches
(
    fvMesh& mesh,
    const labelList& oldToNew,
    const label nNewPatches,
    const bool validBoundary
)
{
    polyBoundaryMesh& polyPatches =
        const_cast<polyBoundaryMesh&>(mesh.boundaryMesh());
    fvBoundaryMesh& fvPatches = const_cast<fvBoundaryMesh&>(mesh.boundary());

    // Shuffle into place
    polyPatches.reorder(oldToNew, validBoundary);
    fvPatches.reorder(oldToNew);

    reorderPatchFields<volScalarField>(mesh, oldToNew);
    reorderPatchFields<volVectorField>(mesh, oldToNew);
    reorderPatchFields<volSphericalTensorField>(mesh, oldToNew);
    reorderPatchFields<volSymmTensorField>(mesh, oldToNew);
    reorderPatchFields<volTensorField>(mesh, oldToNew);
    reorderPatchFields<surfaceScalarField>(mesh, oldToNew);
    reorderPatchFields<surfaceVectorField>(mesh, oldToNew);
    reorderPatchFields<surfaceSphericalTensorField>(mesh, oldToNew);
    reorderPatchFields<surfaceSymmTensorField>(mesh, oldToNew);
    reorderPatchFields<surfaceTensorField>(mesh, oldToNew);

    // Remove last.
    trimPatches(mesh, nNewPatches);
}


CML::labelList CML::fvMeshTools::removeEmptyPatches
(
    fvMesh& mesh,
    const bool validBoundary
)
{
    const polyBoundaryMesh& pbm = mesh.boundaryMesh();

    labelList newToOld(pbm.size());
    labelList oldToNew(pbm.size(), -1);
    label newI = 0;


    // Assumes all non-coupled boundaries are on all processors!
    forAll(pbm, patchi)
    {
        const polyPatch& pp = pbm[patchi];

        if (!isA<processorPolyPatch>(pp))
        {
            label nFaces = pp.size();
            if (validBoundary)
            {
                reduce(nFaces, sumOp<label>());
            }

            if (nFaces > 0)
            {
                newToOld[newI] = patchi;
                oldToNew[patchi] = newI++;
            }
        }
    }

    // Same for processor patches (but need no reduction)
    forAll(pbm, patchi)
    {
        const polyPatch& pp = pbm[patchi];

        if (isA<processorPolyPatch>(pp) && pp.size())
        {
            newToOld[newI] = patchi;
            oldToNew[patchi] = newI++;
        }
    }

    newToOld.setSize(newI);

    // Move all deleteable patches to the end
    forAll(oldToNew, patchi)
    {
        if (oldToNew[patchi] == -1)
        {
            oldToNew[patchi] = newI++;
        }
    }

    reorderPatches(mesh, oldToNew, newToOld.size(), validBoundary);

    return newToOld;
}


CML::autoPtr<CML::fvMesh> CML::fvMeshTools::newMesh
(
    const IOobject& io,
    const bool masterOnlyReading
)
{
    // Region name
    // ~~~~~~~~~~~

    fileName meshSubDir;

    if (io.name() == polyMesh::defaultRegion)
    {
        meshSubDir = polyMesh::meshSubDir;
    }
    else
    {
        meshSubDir = io.name()/polyMesh::meshSubDir;
    }


    fileName facesInstance;

    // Patch types
    // ~~~~~~~~~~~
    // Read and scatter master patches (without reading master mesh!)

    PtrList<entry> patchEntries;
    if (Pstream::master())
    {
        facesInstance = io.time().findInstance
        (
            meshSubDir,
            "faces",
            IOobject::MUST_READ
        );

        patchEntries = polyBoundaryMeshEntries
        (
            IOobject
            (
                "boundary",
                facesInstance,
                meshSubDir,
                io.db(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE,
                false
            )
        );

        // Send patches
        for
        (
            int slave=Pstream::firstSlave();
            slave<=Pstream::lastSlave();
            slave++
        )
        {
            OPstream toSlave(Pstream::commsTypes::scheduled, slave);
            toSlave << patchEntries;
        }
    }
    else
    {
        // Receive patches
        IPstream fromMaster(Pstream::commsTypes::scheduled, Pstream::masterNo());
        fromMaster >> patchEntries;
    }


    Pstream::scatter(facesInstance);

    // Dummy meshes
    // ~~~~~~~~~~~~

    // Check who has a mesh
    const fileName meshDir = io.time().path()/facesInstance/meshSubDir;
    bool haveMesh = isDir(meshDir);
    if (masterOnlyReading && !Pstream::master())
    {
        haveMesh = false;
    }


    // Set up to read-if-present. Note: does not search for mesh so set
    // instance explicitly
    IOobject meshIO(io);
    meshIO.instance() = facesInstance;
    if (masterOnlyReading && !Pstream::master())
    {
        meshIO.readOpt() = IOobject::NO_READ;
    }
    else
    {
        meshIO.readOpt() = IOobject::READ_IF_PRESENT;
    }


    // Read mesh
    // ~~~~~~~~~
    // Now all processors read a mesh and use supplied points,faces etc
    // if there is none.
    // Note: fvSolution, fvSchemes are also using the supplied Ioobject so
    //       on slave will be NO_READ, on master READ_IF_PRESENT. This will
    //       conflict with e.g. timeStampMaster reading so switch off.

    const regIOobject::fileCheckTypes oldCheckType =
        regIOobject::fileModificationChecking;
    regIOobject::fileModificationChecking = regIOobject::timeStamp;

    autoPtr<fvMesh> meshPtr
    (
        new fvMesh
        (
            meshIO,
            pointField(),
            faceList(),
            labelList(),
            labelList()
        )
    );
    fvMesh& mesh = meshPtr();

    regIOobject::fileModificationChecking = oldCheckType;



    // Add patches
    // ~~~~~~~~~~~


    bool havePatches = mesh.boundary().size();
    reduce(havePatches, andOp<bool>());

    if (!havePatches)
    {
        // There are one or more processors without full complement of
        // patches

        DynamicList<polyPatch*> newPatches;

        if (haveMesh)   //Pstream::master())
        {
            forAll(mesh.boundary(), patchi)
            {
                newPatches.append
                (
                    mesh.boundaryMesh()[patchi].clone(mesh.boundaryMesh()).ptr()
                );
            }
        }
        else
        {
            forAll(patchEntries, patchi)
            {
                const entry& e = patchEntries[patchi];
                const word type(e.dict().lookup("type"));

                if
                (
                    type == processorPolyPatch::typeName
                 || type == processorCyclicPolyPatch::typeName
                )
                {}
                else
                {
                    dictionary patchDict(e.dict());
                    patchDict.set("nFaces", 0);
                    patchDict.set("startFace", 0);

                    newPatches.append
                    (
                        polyPatch::New
                        (
                            patchEntries[patchi].keyword(),
                            patchDict,
                            newPatches.size(),
                            mesh.boundaryMesh()
                        ).ptr()
                    );
                }
            }
        }
        mesh.removeFvBoundary();
        mesh.addFvPatches(newPatches);
    }

    //Pout<< "patches:" << endl;
    //forAll(mesh.boundary(), patchi)
    //{
    //    Pout<< "    type" << mesh.boundary()[patchi].type()
    //        << " size:" << mesh.boundary()[patchi].size()
    //        << " start:" << mesh.boundary()[patchi].start() << endl;
    //}
    //Pout<< endl;


    // Determine zones
    // ~~~~~~~~~~~~~~~

    wordList pointZoneNames(mesh.pointZones().names());
    Pstream::scatter(pointZoneNames);
    wordList faceZoneNames(mesh.faceZones().names());
    Pstream::scatter(faceZoneNames);
    wordList cellZoneNames(mesh.cellZones().names());
    Pstream::scatter(cellZoneNames);

    if (!haveMesh)
    {
        // Add the zones. Make sure to remove the old dummy ones first
        mesh.pointZones().clear();
        mesh.faceZones().clear();
        mesh.cellZones().clear();

        List<pointZone*> pz(pointZoneNames.size());
        forAll(pointZoneNames, i)
        {
            pz[i] = new pointZone
            (
                pointZoneNames[i],
                labelList(0),
                i,
                mesh.pointZones()
            );
        }
        List<faceZone*> fz(faceZoneNames.size());
        forAll(faceZoneNames, i)
        {
            fz[i] = new faceZone
            (
                faceZoneNames[i],
                labelList(0),
                boolList(0),
                i,
                mesh.faceZones()
            );
        }
        List<cellZone*> cz(cellZoneNames.size());
        forAll(cellZoneNames, i)
        {
            cz[i] = new cellZone
            (
                cellZoneNames[i],
                labelList(0),
                i,
                mesh.cellZones()
            );
        }

        if (pz.size() && fz.size() && cz.size())
        {
            mesh.addZones(pz, fz, cz);
        }
    }

    return meshPtr;
}


// ************************************************************************* //
