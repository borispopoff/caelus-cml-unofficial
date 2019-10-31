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
    CML::uniformDiffusivity

Description
    Uniform uniform finite volume mesh motion diffusivity.

SourceFiles
    uniformDiffusivity.cpp

\*---------------------------------------------------------------------------*/

#ifndef uniformDiffusivity_H
#define uniformDiffusivity_H

#include "motionDiffusivity.hpp"
#include "surfaceFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class uniformDiffusivity Declaration
\*---------------------------------------------------------------------------*/

class uniformDiffusivity
:
    public motionDiffusivity
{

protected:

    // Protected data

        surfaceScalarField faceDiffusivity_;


private:

    // Private Member Functions

        //- Disallow default bitwise copy construct
        uniformDiffusivity(const uniformDiffusivity&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const uniformDiffusivity&) = delete;


public:

    //- Runtime type information
    TypeName("uniform");


    // Constructors

        //- Construct for the given fvMesh and data Istream
        uniformDiffusivity(const fvMesh& mesh, Istream& mdData);


    //- Destructor
    virtual ~uniformDiffusivity();


    // Member Functions

        //- Return diffusivity field
        virtual tmp<surfaceScalarField> operator()() const
        {
            return faceDiffusivity_;
        }

        //- Do not correct the motion diffusivity
        virtual void correct()
        {}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
