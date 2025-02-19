/*---------------------------------------------------------------------------*\
Copyright (C) 2015 OpenCFD Ltd.
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

Class
    Foam::regionSplit2D

Description
    Splits a patch into regions based on a mask field.  Result is a globally
    consistent label list of region index per patch face.

SourceFiles
    regionSplit2D.cpp

\*---------------------------------------------------------------------------*/

#ifndef regionSplit2D_HPP
#define regionSplit2D_HPP

#include "DynamicList.hpp"
#include "boolList.hpp"
#include "labelList.hpp"
#include "indirectPrimitivePatch.hpp"
#include "patchEdgeFaceRegion.hpp"
#include "globalIndex.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

class faceZone;
class polyMesh;

/*---------------------------------------------------------------------------*\
                        Class regionSplit2D Declaration
\*---------------------------------------------------------------------------*/

class regionSplit2D
:
    public labelList
{
    // Private data

        //- Number of regions
        label nRegions_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        regionSplit2D(const regionSplit2D&);

        //- Disallow default bitwise assignment
        void operator=(const regionSplit2D&);


public:

    // Constructors

        //- Construct from mesh and list of blocked faces
        regionSplit2D
        (
            const polyMesh& mesh,
            const indirectPrimitivePatch& patch,
            const boolList& blockedFaces,
            const label offset = 0
        );


    //- Destructor
    ~regionSplit2D();


    // Member Functions

        //- Return the global number of regions
        label nRegions() const
        {
            return nRegions_;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
