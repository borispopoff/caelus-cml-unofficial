/*---------------------------------------------------------------------------*\
Copyright (C) 2010 I. Clifford
Copyright (C) 2017 Applied CCM Pty Ltd
-------------------------------------------------------------------------------
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    Caelus is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

Description
    This file was obtained from Ivor Clifford at the OpenFOAM Workshop
    in PennState, USA, 2011.

    The original file contained "Copyright held by original author",
    but no author was named in the original file. Authorship confirmed by
    Ivor Clifford (2017).

    Please report any omissions of authorship and/or copyright to
    info@appliedccm.com.au. Corrections will be made upon provision of proof.

Author
    Ivor Clifford
    Darrin Stephens

\*---------------------------------------------------------------------------*/

#include "wedgeFvPatchHyperVectorFields.hpp"
#include "addToRunTimeSelectionTable.hpp"


namespace CML
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

#define makeHyperVectorTensorWedgeFuncDefs(Type)                              \
template<>                                                                    \
tmp<Field<Type>> wedgeFvPatchField<Type>::snGrad() const                      \
{                                                                             \
    return tmp<Field<Type>>                                                   \
    (                                                                         \
        new Field<Type>(size(), pTraits<Type>::zero)                          \
    );                                                                        \
}                                                                             \
                                                                              \
                                                                              \
template<>                                                                    \
void wedgeFvPatchField<Type>::evaluate                                        \
(                                                                             \
    const Pstream::commsTypes commsType                                       \
)                                                                             \
{                                                                             \
    if (not updated())                                                        \
    {                                                                         \
        updateCoeffs();                                                       \
    }                                                                         \
                                                                              \
    this->operator==(patchInternalField());                                   \
}                                                                             \
                                                                              \
                                                                              \
template<>                                                                    \
tmp<Field<Type>> wedgeFvPatchField<Type>::snGradTransformDiag()               \
const                                                                         \
{                                                                             \
    return tmp<Field<Type>>                                                   \
    (                                                                         \
        new Field<Type>(this->size(), pTraits<Type>::zero)                    \
    );                                                                        \
}


#define doMakePatchTypeField(type, Type, args...)                             \
                                                                              \
makeHyperVectorTensorWedgeFuncDefs(type)                                      \
                                                                              \
makeTemplatePatchTypeField                                                    \
(                                                                             \
    fvPatch##Type##Field,                                                     \
    wedgeFvPatch##Type##Field                                                 \
);


forAllHyperVectorTypes(doMakePatchTypeField)

#undef doMakePatchTypeField

#undef makeHyperVectorTensorWedgeFuncDefs


} // End namespace CML
