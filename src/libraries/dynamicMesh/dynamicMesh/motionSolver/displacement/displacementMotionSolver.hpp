/*---------------------------------------------------------------------------*\
Copyright (C) 2012-2019 OpenFOAM Foundation
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
    CML::displacementMotionSolver

Description
    Virtual base class for displacement motion solver

    The boundary displacement is set as a boundary condition
    on the pointDisplacement pointVectorField.

SourceFiles
    displacementMotionSolver.cpp

\*---------------------------------------------------------------------------*/

#ifndef displacementMotionSolver_HPP
#define displacementMotionSolver_HPP

#include "motionSolver.hpp"
#include "pointFields.hpp"
#include "pointIOField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

class mapPolyMesh;

/*---------------------------------------------------------------------------*\
                   Class displacementMotionSolver Declaration
\*---------------------------------------------------------------------------*/

class displacementMotionSolver
:
    public motionSolver
{
protected:

    // Protected data

        //- Point motion field
        mutable pointVectorField pointDisplacement_;

        //- Starting points
        pointIOField points0_;


    // Protected Member Functions

        //- Return IO object for points0
        IOobject points0IO(const polyMesh& mesh) const;

private:


    // Private Member Functions

        //- Disallow default bitwise copy construct
        displacementMotionSolver(const displacementMotionSolver&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const displacementMotionSolver&) = delete;

public:

    //- Runtime type information
    TypeName("displacementMotionSolver");


    // Constructors

        //- Construct from mesh and dictionary
        displacementMotionSolver
        (
            const polyMesh&,
            const IOdictionary&,
            const word& type
        );


    //- Destructor
    virtual ~displacementMotionSolver();


    // Member Functions

        //- Return reference to the reference field
        pointField& points0()
        {
            return points0_;
        }

        //- Return const reference to the reference field
        const pointField& points0() const
        {
            return points0_;
        }

        //- Return reference to the point motion displacement field
        pointVectorField& pointDisplacement()
        {
            return pointDisplacement_;
        }

        //- Return const reference to the point motion displacement field
        const pointVectorField& pointDisplacement() const
        {
            return pointDisplacement_;
        }

        //- Update local data for geometry changes
        virtual void movePoints(const pointField&);

        //-  Update local data for topology changes
        virtual void updateMesh(const mapPolyMesh&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
