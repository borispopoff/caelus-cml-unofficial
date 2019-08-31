/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
-------------------------------------------------------------------------------
License
    This file is part of cfMesh.

    cfMesh is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    cfMesh is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with cfMesh.  If not, see <http://www.gnu.org/licenses/>.

Author: Franjo Juretic (franjo.juretic@c-fields.com)

\*---------------------------------------------------------------------------*/

#include "polyMeshGenModifier.hpp"
#include "demandDrivenData.hpp"

# ifdef USE_OMP
#include <omp.h>
# endif

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void polyMeshGenModifier::removeFaces(const boolList& removeFace)
{
    //mesh_.clearOut();

    Info << "Removing faces" << endl;
    faceListPMG& faces = mesh_.faces_;
    cellListPMG& cells = mesh_.cells_;

    label nFaces(0);
    labelLongList newFaceLabel(faces.size(), -1);

    //- copy internal faces
    const label nInternalFaces = mesh_.nInternalFaces();
    for(label facei=0;facei<nInternalFaces;++facei)
        if( !removeFace[facei] )
        {
            if( nFaces < facei )
                faces[nFaces].transfer(faces[facei]);

            newFaceLabel[facei] = nFaces;
            ++nFaces;
        }

    //- copy boundary faces
    PtrList<boundaryPatch>& boundaries = mesh_.boundaries_;
    labelList patchStart(boundaries.size());
    labelList nFacesInPatch(boundaries.size());
    label npI(0);
    forAll(boundaries, patchI)
    {
        const label oldStart = boundaries[patchI].patchStart();
        const label oldNumFacesInPatch = boundaries[patchI].patchSize();

        patchStart[npI] = nFaces;
        nFacesInPatch[npI] = 0;

        for(label facei=0;facei<oldNumFacesInPatch;++facei)
            if( !removeFace[oldStart+facei] )
            {
                ++nFacesInPatch[npI];
                if( nFaces < (oldStart+facei) )
                    faces[nFaces].transfer(faces[oldStart+facei]);
                newFaceLabel[oldStart+facei] = nFaces++;
            }

        ++npI;
    }

    forAll(boundaries, patchI)
    {
        boundaries[patchI].patchStart() = patchStart[patchI];
        boundaries[patchI].patchSize() = nFacesInPatch[patchI];
    }

    if( Pstream::parRun() )
    {
        PtrList<processorBoundaryPatch>& procBoundaries =
            mesh_.procBoundaries_;

        label nProcFaces(0);
        forAll(procBoundaries, patchI)
            nProcFaces += procBoundaries[patchI].patchSize();

        //- copy processor faces into a separate list
        //- this preserves the order of faces after the modification is finished
        faceList procFaces(nProcFaces);
        nProcFaces = 0;
        forAll(procBoundaries, patchI)
        {
            const label start = procBoundaries[patchI].patchStart();
            const label end = start + procBoundaries[patchI].patchSize();
            for(label facei=start;facei<end;++facei)
                procFaces[nProcFaces++].transfer(faces[facei]);
        }

        //- create ordinary boundary faces from processor faces
        //- which are removed from one processor only
        //- these faces are stored in the latest ordinary patch
        forAll(procBoundaries, patchI)
        {
            //- send data to other proc
            const label start = procBoundaries[patchI].patchStart();
            boolList removeProcFace(procBoundaries[patchI].patchSize(), false);
            forAll(removeProcFace, facei)
                if( removeFace[start+facei] )
                    removeProcFace[facei] = true;

            OPstream toOtherProc
            (
                Pstream::blocking,
                procBoundaries[patchI].neiProcNo(),
                removeProcFace.byteSize()
            );
            toOtherProc << removeProcFace;
        }

        --npI;
        boolList ordinaryBoundaryFace(procFaces.size(), false);
        nProcFaces = 0;
        forAll(procBoundaries, patchI)
        {
            const label start = procBoundaries[patchI].patchStart();
            boolList removeOtherProcFace;
            IPstream fromOtherProc
            (
                Pstream::blocking,
                procBoundaries[patchI].neiProcNo()
            );
            fromOtherProc >> removeOtherProcFace;

            forAll(removeOtherProcFace, facei)
            {
                if( !removeFace[start+facei] && removeOtherProcFace[facei] )
                {
                    //- this face becomes an ordinary boundary face
                    //- because the face on the other side of the processor
                    //- boundary has been deleted
                    ordinaryBoundaryFace[nProcFaces] = true;
                    ++boundaries[npI].patchSize();
                    if( nFaces < (start+facei) )
                        faces[nFaces].transfer(procFaces[nProcFaces]);
                    newFaceLabel[start+facei] = nFaces++;
                }

                ++nProcFaces;
            }
        }

        if( nProcFaces != procFaces.size() )
            FatalErrorInFunction
              << "Invalid number of processor faces!" << abort(FatalError);

        //- remove faces from processor patches
        npI = 0;
        nFacesInPatch.setSize(procBoundaries.size());
        patchStart.setSize(procBoundaries.size());
        labelList neiProcNo(procBoundaries.size());
        nProcFaces = 0;

        forAll(procBoundaries, patchI)
        {
            const label oldStart = procBoundaries[patchI].patchStart();
            const label oldNumFacesInPatch = procBoundaries[patchI].patchSize();

            patchStart[npI] = nFaces;
            neiProcNo[npI] = procBoundaries[patchI].neiProcNo();
            nFacesInPatch[npI] = 0;

            for(label facei=0;facei<oldNumFacesInPatch;++facei)
            {
                if(
                    !removeFace[oldStart+facei] &&
                    !ordinaryBoundaryFace[nProcFaces]
                )
                {
                    ++nFacesInPatch[npI];
                    faces[nFaces].transfer(procFaces[nProcFaces]);
                    newFaceLabel[oldStart+facei] = nFaces++;
                }

                ++nProcFaces;
            }

            ++npI;
        }

        //- all patches still exist
        forAll(procBoundaries, patchI)
        {
            procBoundaries[patchI].patchSize() = nFacesInPatch[patchI];
            procBoundaries[patchI].patchStart() = patchStart[patchI];
        }
    }

    faces.setSize(nFaces);

    //- update face subsets in the mesh
    mesh_.updateFaceSubsets(newFaceLabel);

    //- change cells
    # ifdef USE_OMP
    # pragma omp parallel for if( cells.size() > 1000 ) schedule(dynamic, 40)
    # endif
    forAll(cells, celli)
    {
        cell& c = cells[celli];

        DynList<label> newC;

        forAll(c, fI)
            if( newFaceLabel[c[fI]] != -1 )
                newC.append(newFaceLabel[c[fI]]);

        c.setSize(newC.size());

        forAll(c, fI)
            c[fI] = newC[fI];
    }

    mesh_.clearOut();

    Info << "Finished removing faces" << endl;
}

void polyMeshGenModifier::removeDuplicateFaces()
{
    VRWGraph& pointFaces = this->pointFaces();

    faceListPMG& faces = mesh_.faces_;

    labelLongList newFaceLabel(faces.size(), -1);

    label nFaces(0);
    forAll(faces, facei)
    {
        if( newFaceLabel[facei] != -1 )
            continue;

        const face& f = faces[facei];
        DynList<label> duplicates;
        forAllRow(pointFaces, f[0], pfI)
        {
            const label faceJ = pointFaces(f[0], pfI);

            if( facei == faceJ )
                continue;

            if( faces[faceJ] == f )
                duplicates.append(faceJ);
        }

        newFaceLabel[facei] = nFaces;
        forAll(duplicates, i)
            newFaceLabel[duplicates[i]] = nFaces;
        ++nFaces;
    }

    label nInternalFaces(faces.size());
    if( mesh_.boundaries_.size() != 0 )
        nInternalFaces = mesh_.boundaries_[0].patchStart();

    //- remove internal faces
    for(label facei=0;facei<nInternalFaces;++facei)
    {
        if( newFaceLabel[facei] >= facei )
            continue;

        faces[newFaceLabel[facei]].transfer(faces[facei]);
    }

    //- update boundary faces (they cannot be duplicated)
    forAll(mesh_.boundaries_, patchI)
    {
        boundaryPatch& patch = mesh_.boundaries_[patchI];

        const label start = patch.patchStart();
        const label end = start + patch.patchSize();

        patch.patchStart() = newFaceLabel[start];
        for(label facei=start;facei<end;++facei)
        {
            if( newFaceLabel[facei] <  facei )
                faces[newFaceLabel[facei]].transfer(faces[facei]);
        }
    }

    //- update processor faces (they cannot be duplicated)
    forAll(mesh_.procBoundaries_, patchI)
    {
        processorBoundaryPatch& patch = mesh_.procBoundaries_[patchI];

        const label start = patch.patchStart();
        const label end = start + patch.patchSize();

        patch.patchStart() = newFaceLabel[start];
        for(label facei=start;facei<end;++facei)
        {
            if( newFaceLabel[facei] < facei )
                faces[newFaceLabel[facei]].transfer(faces[facei]);
        }
    }

    faces.setSize(nFaces);
    mesh_.updateFaceSubsets(newFaceLabel);

    //- change cells
    cellListPMG& cells = mesh_.cells_;
    # ifdef USE_OMP
    # pragma omp parallel for if( cells.size() > 1000 ) schedule(dynamic, 40)
    # endif
    forAll(cells, celli)
    {
        cell& c = cells[celli];

        DynList<label> newC;

        forAll(c, fI)
            if( newFaceLabel[c[fI]] != -1 )
                newC.append(newFaceLabel[c[fI]]);

        c.setSize(newC.size());

        forAll(c, fI)
            c[fI] = newC[fI];
    }

    mesh_.clearOut();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
