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

#include "triSurface.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void triSurface::writeOFF(const bool writeSorted, Ostream& os) const
{
    // Write header
    os  << "OFF" << endl
        << "# Geomview OFF file" << endl
        << "# Regions:" << endl;

    labelList faceMap;
    surfacePatchList myPatches(calcPatches(faceMap));

    // Print patch names as comment
    forAll(myPatches, patchI)
    {
        os  << "#     " << patchI << "    "
            << myPatches[patchI].name() << endl;
    }
    os  << nl << endl;

    const pointField& ps = points();

    os  << "# nPoints  nTriangles  nEdges" << endl
        << ps.size()
        << ' ' << size()
        << ' ' << nEdges()
        << nl << endl;

    // Write vertex coords
    forAll(ps, pointi)
    {
        os  << ps[pointi].x() << ' '
            << ps[pointi].y() << ' '
            << ps[pointi].z() << " #" << pointi << endl;
    }

    os  << endl;

    if (writeSorted)
    {
        label faceIndex = 0;

        forAll(myPatches, patchI)
        {
            // Print all faces belonging to this patch

            for
            (
                label patchFaceI = 0;
                patchFaceI < myPatches[patchI].size();
                patchFaceI++
            )
            {
                const label facei = faceMap[faceIndex++];

                os  << "3 "
                    << operator[](facei)[0] << ' '
                    << operator[](facei)[1] << ' '
                    << operator[](facei)[2] << ' '
                    << operator[](facei).region()
                    << endl;
            }
        }
    }
    else
    {
        forAll(*this, facei)
        {
            os  << "3 "
                << operator[](facei)[0] << ' '
                << operator[](facei)[1] << ' '
                << operator[](facei)[2] << ' '
                << operator[](facei).region()
                << endl;
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
