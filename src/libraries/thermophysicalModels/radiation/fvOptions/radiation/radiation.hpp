/*---------------------------------------------------------------------------*\
Copyright (C) 2017 OpenFOAM Foundation
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
    CML::fv::radiation

Description
    Calculates and applies the radiation source to the energy equation.

Usage
    Example usage:
    \verbatim
    radiationCoeffs
    {
        fields          (h);                    // Name of energy field
    }
    \endverbatim

SourceFiles
    radiation.cpp

\*---------------------------------------------------------------------------*/

#ifndef radiation_HPP
#define radiation_HPP

#include "fvOption.hpp"
#include "uniformDimensionedFields.hpp"
#include "radiationModel.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class radiation Declaration
\*---------------------------------------------------------------------------*/

class radiation
:
    public option
{
    // Private data

        //- The radiation model pointer
        autoPtr<CML::radiation::radiationModel> radiation_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        radiation(const radiation&);

        //- Disallow default bitwise assignment
        void operator=(const radiation&);


public:

    //- Runtime type information
    TypeName("radiation");


    // Constructors

        //- Construct from explicit source name and mesh
        radiation
        (
            const word& sourceName,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Member Functions

        // Evaluate

            //- Add explicit contribution to compressible momentum equation
            virtual void addSup
            (
                const volScalarField& rho,
                fvMatrix<scalar>& eqn,
                const label fieldi
            );


        // IO

            //- Read source dictionary
            virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
