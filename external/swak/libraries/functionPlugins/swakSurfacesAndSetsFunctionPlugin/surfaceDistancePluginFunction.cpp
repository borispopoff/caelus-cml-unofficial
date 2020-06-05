/*---------------------------------------------------------------------------*\
Copyright: ICE Stroemungsfoschungs GmbH
Copyright (C) 1991-2008 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

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

#include "surfaceDistancePluginFunction.hpp"
#include "FieldValueExpressionDriver.hpp"

#include "addToRunTimeSelectionTable.hpp"

namespace CML {

defineTypeNameAndDebug(surfaceDistancePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceDistancePluginFunction , name, surfaceDistance);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

surfaceDistancePluginFunction::surfaceDistancePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    GeneralSurfacesPluginFunction(
        parentDriver,
        name,
        "volScalarField",
        string("surfaceName primitive word")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void surfaceDistancePluginFunction::doEvaluation()
{
    autoPtr<volScalarField> pDistance(
        new volScalarField(
            IOobject(
                "surfaceDistanceInCell",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("no",dimless,HUGE_VAL)
        )
    );

    const vectorField &cc=mesh().C();

    List<vectorField> faceCenters(Pstream::nProcs());
    faceCenters[Pstream::myProcNo()]=theSurface().Cf();
    Pstream::scatterList(faceCenters);

    forAll(cc,celli) {
        const vector &here=cc[celli];
        scalar &minDist=(pDistance)()[celli];

        forAll(faceCenters,i){
            const vectorField &fc=faceCenters[i];
            forAll(fc,facei){
                const scalar dist=mag(here-fc[facei]);
                if(dist<minDist) {
                    minDist=dist;
                }
            }
        }
    }

    pDistance->correctBoundaryConditions();

    result().setObjectResult(pDistance);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
