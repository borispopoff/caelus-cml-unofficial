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

#include "demandDrivenData.hpp"
#include "topologicalCleaner.hpp"
#include "decomposeFaces.hpp"
#include "DynList.hpp"

//#define DEBUGCleaner

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void topologicalCleaner::checkNonMappableCells()
{
    Info << "Checking for non-mappable cells" << endl;
    //- decompose cells with more than one boundary face
    const labelList& owner = mesh_.owner();

    List<direction> nBoundaryFaces(mesh_.cells().size(), direction(0));
    const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();
    forAll(boundaries, patchI)
    {
        const label start = boundaries[patchI].patchStart();
        const label end = start + boundaries[patchI].patchSize();

        for(label facei=start;facei<end;++facei)
        {
            ++nBoundaryFaces[owner[facei]];
        }
    }

    label nBadCells(0);
    forAll(nBoundaryFaces, cI)
        if( nBoundaryFaces[cI] > 1 )
        {
            ++nBadCells;
            decomposeCell_[cI] = true;
        }

    if( Pstream::parRun() )
        reduce(nBadCells, sumOp<label>());

    if( nBadCells != 0 )
        changed_ = true;

    Info << "Found " << nBadCells << " non-mappable cells" << endl;
    Info << "Finished checking for non-mappable cells" << endl;
}

void topologicalCleaner::checkNonMappableFaces()
{
    Info << "Checking for non-mappable faces" << endl;

    const faceListPMG& faces = mesh_.faces();
    const labelList& owner = mesh_.owner();
    const labelList& neighbour = mesh_.neighbour();

    //- find boundary vertices
    boolList boundaryVertex(mesh_.points().size(), false);
    const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();
    forAll(boundaries, patchI)
    {
        const label start = boundaries[patchI].patchStart();
        const label end = start + boundaries[patchI].patchSize();

        for(label facei=start;facei<end;++facei)
        {
            const face& f = faces[facei];
            forAll(f, pI)
                boundaryVertex[f[pI]] = true;
        }
    }

    boolList decomposeFace(faces.size(), false);

    //- internal faces which have more than two vertices at the boundary
    //- cannot always be mapped at the boundary and form a valid cell
    //- The second case of interest are faces which have two vertices at the
    //- boundary but are not connected over an edge
    const label nIntFaces = mesh_.nInternalFaces();

    //bool changed(false);

    label nBadFaces(0);
    for(label facei=0;facei<nIntFaces;++facei)
    {
        const face& f = faces[facei];

        DynList<label> bPos;
        forAll(f, pI)
            if( boundaryVertex[f[pI]] )
                bPos.append(pI);

        if(
            (bPos.size() > 2) ||
            (
                (bPos.size() == 2) &&
                (
                    (bPos[1] != (bPos[0] + 1)) &&
                    !((bPos[0] == 0) && (bPos[1] == (f.size() - 1)))
                )
            )
        )
        {
            ++nBadFaces;
            decomposeFace[facei] = true;
            decomposeCell_[owner[facei]] = true;
            decomposeCell_[neighbour[facei]] = true;
        }
    }

    if( Pstream::parRun() )
    {
        //- check processor faces
        const PtrList<processorBoundaryPatch>& procBoundaries =
            mesh_.procBoundaries();

        forAll(procBoundaries, patchI)
        {
            const label start = procBoundaries[patchI].patchStart();
            const label end = start + procBoundaries[patchI].patchSize();

            boolList decProcFace(procBoundaries[patchI].patchSize(), false);

            for(label facei=start;facei<end;++facei)
            {
                const face& f = faces[facei];

                DynList<label> bPos;
                forAll(f, pI)
                    if( boundaryVertex[f[pI]] )
                        bPos.append(pI);

                if(
                    (bPos.size() > 2) ||
                    (
                        (bPos.size() == 2) &&
                        (
                            (bPos[1] != (bPos[0] + 1)) &&
                            !((bPos[0] == 0) && (bPos[1] == (f.size() - 1)))
                        )
                    )
                )
                {
                    ++nBadFaces;
                    decProcFace[facei-start] = true;
                    decomposeFace[facei] = true;
                    decomposeCell_[owner[facei]] = true;
                }
            }

            //- send information about decomposed faces to other processor
            OPstream toOtherProc
            (
                Pstream::blocking,
                procBoundaries[patchI].neiProcNo(),
                decProcFace.byteSize()
            );
            toOtherProc << decProcFace;
        }

        forAll(procBoundaries, patchI)
        {
            boolList decOtherProc;
            IPstream fromOtherProc
            (
                Pstream::blocking,
                procBoundaries[patchI].neiProcNo()
            );
            fromOtherProc >> decOtherProc;

            const label start = procBoundaries[patchI].patchStart();
            forAll(decOtherProc, facei)
                if( decOtherProc[facei] )
                {
                    decomposeFace[start+facei] = true;
                    decomposeCell_[owner[start+facei]] = true;
                }
        }

        reduce(nBadFaces, sumOp<label>());
    }

    Info << "Found " << nBadFaces << " non-mappable faces" << endl;

    if( nBadFaces != 0 )
    {
        changed_ = true;
        decomposeFaces(mesh_).decomposeMeshFaces(decomposeFace);
    }

    Info << "Finished checking non-mappable faces" << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
