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

#include "sortLabelledTri.hpp"
#include "labelledTri.hpp"
#include "triSurface.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * Private Classes * * * * * * * * * * * * * * //

inline bool surfAndLabel::less::operator()
(
    const surfAndLabel& one,
    const surfAndLabel& two
) const
{
    const triSurface& surf = *one.surfPtr_;
    return surf[one.index_].region() < surf[two.index_].region();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
sortLabelledTri::sortLabelledTri(const triSurface& surf)
:
    List<surfAndLabel>(surf.size(), surfAndLabel(surf, -1))
{

    // Set the face label
    forAll(surf, facei)
    {
        operator[](facei).index_ = facei;
    }

    // Sort according to region number.
    sort(*this, surfAndLabel::less());
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void sortLabelledTri::indices(labelList& newIndices) const
{
    newIndices.setSize(size());

    forAll(newIndices, i)
    {
        newIndices[i] = operator[](i).index_;
    }
}


labelList sortLabelledTri::indices() const
{
    labelList newIndices(size());
    indices(newIndices);
    return newIndices;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
