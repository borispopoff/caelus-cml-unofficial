/*---------------------------------------------------------------------------*\
Copyright (C) 2014 Applied CCM
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
    CML::cellQuality

Description
    Class calculates cell quality measures.

SourceFiles
    cellQuality.cpp

\*---------------------------------------------------------------------------*/

#ifndef cellQuality_H
#define cellQuality_H

#include "polyMesh.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{


/*---------------------------------------------------------------------------*\
                           Class cellQuality Declaration
\*---------------------------------------------------------------------------*/

class cellQuality
{
    // Private data

        //- Reference to mesh
        const polyMesh& mesh_;


public:

    // Constructors

        //- Construct from mesh
        cellQuality(const polyMesh& mesh);

        //- Disallow default bitwise copy construct
        cellQuality(const cellQuality&) = delete;


    // Destructor - default


    // Member Functions

        //- Return cell non-orthogonality
        tmp<scalarField> nonOrthogonality() const;

        //- Return cell skewness
        tmp<scalarField> skewness() const;

        //- Return face non-orthogonality
        tmp<scalarField> faceNonOrthogonality() const;

        //- Return face skewness
        tmp<scalarField> faceSkewness() const;


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const cellQuality&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
