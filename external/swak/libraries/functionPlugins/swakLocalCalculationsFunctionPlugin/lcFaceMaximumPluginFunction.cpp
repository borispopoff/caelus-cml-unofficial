/*---------------------------------------------------------------------------*\
Copyright: ICE Stroemungsfoschungs GmbH
Copyright (C) 1991-2008 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is based on CAELUS.

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

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

\*---------------------------------------------------------------------------*/

#include "lcFaceMaximumPluginFunction.hpp"
#include "FieldValueExpressionDriver.hpp"

#include "addToRunTimeSelectionTable.hpp"

namespace CML {

defineTypeNameAndDebug(lcFaceMaximumPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, lcFaceMaximumPluginFunction , name, lcFaceMaximum);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcFaceMaximumPluginFunction::lcFaceMaximumPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    localCellCalculationFunction(
        parentDriver,
        name,
        string("faceField internalField surfaceScalarField")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void lcFaceMaximumPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0);

    original_.set(
        new surfaceScalarField(
            //            dynamicCast<const FieldValueExpressionDriver &>(
            dynamic_cast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<surfaceScalarField>()
        )
    );
}

void lcFaceMaximumPluginFunction::doCellCalculation(volScalarField &field)
{
    const cellList &cl=field.mesh().cells();
    const surfaceScalarField &o=original_();

    forAll(field,celli) {
        scalar maxVal=-1e30;

        const cell &c=cl[celli];
        forAll(c,i) {
            const label facei=c[i];
            if(facei<field.mesh().nInternalFaces()) {
                maxVal=max(maxVal,o[facei]);
            } else {
                label patchID=field.mesh().boundaryMesh().whichPatch(facei);
                label startI=field.mesh().boundaryMesh()[patchID].start();
                maxVal=max(
                    maxVal,
                    o.boundaryField()[patchID][facei-startI]
                );
            }
        }

        field[celli]=maxVal;
    }
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
