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
#include "IOmanip.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void triSurface::writeTRI(const bool writeSorted, Ostream& os) const
{
    const pointField& ps = points();

    // Write as cloud of triangles

    labelList faceMap;

    surfacePatchList myPatches(calcPatches(faceMap));

    if (writeSorted)
    {
        label faceIndex = 0;

        forAll(myPatches, patchi)
        {
            for
            (
                label patchFaceI = 0;
                patchFaceI < myPatches[patchi].size();
                patchFaceI++
            )
            {
                const label facei = faceMap[faceIndex++];

                const point& p1 = ps[operator[](facei)[0]];
                const point& p2 = ps[operator[](facei)[1]];
                const point& p3 = ps[operator[](facei)[2]];

                os  << p1.x() << token::SPACE
                    << p1.y() << token::SPACE
                    << p1.z() << token::SPACE

                    << p2.x() << token::SPACE
                    << p2.y() << token::SPACE
                    << p2.z() << token::SPACE

                    << p3.x() << token::SPACE
                    << p3.y() << token::SPACE
                    << p3.z() << token::SPACE

                    << "0x" << hex << operator[](facei).region() << dec
                    << endl;
            }
        }
    }
    else
    {
        forAll(*this, facei)
        {
            const point& p1 = ps[operator[](facei)[0]];
            const point& p2 = ps[operator[](facei)[1]];
            const point& p3 = ps[operator[](facei)[2]];

            os  << p1.x() << token::SPACE
                << p1.y() << token::SPACE
                << p1.z() << token::SPACE

                << p2.x() << token::SPACE
                << p2.y() << token::SPACE
                << p2.z() << token::SPACE

                << p3.x() << token::SPACE
                << p3.y() << token::SPACE
                << p3.z() << token::SPACE

                << "0x" << hex << operator[](facei).region() << dec
                << endl;
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
