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
    CML::emptyFvPatchField

Description
    This boundary condition provides an 'empty' condition for reduced
    dimensions cases, i.e. 1- and 2-D geometries.  Apply this condition to
    patches whose normal is aligned to geometric directions that do not
    constitue solution directions.

Usage
    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            empty;
    }
    \endverbatim

SourceFiles
    emptyFvPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef emptyFvPatchField_H
#define emptyFvPatchField_H

#include "fvPatchField.hpp"
#include "emptyFvPatch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                         Class emptyFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class emptyFvPatchField
:
    public fvPatchField<Type>
{

public:

    //- Runtime type information
    TypeName(emptyFvPatch::typeName_());


    // Constructors

        //- Construct from patch and internal field
        emptyFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        emptyFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given emptyFvPatchField onto a new patch
        emptyFvPatchField
        (
            const emptyFvPatchField<Type>&,
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        emptyFvPatchField
        (
            const emptyFvPatchField<Type>&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type>> clone() const
        {
            return tmp<fvPatchField<Type>>
            (
                new emptyFvPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        emptyFvPatchField
        (
            const emptyFvPatchField<Type>&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchField<Type>> clone
        (
            const DimensionedField<Type, volMesh>& iF
        ) const
        {
            return tmp<fvPatchField<Type>>
            (
                new emptyFvPatchField<Type>(*this, iF)
            );
        }


    // Member functions

        // Mapping functions

            //- Map (and resize as needed) from self given a mapping object
            virtual void autoMap
            (
                const fvPatchFieldMapper&
            )
            {}

            //- Reverse map the given fvPatchField onto this fvPatchField
            virtual void rmap
            (
                const fvPatchField<Type>&,
                const labelList&
            )
            {}


        // Evaluation functions

            //- Update the coefficients associated with the patch field
            //  This only checks to see the case is actually 1D or 2D
            //  for which this boundary condition is valid
            void updateCoeffs();


            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type>> valueInternalCoeffs
            (
                const tmp<scalarField>&
            ) const
            {
                return tmp<Field<Type>>(new Field<Type>(0));
            }

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type>> valueBoundaryCoeffs
            (
                const tmp<scalarField>&
            ) const
            {
                return tmp<Field<Type>>(new Field<Type>(0));
            }

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            tmp<Field<Type>> gradientInternalCoeffs() const
            {
                return tmp<Field<Type>>(new Field<Type>(0));
            }

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            tmp<Field<Type>> gradientBoundaryCoeffs() const
            {
                return tmp<Field<Type>>(new Field<Type>(0));
            }
};


} // End namespace CML


#include "fvPatchFieldMapper.hpp"


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
CML::emptyFvPatchField<Type>::emptyFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    fvPatchField<Type>(p, iF, Field<Type>(0))
{}


template<class Type>
CML::emptyFvPatchField<Type>::emptyFvPatchField
(
    const emptyFvPatchField<Type>&,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper&
)
:
    fvPatchField<Type>(p, iF, Field<Type>(0))
{
    if (!isType<emptyFvPatch>(p))
    {
        FatalErrorInFunction
            << "\n    patch type '" << p.type()
            << "' not constraint type '" << typeName << "'"
            << "\n    for patch " << p.name()
            << " of field " << this->internalField().name()
            << " in file " << this->internalField().objectPath()
            << exit(FatalIOError);
    }
}


template<class Type>
CML::emptyFvPatchField<Type>::emptyFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fvPatchField<Type>(p, iF, Field<Type>(0))
{
    if (!isType<emptyFvPatch>(p))
    {
        FatalIOErrorInFunction(dict)
            << "\n    patch type '" << p.type()
            << "' not constraint type '" << typeName << "'"
            << "\n    for patch " << p.name()
            << " of field " << this->internalField().name()
            << " in file " << this->internalField().objectPath()
            << exit(FatalIOError);
    }
}


template<class Type>
CML::emptyFvPatchField<Type>::emptyFvPatchField
(
    const emptyFvPatchField<Type>& ptf
)
:
    fvPatchField<Type>
    (
        ptf.patch(),
        ptf.internalField(),
        Field<Type>(0)
    )
{}


template<class Type>
CML::emptyFvPatchField<Type>::emptyFvPatchField
(
    const emptyFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fvPatchField<Type>(ptf.patch(), iF, Field<Type>(0))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void CML::emptyFvPatchField<Type>::updateCoeffs()
{
    //- Check moved to checkMesh. Test here breaks down if multiple empty
    //  patches.
    //if
    //(
    //    this->patch().patch().size()
    //  % this->internalField().mesh().nCells()
    //)
    //{
    //    FatalErrorInFunction
    //        << "This mesh contains patches of type empty but is not"
    //        << "1D or 2D\n"
    //           "    by virtue of the fact that the number of faces of this\n"
    //           "    empty patch is not divisible by the number of cells."
    //        << exit(FatalError);
    //}
}


#endif
