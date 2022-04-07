/*---------------------------------------------------------------------------*\
Copyright (C) 2017-2019 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Caelus is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

Class
    CML::fv::damping

Description
    Base fvOption for damping functions.

See also
    CML::fv::isotropicDamping

SourceFiles
    damping.cpp

\*---------------------------------------------------------------------------*/

#ifndef damping_HPP
#define damping_HPP

#include "fvOption.hpp"
#include "DataEntry.hpp"
#include "volFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                   Class damping Declaration
\*---------------------------------------------------------------------------*/

class damping
:
    public option
{
protected:

    // Protected Data

        //- Damping coefficient [1/s]
        dimensionedScalar lambda_;

        //- The scaling function
        autoPtr<DataEntry<scalar> > scale_;

        //- Origins of the scaling coordinate
        vectorField origins_;

        //- Directions of increasing scaling coordinate
        vectorField directions_;


    // Protected Member Functions

        tmp<volScalarField> forceCoeff() const;


public:

    //- Runtime type information
    TypeName("damping");


    // Constructors

        //- Construct from components
        damping
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~damping()
    {}


    // Member Functions

        // IO

            //- Read dictionary
            virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
