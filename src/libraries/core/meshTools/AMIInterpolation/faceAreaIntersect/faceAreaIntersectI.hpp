/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2017 OpenFOAM Foundation
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

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

inline void CML::faceAreaIntersect::setTriPoints
(
    const point& a,
    const point& b,
    const point& c,
    label& count,
    FixedList<triPoints, 10>& tris
) const
{
    triPoints& tp = tris[count++];
    tp[0] = a;
    tp[1] = b;
    tp[2] = c;
}


inline CML::faceAreaIntersect::triPoints CML::faceAreaIntersect::getTriPoints
(
    const pointField& points,
    const face& f,
    const bool reverse
) const
{
    triPoints result;

    if (reverse)
    {
        result[2] = points[f[0]];
        result[1] = points[f[1]];
        result[0] = points[f[2]];
    }
    else
    {
        result[0] = points[f[0]];
        result[1] = points[f[1]];
        result[2] = points[f[2]];
    }

    return result;
}


inline CML::point CML::faceAreaIntersect::planeIntersection
(
    const FixedList<scalar, 3>& d,
    const triPoints& t,
    const label negI,
    const label posI
) const
{
    return (d[posI]*t[negI] - d[negI]*t[posI])/(-d[negI] + d[posI]);
}


inline CML::scalar CML::faceAreaIntersect::triArea(const triPoints& t) const
{
    return mag(0.5*((t[1] - t[0])^(t[2] - t[0])));
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

CML::scalar& CML::faceAreaIntersect::tolerance()
{
    return tol;
}


// ************************************************************************* //
