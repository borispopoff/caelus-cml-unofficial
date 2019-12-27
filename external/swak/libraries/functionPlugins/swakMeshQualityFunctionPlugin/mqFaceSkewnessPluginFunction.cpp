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

#include "mqFaceSkewnessPluginFunction.hpp"
#include "FieldValueExpressionDriver.hpp"

#include "addToRunTimeSelectionTable.hpp"

namespace CML {

defineTypeNameAndDebug(mqFaceSkewnessPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, mqFaceSkewnessPluginFunction , name, mqFaceSkewness);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

mqFaceSkewnessPluginFunction::mqFaceSkewnessPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("surfaceScalarField"),
        string("")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void mqFaceSkewnessPluginFunction::doEvaluation()
{
    autoPtr<surfaceScalarField> pSkewness(
        new surfaceScalarField(
            IOobject(
                "faceSkewness",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("nonOr",dimless,0),
            "fixedValue"
        )
    );

    surfaceScalarField &skewness=pSkewness();

    const pointField& p = mesh().points();
    const faceList& fcs = mesh().faces();

    const labelList& own = mesh().faceOwner();
    const labelList& nei = mesh().faceNeighbour();
    const vectorField& cellCtrs = mesh().cellCentres();
    const vectorField& faceCtrs = mesh().faceCentres();
    const vectorField& fAreas = mesh().faceAreas();

    forAll(nei, facei)
    {
        vector Cpf = faceCtrs[facei] - cellCtrs[own[facei]];
        vector d = cellCtrs[nei[facei]] - cellCtrs[own[facei]];

        // Skewness vector
        vector sv =
            Cpf - ((fAreas[facei] & Cpf)/((fAreas[facei] & d) + SMALL))*d;
        vector svHat = sv/(mag(sv) + VSMALL);

        // Normalisation distance calculated as the approximate distance
        // from the face centre to the edge of the face in the direction of
        // the skewness
        scalar fd = 0.2*mag(d) + VSMALL;
        const face& f = fcs[facei];
        forAll(f, pi)
        {
            fd = max(fd, mag(svHat & (p[f[pi]] - faceCtrs[facei])));
        }

        // Normalised skewness
        skewness[facei] = mag(sv)/fd;
    }


    // Boundary faces: consider them to have only skewness error.
    // (i.e. treat as if mirror cell on other side)

    forAll(skewness.boundaryField(),patchI)
    {
        fvsPatchField<scalar> &thePatch=skewness.boundaryFieldRef()[patchI];

        forAll(thePatch,i)
        {
            scalar facei=i+thePatch.patch().patch().start();

            vector Cpf = faceCtrs[facei] - cellCtrs[own[facei]];

            vector normal = fAreas[facei];
            normal /= mag(normal) + VSMALL;
            vector d = normal*(normal & Cpf);


            // Skewness vector
            vector sv = Cpf - ((fAreas[facei]&Cpf)/((fAreas[facei]&d)+VSMALL))*d;
            vector svHat = sv/(mag(sv) + VSMALL);

            // Normalisation distance calculated as the approximate distance
            // from the face centre to the edge of the face in the direction of
            // the skewness
            scalar fd = 0.4*mag(d) + VSMALL;
            const face& f = fcs[facei];
            forAll(f, pi)
            {
                fd = max(fd, mag(svHat & (p[f[pi]] - faceCtrs[facei])));
            }

            // Normalised skewness
            thePatch[i] = mag(sv)/fd;
        }
    }

    result().setObjectResult(pSkewness);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
