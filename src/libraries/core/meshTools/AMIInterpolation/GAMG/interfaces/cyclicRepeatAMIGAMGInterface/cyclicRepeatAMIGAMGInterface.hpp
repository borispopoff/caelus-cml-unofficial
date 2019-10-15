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
    CML::cyclicRepeatAMIGAMGInterface

Description
    GAMG agglomerated repeat AMI interface.

SourceFiles
    cyclicRepeatAMIGAMGInterface.cpp

\*---------------------------------------------------------------------------*/

#ifndef cyclicRepeatAMIGAMGInterface_HPP
#define cyclicRepeatAMIGAMGInterface_HPP

#include "cyclicAMIGAMGInterface.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                  Class cyclicRepeatAMIGAMGInterface Declaration
\*---------------------------------------------------------------------------*/

class cyclicRepeatAMIGAMGInterface
:
    public cyclicAMIGAMGInterface
{
public:

    //- Runtime type information
    TypeName("cyclicRepeatAMI");


    // Constructors

        //- Construct from fine level interface,
        //  local and neighbour restrict addressing
        cyclicRepeatAMIGAMGInterface
        (
            const label index,
            const lduInterfacePtrsList& coarseInterfaces,
            const lduInterface& fineInterface,
            const labelField& restrictAddressing,
            const labelField& neighbourRestrictAddressing
        );


    //- Destructor
    virtual ~cyclicRepeatAMIGAMGInterface();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
