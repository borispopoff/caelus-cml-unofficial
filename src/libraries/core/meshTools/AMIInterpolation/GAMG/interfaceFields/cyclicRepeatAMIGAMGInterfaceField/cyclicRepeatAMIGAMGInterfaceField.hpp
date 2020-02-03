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
    CML::cyclicRepeatAMIGAMGInterfaceField

Description
    GAMG agglomerated repeat AMI interface field.

SourceFiles
    cyclicRepeatAMIGAMGInterfaceField.C

\*---------------------------------------------------------------------------*/

#ifndef cyclicRepeatAMIGAMGInterfaceField_HPP
#define cyclicRepeatAMIGAMGInterfaceField_HPP

#include "cyclicAMIGAMGInterfaceField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                 Class cyclicRepeatAMIGAMGInterfaceField Declaration
\*---------------------------------------------------------------------------*/

class cyclicRepeatAMIGAMGInterfaceField
:
    public cyclicAMIGAMGInterfaceField
{
public:

    //- Runtime type information
    TypeName("cyclicRepeatAMI");


    // Constructors

        //- Construct from GAMG interface and fine level interface field
        cyclicRepeatAMIGAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            const lduInterfaceField& fineInterfaceField
        );

        //- Construct from GAMG interface and fine level interface field
        cyclicRepeatAMIGAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            const bool doTransform,
            const int rank
        );


    //- Destructor
    virtual ~cyclicRepeatAMIGAMGInterfaceField();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
