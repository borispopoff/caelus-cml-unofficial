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

#include "cellLooper.hpp"
#include "polyMesh.hpp"
#include "ListOps.hpp"
#include "meshTools.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(cellLooper, 0);
    defineRunTimeSelectionTable(cellLooper, word);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

CML::autoPtr<CML::cellLooper> CML::cellLooper::New
(
    const word& type,
    const polyMesh& mesh
)
{
    wordConstructorTable::iterator cstrIter =
        wordConstructorTablePtr_->find(type);

    if (cstrIter == wordConstructorTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown set type "
            << type << nl << nl
            << "Valid cellLooper types : " << endl
            << wordConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    return autoPtr<cellLooper>(cstrIter()(mesh));
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// Get faces (on cell) connected to vertI which are not using edgeI
CML::labelList CML::cellLooper::getVertFacesNonEdge
(
    const label celli,
    const label edgeI,
    const label vertI
) const
{
    // Get faces connected to startEdge
    label face0, face1;
    meshTools::getEdgeFaces(mesh(), celli, edgeI, face0, face1);

    const labelList& pFaces = mesh().pointFaces()[vertI];

    labelList vertFaces(pFaces.size());
    label vertFaceI = 0;

    forAll(pFaces, pFaceI)
    {
        label facei = pFaces[pFaceI];

        if
        (
            (facei != face0)
         && (facei != face1)
         && (meshTools::faceOnCell(mesh(), celli, facei))
        )
        {
            vertFaces[vertFaceI++] = facei;
        }
    }
    vertFaces.setSize(vertFaceI);

    return vertFaces;
}


// Get first edge connected to vertI and on facei
CML::label CML::cellLooper::getFirstVertEdge
(
    const label facei,
    const label vertI
) const
{
    const labelList& fEdges = mesh().faceEdges()[facei];

    forAll(fEdges, fEdgeI)
    {
        label edgeI = fEdges[fEdgeI];

        const edge& e = mesh().edges()[edgeI];

        if ((e.start() == vertI) || (e.end() == vertI))
        {
            return edgeI;
        }
    }

    FatalErrorInFunction
        << "Can not find edge on face " << facei
        << " using vertex " << vertI
        << abort(FatalError);

    return -1;
}


// Get edges (on cell) connected to vertI which are not on facei
CML::labelList CML::cellLooper::getVertEdgesNonFace
(
    const label celli,
    const label facei,
    const label vertI
) const
{
    const labelList& exclEdges = mesh().faceEdges()[facei];

    const labelList& pEdges = mesh().pointEdges()[vertI];

    labelList vertEdges(pEdges.size());
    label vertEdgeI = 0;

    forAll(pEdges, pEdgeI)
    {
        label edgeI = pEdges[pEdgeI];

        if
        (
            (findIndex(exclEdges, edgeI) == -1)
         && meshTools::edgeOnCell(mesh(), celli, edgeI)
        )
        {
            vertEdges[vertEdgeI++] = edgeI;
        }
    }

    vertEdges.setSize(vertEdgeI);

    return vertEdges;
}


// Return edge from cellEdges that is most perpendicular
// to refinement direction.
CML::label CML::cellLooper::getMisAlignedEdge
(
    const vector& refDir,
    const label celli
) const
{
    const labelList& cEdges = mesh().cellEdges()[celli];

    label cutEdgeI = -1;
    scalar maxCos = -GREAT;

    forAll(cEdges, cEdgeI)
    {
        label edgeI = cEdges[cEdgeI];

        scalar cosAngle = mag(refDir & meshTools::normEdgeVec(mesh(), edgeI));

        if (cosAngle > maxCos)
        {
            maxCos = cosAngle;

            cutEdgeI = edgeI;
        }
    }

    return cutEdgeI;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::cellLooper::cellLooper(const polyMesh& mesh)
:
    edgeVertex(mesh)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::cellLooper::~cellLooper()
{}


// ************************************************************************* //
