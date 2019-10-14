/*---------------------------------------------------------------------------*\
Copyright (C) 2012-2017 OpenFOAM Foundation
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
    CML::fv::limitTemperature

Description
    Correction for temperature to apply limits between minimum and maximum
    values.

Usage
    Example usage:
    \verbatim
    limitT
    {
        type            limitTemperature;
        active          yes;

        selectionMode   all;
        min             200;
        max             500;
    }
    \endverbatim

SourceFiles
    limitTemperature.cpp

\*---------------------------------------------------------------------------*/

#ifndef limitTemperature_HPP
#define limitTemperature_HPP

#include "cellSetOption.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                 Class limitTemperature Declaration
\*---------------------------------------------------------------------------*/

class limitTemperature
:
    public cellSetOption
{

protected:

    // Protected data

        //- Minimum temperature limit [K]
        scalar Tmin_;

        //- Maximum temperature limit [K]
        scalar Tmax_;


private:

    // Private Member Functions

        //- Disallow default bitwise copy construct
        limitTemperature(const limitTemperature&);

        //- Disallow default bitwise assignment
        void operator=(const limitTemperature&);


public:

    //- Runtime type information
    TypeName("limitTemperature");


    // Constructors

        //- Construct from components
        limitTemperature
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~limitTemperature()
    {}


    // Member Functions

        //- Read dictionary
        virtual bool read(const dictionary& dict);

        //- Correct the energy field
        virtual void correct(volScalarField& he);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
