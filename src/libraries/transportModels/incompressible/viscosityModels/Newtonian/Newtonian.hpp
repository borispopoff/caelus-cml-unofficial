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

Class
    CML::viscosityModels::Newtonian

Description
    An incompressible Newtonian viscosity model.

SourceFiles
    Newtonian.cpp

\*---------------------------------------------------------------------------*/

#ifndef Newtonian_H
#define Newtonian_H

#include "viscosityModel.hpp"
#include "dimensionedScalar.hpp"
#include "volFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace viscosityModels
{

/*---------------------------------------------------------------------------*\
                           Class Newtonian Declaration
\*---------------------------------------------------------------------------*/

class Newtonian
:
    public viscosityModel
{
    // Private data

        dimensionedScalar nu0_;

        volScalarField nu_;


public:

    //- Runtime type information
    TypeName("Newtonian");


    // Constructors

        //- construct from components
        Newtonian
        (
            const word& name,
            const dictionary& viscosityProperties,
            const volVectorField& U,
            const surfaceScalarField& phi
        );


    //- Destructor
    ~Newtonian()
    {}


    // Member Functions

        //- Return the laminar viscosity
        tmp<volScalarField> nu() const
        {
            return nu_;
        }

        //- Correct the laminar viscosity (not appropriate, viscosity constant)
        void correct()
        {}

        //- Read transportProperties dictionary
        bool read(const dictionary& viscosityProperties);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace viscosityModels
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
