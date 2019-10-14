/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2017 OpenFOAM Foundation
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

#include "fvOption.hpp"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::fv::option::writeHeader(Ostream& os) const
{
    os  << indent << name_ << nl
        << indent << token::BEGIN_BLOCK << incrIndent << nl;
}


void CML::fv::option::writeFooter(Ostream& os) const
{
    os  << decrIndent << indent << token::END_BLOCK << endl;
}


void CML::fv::option::writeData(Ostream& os) const
{
    os.writeKeyword("type") << type() << token::END_STATEMENT << nl;
    os.writeKeyword("active") << active_ << token::END_STATEMENT << nl << nl;

    os << indent << word(type() + "Coeffs");
    coeffs_.write(os);
}


bool CML::fv::option::read(const dictionary& dict)
{
    dict.readIfPresent("active", active_);

    if (dict.found(modelType_ + "Coeffs"))
    {
        coeffs_ = dict.subDict(modelType_ + "Coeffs");
    }
    else
    {
        coeffs_ = dict;
    }

    return true;
}


