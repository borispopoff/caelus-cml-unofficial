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

#include "randomise.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    namespace calcTypes
    {
        defineTypeNameAndDebug(randomise, 0);
        addToRunTimeSelectionTable(calcType, randomise, dictionary);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::calcTypes::randomise::randomise()
:
    calcType()
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::calcTypes::randomise::~randomise()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::calcTypes::randomise::init()
{
    argList::validArgs.append("randomise");
    argList::validArgs.append("perturbation");
    argList::validArgs.append("field");
}


void CML::calcTypes::randomise::preCalc
(
    const argList& args,
    const Time& runTime,
    const fvMesh& mesh
)
{}


void CML::calcTypes::randomise::calc
(
    const argList& args,
    const Time& runTime,
    const fvMesh& mesh
)
{
    const scalar pertMag = args.argRead<scalar>(2);
    const word fieldName = args[3];

    Random rand(1234567);

    IOobject fieldHeader
    (
        fieldName,
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    // Check field exists
    if (fieldHeader.headerOk())
    {
        bool processed = false;

        writeRandomField<vector>
        (
            fieldHeader,
            pertMag,
            rand,
            mesh,
            processed
        );
        writeRandomField<sphericalTensor>
        (
            fieldHeader,
            pertMag,
            rand,
            mesh,
            processed
        );
        writeRandomField<symmTensor>
        (
            fieldHeader,
            pertMag,
            rand,
            mesh,
            processed
        );
        writeRandomField<tensor>
        (
            fieldHeader,
            pertMag,
            rand,
            mesh,
            processed
        );

        if (!processed)
        {
            FatalError
                << "Unable to process " << fieldName << nl
                << "No call to randomise for fields of type "
                << fieldHeader.headerClassName() << nl << nl
                << exit(FatalError);
        }
    }
    else
    {
        Info<< "    No " << fieldName << endl;
    }
}


// ************************************************************************* //
