/*---------------------------------------------------------------------------*\
Copyright (C) 2015-2019 OpenFOAM Foundation
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
    CML::tabulated6DoFAcceleration

Description
    Tabulated 6DoF acceleration.

    Obtained by interpolating tabulated data for linear acceleration,
    angular velocity and angular acceleration.

SourceFiles
    tabulated6DoFAcceleration.cpp

\*---------------------------------------------------------------------------*/

#ifndef tabulated6DoFAcceleration_HPP
#define tabulated6DoFAcceleration_HPP

#include "primitiveFields.hpp"
#include "Vector2D_.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                 Class tabulated6DoFAcceleration Declaration
\*---------------------------------------------------------------------------*/

class tabulated6DoFAcceleration
{
    // Private data

        const Time& time_;

        dictionary accelerationCoeffs_;

        //- Time data file name read from dictionary
        fileName timeDataFileName_;

        //- Type used to read in the acceleration "vectors"
        typedef Vector<vector> accelerationVectors;

        //- Field of times
        scalarField times_;

        //- Field of acceleration "vectors"
        Field<accelerationVectors> values_;


    // Private Member Functions

        //- Disallow copy construct
        tabulated6DoFAcceleration(const tabulated6DoFAcceleration&);

        //- Disallow default bitwise assignment
        void operator=(const tabulated6DoFAcceleration&) = delete;


public:

    //- Runtime type information
    TypeName("tabulated6DoFAcceleration");


    // Constructors

        //- Construct from components
        tabulated6DoFAcceleration
        (
            const dictionary& accelerationCoeffs,
            const Time& runTime
        );


    //- Destructor
    virtual ~tabulated6DoFAcceleration();


    // Member Functions

        //- Return the solid-body accelerations
        virtual Vector<vector> acceleration() const;

        //- Update properties from given dictionary
        virtual bool read(const dictionary& accelerationCoeffs);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
