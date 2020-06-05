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
    CML::cyclicRepeatAMILduInterfaceField

Description
    Abstract base class for overlapping AMI coupled interfaces

SourceFiles
    cyclicRepeatAMILduInterfaceField.cpp

\*---------------------------------------------------------------------------*/

#ifndef cyclicRepeatAMILduInterfaceField_HPP
#define cyclicRepeatAMILduInterfaceField_HPP

#include "cyclicAMILduInterfaceField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                  Class cyclicRepeatAMILduInterfaceField Declaration
\*---------------------------------------------------------------------------*/

class cyclicRepeatAMILduInterfaceField
:
    public cyclicAMILduInterfaceField
{

public:

    //- Runtime type information
    TypeName("cyclicRepeatAMILduInterfaceField");


    // Constructors

        //- Construct null
        cyclicRepeatAMILduInterfaceField()
        {}


    //- Destructor
    virtual ~cyclicRepeatAMILduInterfaceField();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
