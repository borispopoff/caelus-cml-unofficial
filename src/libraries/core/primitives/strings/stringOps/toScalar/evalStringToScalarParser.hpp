/*--------------------------------*- C++ -*----------------------------------*\
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
    CML::parsing::evalStringToScalar::parser

Description
    Interface to lemon parser to simple string to scalar evaluation, which
    is used by stringOps::toScalar

\*---------------------------------------------------------------------------*/

#ifndef evalStringToScalarParser_HPP
#define evalStringToScalarParser_HPP

namespace CML
{
namespace parsing
{
namespace evalStringToScalar
{

// Forward Declarations
class parseDriver;


/*---------------------------------------------------------------------------*\
                           Class parser Declaration
\*---------------------------------------------------------------------------*/

class parser
{
    // Private Data

        //- The lemon parser (demand-driven)
        void* lemon_;

public:

    // Constructors

        //- Construct null
        parser() : lemon_(nullptr) {}


    //- Destructor, delete lemon parser
    ~parser()
    {
        stop();
    }


    // Member Functions

        //- Start parsing, with given driver context
        void start(parseDriver& driver);

        //- Stop parsing, freeing the allocated parser
        void stop();

        //- Push token/value to parser
        void parse(int tokenId, CML::scalar val);
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace evalStringToScalar
} // End namespace parsing
} // End namespace CML


#endif
