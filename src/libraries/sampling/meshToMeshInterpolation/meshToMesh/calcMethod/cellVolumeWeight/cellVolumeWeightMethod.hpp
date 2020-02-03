/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2019 OpenFOAM Foundation
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

Class
    CML::cellVolumeWeightMethod

Description
    Cell-volume-weighted mesh-to-mesh interpolation class

    Volume conservative.

SourceFiles
    cellVolumeWeightMethod.cpp

\*---------------------------------------------------------------------------*/

#ifndef cellVolumeWeightMethod_H
#define cellVolumeWeightMethod_H

#include "meshToMeshMethod.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                   Class cellVolumeWeightMethod Declaration
\*---------------------------------------------------------------------------*/

class cellVolumeWeightMethod
:
    public meshToMeshMethod
{
protected:

    // Protected Member Functions

        //- Find indices of overlapping cells in src and tgt meshes - returns
        //  true if found a matching pair
        bool findInitialSeeds
        (
            const labelList& srcCellIDs,
            const boolList& mapFlag,
            const label startSeedI,
            label& srcSeedI,
            label& tgtSeedI
        ) const;

        //- Calculate the mesh-to-mesh addressing and weights
        void calculateAddressing
        (
            labelListList& srcToTgtCellAddr,
            scalarListList& srcToTgtCellWght,
            labelListList& tgtToSrcCellAddr,
            scalarListList& tgtToSrcCellWght,
            const label srcSeedI,
            const label tgtSeedI,
            const labelList& srcCellIDs,
            boolList& mapFlag,
            label& startSeedI
        );

        //- Set the next cells in the advancing front algorithm
        void setNextCells
        (
            label& startSeedI,
            label& srcCelli,
            label& tgtCelli,
            const labelList& srcCellIDs,
            const boolList& mapFlag,
            const DynamicList<label>& visitedCells,
            labelList& seedCells
        ) const;


private:

    // Private member functions

        //- Disallow default bitwise copy construct
        cellVolumeWeightMethod(const cellVolumeWeightMethod&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const cellVolumeWeightMethod&) = delete;


public:

    //- Run-time type information
    TypeName("cellVolumeWeight");

    //- Construct from source and target meshes
    cellVolumeWeightMethod(const polyMesh& src, const polyMesh& tgt);

    //- Destructor
    virtual ~cellVolumeWeightMethod();


    // Member Functions

        // Evaluate

            //- Calculate addressing and weights
            virtual void calculate
            (
                labelListList& srcToTgtAddr,
                scalarListList& srcToTgtWght,
                labelListList& tgtToTgtAddr,
                scalarListList& tgtToTgtWght
            );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
