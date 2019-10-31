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
    CML::inverseDistanceDiffusivity

Description
    Inverse distance to the given patches motion diffusivity.

SourceFiles
    inverseDistanceDiffusivity.cpp

\*---------------------------------------------------------------------------*/

#ifndef inverseDistanceDiffusivity_H
#define inverseDistanceDiffusivity_H

#include "uniformDiffusivity.hpp"
#include "wordReList.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                 Class inverseDistanceDiffusivity Declaration
\*---------------------------------------------------------------------------*/

class inverseDistanceDiffusivity
:
    public uniformDiffusivity
{
    // Private data

        //- Patches selected to base the distance on
        //  These can contain patch names or regular expressions to search for.
        wordReList patchNames_;


    // Private Member Functions

        //- Return patch-cell-centre distance field
        tmp<scalarField> y() const;

        //- Disallow default bitwise copy construct
        inverseDistanceDiffusivity(const inverseDistanceDiffusivity&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const inverseDistanceDiffusivity&) = delete;


public:

    //- Runtime type information
    TypeName("inverseDistance");


    // Constructors

        //- Construct for the given fvMesh and data Istream
        inverseDistanceDiffusivity(const fvMesh& mesh, Istream& mdData);


    //- Destructor
    virtual ~inverseDistanceDiffusivity();


    // Member Functions

        //- Correct the motion diffusivity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
