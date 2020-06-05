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

#include "mqCellAspectRatioPluginFunction.hpp"
#include "FieldValueExpressionDriver.hpp"

#include "addToRunTimeSelectionTable.hpp"

namespace CML {

defineTypeNameAndDebug(mqCellAspectRatioPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, mqCellAspectRatioPluginFunction , name, mqCellAspectRatio);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

mqCellAspectRatioPluginFunction::mqCellAspectRatioPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        string("")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void mqCellAspectRatioPluginFunction::doEvaluation()
{
    autoPtr<volScalarField> pAspectRatio(
        new volScalarField(
            IOobject(
                "cellAspectRatio",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("aRatio",dimless,0),
            "zeroGradient"
        )
    );

    volScalarField &aspectRatio=pAspectRatio();

    Vector<label> meshD=mesh().geometricD();

    vectorField sumMagClosed(mesh().nCells(), vector::zero);
    const labelList& own = mesh().faceOwner();
    const labelList& nei = mesh().faceNeighbour();
    const vectorField& areas = mesh().faceAreas();

    forAll (own, facei)
    {
        sumMagClosed[own[facei]] += cmptMag(areas[facei]);
    }

    forAll (nei, facei)
    {
        sumMagClosed[nei[facei]] += cmptMag(areas[facei]);
    }

    const scalarField& vols = mesh().cellVolumes();

    label nDims = 0;
    for (direction dir = 0; dir < vector::nComponents; dir++)
    {
        if (meshD[dir] == 1)
        {
            nDims++;
        }
    }

    // Check the sums
    forAll(aspectRatio, celli)
    {
        // Calculate the aspect ration as the maximum of Cartesian component
        // aspect ratio to the total area hydraulic area aspect ratio
        scalar minCmpt = VGREAT;
        scalar maxCmpt = -VGREAT;
        for (direction dir = 0; dir < vector::nComponents; dir++)
        {
            if (meshD[dir] == 1)
            {
                minCmpt = min(minCmpt, sumMagClosed[celli][dir]);
                maxCmpt = max(maxCmpt, sumMagClosed[celli][dir]);
            }
        }

        aspectRatio[celli] = maxCmpt/(minCmpt + VSMALL);
        if (nDims == 3)
        {
            aspectRatio[celli] = max
            (
                aspectRatio[celli],
                1.0/6.0*cmptSum(sumMagClosed[celli])/pow(vols[celli], 2.0/3.0)
            );
        }
    }

    result().setObjectResult(pAspectRatio);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
