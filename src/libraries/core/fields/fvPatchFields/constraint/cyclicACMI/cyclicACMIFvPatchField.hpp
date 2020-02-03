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
    CML::cyclicACMIFvPatchField

Description
    This boundary condition enforces a cyclic condition between a pair of
    boundaries, whereby communication between the patches is performed using
    an arbitrarily coupled mesh interface (ACMI) interpolation.

Usage
    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            cyclicACMI;
    }
    \endverbatim

See also
    CML::AMIInterpolation


\*---------------------------------------------------------------------------*/

#ifndef cyclicACMIFvPatchField_H
#define cyclicACMIFvPatchField_H

#include "cyclicAMIFvPatchField.hpp"
#include "cyclicACMIFvPatch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                   Class cyclicACMIFvPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class cyclicACMIFvPatchField
:
    public cyclicAMIFvPatchField<Type>
{
public:

    //- Runtime type information
    TypeName(cyclicACMIFvPatch::typeName_());


    // Constructors

        //- Inherit parent constructors
        using cyclicAMIFvPatchField<Type>::cyclicAMIFvPatchField;

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type>> clone() const
        {
            return tmp<fvPatchField<Type>>
            (
                new cyclicACMIFvPatchField<Type>(*this)
            );
        }

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchField<Type>> clone
        (
            const DimensionedField<Type, volMesh>& iF
        ) const
        {
            return tmp<fvPatchField<Type>>
            (
                new cyclicACMIFvPatchField<Type>(*this, iF)
            );
        }


    // Member functions

        // Access

            //- Return the fv patch
            const cyclicACMIFvPatch& cyclicACMIPatch() const
            {
                return refCast<const cyclicACMIFvPatch>(this->patch());
            }


        // Evaluation functions

            //- Return true if this patch field fixes a value
            //  Needed to check if a level has to be specified while solving
            //  Poissons equations
            virtual bool fixesValue() const
            {
                const scalarField& mask =
                    cyclicACMIPatch().cyclicACMIPatch().mask();

                if (gMax(mask) > 1e-5)
                {
                    // regions connected
                    return false;
                }
                else
                {
                    // fully separated
                    return nonOverlapPatchField().fixesValue();
                }
            }

            //- Return reference to non-overlapping patchField
            const fvPatchField<Type>& nonOverlapPatchField() const;

            //- Manipulate matrix
            virtual void manipulateMatrix(fvMatrix<Type>& matrix);

            //- Update the coefficients associated with the patch field
            virtual void updateCoeffs();
};


} // End namespace CML


#include "transformField.hpp"


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
const CML::fvPatchField<Type>&
CML::cyclicACMIFvPatchField<Type>::nonOverlapPatchField() const
{
    const GeometricField<Type, fvPatchField, volMesh>& fld =
        static_cast<const GeometricField<Type, fvPatchField, volMesh>&>
        (
            this->internalField()
        );

    return fld.boundaryField()
    [
        cyclicACMIPatch().cyclicACMIPatch().nonOverlapPatchID()
    ];
}


template<class Type>
void CML::cyclicACMIFvPatchField<Type>::manipulateMatrix
(
    fvMatrix<Type>& matrix
)
{
    const scalarField& mask = cyclicACMIPatch().cyclicACMIPatch().mask();

    // nothing to be done by the AMI, but re-direct to non-overlap patch
    // with non-overlap patch weights
    const fvPatchField<Type>& npf = nonOverlapPatchField();

    const_cast<fvPatchField<Type>&>(npf).manipulateMatrix(matrix, 1.0 - mask);
}


template<class Type>
void CML::cyclicACMIFvPatchField<Type>::updateCoeffs()
{
    // Update non-overlap patch - some will implement updateCoeffs, and
    // others will implement evaluate

    // Pass in (1 - mask) to give non-overlap patch the chance to do
    // manipulation of non-face based data

    const scalarField& mask = cyclicACMIPatch().cyclicACMIPatch().mask();
    const fvPatchField<Type>& npf = nonOverlapPatchField();
    const_cast<fvPatchField<Type>&>(npf).updateWeightedCoeffs(1.0 - mask);
}


#endif
