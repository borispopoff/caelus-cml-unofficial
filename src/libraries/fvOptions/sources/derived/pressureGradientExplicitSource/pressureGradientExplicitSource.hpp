/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
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
    CML::fv::pressureGradientExplicitSource

Description
    Creates a pressure gradient source

    Note: Currently only handles kinematic pressure

    \heading Source usage
    Example usage:
    \verbatim
    pressureGradientExplicitSourceCoeffs
    {
        fieldNames  (U);                        // name of velocity field
        Ubar        (10.0 0 0);                 // desired average velocity
        gradPini    gradPini [0 2 -2 0 0] 0;    // initial pressure gradient
        flowDir     (1 0 0);                    // flow direction
    }
    \endverbatim


SourceFiles
    pressureGradientExplicitSource.cpp

\*---------------------------------------------------------------------------*/

#ifndef pressureGradientExplicitSource_H
#define pressureGradientExplicitSource_H

#include "autoPtr.hpp"
#include "topoSetSource.hpp"
#include "cellSet.hpp"
#include "fvMesh.hpp"
#include "volFields.hpp"
#include "cellSetOption.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class pressureGradientExplicitSource Declaration
\*---------------------------------------------------------------------------*/

class pressureGradientExplicitSource
:
    public cellSetOption
{
    // Private data

        //- Average velocity
        vector Ubar_;

        //- Pressure gradient before correction
        scalar gradP0_;

        //- Change in pressure gradient
        scalar dGradP_;

        //- Flow direction
        vector flowDir_;

        //- Matrix 1/A coefficients field pointer
        autoPtr<volScalarField> invAPtr_;


    // Private Member Functions

        //- Write the pressure gradient to file (for restarts etc)
        void writeProps(const scalar gradP) const;

        //- Correct driving force for a constant mass flow rate
        void update(fvMatrix<vector>& eqn);

        //- Disallow default bitwise copy construct
        pressureGradientExplicitSource(const pressureGradientExplicitSource&);

        //- Disallow default bitwise assignment
        void operator=(const pressureGradientExplicitSource&);


public:

    //- Runtime type information
    TypeName("pressureGradientExplicitSource");


    // Constructors

        //- Construct from explicit source name and mesh
        pressureGradientExplicitSource
        (
            const word& sourceName,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Member Functions

        // Evaluate

            //- Correct the pressure gradient
            virtual void correct(volVectorField& U);

            //- Add explicit contribution to momentum equation
            virtual void addSup
            (
                fvMatrix<vector>& eqn,
                const label fieldi
            );

            //- Add explicit contribution to compressible momentum equation
            virtual void addSup
            (
                const volScalarField& rho,
                fvMatrix<vector>& eqn,
                const label fieldi
            );

            //- Set 1/A coefficient
            virtual void setValue
            (
                fvMatrix<vector>& eqn,
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
