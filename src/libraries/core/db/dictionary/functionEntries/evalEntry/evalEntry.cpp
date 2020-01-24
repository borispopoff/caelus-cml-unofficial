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

\*---------------------------------------------------------------------------*/

#include "evalEntry.hpp"
#include "dictionary.hpp"
#include "stringOps.hpp"
#include "addToMemberFunctionSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
namespace functionEntries
{
    addNamedToMemberFunctionSelectionTable
    (
        functionEntry,
        evalEntry,
        execute,
        dictionaryIstream,
        eval
    );

    addNamedToMemberFunctionSelectionTable
    (
        functionEntry,
        evalEntry,
        execute,
        primitiveEntryIstream,
        eval
    );

} // End namespace functionEntry
} // End namespace CML


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

CML::scalar CML::functionEntries::evalEntry::evaluate
(
    const dictionary& parentDict,
    Istream& is
)
{
    Info
        << "Using #eval at line " << is.lineNumber()
        << " in file " <<  parentDict.name() << nl;

    // String to evaluate
    string s;

    token tok(is);

    if (!tok.good())
    {
        FatalIOErrorInFunction(is)
            << "Bad token - could not get string to evaluate"
            << exit(FatalIOError);
        return 0;
    }

    if (tok.isString())
    {
        s = tok.stringToken();
    }
    else if (tok == token::BEGIN_BLOCK)
    {
        dynamic_cast<ISstream&>(is).getLine(s, token::END_BLOCK);
    }
    else
    {
        is.putBack(tok);

        FatalIOErrorInFunction(is)
            << "Invalid input for #eval" << nl
            << exit(FatalIOError);
    }

    #ifdef FULLDEBUG
    Info
        << "input: " << s << endl;
    #endif

    stringOps::inplaceRemoveComments(s);
    stringOps::inplaceExpand(s, parentDict);

    #ifdef FULLDEBUG
    Info
        << "expanded: " << s << endl;
    #endif

    return stringOps::toScalar(s);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CML::functionEntries::evalEntry::execute
(
    const dictionary& parentDict,
    primitiveEntry& entry,
    Istream& is
)
{
    const scalar value = evaluate(parentDict, is);

    // The result as terminated token entry
    ITstream result
    (
        "eval",
        tokenList({token(value), token(token::END_STATEMENT)})
    );

    entry.read(parentDict, result);

    return true;
}


bool CML::functionEntries::evalEntry::execute
(
    dictionary& parentDict,
    Istream& is
)
{
    const scalar value = evaluate(parentDict, is);

    // The result as terminated token entry
    ITstream result
    (
        "eval",
        tokenList({token(value), token(token::END_STATEMENT)})
    );

    parentDict.read(result);

    return true;
}


// ************************************************************************* //
