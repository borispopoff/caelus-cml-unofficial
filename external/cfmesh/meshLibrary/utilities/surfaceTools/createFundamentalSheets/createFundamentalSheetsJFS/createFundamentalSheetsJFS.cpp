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

#include "createFundamentalSheetsJFS.hpp"
#include "demandDrivenData.hpp"
#include "meshSurfaceEngine.hpp"
#include "extrudeLayer.hpp"

#include "addToRunTimeSelectionTable.hpp"

# ifdef USE_OMP
#include <omp.h>
# endif

//#define DEBUGSheets

# ifdef DEBUGSheets
#include "helperFunctions.hpp"
# endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

defineTypeNameAndDebug(createFundamentalSheetsJFS, 0);
addToRunTimeSelectionTable
(
    createFundamentalSheets,
    createFundamentalSheetsJFS,
    polyMeshGen
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

bool createFundamentalSheetsJFS::isTopologyOk() const
{
    const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();

    const label start = boundaries[0].patchStart();
    const label end
    (
        boundaries[boundaries.size()-1].patchStart() +
        boundaries[boundaries.size()-1].patchSize()
    );

    const labelList& owner = mesh_.owner();

    //- count the number of boundary faces in every cell
    //- cells with more than one boundary face cause problem to the
    //- sheet insertion procedure
    labelList nBndFacesInCell(mesh_.cells().size(), 0);

    bool isOkTopo(true);
    for(label facei=start;facei<end;++facei)
    {
        ++nBndFacesInCell[owner[facei]];

        if( nBndFacesInCell[owner[facei]] > 1 )
        {
            isOkTopo = false;
            break;
        }
    }

    reduce(isOkTopo, minOp<bool>());

    return isOkTopo;
}

void createFundamentalSheetsJFS::createInitialSheet()
{
    if( !createWrapperSheet_ )
    {
        if( isTopologyOk() )
            return;

        Warning << "Found invalid topology!"
                << "\nStarting creating initial wrapper sheet" << endl;
    }

    Info << "Creating initial wrapper sheet" << endl;

    const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();

    const label start = boundaries[0].patchStart();
    const label end
    (
        boundaries[boundaries.size()-1].patchStart() +
        boundaries[boundaries.size()-1].patchSize()
    );

    const labelList& owner = mesh_.owner();

    LongList<labelPair> extrudeFaces(end-start);

    # ifdef USE_OMP
    # pragma omp parallel for schedule(guided, 100)
    # endif
    for(label facei=start;facei<end;++facei)
        extrudeFaces[facei-start] = labelPair(facei, owner[facei]);

    extrudeLayer(mesh_, extrudeFaces);

    Info << "Finished creating initial wrapper sheet" << endl;
}

void createFundamentalSheetsJFS::createSheetsAtFeatureEdges()
{
    Info << "Starting creating sheets at feature edges" << endl;

    const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();

    if( returnReduce(boundaries.size(), maxOp<label>()) < 2 )
    {
        Info << "Skipping creating sheets at feature edges" << endl;
        return;
    }

    const cellListPMG& cells = mesh_.cells();
    const labelList& owner = mesh_.owner();
    const labelList& neighbour = mesh_.neighbour();

    const label start = boundaries[0].patchStart();
    const label end
    (
        boundaries[boundaries.size()-1].patchStart() +
        boundaries[boundaries.size()-1].patchSize()
    );

    faceListPMG::subList bFaces(mesh_.faces(), end-start, start);
    labelList facePatch(bFaces.size());

    forAll(boundaries, patchI)
    {
        const label patchStart = boundaries[patchI].patchStart();
        const label patchEnd = patchStart + boundaries[patchI].patchSize();

        for(label facei=patchStart;facei<patchEnd;++facei)
            facePatch[facei-start] = patchI;
    }

    labelList patchCell(mesh_.cells().size(), -1);
    forAll(facePatch, bfI)
        patchCell[owner[start+bfI]] = facePatch[bfI];

    # ifdef DEBUGSheets
    labelList patchSheetId(boundaries.size());
    forAll(patchSheetId, patchI)
        patchSheetId[patchI] =
            mesh_.addCellSubset("sheetPatch_"+help::labelToText(patchI));

    forAll(patchCell, celli)
    {
        if( patchCell[celli] < 0 )
            continue;

        mesh_.addCellToSubset(patchSheetId[patchCell[celli]], celli);
    }
    # endif

    LongList<labelPair> front;

    # ifdef USE_OMP
    const label nThreads = 3 * omp_get_num_procs();
    # pragma omp parallel num_threads(nThreads)
    # endif
    {
        //- create the front faces
        LongList<labelPair> localFront;

        # ifdef USE_OMP
        # pragma omp for
        # endif
        forAll(facePatch, bfI)
        {
            const label facei = start + bfI;
            const label celli = owner[facei];

            const cell& c = cells[celli];
            const label patchI = facePatch[bfI];

            forAll(c, fI)
            {
                if( neighbour[c[fI]] < 0 )
                    continue;

                label nei = owner[c[fI]];
                if( nei == celli )
                    nei = neighbour[c[fI]];

                if( patchCell[nei] != patchI )
                    localFront.append(labelPair(c[fI], celli));
            }
        }

        label frontStart(-1);
        # ifdef USE_OMP
        # pragma omp critical
        # endif
        {
            frontStart = front.size();
            front.setSize(front.size()+localFront.size());
        }

        # ifdef USE_OMP
        # pragma omp barrier
        # endif

        //- copy the local front into the global front
        forAll(localFront, lfI)
            front[frontStart+lfI] = localFront[lfI];
    }

    # ifdef DEBUGSheets
    const label fId = mesh_.addFaceSubset("facesForFundamentalSheets");
    const label cId = mesh_.addCellSubset("cellsForFundamentalSheets");

    forAll(front, fI)
    {
        mesh_.addFaceToSubset(fId, front[fI].first());
        mesh_.addCellToSubset(cId, front[fI].second());
    }

    mesh_.write();
    # endif

    //- extrude the layer
    extrudeLayer(mesh_, front);

    Info << "Finished creating sheets at feature edges" << endl;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from mesh, octree, regions for boundary vertices
createFundamentalSheetsJFS::createFundamentalSheetsJFS
(
    polyMeshGen& mesh,
    const bool createWrapperSheet
)
:
    createFundamentalSheets(mesh, createWrapperSheet)
{
    createInitialSheet();

    createSheetsAtFeatureEdges();
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

createFundamentalSheetsJFS::~createFundamentalSheetsJFS()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
