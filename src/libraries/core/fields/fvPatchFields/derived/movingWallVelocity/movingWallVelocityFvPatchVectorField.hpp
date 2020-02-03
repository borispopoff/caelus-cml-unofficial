/*---------------------------------------------------------------------------*\
Copyright (C) 2014 Applied CCM
Copyright (C) 2011-2016 OpenFOAM Foundation
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
    CML::movingWallVelocityFvPatchVectorField

Description
    This boundary condition provides a velocity condition for cases with
    moving walls.  In addition, it should also be applied to 'moving' walls
    for moving reference frame (MRF) calculations.

Usage
    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            movingWallVelocity;
        value           uniform (0 0 0);    // Initial value
    }
    \endverbatim

SeeAlso
    CML::fixedValueFvPatchVectorField
    CML::MRFZone

SourceFiles
    movingWallVelocityFvPatchVectorField.cpp

\*---------------------------------------------------------------------------*/

#ifndef movingWallVelocityFvPatchVectorField_H
#define movingWallVelocityFvPatchVectorField_H

#include "fvPatchFields.hpp"
#include "fixedValueFvPatchFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
             Class movingWallVelocityFvPatchVectorField Declaration
\*---------------------------------------------------------------------------*/

class movingWallVelocityFvPatchVectorField
:
    public fixedValueFvPatchVectorField
{
    // Private data

       //- Name of velocity field
       word UName_;


public:

    //- Runtime type information
    TypeName("movingWallVelocity");


    // Constructors

        //- Construct from patch and internal field
        movingWallVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        movingWallVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given movingWallVelocityFvPatchVectorField
        //  onto a new patch
        movingWallVelocityFvPatchVectorField
        (
            const movingWallVelocityFvPatchVectorField&,
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        movingWallVelocityFvPatchVectorField
        (
            const movingWallVelocityFvPatchVectorField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchVectorField> clone() const
        {
            return tmp<fvPatchVectorField>
            (
                new movingWallVelocityFvPatchVectorField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        movingWallVelocityFvPatchVectorField
        (
            const movingWallVelocityFvPatchVectorField&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchVectorField> clone
        (
            const DimensionedField<vector, volMesh>& iF
        ) const
        {
            return tmp<fvPatchVectorField>
            (
                new movingWallVelocityFvPatchVectorField(*this, iF)
            );
        }


    // Member functions

        //- Update the coefficients associated with the patch field
        virtual void updateCoeffs();

        //- Write
        virtual void write(Ostream&) const;
};


} // End namespace CML


#endif
