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

Description
    Given a set of points, find out if the mesh resulting from point motion will
    be valid without actually changing the mesh.

\*---------------------------------------------------------------------------*/

#include "primitiveMesh.hpp"
#include "pyramidPointFaceRef.hpp"
#include "cell.hpp"
#include "unitConversion.hpp"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool CML::primitiveMesh::checkMeshMotion
(
    const pointField& newPoints,
    const bool report
) const
{
    if (debug || report)
    {
        Pout<< "bool primitiveMesh::checkMeshMotion("
            << "const pointField& newPoints, const bool report) const: "
            << "checking mesh motion" << endl;
    }

    bool error = false;

    const faceList& f = faces();

    const labelList& own = faceOwner();
    const labelList& nei = faceNeighbour();

    vectorField fCtrs(nFaces());
    vectorField fAreas(nFaces());

    makeFaceCentresAndAreas(newPoints, fCtrs, fAreas);

    // Check cell volumes and calculate new cell centres
    vectorField cellCtrsGeometric(nCells());
    vectorField cellCtrs(nCells());
    scalarField cellVols(nCells());

    makeCellCentresAndVolsGeometric(fCtrs, fAreas, cellCtrsGeometric, cellVols);
    makeCellCentres(fCtrs, fAreas, cellCtrs);

    scalar minVolume = GREAT;
    label nNegVols = 0;

    forAll(cellVols, celli)
    {
        if (cellVols[celli] < VSMALL)
        {
            if (debug || report)
            {
                Pout<< "Zero or negative cell volume detected for cell "
                    << celli << ".  Volume = " << cellVols[celli] << endl;
            }

            nNegVols++;
        }

        minVolume = min(minVolume, cellVols[celli]);
    }

    if (nNegVols > 0)
    {
        error = true;

        Pout<< "Zero or negative cell volume in mesh motion in " << nNegVols
            << " cells.  Min volume: " << minVolume << endl;
    }
    else
    {
        if (debug || report)
        {
            Pout<< "Min volume = " << minVolume
                << ".  Total volume = " << sum(cellVols)
                << ".  Cell volumes OK." << endl;
        }
    }

    // Check face areas

    scalar minArea = GREAT;
    label nNegAreas = 0;
    label nPyrErrors = 0;
    label nDotProductErrors = 0;

    forAll(f, facei)
    {
        const scalar a = CML::mag(fAreas[facei]);

        if (a < VSMALL)
        {
            if (debug || report)
            {
                if (isInternalFace(facei))
                {
                    Pout<< "Zero or negative face area detected for "
                        << "internal face "<< facei << " between cells "
                        << own[facei] << " and " << nei[facei]
                        << ".  Face area magnitude = " << a << endl;
                }
                else
                {
                    Pout<< "Zero or negative face area detected for "
                        << "boundary face " << facei << " next to cell "
                        << own[facei] << ".  Face area magnitude = "
                        << a << endl;
                }
            }

            nNegAreas++;
        }

        minArea = min(minArea, a);

        // Create the owner pyramid - it will have negative volume
        scalar pyrVol =
            pyramidPointFaceRef(f[facei], cellCtrsGeometric[own[facei]]).mag(newPoints);

        if (pyrVol > SMALL)
        {
            if (debug || report)
            {
                Pout<< "Negative pyramid volume: " << -pyrVol
                    << " for face " << facei << " " << f[facei]
                    << "  and owner cell: " << own[facei] << endl
                    << "Owner cell vertex labels: "
                    << cells()[own[facei]].labels(f)
                    << endl;
            }

            nPyrErrors++;
        }

        if (isInternalFace(facei))
        {
            // Create the neighbour pyramid - it will have positive volume
            scalar pyrVol =
                pyramidPointFaceRef
                (
                    f[facei],
                    cellCtrsGeometric[nei[facei]]
                ).mag(newPoints);

            if (pyrVol < -SMALL)
            {
                if (debug || report)
                {
                    Pout<< "Negative pyramid volume: " << pyrVol
                        << " for face " << facei << " " << f[facei]
                        << "  and neighbour cell: " << nei[facei] << nl
                        << "Neighbour cell vertex labels: "
                        << cells()[nei[facei]].labels(f)
                        << endl;
                }

                nPyrErrors++;
            }

            const vector d = cellCtrs[nei[facei]] - cellCtrs[own[facei]];
            const vector& s = fAreas[facei];
            scalar dDotS = (d & s)/(mag(d)*mag(s) + VSMALL);

            // Only write full message the first time
            if (dDotS < SMALL && nDotProductErrors == 0)
            {
                // Non-orthogonality greater than 90 deg
                WarningInFunction
                    << "Severe non-orthogonality in mesh motion for face "
                    << facei
                    << " between cells " << own[facei] << " and " << nei[facei]
                    << ": Angle = " << radToDeg(::acos(dDotS))
                    << " deg." << endl;

                nDotProductErrors++;
            }
        }
    }

    if (nNegAreas > 0)
    {
        error = true;

        WarningInFunction
            << "Zero or negative face area in mesh motion in " << nNegAreas
            << " faces.  Min area: " << minArea << endl;
    }
    else
    {
        if (debug || report)
        {
            Pout<< "Min area = " << minArea
                << ".  Face areas OK." << endl;
        }
    }

    if (nPyrErrors > 0)
    {
        Pout<< "Detected " << nPyrErrors
            << " negative pyramid volume in mesh motion" << endl;

        error = true;
    }
    else
    {
        if (debug || report)
        {
            Pout<< "Pyramid volumes OK." << endl;
        }
    }

    if (nDotProductErrors > 0)
    {
        Pout<< "Detected " << nDotProductErrors
            << " in non-orthogonality in mesh motion." << endl;

        error = true;
    }
    else
    {
        if (debug || report)
        {
            Pout<< "Non-orthogonality check OK." << endl;
        }
    }

    if (!error && (debug || report))
    {
        Pout<< "Mesh motion check OK." << endl;
    }

    return error;
}


// ************************************************************************* //
