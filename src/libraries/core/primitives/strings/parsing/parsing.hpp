/*---------------------------------------------------------------------------*\
Copyright (C) 2017-2019 OpenCFD Ltd.
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

Namespace
    CML::parsing

Description
    Collection of static functions and data related to parsing
    and an isolated namespace for lexers, parsers, scanners.

SourceFiles
    parsing.cpp

\*---------------------------------------------------------------------------*/
#ifndef parsing_HPP
#define parsing_HPP

#include "Enum.hpp"
#include <cerrno>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                        Namespace parsing Declaration
\*---------------------------------------------------------------------------*/

namespace parsing
{
    //- Enumeration for possible parsing error
    enum class errorType
    {
        NONE = 0,         //!< No error encountered
        GENERAL = 1,      //!< General parsing error
        RANGE = 2,        //!< Range error
        TRAILING = 3,     //!< Trailing content detected
    };


    //- Strings corresponding to the errorType
    extern const CML::Enum<errorType> errorNames;

    //- Sanity check after strtof, strtod, etc.
    //  Should set errno = 0 prior to the conversion.
    inline errorType checkConversion(const char* buf, char* endptr);


} // End namespace parsing
} // End namespace CML


inline CML::parsing::errorType CML::parsing::checkConversion
(
    const char* buf,
    char* endptr
)
{
    if (errno || endptr == buf)
    {
        // Some type of error OR no conversion
        return (errno == ERANGE ? errorType::RANGE : errorType::GENERAL);
    }

    // Trailing spaces are permitted
    while (isspace(*endptr))
    {
        ++endptr;
    }

    if (*endptr != '\0')
    {
        // Trailing content
        return errorType::TRAILING;
    }

    // Valid conversion
    return errorType::NONE;
}


#endif
