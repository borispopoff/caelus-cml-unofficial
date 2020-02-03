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
    CML::cyclicRepeatAMIFvPatchField

Description
    This boundary condition enforces an repeating condition between a pair of
    boundaries, whereby communication between the patches is performed using
    an arbitrary mesh interface (AMI) interpolation.

Usage
    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            cyclicRepeatAMI;
    }
    \endverbatim

Note
    The outer boundary of the patch pairs must be similar, i.e. if the owner
    patch is transformed to the neighbour patch, the outer perimiter of each
    patch should be identical (or very similar).

See also
    CML::AMIInterpolation

SourceFiles
    cyclicRepeatAMIFvPatchField.cpp

\*---------------------------------------------------------------------------*/

#ifndef cyclicRepeatAMIFvPatchField_HPP
#define cyclicRepeatAMIFvPatchField_HPP

#include "cyclicAMIFvPatchField.hpp"
#include "cyclicRepeatAMIFvPatch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                    Class cyclicRepeatAMIFvPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class cyclicRepeatAMIFvPatchField
:
    public cyclicAMIFvPatchField<Type>
{
public:

    //- Runtime type information
    TypeName(cyclicRepeatAMIFvPatch::typeName_());


    // Constructors

        //- Inherit parent constructors
        using cyclicAMIFvPatchField<Type>::cyclicAMIFvPatchField;

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type>> clone() const
        {
            return tmp<fvPatchField<Type>>
            (
                new cyclicRepeatAMIFvPatchField<Type>(*this)
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
                new cyclicRepeatAMIFvPatchField<Type>(*this, iF)
            );
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
