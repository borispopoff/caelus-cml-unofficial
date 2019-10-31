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
    CML::velocityLaplacianFvMotionSolver

Description
    Mesh motion solver for an fvMesh.  Based on solving the cell-centre
    Laplacian for the motion velocity.

SourceFiles
    velocityLaplacianFvMotionSolver.cpp

\*---------------------------------------------------------------------------*/

#ifndef velocityLaplacianFvMotionSolver_HPP
#define velocityLaplacianFvMotionSolver_HPP

#include "velocityMotionSolver.hpp"
#include "fvMotionSolverCore.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward class declarations
class motionDiffusivity;

/*---------------------------------------------------------------------------*\
              Class velocityLaplacianFvMotionSolver Declaration
\*---------------------------------------------------------------------------*/

class velocityLaplacianFvMotionSolver
:
    public velocityMotionSolver,
    public fvMotionSolverCore
{
    // Private data

        //- Cell-centre motion field
        mutable volVectorField cellMotionU_;

        //- Diffusivity used to control the motion
        autoPtr<motionDiffusivity> diffusivityPtr_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        velocityLaplacianFvMotionSolver
        (
            const velocityLaplacianFvMotionSolver&
        );

        //- Disallow default bitwise assignment
        void operator=(const velocityLaplacianFvMotionSolver&) = delete;


public:

    //- Runtime type information
    TypeName("velocityLaplacian");


    // Constructors

        //- Construct from polyMesh and IOdictionary
        velocityLaplacianFvMotionSolver
        (
            const polyMesh&,
            const IOdictionary&
        );


    //- Destructor
    ~velocityLaplacianFvMotionSolver();


    // Member Functions

        //- Return reference to the cell motion velocity field
        volVectorField& cellMotionU()
        {
            return cellMotionU_;
        }

        //- Return const reference to the cell motion velocity field
        const volVectorField& cellMotionU() const
        {
            return cellMotionU_;
        }

        //- Return point location obtained from the current motion field
        virtual tmp<pointField> curPoints() const;

        //- Solve for motion
        virtual void solve();

        //- Update topology
        virtual void updateMesh(const mapPolyMesh&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
