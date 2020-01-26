/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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

#include "primitiveEntry.hpp"
#include "dictionary.hpp"
#include "OSspecific.hpp"
#include "stringOps.hpp"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::primitiveEntry::append(const UList<token>& varTokens)
{
    forAll(varTokens, i)
    {
        newElmt(tokenIndex()++) = varTokens[i];
    }
}


bool CML::primitiveEntry::expandVariable
(
    const string& w,
    const dictionary& dict
)
{
    if (w.size() > 2 && w[0] == '$' && w[1] == token::BEGIN_BLOCK)
    {
        // Recursive substitution mode. Replace between {} with
        // expansion.
        string s(w(2, w.size()-3));
        // Substitute dictionary and environment variables. Do not allow
        // empty substitutions.
        stringOps::inplaceExpand(s, dict, true, false);

        string newW(w);
        newW.std::string::replace(1, newW.size()-1, s);

        return expandVariable(newW, dict);
    }
    else
    {
        string varName = w(1, w.size()-1);

        // Lookup the variable name in the given dictionary....
        // Note: allow wildcards to match? For now disabled since following
        // would expand internalField to wildcard match and not expected
        // internalField:
        //      internalField XXX;
        //      boundaryField { ".*" {YYY;} movingWall {value $internalField;}
        const entry* ePtr = dict.lookupEntryPtr(varName, true, false);

        // ...if defined append its tokens into this
        if (ePtr)
        {
            if (ePtr->isDict())
            {
                append(ePtr->dict().tokens());
            }
            else
            {
                append(ePtr->stream());
            }
        }
        else
        {
            // Not in the dictionary - try an environment variable
            string envStr = getEnv(varName);

            if (envStr.empty())
            {
                FatalIOErrorInFunction
                (
                    dict
                )   << "Illegal dictionary entry or environment variable name "
                    << varName << endl << "Valid dictionary entries are "
                    << dict.toc() << exit(FatalIOError);

                return false;
            }
            append(tokenList(IStringStream('(' + envStr + ')')()));
        }
    }
    return true;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::primitiveEntry::primitiveEntry(const keyType& key, const ITstream& is)
:
    entry(key),
    ITstream(is)
{
    name() += "::" + keyword();
}


CML::primitiveEntry::primitiveEntry(const keyType& key, const token& t)
:
    entry(key),
    ITstream(key, tokenList(1, t))
{}


CML::primitiveEntry::primitiveEntry
(
    const keyType& key,
    const UList<token>& tokens
)
:
    entry(key),
    ITstream(key, tokens)
{}


CML::primitiveEntry::primitiveEntry
(
    const keyType& key,
    List<token>&& tokens
)
:
    entry(key),
    ITstream(key, move(tokens))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::label CML::primitiveEntry::startLineNumber() const
{
    const tokenList& tokens = *this;

    if (tokens.empty())
    {
        return -1;
    }
    else
    {
        return tokens.first().lineNumber();
    }
}


CML::label CML::primitiveEntry::endLineNumber() const
{
    const tokenList& tokens = *this;

    if (tokens.empty())
    {
        return -1;
    }
    else
    {
        return tokens.last().lineNumber();
    }
}


CML::ITstream& CML::primitiveEntry::stream() const
{
    ITstream& is = const_cast<primitiveEntry&>(*this);
    is.rewind();
    return is;
}


const CML::dictionary& CML::primitiveEntry::dict() const
{
    FatalErrorInFunction
        << "Attempt to return primitive entry " << info()
        << " as a sub-dictionary"
        << abort(FatalError);

    return dictionary::null;
}


CML::dictionary& CML::primitiveEntry::dict()
{
    FatalErrorInFunction
        << "Attempt to return primitive entry " << info()
        << " as a sub-dictionary"
        << abort(FatalError);

    return const_cast<dictionary&>(dictionary::null);
}


// ************************************************************************* //
