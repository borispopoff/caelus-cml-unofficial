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

#include "Switch.hpp"
#include "error.hpp"
#include "dictionary.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const char* CML::Switch::names[nSwitchType] =
{
    "false",
    "true",
    "off",
    "on",
    "no",
    "yes",
    "n",
    "y",
    "f",
    "t",
    "none",
    "any",
    "invalid"
};


// * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * * //

CML::Switch::switchType CML::Switch::asEnum
(
    const std::string& str,
    const bool allowInvalid
)
{
    for (switchType sw=switchType::False; sw<switchType::invalid; ++sw)
    {
        if (str == names[toInt(sw)])
        {
            // Handle aliases
            switch (sw)
            {
                case switchType::n:
                case switchType::none:
                {
                    return switchType::no;
                    break;
                }

                case switchType::y:
                case switchType::any:
                {
                    return switchType::yes;
                    break;
                }

                case switchType::f:
                {
                    return switchType::False;
                    break;
                }

                case switchType::t:
                {
                    return switchType::True;
                    break;
                }

                default:
                {
                    return switchType(sw);
                    break;
                }
            }
        }
    }

    if (!allowInvalid)
    {
        FatalErrorInFunction
            << "unknown switch word " << str << nl
            << abort(FatalError);
    }

    return switchType::invalid;
}


CML::Switch CML::Switch::lookupOrAddToDict
(
    const word& name,
    dictionary& dict,
    const Switch& defaultValue
)
{
    return dict.lookupOrAddDefault<Switch>(name, defaultValue);
}


// * * * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * * //

bool CML::Switch::valid() const
{
    return switch_ <= switchType::none;
}


const char* CML::Switch::asText() const
{
    return names[toInt(switch_)];
}


bool CML::Switch::readIfPresent(const word& name, const dictionary& dict)
{
    return dict.readIfPresent<Switch>(name, *this);
}


// ************************************************************************* //
