/*---------------------------------------------------------------------------*\
Copyright (C) 2014 Applied CCM
Copyright (C) 2011 OpenFOAM Foundation
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
    CML::fixedGradientFvPatchField

Description
    CML::fixedGradientFvPatchField


\*---------------------------------------------------------------------------*/

#ifndef fixedGradientFvPatchField_H
#define fixedGradientFvPatchField_H

#include "fvPatchField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class fixedGradientFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class fixedGradientFvPatchField
:
    public fvPatchField<Type>
{
    // Private data

        Field<Type> gradient_;


public:

    //- Runtime type information
    TypeName("fixedGradient");


    // Constructors

        //- Construct from patch and internal field
        fixedGradientFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        fixedGradientFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping the given fixedGradientFvPatchField
        //  onto a new patch
        fixedGradientFvPatchField
        (
            const fixedGradientFvPatchField<Type>&,
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        fixedGradientFvPatchField
        (
            const fixedGradientFvPatchField<Type>&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type>> clone() const
        {
            return tmp<fvPatchField<Type>>
            (
                new fixedGradientFvPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        fixedGradientFvPatchField
        (
            const fixedGradientFvPatchField<Type>&,
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
                new fixedGradientFvPatchField<Type>(*this, iF)
            );
        }


    // Member functions

            virtual bool fixesGradient() const
            {
                return true;
            }

        // Return defining fields

            //- Return gradient at boundary
            virtual Field<Type>& gradient()
            {
                return gradient_;
            }

            virtual const Field<Type>& gradient() const
            {
                return gradient_;
            }


        // Mapping functions

            //- Map (and resize as needed) from self given a mapping object
            virtual void autoMap
            (
                const fvPatchFieldMapper&
            );

            //- Reverse map the given fvPatchField onto this fvPatchField
            virtual void rmap
            (
                const fvPatchField<Type>&,
                const labelList&
            );


        // Evaluation functions

            //- Return gradient at boundary
            virtual tmp<Field<Type>> snGrad() const
            {
                return gradient_;
            }

            //- Evaluate the patch field
            virtual void evaluate
            (
                const Pstream::commsTypes commsType=Pstream::blocking
            );

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type>> valueInternalCoeffs
            (
                const tmp<scalarField>&
            ) const;

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type>> valueBoundaryCoeffs
            (
                const tmp<scalarField>&
            ) const;

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            virtual tmp<Field<Type>> gradientInternalCoeffs() const;

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            virtual tmp<Field<Type>> gradientBoundaryCoeffs() const;


        //- Write
        virtual void write(Ostream&) const;
};


} // End namespace CML


#include "dictionary.hpp"


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
CML::fixedGradientFvPatchField<Type>::fixedGradientFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    fvPatchField<Type>(p, iF),
    gradient_(p.size(), Zero)
{}


template<class Type>
CML::fixedGradientFvPatchField<Type>::fixedGradientFvPatchField
(
    const fixedGradientFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fvPatchField<Type>(ptf, p, iF, mapper),
    gradient_(ptf.gradient_, mapper)
{}


template<class Type>
CML::fixedGradientFvPatchField<Type>::fixedGradientFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fvPatchField<Type>(p, iF, dict),
    gradient_("gradient", dict, p.size())
{
    evaluate();
}


template<class Type>
CML::fixedGradientFvPatchField<Type>::fixedGradientFvPatchField
(
    const fixedGradientFvPatchField<Type>& ptf
)
:
    fvPatchField<Type>(ptf),
    gradient_(ptf.gradient_)
{}


template<class Type>
CML::fixedGradientFvPatchField<Type>::fixedGradientFvPatchField
(
    const fixedGradientFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fvPatchField<Type>(ptf, iF),
    gradient_(ptf.gradient_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void CML::fixedGradientFvPatchField<Type>::autoMap
(
    const fvPatchFieldMapper& m
)
{
    fvPatchField<Type>::autoMap(m);
    gradient_.autoMap(m);
}


template<class Type>
void CML::fixedGradientFvPatchField<Type>::rmap
(
    const fvPatchField<Type>& ptf,
    const labelList& addr
)
{
    fvPatchField<Type>::rmap(ptf, addr);

    const fixedGradientFvPatchField<Type>& fgptf =
        refCast<const fixedGradientFvPatchField<Type>>(ptf);

    gradient_.rmap(fgptf.gradient_, addr);
}


template<class Type>
void CML::fixedGradientFvPatchField<Type>::evaluate(const Pstream::commsTypes)
{
    if (!this->updated())
    {
        this->updateCoeffs();
    }

    Field<Type>::operator=
    (
        this->patchInternalField() + gradient_/this->patch().deltaCoeffs()
    );

    fvPatchField<Type>::evaluate();
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::fixedGradientFvPatchField<Type>::valueInternalCoeffs
(
    const tmp<scalarField>&
) const
{
    return tmp<Field<Type>>(new Field<Type>(this->size(), pTraits<Type>::one));
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::fixedGradientFvPatchField<Type>::valueBoundaryCoeffs
(
    const tmp<scalarField>&
) const
{
    return gradient()/this->patch().deltaCoeffs();
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::fixedGradientFvPatchField<Type>::
gradientInternalCoeffs() const
{
    return tmp<Field<Type>>
    (
        new Field<Type>(this->size(), Zero)
    );
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::fixedGradientFvPatchField<Type>::
gradientBoundaryCoeffs() const
{
    return gradient();
}


template<class Type>
void CML::fixedGradientFvPatchField<Type>::write(Ostream& os) const
{
    fvPatchField<Type>::write(os);
    gradient_.writeEntry("gradient", os);
}


#endif
