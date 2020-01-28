/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2018 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "stringOps.hpp"
#include "typeInfo.hpp"
#include "OSspecific.hpp"
#include "OStringStream.hpp"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//! \cond fileScope
//  Find the type/position of the ":-" or ":+" alternative values
//
static inline int findParameterAlternative
(
    const std::string& s,
    std::string::size_type& pos,
    std::string::size_type endPos
)
{
    while (pos != std::string::npos)
    {
        pos = s.find(':', pos);
        if (pos != std::string::npos)
        {
            if (pos < endPos)
            {
                // in-range: check for '+' or '-' following the ':'
                const int altType = s[pos+1];
                if (altType == '+' || altType == '-')
                {
                    return altType;
                }

                ++pos;    // unknown/unsupported - continue at next position
            }
            else
            {
                // out-of-range: abort
                pos = std::string::npos;
            }
        }
    }

    return 0;
}
//! \endcond


CML::string CML::stringOps::expand
(
    const std::string& original,
    const HashTable<string, word, string::hash>& mapping,
    const char sigil
)
{
    string s(original);
    inplaceExpand(s, mapping);
    return s;
}


void CML::stringOps::inplaceExpand
(
    std::string& s,
    const HashTable<string, word, string::hash>& mapping,
    const char sigil
)
{
    string::size_type begVar = 0;

    // Expand $VAR or ${VAR}
    // Repeat until nothing more is found
    while
    (
        (begVar = s.find(sigil, begVar)) != string::npos
     && begVar < s.size()-1
    )
    {
        if (begVar == 0 || s[begVar-1] != '\\')
        {
            // Find end of first occurrence
            string::size_type endVar = begVar;
            string::size_type delim = 0;

            // The type/position of the ":-" or ":+" alternative values
            int altType = 0;
            string::size_type altPos = string::npos;

            if (s[begVar+1] == '{')
            {
                endVar = s.find('}', begVar);
                delim = 1;

                // check for ${parameter:-word} or ${parameter:+word}
                if (endVar != string::npos)
                {
                    altPos = begVar;
                    altType = findParameterAlternative(s, altPos, endVar);
                }
            }
            else
            {
                string::iterator iter = s.begin() + begVar + 1;

                // more generous in accepting keywords than for env variables
                while
                (
                    iter != s.end()
                 &&
                    (
                        isalnum(*iter)
                     || *iter == '.'
                     || *iter == ':'
                     || *iter == '_'
                    )
                )
                {
                    ++iter;
                    ++endVar;
                }
            }

            if (endVar == string::npos)
            {
                // likely parsed '${...' without closing '}' - abort
                break;
            }
            else if (endVar == begVar)
            {
                // parsed '${}' or $badChar  - skip over
                begVar = endVar + 1;
            }
            else
            {
                const word varName
                (
                    s.substr
                    (
                        begVar + 1 + delim,
                        (
                            (altPos == string::npos ? endVar : altPos)
                          - begVar - 2*delim
                        )
                    ),
                    false
                );

                std::string altValue;
                if (altPos != string::npos)
                {
                    // had ":-" or ":+" alternative value
                    altValue = s.substr
                    (
                        altPos + 2,
                        endVar - altPos - 2*delim
                    );
                }


                HashTable<string, word, string::hash>::const_iterator fnd =
                    mapping.find(varName);

                if (fnd != HashTable<string, word, string::hash>::end())
                {
                    if (altPos != string::npos && altType == '+')
                    {
                        // was found, use ":+" alternative
                        s.std::string::replace
                        (
                            begVar,
                            endVar - begVar + 1,
                            altValue
                        );
                        begVar += altValue.size();
                    }
                    else
                    {
                        // was found, use value
                        s.std::string::replace
                        (
                            begVar,
                            endVar - begVar + 1,
                            *fnd
                        );
                        begVar += (*fnd).size();
                    }
                }
                else if (altPos != string::npos && altType == '-')
                {
                    // was not found, use ":-" alternative
                    s.std::string::replace
                    (
                        begVar,
                        endVar - begVar + 1,
                        altValue
                    );
                    begVar += altValue.size();
                }
                else
                {
                    // substitute with nothing, also for ":+" alternative
                    s.std::string::erase(begVar, endVar - begVar + 1);
                }
            }
        }
        else
        {
            ++begVar;
        }
    }
}


CML::string CML::stringOps::expand
(
    const string& original,
    const dictionary& dict,
    const char sigil
)
{
    string s(original);
    return inplaceExpand(s, dict, sigil);
}


CML::string CML::stringOps::getVariable
(
    const word& name,
    const dictionary& dict,
    const bool allowEnvVars,
    const bool allowEmpty
)
{
    string value;

    const entry* ePtr = dict.lookupScopedEntryPtr
    (
        name,
        true,
        false
    );
    if (ePtr)
    {
        OStringStream buf;
        // Force floating point numbers to be printed with at least
        // some decimal digits.
        buf << scientific;
        buf.precision(IOstream::defaultPrecision());

        // Fail for non-primitiveEntry
        const primitiveEntry& pe =
            dynamicCast<const primitiveEntry>(*ePtr);

        if (pe.size() == 1 && pe[0].isAnyString())
        {
            // Already a string-type (WORD, STRING, ...). Just copy.
            value = pe[0].anyStringToken();
        }
        else
        {
            pe.write(buf, true);
            value = buf.str();
        }
    }
    else if (allowEnvVars)
    {
        value = getEnv(name);

        if (value.empty())
        {
            FatalIOErrorInFunction
            (
                dict
            )   << "Cannot find dictionary or environment variable "
                << name << exit(FatalIOError);
        }
    }
    else
    {
        FatalIOErrorInFunction
        (
            dict
        )   << "Cannot find dictionary variable "
            << name << exit(FatalIOError);
    }

    return value;
}


CML::string CML::stringOps::expand
(
    const string& s,
    string::size_type& index,
    const dictionary& dict,
    const bool allowEnvVars,
    const bool allowEmpty
)
{
    string newString;

    while (index < s.size())
    {
        if (s[index] == '$' && s[index+1] == '{')
        {
            // Recurse to parse variable name
            index += 2;
            string val = expand(s, index, dict, allowEnvVars, allowEmpty);
            newString.append(val);
        }
        else if (s[index] == '}')
        {
            return getVariable(newString, dict, allowEnvVars, allowEmpty);
        }
        else
        {
            newString.append(string(s[index]));
        }
        index++;
    }
    return newString;
}


void CML::stringOps::inplaceExpand
(
    std::string& s,
    const dictionary& dict,
    const bool allowEnvVars,
    const bool allowEmpty,
    const char sigil
)
{
    string::size_type begVar = 0;

    // Expand $VAR or ${VAR}
    // Repeat until nothing more is found
    while
    (
        (begVar = s.find(sigil, begVar)) != string::npos
     && begVar < s.size()-1
    )
    {
        if (begVar == 0 || s[begVar-1] != '\\')
        {
            if (s[begVar+1] == '{')
            {
                // Recursive variable expansion mode
                label stringStart = begVar;
                begVar += 2;
                string varValue
                (
                    expand
                    (
                        s,
                        begVar,
                        dict,
                        allowEnvVars,
                        allowEmpty
                    )
                );

                s.std::string::replace
                (
                    stringStart,
                    begVar - stringStart + 1,
                    varValue
                );

                begVar = stringStart+varValue.size();
            }
            else
            {
                string::iterator iter = s.begin() + begVar + 1;

                // more generous in accepting keywords than for env variables
                string::size_type endVar = begVar;
                while
                (
                    iter != s.end()
                 &&
                    (
                        isalnum(*iter)
                     || *iter == '.'
                     || *iter == ':'
                     || *iter == '_'
                    )
                )
                {
                    ++iter;
                    ++endVar;
                }

                const word varName
                (
                    s.substr
                    (
                        begVar + 1,
                        endVar - begVar
                    ),
                    false
                );

                string varValue
                (
                    getVariable
                    (
                        varName,
                        dict,
                        allowEnvVars,
                        allowEmpty
                    )
                );

                s.std::string::replace
                (
                    begVar,
                    varName.size()+1,
                    varValue
                );
                begVar += varValue.size();
            }
        }
        else
        {
            ++begVar;
        }
    }

    if (!s.empty())
    {
        if (s[0] == '~')
        {
            // Expand initial ~
            //   ~/        => home directory
            //   ~CAELUS => site/user CAELUS configuration directory
            //   ~user     => home directory for specified user

            string user;
            fileName file;

            if ((begVar = s.find('/')) != string::npos)
            {
                user = s.substr(1, begVar - 1);
                file = s.substr(begVar + 1);
            }
            else
            {
                user = s.substr(1);
            }

            // NB: be a bit lazy and expand ~unknownUser as an
            // empty string rather than leaving it untouched.
            // otherwise add extra test
            if (user == "CAELUS")
            {
                s = findEtcFile(file);
            }
            else
            {
                s = home(user)/file;
            }
        }
        else if (s[0] == '.')
        {
            // Expand a lone '.' and an initial './' into cwd
            if (s.size() == 1)
            {
                s = cwd();
            }
            else if (s[1] == '/')
            {
                s.std::string::replace(0, 1, cwd());
            }
        }
    }
}


CML::string& CML::stringOps::inplaceExpand
(
    string& s,
    const dictionary& dict,
    const char sigil
)
{
    string::size_type begVar = 0;

    // Expand $VAR or ${VAR}
    // Repeat until nothing more is found
    while
    (
        (begVar = s.find(sigil, begVar)) != string::npos
     && begVar < s.size()-1
    )
    {
        if (begVar == 0 || s[begVar-1] != '\\')
        {
            // Find end of first occurrence
            string::size_type endVar = begVar;
            string::size_type delim = 0;

            if (s[begVar+1] == '{')
            {
                endVar = s.find('}', begVar);
                delim = 1;
            }
            else
            {
                string::iterator iter = s.begin() + begVar + 1;

                // more generous in accepting keywords than for env variables
                while
                (
                    iter != s.end()
                 &&
                    (
                        isalnum(*iter)
                     || *iter == '.'
                     || *iter == ':'
                     || *iter == '_'
                    )
                )
                {
                    ++iter;
                    ++endVar;
                }
            }

            if (endVar == string::npos)
            {
                // likely parsed '${...' without closing '}' - abort
                break;
            }
            else if (endVar == begVar)
            {
                // parsed '${}' or $badChar  - skip over
                begVar = endVar + 1;
            }
            else
            {
                const word varName
                (
                    s.substr
                    (
                        begVar + 1 + delim,
                        endVar - begVar - 2*delim
                    ),
                    false
                );


                // lookup in the dictionary
                const entry* ePtr = dict.lookupScopedEntryPtr
                (
                    varName,
                    true,
                    false   // wildcards disabled. See primitiveEntry
                );

                // if defined - copy its entries
                if (ePtr)
                {
                    OStringStream buf;
                    // Force floating point numbers to be printed with at least
                    // some decimal digits.
                    buf << scientific;
                    buf.precision(IOstream::defaultPrecision());
                    if (ePtr->isDict())
                    {
                        ePtr->dict().write(buf, false);
                    }
                    else
                    {
                        // fail for other types
                        dynamicCast<const primitiveEntry>
                        (
                            *ePtr
                        ).write(buf, true);
                    }

                    s.std::string::replace
                    (
                        begVar,
                        endVar - begVar + 1,
                        buf.str()
                    );
                    begVar += buf.str().size();
                }
                else
                {
                    // not defined - leave original string untouched
                    begVar = endVar + 1;
                }
            }
        }
        else
        {
            ++begVar;
        }
    }

    return s;
}


CML::string CML::stringOps::expand
(
    const string& original,
    const bool allowEmpty
)
{
    string s(original);
    return inplaceExpand(s, allowEmpty);
}


CML::string& CML::stringOps::inplaceExpand
(
    string& s,
    const bool allowEmpty
)
{
    string::size_type begVar = 0;

    // Expand $VARS
    // Repeat until nothing more is found
    while
    (
        (begVar = s.find('$', begVar)) != string::npos
     && begVar < s.size()-1
    )
    {
        if (begVar == 0 || s[begVar-1] != '\\')
        {
            // Find end of first occurrence
            string::size_type endVar = begVar;
            string::size_type delim = 0;

            // The type/position of the ":-" or ":+" alternative values
            int altType = 0;
            string::size_type altPos = string::npos;

            if (s[begVar+1] == '{')
            {
                endVar = s.find('}', begVar);
                delim = 1;

                // check for ${parameter:-word} or ${parameter:+word}
                if (endVar != string::npos)
                {
                    altPos = begVar;
                    altType = findParameterAlternative(s, altPos, endVar);
                }
            }
            else
            {
                string::iterator iter = s.begin() + begVar + 1;

                while
                (
                    iter != s.end()
                 && (isalnum(*iter) || *iter == '_')
                )
                {
                    ++iter;
                    ++endVar;
                }
            }


            if (endVar == string::npos)
            {
                // likely parsed '${...' without closing '}' - abort
                break;
            }
            else if (endVar == begVar)
            {
                // parsed '${}' or $badChar  - skip over
                begVar = endVar + 1;
            }
            else
            {
                const word varName
                (
                    s.substr
                    (
                        begVar + 1 + delim,
                        (
                            (altPos == string::npos ? endVar : altPos)
                          - begVar - 2*delim
                        )
                    ),
                    false
                );

                std::string altValue;
                if (altPos != string::npos)
                {
                    // had ":-" or ":+" alternative value
                    altValue = s.substr
                    (
                        altPos + 2,
                        endVar - altPos - 2*delim
                    );
                }

                const string varValue = getEnv(varName);
                if (varValue.size())
                {
                    if (altPos != string::npos && altType == '+')
                    {
                        // was found, use ":+" alternative
                        s.std::string::replace
                        (
                            begVar,
                            endVar - begVar + 1,
                            altValue
                        );
                        begVar += altValue.size();
                    }
                    else
                    {
                        // was found, use value
                        s.std::string::replace
                        (
                            begVar,
                            endVar - begVar + 1,
                            varValue
                        );
                        begVar += varValue.size();
                    }
                }
                else if (altPos != string::npos)
                {
                    // use ":-" or ":+" alternative values
                    if (altType == '-')
                    {
                        // was not found, use ":-" alternative
                        s.std::string::replace
                        (
                            begVar,
                            endVar - begVar + 1,
                            altValue
                        );
                        begVar += altValue.size();
                    }
                    else
                    {
                        // was not found, ":+" alternative implies
                        // substitute with nothing
                        s.std::string::erase(begVar, endVar - begVar + 1);
                    }
                }
                else if (allowEmpty)
                {
                    s.std::string::erase(begVar, endVar - begVar + 1);
                }
                else
                {
                    FatalErrorInFunction
                        << "Unknown variable name '" << varName << "'"
                        << exit(FatalError);
                }
            }
        }
        else
        {
            ++begVar;
        }
    }

    if (!s.empty())
    {
        if (s[0] == '~')
        {
            // Expand initial ~
            //   ~/        => home directory
            //   ~CAELUS => site/user CAELUS configuration directory
            //   ~user     => home directory for specified user

            string user;
            fileName file;

            if ((begVar = s.find('/')) != string::npos)
            {
                user = s.substr(1, begVar - 1);
                file = s.substr(begVar + 1);
            }
            else
            {
                user = s.substr(1);
            }

            // NB: be a bit lazy and expand ~unknownUser as an
            // empty string rather than leaving it untouched.
            // otherwise add extra test
            if (user == "CAELUS")
            {
                s = findEtcFile(file);
            }
            else
            {
                s = home(user)/file;
            }
        }
        else if (s[0] == '.')
        {
            // Expand a lone '.' and an initial './' into cwd
            if (s.size() == 1)
            {
                s = cwd();
            }
            else if (s[1] == '/')
            {
                s.std::string::replace(0, 1, cwd());
            }
        }
    }

    return s;
}


CML::string CML::stringOps::trimLeft(const string& s)
{
    if (!s.empty())
    {
        string::size_type beg = 0;
        while (beg < s.size() && isspace(s[beg]))
        {
            ++beg;
        }

        if (beg)
        {
            return s.substr(beg);
        }
    }

    return s;
}


void CML::stringOps::inplaceTrimLeft(string& s)
{
    if (!s.empty())
    {
        string::size_type beg = 0;
        while (beg < s.size() && isspace(s[beg]))
        {
            ++beg;
        }

        if (beg)
        {
            s.erase(0, beg);
        }
    }
}


CML::string CML::stringOps::trimRight(const string& s)
{
    if (!s.empty())
    {
        string::size_type sz = s.size();
        while (sz && isspace(s[sz-1]))
        {
            --sz;
        }

        if (sz < s.size())
        {
            return s.substr(0, sz);
        }
    }

    return s;
}


void CML::stringOps::inplaceTrimRight(string& s)
{
    if (!s.empty())
    {
        string::size_type sz = s.size();
        while (sz && isspace(s[sz-1]))
        {
            --sz;
        }

        s.resize(sz);
    }
}


CML::string CML::stringOps::trim(const string& original)
{
    return trimLeft(trimRight(original));
}


void CML::stringOps::inplaceTrim(string& s)
{
    inplaceTrimRight(s);
    inplaceTrimLeft(s);
}


CML::string CML::stringOps::removeComments(const std::string& str)
{
    string s(str);
    inplaceRemoveComments(s);
    return s;
}


void CML::stringOps::inplaceRemoveComments(std::string& s)
{
    const string::size_type len = s.length();
//DWSFIX    const auto len = s.length();

    if (len < 2)
    {
        return;
    }

    std::string::size_type n = 0;

    for (std::string::size_type i = 0; i < len; ++i)
    {
        char c = s[i];

        if (n != i)
        {
            s[n] = c;
        }
        ++n;

        // The start of a C/C++ comment?
        if (c == '/')
        {
            ++i;

            if (i == len)
            {
                // No further characters
                break;
            }

            c = s[i];

            if (c == '/')
            {
                // C++ comment - remove to end-of-line

                --n;
                s[n] = '\n';

                // Backtrack to eliminate leading spaces,
                // up to the previous newline

                while (n && std::isspace(s[n-1]))
                {
                    --n;

                    if (s[n] == '\n')
                    {
                        break;
                    }

                    s[n] = '\n';
                }

                i = s.find('\n', ++i);

                if (i == std::string::npos)
                {
                    // Trucated - done
                    break;
                }

                ++n;  // Include newline in output
            }
            else if (c == '*')
            {
                // C comment - search for '*/'
                --n;
                i = s.find("*/", ++i, 2);

                if (i == std::string::npos)
                {
                    // Trucated - done
                    break;
                }

                ++i;  // Index past first of "*/", loop increment does the rest
            }
            else
            {
                // Not a C/C++ comment
                if (n != i)
                {
                    s[n] = c;
                }
                ++n;
            }
        }
    }

    s.erase(n);
}


// ************************************************************************* //
