/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
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

#include "jplotGraph.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //


namespace CML
{
    defineTypeNameAndDebug(jplotGraph, 0);
    const word jplotGraph::ext_("dat");

    typedef graph::writer graphWriter;
    addToRunTimeSelectionTable(graphWriter, jplotGraph, word);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::jplotGraph::write(const graph& g, Ostream& os) const
{
    os  << "# JPlot file" << nl
        << "# column 1: " << g.xName() << endl;

    label fieldi = 0;

    forAllConstIter(graph, g, iter)
    {
        os  << "# column " << fieldi + 2 << ": " << (*iter()).name() << endl;
        fieldi++;
    }

    g.writeTable(os);
}


// ************************************************************************* //
