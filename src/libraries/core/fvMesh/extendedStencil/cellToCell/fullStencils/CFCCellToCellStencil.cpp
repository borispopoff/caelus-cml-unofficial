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

\*---------------------------------------------------------------------------*/

#include "CFCCellToCellStencil.hpp"
#include "syncTools.hpp"
#include "SortableList.hpp"
#include "emptyPolyPatch.hpp"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// Calculates per face the neighbour data (= cell or boundary face)
void CML::CFCCellToCellStencil::calcFaceBoundaryData
(
    labelList& neiGlobal
) const
{
    const polyBoundaryMesh& patches = mesh().boundaryMesh();
    const label nBnd = mesh().nFaces()-mesh().nInternalFaces();
    const labelList& own = mesh().faceOwner();

    neiGlobal.setSize(nBnd);

    forAll(patches, patchi)
    {
        const polyPatch& pp = patches[patchi];
        label facei = pp.start();

        if (pp.coupled())
        {
            // For coupled faces get the cell on the other side
            forAll(pp, i)
            {
                label bFaceI = facei-mesh().nInternalFaces();
                neiGlobal[bFaceI] = globalNumbering().toGlobal(own[facei]);
                facei++;
            }
        }
        else if (isA<emptyPolyPatch>(pp))
        {
            forAll(pp, i)
            {
                label bFaceI = facei-mesh().nInternalFaces();
                neiGlobal[bFaceI] = -1;
                facei++;
            }
        }
        else
        {
            // For noncoupled faces get the boundary face.
            forAll(pp, i)
            {
                label bFaceI = facei-mesh().nInternalFaces();
                neiGlobal[bFaceI] =
                    globalNumbering().toGlobal(mesh().nCells()+bFaceI);
                facei++;
            }
        }
    }
    syncTools::swapBoundaryFaceList(mesh(), neiGlobal);
}


// Calculates per cell the neighbour data (= cell or boundary in global
// numbering). First element is always cell itself!
void CML::CFCCellToCellStencil::calcCellStencil(labelListList& globalCellCells)
 const
{
    const label nBnd = mesh().nFaces()-mesh().nInternalFaces();
    const labelList& own = mesh().faceOwner();
    const labelList& nei = mesh().faceNeighbour();


    // Calculate coupled neighbour (in global numbering)
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    labelList neiGlobal(nBnd);
    calcFaceBoundaryData(neiGlobal);


    // Determine cellCells in global numbering
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    globalCellCells.setSize(mesh().nCells());
    forAll(globalCellCells, celli)
    {
        const cell& cFaces = mesh().cells()[celli];

        labelList& cCells = globalCellCells[celli];

        cCells.setSize(cFaces.size()+1);

        label nNbr = 0;

        // Myself
        cCells[nNbr++] = globalNumbering().toGlobal(celli);

        // Collect neighbouring cells/faces
        forAll(cFaces, i)
        {
            label facei = cFaces[i];

            if (mesh().isInternalFace(facei))
            {
                label nbrCellI = own[facei];
                if (nbrCellI == celli)
                {
                    nbrCellI = nei[facei];
                }
                cCells[nNbr++] = globalNumbering().toGlobal(nbrCellI);
            }
            else
            {
                label nbrCellI = neiGlobal[facei-mesh().nInternalFaces()];
                if (nbrCellI != -1)
                {
                    cCells[nNbr++] = nbrCellI;
                }
            }
        }
        cCells.setSize(nNbr);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::CFCCellToCellStencil::CFCCellToCellStencil(const polyMesh& mesh)
:
    cellToCellStencil(mesh)
{
    // Calculate per cell the (face) connected cells (in global numbering)
    calcCellStencil(*this);
}


// ************************************************************************* //
