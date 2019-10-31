/*---------------------------------------------------------------------------*\
Copyright (C) 2015-2019 OpenFOAM Foundation
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
    Foam::pisoControl

Description
    Specialization of the pimpleControl class for PISO control.

\*---------------------------------------------------------------------------*/

#ifndef pisoControl_HPP
#define pisoControl_HPP

#include "pimpleControl.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                        Class pisoControl Declaration
\*---------------------------------------------------------------------------*/

class pisoControl
:
    public pimpleControl
{
    // Private member functions

        //- Disallow default bitwise copy construct
        pisoControl(const pisoControl&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const pisoControl&) = delete;


public:

    // Static Data Members

        //- Run-time type information
        TypeName("pisoControl");


    // Constructors

        //- Construct from mesh and the name of control sub-dictionary
        pisoControl(fvMesh& mesh, const word& dictName="PISO");


    //- Destructor
    virtual ~pisoControl();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
