/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
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

\*---------------------------------------------------------------------------*/

#include "meshOctreeCreator.hpp"
#include "triSurf.hpp"
#include "boundBox.hpp"
#include "demandDrivenData.hpp"


// #define DEBUGSearch

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

meshOctreeCreator::meshOctreeCreator(meshOctree& mo)
:
    octree_(mo),
    scalingFactor_(1.0),
    meshDictPtr_(nullptr),
    hexRefinement_(false),
    globalRefLevel_(0),
    surfRefLevel_(mo.surface().size())
{}

meshOctreeCreator::meshOctreeCreator
(
    meshOctree& mo,
    const IOdictionary& dict
)
:
    octree_(mo),
    scalingFactor_(1.0),
    meshDictPtr_(&dict),
    hexRefinement_(false),
    globalRefLevel_(0),
    surfRefLevel_(mo.surface().size())
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

meshOctreeCreator::~meshOctreeCreator()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void meshOctreeCreator::setScalingFactor(const scalar s)
{
    scalingFactor_ = s;
}

void meshOctreeCreator::activateHexRefinement()
{
    hexRefinement_ = true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
