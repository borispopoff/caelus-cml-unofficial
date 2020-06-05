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
    CML::inverseVolumeDiffusivity

Description
    Inverse cell-volume motion diffusivity.

SourceFiles
    inverseVolumeDiffusivity.cpp

\*---------------------------------------------------------------------------*/

#ifndef inverseVolumeDiffusivity_H
#define inverseVolumeDiffusivity_H

#include "uniformDiffusivity.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class inverseVolumeDiffusivity Declaration
\*---------------------------------------------------------------------------*/

class inverseVolumeDiffusivity
:
    public uniformDiffusivity
{
    // Private Member Functions

        //- Disallow default bitwise copy construct
        inverseVolumeDiffusivity(const inverseVolumeDiffusivity&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const inverseVolumeDiffusivity&) = delete;


public:

    //- Runtime type information
    TypeName("inverseVolume");


    // Constructors

        //- Construct for the given fvMesh and data Istream
        inverseVolumeDiffusivity(const fvMesh& mesh, Istream& mdData);


    //- Destructor
    virtual ~inverseVolumeDiffusivity();


    // Member Functions

        //- Correct the motion diffusivity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
