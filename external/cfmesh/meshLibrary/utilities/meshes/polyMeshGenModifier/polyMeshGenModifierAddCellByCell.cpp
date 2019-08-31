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

#include "polyMeshGenModifierAddCellByCell.hpp"
#include "demandDrivenData.hpp"

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

polyMeshGenModifierAddCellByCell::polyMeshGenModifierAddCellByCell
(
    polyMeshGen& mesh
)
:
    polyMeshGenModifier(mesh),
    nFaces_(mesh.faces().size()),
    newFaces_(nFaces_),
    nCells_(mesh.cells().size()),
    newCells_(nCells_)
{
    this->pointFaces();
    faceListPMG& faces = this->facesAccess();
    forAll(faces, facei)
        newFaces_[facei].transfer(faces[facei]);
    
    cellListPMG& cells = this->cellsAccess();
    forAll(cells, celli)
        newCells_[celli].transfer(cells[celli]);
};
            
// Destructor
polyMeshGenModifierAddCellByCell::~polyMeshGenModifierAddCellByCell()
{
    faceListPMG& faces = this->facesAccess();
    faces.setSize(nFaces_);
    forAll(faces, facei)
        faces[facei].transfer(newFaces_[facei]);
    
    cellListPMG& cells = this->cellsAccess();
    cells.setSize(newCells_.size());
    forAll(cells, celli)
        cells[celli].transfer(newCells_[celli]);
}
    
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void polyMeshGenModifierAddCellByCell::addCell(const faceList& cellFaces)
{
    cell c(cellFaces.size());
    
    VRWGraph& pointFaces = this->pointFaces();
    
    forAll(cellFaces, facei)
    {
        const face& f = cellFaces[facei];
        
        const label pointI = f[0];
        
        label fLabel(-1);
        forAllRow(pointFaces, pointI, pfI)
        {
            const label facei = pointFaces(pointI, pfI);
            
            if( newFaces_[facei] == f )
            {
                fLabel = facei;
                break;
            }
        }
            
        if( fLabel == -1 )
        {
            newFaces_.append(f);
            c[facei] = nFaces_;
            forAll(f, pI)
                pointFaces.append(f[pI], nFaces_);
            
            ++nFaces_;
        }
        else
        {
            c[facei] = fLabel;
        }
    }
    
    newCells_.append(c);
    ++nCells_;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
