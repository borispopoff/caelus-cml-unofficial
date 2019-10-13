/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
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
    CML::fv::explicitSetValue

Description
    Set values field values explicity.

    Sources described by:

    \verbatim
    <Type>ExplicitSetValueCoeffs
    {
        injectionRate
        {
            k           30.7;
            epsilon     1.5;
        }
    }
    \endverbatim

SeeAlso
    CML::fvOption

SourceFiles
    explicitSetValue.C

\*---------------------------------------------------------------------------*/

#ifndef ExplicitSetValue_H
#define ExplicitSetValue_H

#include "cellSetOption.hpp"
#include "Tuple2.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                       Class explicitSetValue Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class ExplicitSetValue
:
    public cellSetOption
{

protected:

    // Protected data

        //- Source value per field
        List<Type> injectionRate_;


    // Protected functions

        //- Set the local field data
        void setFieldData(const dictionary& dict);


public:

    //- Runtime type information
    TypeName("explicitSetValue");


    // Constructors

        //- Construct from components
        ExplicitSetValue
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Member Functions

        // Evaluation

            //- Set value on field
            virtual void constrain(fvMatrix<Type>& eqn, const label fieldI);


        // IO

            //- Read source dictionary
            virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "fvMesh.hpp"
#include "fvMatrices.hpp"
#include "DimensionedField.hpp"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class Type>
void CML::fv::ExplicitSetValue<Type>::setFieldData(const dictionary& dict)
{
    fieldNames_.setSize(dict.toc().size());
    injectionRate_.setSize(fieldNames_.size());

    applied_.setSize(fieldNames_.size(), false);

    label i = 0;
    forAllConstIter(dictionary, dict, iter)
    {
        fieldNames_[i] = iter().keyword();
        dict.lookup(iter().keyword()) >> injectionRate_[i];
        i++;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
CML::fv::ExplicitSetValue<Type>::ExplicitSetValue
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    cellSetOption(name, modelType, dict, mesh),
    injectionRate_()
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void CML::fv::ExplicitSetValue<Type>::constrain
(
    fvMatrix<Type>& eqn,
    const label fieldi
)
{
    if (debug)
    {
        Info<< "ExplicitSetValue<"<< pTraits<Type>::typeName
            << ">::constrain for source " << name_ << endl;
    }

    List<Type> values(cells_.size(), injectionRate_[fieldi]);

    eqn.setValues(cells_, values);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
bool CML::fv::ExplicitSetValue<Type>::read(const dictionary& dict)
{
    if (cellSetOption::read(dict))
    {
        setFieldData(coeffs_.subDict("injectionRate"));
        return true;
    }
    else
    {
        return false;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
