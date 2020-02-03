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

#include "cartesianMeshExtractor.hpp"
#include "demandDrivenData.hpp"
#include "decomposeFaces.hpp"
#include "decomposeCells.hpp"
#include "hexMatcher.hpp"

//#define DEBUGMesh

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void cartesianMeshExtractor::decomposeSplitHexesIntoTetsAndPyramids()
{
    if( !decomposeSplitHexes_ ) return;

    Info << "Decomposing split-hex cells" << endl;
    
    const faceListPMG& faces = mesh_.faces();
    
    //- decompose faces which have more than 4 vertices
    boolList decompose(faces.size(), false);
    
    label nDecomposed(0);
    forAll(faces, facei)
    {
        if( faces[facei].size() > 4 )
        {
            ++nDecomposed;
            
            decompose[facei] = true;
        }
    }
    
    reduce(nDecomposed, sumOp<label>());
    
    Info << "Decomposing " << nDecomposed
        << " faces with more than 4 vertices" << endl;
    
    if( nDecomposed != 0 )
    {
        //- decompose marked faces into triangles
        decomposeFaces(mesh_).decomposeMeshFaces(decompose);
    }
    
    //- decompose cells with 24 faces
    const cellListPMG& cells = mesh_.cells();
    decompose.setSize(cells.size());
    decompose = false;
    
    hexMatcher hex;
    forAll(cells, celli)
    {
        if( !hex.matchShape(true, faces, mesh_.owner(), celli, cells[celli]) )
        {
            ++nDecomposed;
            decompose[celli] = true;
        }
    }
    
    reduce(nDecomposed, sumOp<label>());
    
    Info << "Decomposing " << nDecomposed
        << " cells into tetrahedra and pyramids" << endl;
    
    if( nDecomposed )
    {
        //- decompose marked cells into tets and pyramids
        decomposeCells dc(mesh_);
        dc.decomposeMesh(decompose);
    }
    
    Info << "Finished decomposing split-hex cells" << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
