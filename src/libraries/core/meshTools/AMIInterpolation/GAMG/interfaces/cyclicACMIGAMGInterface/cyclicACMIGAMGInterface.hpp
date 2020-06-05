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
    CML::cyclicACMIGAMGInterface

Description
    GAMG agglomerated cyclic ACMI interface.

SourceFiles
    cyclicACMIGAMGInterface.C

\*---------------------------------------------------------------------------*/

#ifndef cyclicACMIGAMGInterface_H
#define cyclicACMIGAMGInterface_H

#include "cyclicAMIGAMGInterface.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                   Class cyclicACMIGAMGInterface Declaration
\*---------------------------------------------------------------------------*/

class cyclicACMIGAMGInterface
:
    public cyclicAMIGAMGInterface
{
public:

    //- Runtime type information
    TypeName("cyclicACMI");


    // Constructors

        //- Construct from fine level interface,
        //  local and neighbour restrict addressing
        cyclicACMIGAMGInterface
        (
            const label index,
            const lduInterfacePtrsList& coarseInterfaces,
            const lduInterface& fineInterface,
            const labelField& restrictAddressing,
            const labelField& neighbourRestrictAddressing
        );


    //- Destructor
    virtual ~cyclicACMIGAMGInterface();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
