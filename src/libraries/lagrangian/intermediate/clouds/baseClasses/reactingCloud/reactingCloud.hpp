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
    CML::reactingCloud

Description
    Virtual abstract base class for templated ReactingCloud

SourceFiles
    reactingCloud.cpp

\*---------------------------------------------------------------------------*/

#ifndef reactingCloud_H
#define reactingCloud_H

#include "typeInfo.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                       Class reactingCloud Declaration
\*---------------------------------------------------------------------------*/

class reactingCloud
{
    // Private Member Functions

        //- Disallow default bitwise copy construct
        reactingCloud(const reactingCloud&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const reactingCloud&) = delete;


public:

    //- Runtime type information
    TypeName("reactingCloud");

    // Constructors

        //- Null constructor
        reactingCloud();


    //- Destructor
    virtual ~reactingCloud();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
