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

#include "polyMesh.hpp"
#include "pointFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTemplate2TypeNameAndDebug(pointScalarField::Internal, 0);
defineTemplate2TypeNameAndDebug(pointVectorField::Internal, 0);
defineTemplate2TypeNameAndDebug
(
    pointSphericalTensorField::Internal,
    0
);
defineTemplate2TypeNameAndDebug
(
    pointSymmTensorField::Internal,
    0
);
defineTemplate2TypeNameAndDebug(pointTensorField::Internal, 0);


defineTemplateTypeNameAndDebug(pointScalarField, 0);
defineTemplateTypeNameAndDebug(pointVectorField, 0);
defineTemplateTypeNameAndDebug(pointSphericalTensorField, 0);
defineTemplateTypeNameAndDebug(pointSymmTensorField, 0);
defineTemplateTypeNameAndDebug(pointTensorField, 0);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
