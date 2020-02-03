/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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

Class
    CML::SRF::rpm

Description
    Basic SRF model whereby angular velocity is specified in terms of
    a (global) axis and revolutions-per-minute [rpm]

SourceFiles
    rpm.cpp

\*---------------------------------------------------------------------------*/

#ifndef SRFModelRpm_H
#define SRFModelRpm_H

#include "SRFModel.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace SRF
{

/*---------------------------------------------------------------------------*\
                            Class rpm Declaration
\*---------------------------------------------------------------------------*/

class rpm
:
    public SRFModel
{

    // Private data

        //- Revolutions per minute
        scalar rpm_;


public:

    //- Runtime type information
    TypeName("rpm");


    // Constructors

        //- Construct from components
        rpm(const volVectorField& U);

        //- Disallow default bitwise copy construct
        rpm(const rpm&) = delete;


    //- Destructor
    ~rpm();

    // Member functions

        // I-O

            //- Read
            bool read();


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const rpm&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace SRF
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
