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

Typedef
    CML::vectorField

Description
    Specialisation of Field\<T\> for vector.

SourceFiles
    vectorField.cpp

\*---------------------------------------------------------------------------*/

#ifndef vectorField_H
#define vectorField_H

#include "scalarField.hpp"
#include "vector.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{


typedef Field<vector> vectorField;

tmp<vectorField> zip
(
    const tmp<scalarField>& x,
    const tmp<scalarField>& y,
    const tmp<scalarField>& z
);


} // End namespace CML


#endif
