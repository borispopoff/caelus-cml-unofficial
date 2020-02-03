/*---------------------------------------------------------------------------*\
Copyright (C) 2018 OpenFOAM Foundation
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
    CML::cyclicRepeatAMIPointPatch

Description
    Repeat AMI point patch - place holder only

SourceFiles
    cyclicRepeatAMIPointPatch.cpp

\*---------------------------------------------------------------------------*/

#ifndef cyclicRepeatAMIPointPatch_HPP
#define cyclicRepeatAMIPointPatch_HPP

#include "cyclicAMIPointPatch.hpp"
#include "cyclicRepeatAMIPolyPatch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                      Class cyclicRepeatAMIPointPatch Declaration
\*---------------------------------------------------------------------------*/

class cyclicRepeatAMIPointPatch
:
    public cyclicAMIPointPatch
{
public:

    //- Runtime type information
    TypeName(cyclicRepeatAMIPolyPatch::typeName_());


    // Constructors

        //- Construct from components
        cyclicRepeatAMIPointPatch
        (
            const polyPatch& patch,
            const pointBoundaryMesh& bm
        );


    //- Destructor
    virtual ~cyclicRepeatAMIPointPatch();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
