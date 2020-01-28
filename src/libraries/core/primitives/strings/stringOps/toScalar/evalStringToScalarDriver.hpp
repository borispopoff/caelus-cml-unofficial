/*---------------------------------------------------------------------------*\
Copyright (C) 2019 OpenCFD Ltd.
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
    CML::parsing::evalStringToScalar::parseDriver

Description
    Driver for stringOps::toScalar parsing

SourceFiles
    evalStringToScalarDriver.cpp

\*---------------------------------------------------------------------------*/

#ifndef evalStringToScalarDriver_HPP
#define evalStringToScalarDriver_HPP

#include "scalar.hpp"
#include "className.hpp"
#include "genericRagelLemonDriver.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace parsing
{
namespace evalStringToScalar
{

/*---------------------------------------------------------------------------*\
                         Class parseDriver Declaration
\*---------------------------------------------------------------------------*/

class parseDriver
:
    public genericRagelLemonDriver
{
    // Private Data

        //- The result
        scalar value_;

public:

    ClassName("evalStringToScalar::driver");

    // Constructors

        //- Construct null
        parseDriver();


    //- Destructor
    virtual ~parseDriver() = default;


    // Member Functions

        //- Perform parsing on (sub) string
        //  \return evaluated value
        scalar execute
        (
            const std::string& s,
            size_t strPos = 0,
            size_t strLen = std::string::npos
        );

        //- Get value
        scalar value() const
        {
            return value_;
        }

        //- Set value
        void setValue(scalar val)
        {
            value_ = val;
        }
};


} // End namespace evalStringToScalar
} // End namespace parsing
} // End namespace CML


#endif
