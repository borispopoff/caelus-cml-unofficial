/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2018 OpenFOAM Foundation
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

#include "bound.hpp"
#include "volFields.hpp"
#include "fvc.hpp"

// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

CML::volScalarField&
CML::bound(volScalarField& vsf, const dimensionedScalar& lowerBound)
{
    const scalar minVsf = min(vsf).value();

    if (minVsf < lowerBound.value())
    {
        Info<< "bounding " << vsf.name()
            << ", min: " << minVsf
            << " max: " << max(vsf).value()
            << " average: " << gAverage(vsf.primitiveField())
            << endl;

        vsf.primitiveFieldRef() = max
        (
            max
            (
                vsf.primitiveField(),
                fvc::average(max(vsf, lowerBound))().primitiveField()
              * pos(-vsf.primitiveField())
            ),
            lowerBound.value()
        );

        vsf.boundaryFieldRef() = max(vsf.boundaryField(), lowerBound.value());
    }

    return vsf;
}


void CML::boundMinMax
(
    volScalarField& vsf,
    const dimensionedScalar& vsf0,
    const dimensionedScalar& vsf1
)
{
    scalar minVsf = min(vsf).value();
    scalar maxVsf = max(vsf).value();

    if (minVsf < vsf0.value() || maxVsf > vsf1.value())
    {
        Info<< "bounding " << vsf.name()
            << ", min: " << gMin(vsf.primitiveField())
            << " max: " << gMax(vsf.primitiveField())
            << " average: " << gAverage(vsf.primitiveField())
            << endl;
    }

    if (minVsf < vsf0.value())
    {
        vsf.primitiveFieldRef() = max
        (
            max
            (
                vsf.primitiveField(),
                fvc::average(max(vsf, vsf0))().primitiveField()
                *pos(vsf0.value() - vsf.primitiveField())
            ),
            vsf0.value()
        );

        vsf.correctBoundaryConditions();
        vsf.boundaryFieldRef() = max(vsf.boundaryField(), vsf0.value());
    }

    if (maxVsf > vsf1.value())
    {
        vsf.primitiveFieldRef() = min
        (
            min
            (
                vsf.primitiveField(),
                fvc::average(min(vsf, vsf1))().primitiveField()
                *neg(vsf1.value() - vsf.primitiveField())
                // This is needed when all values are above max
              + pos(vsf1.value() - vsf.primitiveField())*vsf1.value()
            ),
            vsf1.value()
        );

        vsf.correctBoundaryConditions();
        vsf.boundaryFieldRef() = min(vsf.boundaryField(), vsf1.value());
    }
}

// ************************************************************************* //
