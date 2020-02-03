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

#include "meshOctreeAddressing.hpp"
#include "meshOctree.hpp"
#include "demandDrivenData.hpp"
#include "helperFunctionsPar.hpp"

#include <map>

//#define DEBUGAutoRef

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void meshOctreeAddressing::checkAndFixIrregularConnections()
{
    Info << "Checking the surface of the selected boxes" << endl;

    const labelLongList& owner = this->octreeFaceOwner();
    const labelLongList& neighbour = this->octreeFaceNeighbour();
    const VRWGraph& faceEdges = this->faceEdges();
    const VRWGraph& edgeFaces = this->edgeFaces();
    const VRWGraph& edgeLeaves = this->edgeLeaves();
    const VRWGraph& pointFaces = this->nodeFaces();

    List<direction>& boxType = *boxTypePtr_;

    boolList boundaryFace(owner.size());

    label nIrregular;
    DynList<label> front;

    do
    {
        nIrregular = 0;

        labelHashSet changedBoxType(100);

        //- find boundary faces
        boundaryFace = false;

        forAll(owner, facei)
        {
            const label own = owner[facei];
            const label nei = neighbour[facei];

            if( nei < 0 )
            {
                continue;
            }
            else
            {

                if(
                    ((boxType[nei] & BOUNDARY) && (boxType[own] & MESHCELL))
                ||  ((boxType[own] & BOUNDARY) && (boxType[nei] & MESHCELL))
                )
                    boundaryFace[facei] = true;
            }
        }

        //- remove irregular connections over edges
        forAll(edgeFaces, edgeI)
        {
            label nBoundaryFaces(0);
            forAllRow(edgeFaces, edgeI, efI)
            {
                const label facei = edgeFaces(edgeI, efI);

                if( boundaryFace[facei] )
                    ++nBoundaryFaces;
            }

            if( nBoundaryFaces > 2 )
            {
                ++nIrregular;
                forAllRow(edgeLeaves, edgeI, elI)
                {
                    const label leafI = edgeLeaves(edgeI, elI);
                    boxType[leafI] = BOUNDARY;
                    changedBoxType.insert(leafI);
                }
            }
        }

        //- check if there exist two or more boundary face groups
        //- connected to a vertex
        forAll(pointFaces, pI)
        {
            //- find boundary faces connected to the vertex
            labelHashSet bndFacesAtNode(pointFaces.sizeOfRow(pI));
            forAllRow(pointFaces, pI, pfI)
            {
                const label facei = pointFaces(pI, pfI);
                if( boundaryFace[facei] )
                    bndFacesAtNode.insert(facei);
            }

            //- find the number of face groups at a given vertex
            label nGroups(0);
            bool watertightSurface(true);
            while( bndFacesAtNode.size() != 0 )
            {
                front.clear();
                front.append(bndFacesAtNode.begin().key());
                bndFacesAtNode.erase(front[0]);

                while( front.size() != 0 )
                {
                    const label fLabel = front.removeLastElement();

                    forAllRow(faceEdges, fLabel, feI)
                    {
                        const label eI = faceEdges(fLabel, feI);

                        bool found(false);
                        forAllRow(edgeFaces, eI, efI)
                        {
                            const label fJ = edgeFaces(eI, efI);

                            if( bndFacesAtNode.found(fJ) )
                            {
                                found = true;
                                front.append(fJ);
                                bndFacesAtNode.erase(fJ);
                            }
                        }

                        if( !found )
                        {
                            watertightSurface = false;
                            break;
                        }
                    }
                }

                ++nGroups;
            }

            if( watertightSurface && (nGroups > 1) )
            {
                ++nIrregular;

                //- this vertex has two groups of faces connected to it
                forAllRow(pointFaces, pI, pfI)
                {
                    const label facei = pointFaces(pI, pfI);
                    if( boundaryFace[facei] )
                    {
                        //- set BOUNDARY flag to all boxes connected to it
                        if( boxType[owner[facei]] & MESHCELL )
                        {
                            changedBoxType.insert(owner[facei]);
                            boxType[owner[facei]] = BOUNDARY;
                        }

                        if( neighbour[facei] == -1 )
                            continue;

                        if( boxType[neighbour[facei]] & MESHCELL )
                        {
                            changedBoxType.insert(neighbour[facei]);
                            boxType[neighbour[facei]] = BOUNDARY;
                        }
                    }
                }
            }
        }

        reduce(nIrregular, sumOp<label>());
        Info << nIrregular << " surface connections found!" << endl;

        if( Pstream::parRun() && (nIrregular != 0) )
        {
            LongList<meshOctreeCubeCoordinates> exchangeData;
            forAllConstIter(labelHashSet, changedBoxType, it)
                exchangeData.append(octree_.returnLeaf(it.key()).coordinates());

            LongList<meshOctreeCubeCoordinates> receivedData;
            octree_.exchangeRequestsWithNeighbourProcessors
            (
                exchangeData,
                receivedData
            );

            forAll(receivedData, i)
            {
                const label leafI =
                    octree_.findLeafLabelForPosition(receivedData[i]);
                if( leafI < 0 )
                    continue;

                boxType[leafI] = BOUNDARY;
            }
        }

    } while( nIrregular != 0 );

    clearNodeAddressing();
    clearOctreeFaces();
    clearAddressing();

    Info << "Finished checking the surface of the selected boxes" << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
