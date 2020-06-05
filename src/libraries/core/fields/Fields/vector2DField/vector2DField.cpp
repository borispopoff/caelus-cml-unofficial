/*---------------------------------------------------------------------------*\
Copyright (C) 2017 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Caelus is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "vector2DField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

CML::tmp<CML::vector2DField> CML::zip
(
    const tmp<scalarField>& x,
    const tmp<scalarField>& y
)
{
    tmp<vector2DField> txy(new vector2DField(x->size()));
    vector2DField& xy = txy.ref();
    xy.replace(0, x);
    xy.replace(1, y);
    return txy;
}

// ************************************************************************* //
