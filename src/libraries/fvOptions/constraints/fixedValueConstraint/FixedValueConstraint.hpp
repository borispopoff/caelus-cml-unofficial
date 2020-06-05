/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2017 OpenFOAM Foundation
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
    CML::fv::FixedValueConstraint

Description
    Constrain the field values within a specified region.

Usage
    For example to set the turbulence properties within a porous region:
    \verbatim
    porosityTurbulence
    {
        type            scalarFixedValueConstraint;
        active          yes;

        selectionMode   cellZone;
        cellZone        porosity;
        fieldValues
        {
            k           1;
            epsilon     150;
        }
    }
    \endverbatim

See also
    CML::fvOption

SourceFiles
    FixedValueConstraint.cpp
    fixedValueConstraints.cpp

\*---------------------------------------------------------------------------*/

#ifndef FixedValueConstraint_H
#define FixedValueConstraint_H

#include "cellSetOption.hpp"
#include "Tuple2.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                       Class FixedValueConstraint Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class FixedValueConstraint
:
    public cellSetOption
{
    // Private member data

        //- Field values
        List<Type> fieldValues_;


public:

    //- Runtime type information
    TypeName("FixedValueConstraint");


    // Constructors

        //- Construct from components
        FixedValueConstraint
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Member Functions

        //- Read source dictionary
        virtual bool read(const dictionary& dict);

        //- Set value on field
        virtual void constrain(fvMatrix<Type>& eqn, const label fieldi);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "fvMesh.hpp"
#include "fvMatrices.hpp"
#include "DimensionedField.hpp"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
CML::fv::FixedValueConstraint<Type>::FixedValueConstraint
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    cellSetOption(name, modelType, dict, mesh)
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
bool CML::fv::FixedValueConstraint<Type>::read(const dictionary& dict)
{
    if (cellSetOption::read(dict))
    {
        const dictionary& fieldValuesDict = coeffs_.subDict("fieldValues");

        fieldNames_.setSize(fieldValuesDict.size());
        fieldValues_.setSize(fieldNames_.size());

        label i = 0;
        forAllConstIter(dictionary, fieldValuesDict, iter)
        {
            fieldNames_[i] = iter().keyword();
            fieldValuesDict.lookup(iter().keyword()) >> fieldValues_[i];
            i++;
        }

        applied_.setSize(fieldNames_.size(), false);

        return true;
    }
    else
    {
        return false;
    }
}


template<class Type>
void CML::fv::FixedValueConstraint<Type>::constrain
(
    fvMatrix<Type>& eqn,
    const label fieldi
)
{
    if (debug)
    {
        Info<< "FixedValueConstraint<"
            << pTraits<Type>::typeName
            << ">::constrain for source " << name_ << endl;
    }
    eqn.setValues(cells_, List<Type>(cells_.size(), fieldValues_[fieldi]));
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
