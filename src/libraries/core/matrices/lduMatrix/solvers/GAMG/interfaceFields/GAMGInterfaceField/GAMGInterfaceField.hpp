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
    CML::GAMGInterfaceField

Description
     Abstract base class for GAMG agglomerated interface fields.

SourceFiles
    GAMGInterfaceField.cpp
    GAMGInterfaceFieldNew.cpp

\*---------------------------------------------------------------------------*/

#ifndef GAMGInterfaceField_H
#define GAMGInterfaceField_H

#include "lduInterfaceField.hpp"
#include "GAMGInterface.hpp"
#include "autoPtr.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                      Class GAMGInterfaceField Declaration
\*---------------------------------------------------------------------------*/

class GAMGInterfaceField
:
    public lduInterfaceField
{
    // Private data

        //- Local reference cast into the interface
        const GAMGInterface& interface_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        GAMGInterfaceField(const GAMGInterfaceField&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const GAMGInterfaceField&) = delete;


public:

    //- Runtime type information
    TypeName("GAMGInterfaceField");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            GAMGInterfaceField,
            lduInterfaceField,
            (
                const GAMGInterface& GAMGCp,
                const lduInterfaceField& fineInterface
            ),
            (GAMGCp, fineInterface)
        );

        declareRunTimeSelectionTable
        (
            autoPtr,
            GAMGInterfaceField,
            lduInterface,
            (
                const GAMGInterface& GAMGCp,
                const bool doTransform,
                const int rank
            ),
            (GAMGCp, doTransform, rank)
        );


    // Selectors

        //- Return a pointer to a new interface created on freestore given
        //  the fine interface
        static autoPtr<GAMGInterfaceField> New
        (
            const GAMGInterface& GAMGCp,
            const lduInterfaceField& fineInterface
        );

        //- Return a pointer to a new interface created on freestore given
        //  the fine interface
        static autoPtr<GAMGInterfaceField> New
        (
            const GAMGInterface& GAMGCp,
            const bool doTransform,
            const int rank
        );


    // Constructors

        //- Construct from GAMG interface and fine level interface field
        GAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            const lduInterfaceField&
        )
        :
            lduInterfaceField(GAMGCp),
            interface_(GAMGCp)
        {}

        //- Construct from GAMG interface and fine level interface field
        GAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            const bool doTransform,
            const int rank
        )
        :
            lduInterfaceField(GAMGCp),
            interface_(GAMGCp)
        {}


    // Member Functions

        // Access

            //- Return interface
            const GAMGInterface& interface() const
            {
                return interface_;
            }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
