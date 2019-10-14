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
    CML::fv::fixedTemperatureConstraint

Description
    Fixed temperature equation constraint

Usage
    \verbatim
    fixedTemperature
    {
        type            fixedTemperatureConstraint;
        active          yes;

        mode            uniform;      // uniform or lookup

        // For uniform option
        temperature     constant 500; // fixed temperature with time [K]

        // For lookup option
        // T            <Tname>;      // optional temperature field name
    }
    \endverbatim

Note:
    The 'uniform' option allows the use of a time-varying uniform temperature
    by means of the DataEntry type.

SourceFiles
    fvOption.cpp

\*---------------------------------------------------------------------------*/

#ifndef fixedTemperatureConstraint_HPP
#define fixedTemperatureConstraint_HPP

#include "cellSetOption.hpp"
#include "NamedEnum.hpp"
#include "DataEntry.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                 Class fixedTemperatureConstraint Declaration
\*---------------------------------------------------------------------------*/

class fixedTemperatureConstraint
:
    public cellSetOption
{

public:

    //- Temperature mode
    enum temperatureMode
    {
        tmUniform,
        tmLookup
    };


    //- String representation of temperatureMode enums
    static const NamedEnum<temperatureMode, 2> temperatureModeNames_;


protected:

    // Protected data

        //- Operation mode
        temperatureMode mode_;

        //- Uniform temperature [K]
        autoPtr<DataEntry<scalar>> Tuniform_;

        //- Temperature field name
        word TName_;


private:

    // Private Member Functions

        //- Disallow default bitwise copy construct
        fixedTemperatureConstraint(const fixedTemperatureConstraint&);

        //- Disallow default bitwise assignment
        void operator=(const fixedTemperatureConstraint&);


public:

    //- Runtime type information
    TypeName("fixedTemperatureConstraint");


    // Constructors

        //- Construct from components
        fixedTemperatureConstraint
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~fixedTemperatureConstraint()
    {}


    // Member Functions

        //- Constrain energy equation to fix the temperature
        virtual void constrain(fvMatrix<scalar>& eqn, const label fieldi);


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
