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

#include "vectorField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

CML::tmp<CML::vectorField> CML::zip
(
    const tmp<scalarField>& x,
    const tmp<scalarField>& y,
    const tmp<scalarField>& z
)
{
    tmp<vectorField> txyz(new vectorField(x->size()));
    vectorField& xyz = txyz.ref();
    xyz.replace(0, x);
    xyz.replace(1, y);
    xyz.replace(2, z);
    return txyz;
}

// ************************************************************************* //
