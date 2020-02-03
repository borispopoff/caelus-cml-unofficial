/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
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

#include "edgeCollapser.hpp"
#include "polyMesh.hpp"
#include "polyTopoChange.hpp"
#include "ListOps.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

CML::label CML::edgeCollapser::findIndex
(
    const labelList& elems,
    const label start,
    const label size,
    const label val
)
{
    for (label i = start; i < size; i++)
    {
        if (elems[i] == val)
        {
            return i;
        }
    }
    return -1;
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// Changes region of connected set of points
CML::label CML::edgeCollapser::changePointRegion
(
    const label pointi,
    const label oldRegion,
    const label newRegion
)
{
    label nChanged = 0;

    if (pointRegion_[pointi] == oldRegion)
    {
        pointRegion_[pointi] = newRegion;
        nChanged++;

        // Step to neighbouring point across edges with same region number

        const labelList& pEdges = mesh_.pointEdges()[pointi];

        forAll(pEdges, i)
        {
            label otherPointi = mesh_.edges()[pEdges[i]].otherVertex(pointi);

            nChanged += changePointRegion(otherPointi, oldRegion, newRegion);
        }
    }
    return nChanged;
}


bool CML::edgeCollapser::pointRemoved(const label pointi) const
{
    label region = pointRegion_[pointi];

    if (region == -1 || pointRegionMaster_[region] == pointi)
    {
        return false;
    }
    else
    {
        return true;
    }
}


void CML::edgeCollapser::filterFace(const label facei, face& f) const
{
    label newFp = 0;

    forAll(f, fp)
    {
        label pointi = f[fp];

        label region = pointRegion_[pointi];

        if (region == -1)
        {
            f[newFp++] = pointi;
        }
        else
        {
            label master = pointRegionMaster_[region];

            if (findIndex(f, 0, newFp, master) == -1)
            {
                f[newFp++] = master;
            }
        }
    }

    // Check for pinched face. Tries to correct
    // - consecutive duplicate vertex. Removes duplicate vertex.
    // - duplicate vertex with one other vertex in between (spike).
    // Both of these should not really occur! and should be checked before
    // collapsing edges.

    const label size = newFp;

    newFp = 2;

    for (label fp = 2; fp < size; fp++)
    {
        label fp1 = fp-1;
        label fp2 = fp-2;

        label pointi = f[fp];

        // Search for previous occurrence.
        label index = findIndex(f, 0, fp, pointi);

        if (index == fp1)
        {
            WarningInFunction
                << "Removing consecutive duplicate vertex in face "
                << f << endl;
            // Don't store current pointi
        }
        else if (index == fp2)
        {
            WarningInFunction
                << "Removing non-consecutive duplicate vertex in face "
                << f << endl;
            // Don't store current pointi and remove previous
            newFp--;
        }
        else if (index != -1)
        {
            WarningInFunction
                << "Pinched face " << f << endl;
            f[newFp++] = pointi;
        }
        else
        {
            f[newFp++] = pointi;
        }
    }

    f.setSize(newFp);
}


// Debugging.
void CML::edgeCollapser::printRegions() const
{
    forAll(pointRegionMaster_, regionI)
    {
        label master = pointRegionMaster_[regionI];

        if (master != -1)
        {
            Info<< "Region:" << regionI << nl
                << "    master:" << master
                << ' ' << mesh_.points()[master] << nl;

            forAll(pointRegion_, pointi)
            {
                if (pointRegion_[pointi] == regionI && pointi != master)
                {
                    Info<< "    slave:" << pointi
                        << ' ' <<  mesh_.points()[pointi] << nl;
                }
            }
        }
    }
}



// Collapse list of edges
void CML::edgeCollapser::collapseEdges(const labelList& edgeLabels)
{
    const edgeList& edges = mesh_.edges();

    forAll(edgeLabels, i)
    {
        label edgeI = edgeLabels[i];
        const edge& e = edges[edgeI];

        label region0 = pointRegion_[e[0]];
        label region1 = pointRegion_[e[1]];

        if (region0 == -1)
        {
            if (region1 == -1)
            {
                // Both unaffected. Choose ad lib.
                collapseEdge(edgeI, e[0]);
            }
            else
            {
                // Collapse to whatever e[1] collapses
                collapseEdge(edgeI, e[1]);
            }
        }
        else
        {
            if (region1 == -1)
            {
                // Collapse to whatever e[0] collapses
                collapseEdge(edgeI, e[0]);
            }
            else
            {
                // Both collapsed.
                if (pointRegionMaster_[region0] == e[0])
                {
                    // e[0] is a master
                    collapseEdge(edgeI, e[0]);
                }
                else if (pointRegionMaster_[region1] == e[1])
                {
                    // e[1] is a master
                    collapseEdge(edgeI, e[1]);
                }
                else
                {
                    // Don't know
                    collapseEdge(edgeI, e[0]);
                }
            }
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from mesh
CML::edgeCollapser::edgeCollapser(const polyMesh& mesh)
:
    mesh_(mesh),
    pointRegion_(mesh.nPoints(), -1),
    pointRegionMaster_(mesh.nPoints() / 100),
    freeRegions_()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CML::edgeCollapser::unaffectedEdge(const label edgeI) const
{
    const edge& e = mesh_.edges()[edgeI];

    return (pointRegion_[e[0]] == -1) && (pointRegion_[e[1]] == -1);
}


bool CML::edgeCollapser::collapseEdge(const label edgeI, const label master)
{
    const edge& e = mesh_.edges()[edgeI];

    label pointRegion0 = pointRegion_[e[0]];
    label pointRegion1 = pointRegion_[e[1]];

    if (pointRegion0 == -1)
    {
        if (pointRegion1 == -1)
        {
            // Both endpoints not collapsed. Create new region.

            label freeRegion = -1;

            if (freeRegions_.size())
            {
                freeRegion = freeRegions_.removeHead();

                if (pointRegionMaster_[freeRegion] != -1)
                {
                    FatalErrorInFunction
                        << "Problem : freeed region :" << freeRegion
                        << " has already master "
                        << pointRegionMaster_[freeRegion]
                        << abort(FatalError);
                }
            }
            else
            {
                // If no region found create one. This is the only place where
                // new regions are created.
                freeRegion = pointRegionMaster_.size();
            }

            pointRegion_[e[0]] = freeRegion;
            pointRegion_[e[1]] = freeRegion;

            pointRegionMaster_(freeRegion) = master;
        }
        else
        {
            // e[1] is part of collapse network, e[0] not. Add e0 to e1 region.
            pointRegion_[e[0]] = pointRegion1;

            pointRegionMaster_[pointRegion1] = master;
        }
    }
    else
    {
        if (pointRegion1 == -1)
        {
            // e[0] is part of collapse network. Add e1 to e0 region
            pointRegion_[e[1]] = pointRegion0;

            pointRegionMaster_[pointRegion0] = master;
        }
        else if (pointRegion0 != pointRegion1)
        {
            // Both part of collapse network. Merge the two regions.

            // Use the smaller region number for the whole network.
            label minRegion = min(pointRegion0, pointRegion1);
            label maxRegion = max(pointRegion0, pointRegion1);

            // Use minRegion as region for combined net, free maxRegion.
            pointRegionMaster_[minRegion] = master;
            pointRegionMaster_[maxRegion] = -1;
            freeRegions_.insert(maxRegion);

            if (minRegion != pointRegion0)
            {
                changePointRegion(e[0], pointRegion0, minRegion);
            }
            if (minRegion != pointRegion1)
            {
                changePointRegion(e[1], pointRegion1, minRegion);
            }
        }
    }

    return true;
}


bool CML::edgeCollapser::setRefinement(polyTopoChange& meshMod)
{
    const cellList& cells = mesh_.cells();
    const labelList& faceOwner = mesh_.faceOwner();
    const labelList& faceNeighbour = mesh_.faceNeighbour();
    const labelListList& pointFaces = mesh_.pointFaces();
    const labelListList& cellEdges = mesh_.cellEdges();

    // Print regions:
    //printRegions()

    bool meshChanged = false;


    // Current faces (is also collapseStatus: f.size() < 3)
    faceList newFaces(mesh_.faces());

    // Current cellCollapse status
    boolList cellRemoved(mesh_.nCells(), false);


    do
    {
        // Update face collapse from edge collapses
        forAll(newFaces, facei)
        {
            filterFace(facei, newFaces[facei]);
        }

        // Check if faces to be collapsed cause cells to become collapsed.
        label nCellCollapsed = 0;

        forAll(cells, celli)
        {
            if (!cellRemoved[celli])
            {
                const cell& cFaces = cells[celli];

                label nFaces = cFaces.size();

                forAll(cFaces, i)
                {
                    label facei = cFaces[i];

                    if (newFaces[facei].size() < 3)
                    {
                        --nFaces;

                        if (nFaces < 4)
                        {
                            Info<< "Cell:" << celli
                                << " uses faces:" << cFaces
                                << " of which too many are marked for removal:"
                                << endl
                                << "   ";
                            forAll(cFaces, j)
                            {
                                if (newFaces[cFaces[j]].size() < 3)
                                {
                                    Info<< ' '<< cFaces[j];
                                }
                            }
                            Info<< endl;

                            cellRemoved[celli] = true;

                            // Collapse all edges of cell to nothing
                            collapseEdges(cellEdges[celli]);

                            nCellCollapsed++;

                            break;
                        }
                    }
                }
            }
        }

        if (nCellCollapsed == 0)
        {
            break;
        }
    } while (true);


    // Keep track of faces that have been done already.
    boolList doneFace(mesh_.nFaces(), false);

    {
        // Mark points used.
        boolList usedPoint(mesh_.nPoints(), false);


        forAll(cellRemoved, celli)
        {
            if (cellRemoved[celli])
            {
                meshMod.removeCell(celli, -1);
            }
        }


        // Remove faces
        forAll(newFaces, facei)
        {
            const face& f = newFaces[facei];

            if (f.size() < 3)
            {
                meshMod.removeFace(facei, -1);
                meshChanged = true;

                // Mark face as been done.
                doneFace[facei] = true;
            }
            else
            {
                // Kept face. Mark vertices
                forAll(f, fp)
                {
                    usedPoint[f[fp]] = true;
                }
            }
        }

        // Remove unused vertices that have not been marked for removal already
        forAll(usedPoint, pointi)
        {
            if (!usedPoint[pointi] && !pointRemoved(pointi))
            {
                meshMod.removePoint(pointi, -1);
                meshChanged = true;
            }
        }
    }



    // Remove points.
    forAll(pointRegion_, pointi)
    {
        if (pointRemoved(pointi))
        {
            meshMod.removePoint(pointi, -1);
            meshChanged = true;
        }
    }



    const polyBoundaryMesh& boundaryMesh = mesh_.boundaryMesh();
    const faceZoneMesh& faceZones = mesh_.faceZones();


    // Renumber faces that use points
    forAll(pointRegion_, pointi)
    {
        if (pointRemoved(pointi))
        {
            const labelList& changedFaces = pointFaces[pointi];

            forAll(changedFaces, changedFaceI)
            {
                label facei = changedFaces[changedFaceI];

                if (!doneFace[facei])
                {
                    doneFace[facei] = true;

                    // Get current zone info
                    label zoneID = faceZones.whichZone(facei);

                    bool zoneFlip = false;

                    if (zoneID >= 0)
                    {
                        const faceZone& fZone = faceZones[zoneID];

                        zoneFlip = fZone.flipMap()[fZone.whichFace(facei)];
                    }

                    // Get current connectivity
                    label own = faceOwner[facei];
                    label nei = -1;
                    label patchID = -1;

                    if (mesh_.isInternalFace(facei))
                    {
                        nei = faceNeighbour[facei];
                    }
                    else
                    {
                        patchID = boundaryMesh.whichPatch(facei);
                    }

                    meshMod.modifyFace
                    (
                        newFaces[facei],            // face
                        facei,                      // facei to change
                        own,                        // owner
                        nei,                        // neighbour
                        false,                      // flipFaceFlux
                        patchID,                    // patch
                        zoneID,
                        zoneFlip
                    );
                    meshChanged = true;
                }
            }
        }
    }

    return meshChanged;
}


void CML::edgeCollapser::updateMesh(const mapPolyMesh& map)
{
    pointRegion_.setSize(mesh_.nPoints());
    pointRegion_ = -1;
    // Reset count, do not remove underlying storage
    pointRegionMaster_.clear();
    freeRegions_.clear();
}


// ************************************************************************* //
