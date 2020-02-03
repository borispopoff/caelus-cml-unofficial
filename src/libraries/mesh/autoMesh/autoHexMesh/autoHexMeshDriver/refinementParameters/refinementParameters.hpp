/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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
    CML::refinementParameters

Description
    Simple container to keep together refinement specific information.

SourceFiles
    refinementParameters.C

\*---------------------------------------------------------------------------*/

#ifndef refinementParameters_H
#define refinementParameters_H

#include "dictionary.hpp"
#include "pointField.hpp"
#include "Switch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Class forward declarations
class polyMesh;

/*---------------------------------------------------------------------------*\
                           Class refinementParameters Declaration
\*---------------------------------------------------------------------------*/

class refinementParameters
{
    // Private data

        //- Total number of cells
        const label maxGlobalCells_;

        //- Per processor max number of cells
        const label maxLocalCells_;

        //- When to stop refining
        const label minRefineCells_;

        //- Curvature
        scalar curvature_;

        //- Number of layers between different refinement levels
        const label nBufferLayers_;

        //- Areas to keep
        const pointField keepPoints_;

        //- FaceZone faces allowed which have owner and neighbour in same
        //  cellZone?
        Switch allowFreeStandingZoneFaces_;

        //- Allowed load unbalance
        scalar maxLoadUnbalance_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        refinementParameters(const refinementParameters&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const refinementParameters&) = delete;


public:

    // Constructors

        //- Construct from dictionary - old syntax
        refinementParameters(const dictionary& dict, const label dummy);

        //- Construct from dictionary - new syntax
        refinementParameters(const dictionary& dict);


    // Member Functions

        // Access

            //- Total number of cells
            label maxGlobalCells() const
            {
                return maxGlobalCells_;
            }

            //- Per processor max number of cells
            label maxLocalCells() const
            {
                return maxLocalCells_;
            }

            //- When to stop refining
            label minRefineCells() const
            {
                return minRefineCells_;
            }

            //- Curvature
            scalar curvature() const
            {
                return curvature_;
            }

            //- Number of layers between different refinement levels
            label nBufferLayers() const
            {
                return nBufferLayers_;
            }

            //- Areas to keep
            const pointField& keepPoints() const
            {
                return keepPoints_;
            }

            //- Are zone faces allowed only inbetween different cell zones
            //  or also just free standing?
            bool allowFreeStandingZoneFaces() const
            {
                return allowFreeStandingZoneFaces_;
            }

            //- Allowed load unbalance
            scalar maxLoadUnbalance() const
            {
                return maxLoadUnbalance_;
            }


        // Other

            //- Checks that cells are in mesh. Returns cells they are in.
            labelList findCells(const polyMesh&) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
