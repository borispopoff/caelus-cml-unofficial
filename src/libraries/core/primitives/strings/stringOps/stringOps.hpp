/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2016 OpenFOAM Foundation
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

Namespace
    CML::stringOps

Description
    Collection of static functions to do various simple string-related
    operations

SourceFiles
    stringOps.cpp

\*---------------------------------------------------------------------------*/
#ifndef stringOps_H
#define stringOps_H

#include "scalar.hpp"
#include "string.hpp"
#include "dictionary.hpp"
#include "HashTable.hpp"

#include "evalStringToScalar.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                        Namespace stringOps Declaration
\*---------------------------------------------------------------------------*/

namespace stringOps
{
    //- Expand occurrences of variables according to the mapping
    //  Expansion includes:
    //  -# variables
    //    - "$VAR", "${VAR}"
    //
    //  Supports default values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:-defValue}"
    //  \endcode
    //  If parameter is unset or null, the \c defValue is substituted.
    //  Otherwise, the value of parameter is substituted.
    //
    //  Supports alternative values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:+altValue}"
    //  \endcode
    //  If parameter is unset or null, nothing is substituted.
    //  Otherwise the \c altValue is substituted.
    //
    //  Any unknown entries are removed silently.
    //
    //  Malformed entries (eg, brace mismatch, sigil followed by bad character)
    //  are left as is.
    //
    //  \note the leading sigil can be changed to avoid conflicts with other
    //  string expansions
    string expand
    (
        const std::string&,
        const HashTable<string, word, string::hash>& mapping,
        const char sigil = '$'
    );


    //- Inplace expand occurrences of variables according to the mapping
    //  Expansion includes:
    //  -# variables
    //    - "$VAR", "${VAR}"
    //
    //  Supports default values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:-defValue}"
    //  \endcode
    //  If parameter is unset or null, the \c defValue is substituted.
    //  Otherwise, the value of parameter is substituted.
    //
    //  Supports alternative values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:+altValue}"
    //  \endcode
    //  If parameter is unset or null, nothing is substituted.
    //  Otherwise the \c altValue is substituted.
    //
    //  Any unknown entries are removed silently.
    //
    //  Malformed entries (eg, brace mismatch, sigil followed by bad character)
    //  are left as is.
    //
    //  \note the leading sigil can be changed to avoid conflicts with other
    //  string expansions
    void inplaceExpand
    (
        std::string&,
        const HashTable<string, word, string::hash>& mapping,
        const char sigil = '$'
    );

    //- Expand occurrences of variables according to the dictionary
    //  Expansion includes:
    //  -# variables
    //    - "$VAR", "${VAR}"
    //
    //  Any unknown entries are left as-is
    //
    //  \note the leading sigil can be changed to avoid conflicts with other
    //  string expansions
    string expand
    (
        const string&,
        const dictionary& dict,
        const char sigil = '$'
    );


    //- Get dictionary or (optionally) environment variable
    string getVariable
    (
        const word& name,
        const dictionary& dict,
        const bool allowEnvVars,
        const bool allowEmpty
    );


    //- Recursively expands (dictionary or environment) variable
    //  starting at index in string. Updates index.
    string expand
    (
        const string& s,
        string::size_type& index,
        const dictionary& dict,
        const bool allowEnvVars,
        const bool allowEmpty
    );


    //- Inplace expand occurrences of variables according to the dictionary
    //  and optionally environment variables
    //  Expansion includes:
    //  -# variables
    //    - "$VAR", "${VAR}"
    //
    //  with the "${}" syntax doing a recursive substitution.
    //  Any unknown entries are left as-is
    //
    //  \note the leading sigil can be changed to avoid conflicts with other
    //  string expansions
    void inplaceExpand
    (
        std::string& s,
        const dictionary& dict,
        const bool allowEnvVars,
        const bool allowEmpty,
        const char sigil = '$'
    );


    //- Inplace expand occurrences of variables according to the dictionary
    //  Expansion includes:
    //  -# variables
    //    - "$VAR", "${VAR}"
    //
    //  Any unknown entries are left as-is
    //
    //  \note the leading sigil can be changed to avoid conflicts with other
    //  string expansions
    string& inplaceExpand
    (
        string&,
        const dictionary& dict,
        const char sigil = '$'
    );


    //- Expand initial tildes and all occurrences of environment variables
    //  Expansion includes:
    //  -# environment variables
    //    - "$VAR", "${VAR}"
    //  -# current directory
    //    - leading "./" : the current directory
    //  -# tilde expansion
    //    - leading "~/" : home directory
    //    - leading "~user" : home directory for specified user
    //    - leading "~CAELUS" : site/user CAELUS configuration directory
    //
    //  Supports default values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:-defValue}"
    //  \endcode
    //  If parameter is unset or null, the \c defValue is substituted.
    //  Otherwise, the value of parameter is substituted.
    //
    //  Supports alternative values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:+altValue}"
    //  \endcode
    //  If parameter is unset or null, nothing is substituted.
    //  Otherwise the \c altValue is substituted.
    //
    //  Any unknown entries are removed silently, if allowEmpty is true.
    //
    //  Malformed entries (eg, brace mismatch, sigil followed by bad character)
    //  are left as is.
    //
    //  \sa
    //  CML::findEtcFile
    string expand
    (
        const string&,
        const bool allowEmpty = false
    );


    //- Expand initial tildes and all occurrences of environment variables
    //  Expansion includes:
    //  -# environment variables
    //    - "$VAR", "${VAR}"
    //  -# current directory
    //    - leading "./" : the current directory
    //  -# tilde expansion
    //    - leading "~/" : home directory
    //    - leading "~user" : home directory for specified user
    //    - leading "~CAELUS" : site/user CAELUS configuration directory
    //
    //  Supports default values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:-defValue}"
    //  \endcode
    //  If parameter is unset or null, the \c defValue is substituted.
    //  Otherwise, the value of parameter is substituted.
    //
    //  Supports alternative values as per the Bourne/Korn shell.
    //  \code
    //      "${parameter:+altValue}"
    //  \endcode
    //  If parameter is unset or null, nothing is substituted.
    //  Otherwise the \c altValue is substituted.
    //
    //  Any unknown entries are removed silently, if allowEmpty is true.
    //
    //  Malformed entries (eg, brace mismatch, sigil followed by bad character)
    //  are left as is.
    //
    //  Any unknown entries are removed silently if allowEmpty is true.
    //  \sa
    //  CML::findEtcFile
    string& inplaceExpand
    (
        string&,
        const bool allowEmpty = false
    );


    //- Return string trimmed of leading whitespace
    string trimLeft(const string&);

    //- Trim leading whitespace inplace
    void inplaceTrimLeft(string&);

    //- Return string trimmed of trailing whitespace
    string trimRight(const string&);

    //- Trim trailing whitespace inplace
    void inplaceTrimRight(string&);

    //- Return string trimmed of leading and trailing whitespace
    string trim(const string&);

    //- Trim leading and trailing whitespace inplace
    void inplaceTrim(string&);

    //- Return string with C/C++ comments removed
    string removeComments(const std::string& str);

    //- Remove C/C++ comments inplace
    void inplaceRemoveComments(std::string& s);

} // End namespace stringOps


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
