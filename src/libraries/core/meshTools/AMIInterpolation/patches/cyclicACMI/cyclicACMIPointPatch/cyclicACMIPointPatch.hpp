/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2018 OpenFOAM Foundation
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
    CML::cyclicACMIPointPatch

Description
    Cyclic AMI point patch - place holder only

SourceFiles
    cyclicACMIPointPatch.C

\*---------------------------------------------------------------------------*/

#ifndef cyclicACMIPointPatch_H
#define cyclicACMIPointPatch_H

#include "cyclicAMIPointPatch.hpp"
#include "cyclicACMIPolyPatch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                    Class cyclicACMIPointPatch Declaration
\*---------------------------------------------------------------------------*/

class cyclicACMIPointPatch
:
    public cyclicAMIPointPatch
{
public:

    //- Runtime type information
    TypeName(cyclicACMIPolyPatch::typeName_());


    // Constructors

        //- Construct from components
        cyclicACMIPointPatch
        (
            const polyPatch& patch,
            const pointBoundaryMesh& bm
        );


    //- Destructor
    virtual ~cyclicACMIPointPatch();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
