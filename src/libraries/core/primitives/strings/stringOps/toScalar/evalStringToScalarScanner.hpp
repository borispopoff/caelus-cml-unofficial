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
    CML::parsing::evalStringToScalar::scanner

Description
    Ragel lexer interface for lemon grammar of a simple string to
    scalar evaluation, which is used by stringOps::toScalar

Note
    Ragel code generated with the ./createCode script.

\*---------------------------------------------------------------------------*/

#ifndef evalStringToScalarScanner_HPP
#define evalStringToScalarScanner_HPP

#include <string>

namespace CML
{
namespace parsing
{
namespace evalStringToScalar
{

// Forward Declarations
class parser;
class parseDriver;

/*---------------------------------------------------------------------------*\
                         Class scanner Declaration
\*---------------------------------------------------------------------------*/

class scanner
{
    // Private Data

        //- Wrapped lemon parser
        parser* parser_;

        // Ragel code state, action
        int cs, act;

public:

    //- Debug/tracing of scan
    static int debug;


    // Constructors

        //- Construct null
        scanner() : parser_(nullptr) {}


    //- Destructor, deletes parser
    ~scanner();


    // Member Functions

        //- Evaluate sub-string
        bool process
        (
            const std::string& str, size_t pos, size_t len,
            parseDriver& driver
        );

        //- Evaluate sub-string
        bool process
        (
            const std::string& str, size_t pos,
            parseDriver& driver
        )
        {
            return process(str, pos, std::string::npos, driver);
        }

        //- Evaluate string
        bool process(const std::string& str, parseDriver& driver)
        {
            return process(str, 0, std::string::npos, driver);
        }
};


} // End namespace evalStringToScalar
} // End namespace parsing
} // End namespace CML


#endif
