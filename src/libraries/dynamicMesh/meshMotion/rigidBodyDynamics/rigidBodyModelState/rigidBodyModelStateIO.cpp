/*---------------------------------------------------------------------------*\
Copyright (C) 2016-2019 OpenFOAM Foundation
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

#include "rigidBodyModelState.hpp"
#include "IOstreams.hpp"

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void CML::RBD::rigidBodyModelState::write(dictionary& dict) const
{
    dict.add("q", q_);
    dict.add("qDot", qDot_);
    dict.add("qDdot", qDdot_);
    dict.add("deltaT", deltaT_);
}


void CML::RBD::rigidBodyModelState::write(Ostream& os) const
{
    writeEntry(os, "q", q_);
    writeEntry(os, "qDot", qDot_);
    writeEntry(os, "qDdot", qDdot_);
    writeEntry(os, "deltaT", deltaT_);
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

CML::Istream& CML::RBD::operator>>
(
    Istream& is,
    rigidBodyModelState& state
)
{
    is  >> state.q_
        >> state.qDot_
        >> state.qDdot_
        >> state.deltaT_;

    // Check state of Istream
    is.check
    (
        "CML::Istream& CML::operator>>"
        "(CML::Istream&, CML::RBD::rigidBodyModelState&)"
    );

    return is;
}


CML::Ostream& CML::RBD::operator<<
(
    Ostream& os,
    const rigidBodyModelState& state
)
{
    os  << state.q_
        << token::SPACE << state.qDot_
        << token::SPACE << state.qDdot_
        << token::SPACE << state.deltaT_;

    // Check state of Ostream
    os.check
    (
        "CML::Ostream& CML::operator<<(CML::Ostream&, "
        "const CML::RBD::rigidBodyModelState&)"
    );

    return os;
}


// ************************************************************************* //
