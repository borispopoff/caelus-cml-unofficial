/*---------------------------------------------------------------------------* \
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

Application
    Q

Description
    Calculates and writes the second invariant of the velocity gradient tensor.

    The -noWrite option just outputs the max/min values without writing
    the field.

\*---------------------------------------------------------------------------*/

#include "calc.hpp"
#include "fvc.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void CML::calc(const argList& args, const Time& runTime, const fvMesh& mesh)
{
    bool writeResults = !args.optionFound("noWrite");

    IOobject Uheader
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    if (Uheader.headerOk())
    {
        Info<< "    Reading U" << endl;
        volVectorField U(Uheader, mesh);
        volTensorField gradU(fvc::grad(U));

        volScalarField Q
        (
            IOobject
            (
                "Q",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            0.5*(sqr(tr(gradU)) - tr(((gradU)&(gradU))))
        );

        /*
        // This is a second way of calculating Q, that delivers results
        // very close, but not identical to the first approach.

        volSymmTensorField S(symm(gradU));  // symmetric part of tensor
        volTensorField W(skew(gradU));      // anti-symmetric part

        volScalarField SS(S && S);
        volScalarField WW(W && W);

        volScalarField Q
        (
            IOobject
            (
                "Q",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            0.5*(WW - SS)
        );
        */

        Info<< "mag(Q) max/min : "
            << max(Q).value() << " "
            << min(Q).value() << endl;

        if (writeResults)
        {
            Q.write();
        }
    }
    else
    {
        Info<< "    No U" << endl;
    }

    Info<< "\nEnd\n" << endl;
}


// ************************************************************************* //
