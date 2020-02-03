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
    CML::freestreamPressureFvPatchScalarField

Description
    This boundary condition provides a free-stream condition for pressure.
    It is a zero-gradient condition that constrains the flux across the patch
    based on the free-stream velocity.

Usage
    \table
        Property     | Description             | Required    | Default value
        U            | velocity field name     | no          | U
        phi          | flux field name         | no          | phi
        rho          | density field name      | no          | none
    \endtable

    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            freestreamPressure;
    }
    \endverbatim

Note
    This condition is designed to operate with a freestream velocity condition

See also
    CML::zeroGradientFvPatchField
    CML::freestreamFvPatchField

SourceFiles
    freestreamPressureFvPatchScalarField.cpp

\*---------------------------------------------------------------------------*/

#ifndef freestreamPressureFvPatchScalarFields_H
#define freestreamPressureFvPatchScalarFields_H

#include "fvPatchFields.hpp"
#include "zeroGradientFvPatchFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
             Class freestreamPressureFvPatchScalarField Declaration
\*---------------------------------------------------------------------------*/

class freestreamPressureFvPatchScalarField
:
    public zeroGradientFvPatchScalarField
{
    // Private data

        //- Name of the velocity field
        word UName_;

        //- Name of the flux transporting the field
        word phiName_;

        //- Name of the density field used to normalise the mass flux
        //  if neccessary
        word rhoName_;


public:

    //- Runtime type information
    TypeName("freestreamPressure");


    // Constructors

        //- Construct from patch and internal field
        freestreamPressureFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        freestreamPressureFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given freestreamPressureFvPatchScalarField onto
        //  a new patch
        freestreamPressureFvPatchScalarField
        (
            const freestreamPressureFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        freestreamPressureFvPatchScalarField
        (
            const freestreamPressureFvPatchScalarField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchScalarField> clone() const
        {
            return tmp<fvPatchScalarField>
            (
                new freestreamPressureFvPatchScalarField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        freestreamPressureFvPatchScalarField
        (
            const freestreamPressureFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new freestreamPressureFvPatchScalarField(*this, iF)
            );
        }


    // Member functions

        // Evaluation functions

            //- Update the coefficients associated with the patch field
            virtual void updateCoeffs();
};


} // End namespace CML


#endif
