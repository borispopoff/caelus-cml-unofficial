/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
Copyright (C) Applied CCM Pty Ltd.
-------------------------------------------------------------------------------
License
    This file is part of cfMesh.

    cfMesh is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    cfMesh is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with cfMesh.  If not, see <http://www.gnu.org/licenses/>.

Author: Franjo Juretic (franjo.juretic@c-fields.com)
        Darrin Stephens, Applied CCM Pty Ltd.
\*---------------------------------------------------------------------------*/

#include "rotatedBoxRefinement.hpp"
#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{

defineTypeNameAndDebug(rotatedBoxRefinement, 0);
addToRunTimeSelectionTable(objectRefinement, rotatedBoxRefinement, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

rotatedBoxRefinement::rotatedBoxRefinement()
:
    objectRefinement(),
    min_(),
    max_(),
    origin_(),
    axis_(),
    direction_(),
    transform_("rotation", origin_, axis_, direction_),
    box_()
{
}

rotatedBoxRefinement::rotatedBoxRefinement
(
    const word& name,
    const scalar cellSize,
    const direction additionalRefLevels,
    const point& min,
    const point& max,
    const point& origin,
    const vector& axis,
    const vector& direction
)
:
    objectRefinement(),
    min_(min),
    max_(max),
    origin_(origin),
    axis_(axis),
    direction_(direction),
    transform_("rotation", origin_, axis_, direction_),
    box_(min, max)
{
    setName(name);
    setCellSize(cellSize);
    setAdditionalRefinementLevels(additionalRefLevels);
}

rotatedBoxRefinement::rotatedBoxRefinement
(
    const word& name,
    const dictionary& dict
)
:
    objectRefinement(name, dict)
{
    this->operator=(dict);

    transform_ = coordinateSystem("rotation", origin_, axis_, direction_);

    box_ = boundBox(min_, max_);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

bool rotatedBoxRefinement::intersectsObject(const boundBox& bb) const
{
    // Transform bb points from global to local co-ordinate system of the box
    pointField bbPoints(transform_.localPosition(bb.points()));

    // Check if any points from bb (in box local co-oordinate system)
    // are in the box 
    if( box_.containsAny(bbPoints) )
        return true;

    return false;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

dictionary rotatedBoxRefinement::dict(bool /*ignoreType*/) const
{
    dictionary dict;

    if( additionalRefinementLevels() == 0 && cellSize() >= 0.0 )
    {
        dict.add("cellSize", cellSize());
    }
    else
    {
        dict.add("additionalRefinementLevels", additionalRefinementLevels());
    }

    dict.add("type", type());
    dict.add("min", min_);
    dict.add("max", max_);
    dict.add("origin", origin_);
    dict.add("e3", axis_);
    dict.add("e1", direction_);

    return dict;
}

void rotatedBoxRefinement::write(Ostream& os) const
{
    os  << " type:   " << type()
        << " min:    " << min_
        << " max:    " << max_
        << " origin: " << origin_
        << " e3:     " << axis_
        << " e1:     " << direction_;
}

void rotatedBoxRefinement::writeDict(Ostream& os, bool subDict) const
{
    if( subDict )
    {
        os << indent << token::BEGIN_BLOCK << incrIndent << nl;
    }

    if( additionalRefinementLevels() == 0 && cellSize() >= 0.0 )
    {
        os.writeKeyword("cellSize") << cellSize() << token::END_STATEMENT << nl;
    }
    else
    {
        os.writeKeyword("additionalRefinementLevels")
                << additionalRefinementLevels()
                << token::END_STATEMENT << nl;
    }

    // only write type for derived types
    if( type() != typeName_() )
    {
        os.writeKeyword("type") << type() << token::END_STATEMENT << nl;
    }

    os.writeKeyword("min") << min_ << token::END_STATEMENT << nl;
    os.writeKeyword("max") << max_ << token::END_STATEMENT << nl;
    os.writeKeyword("origin") << origin_ << token::END_STATEMENT << nl;
    os.writeKeyword("e3") << axis_ << token::END_STATEMENT << nl;
    os.writeKeyword("e1") << direction_ << token::END_STATEMENT << nl;

    if( subDict )
    {
        os << decrIndent << indent << token::END_BLOCK << endl;
    }
}

void rotatedBoxRefinement::operator=(const dictionary& d)
{
    // allow as embedded sub-dictionary "coordinateSystem"
    const dictionary& dict =
    (
        d.found(typeName_())
      ? d.subDict(typeName_())
      : d
    );

    // unspecified min is (0 0 0)
    if( dict.found("min") )
    {
        dict.lookup("min") >> min_;
    }
    else
    {
        FatalErrorInFunction
          << "Entry min is not specified!" << exit(FatalError);
        min_ = vector::zero;
    }

    // unspecified max is (0 0 0)
    if( dict.found("max") )
    {
        dict.lookup("max") >> max_;
    }
    else
    {
        FatalErrorInFunction
          << "Entry max is not specified!" << exit(FatalError);
        max_ = vector::zero;
    }

    // unspecified origin is (0 0 0)
    if( dict.found("origin") )
    {
        dict.lookup("origin") >> origin_;
    }
    else
    {
        FatalErrorInFunction
          << "Entry origin is not specified!" << exit(FatalError);
        origin_ = vector::zero;
    }

    // specify axis
    if( dict.found("e3") )
    {
        dict.lookup("e3") >> axis_;
    }
    else
    {
        FatalErrorInFunction
          << "Entry e3 is not specified!" << exit(FatalError);
        axis_ = vector::zero;
    }

    // specify direction
    if( dict.found("e1") )
    {
        dict.lookup("e1") >> direction_;
    }
    else
    {
        FatalErrorInFunction
          << "Entry e1 is not specified!" << exit(FatalError);
        direction_ = vector::zero;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Ostream& rotatedBoxRefinement::operator<<(Ostream& os) const
{
    os << "name " << name() << nl;
    os << "cell size " << cellSize() << nl;
    os << "additionalRefinementLevels " << additionalRefinementLevels() << endl;

    write(os);
    return os;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
