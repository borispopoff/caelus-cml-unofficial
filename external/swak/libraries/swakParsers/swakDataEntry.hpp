/*---------------------------------------------------------------------------*\
Copyright: ICE Stroemungsfoschungs GmbH
Copyright (C) 1991-2008 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is based on CAELUS.

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
    CML::swakDataEntry

Description

SourceFiles
    swakDataEntry.cpp

\*---------------------------------------------------------------------------*/

#ifndef swakDataEntry_H
#define swakDataEntry_H

#include "CommonValueExpressionDriver.hpp"
#include "exprString.hpp"

#include "DataEntry.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

namespace DataEntryTypes
{

template<class Type>
class swakDataEntry;

template<class Type>
Ostream& operator<<(Ostream&, const swakDataEntry<Type>&);

/*---------------------------------------------------------------------------*\
                           Class swakDataEntry Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class swakDataEntry
:
    public DataEntry<Type>
{
    // Private data

    //- the dictionary with the data (stored for rewriting)
    dictionary data_;

    //- the actual driver for the Expressions
    autoPtr<CommonValueExpressionDriver> driver_;

    //- the expression that should be evaluated
    exprString expression_;

    //- name of the independent variable
    word independentVariableName_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const swakDataEntry<Type>&);


    //- get (and set if necessary) the driver
    CommonValueExpressionDriver &driver();

public:

    // Runtime type information
    TypeName("swak");


    // Constructors

        //- Construct from entry name and Istream
        swakDataEntry(const word& entryName, const dictionary& dict);

        //- Copy constructor
        swakDataEntry(const swakDataEntry<Type>& cnst);

        //- Construct and return a clone
        virtual tmp<DataEntry<Type>> clone() const
        {
            return tmp<DataEntry<Type>>(new swakDataEntry<Type>(*this));
        }


    //- Destructor
    virtual ~swakDataEntry();


    // Member Functions

        //- Return constant value
        Type value(const scalar) const;

        //- Integrate between two values
        Type integrate(const scalar x1, const scalar x2) const;

        //- Return value as a function of (scalar) independent variable
        tmp<Field<Type>> value(const scalarField& x) const;

        //- Integrate between two (scalar) values
        tmp<Field<Type>> integrate
        (
            const scalarField& x1,
            const scalarField& x2
        ) const;

    // I/O

        //- Ostream Operator
        friend Ostream& operator<< <Type>
        (
            Ostream& os,
            const swakDataEntry<Type>& cnst
        );

        //- Write in dictionary format
        virtual void writeData(Ostream& os) const;

};


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
swakDataEntry<Type>::swakDataEntry(const word& entryName, const dictionary& dict)
:
    DataEntry<Type>(entryName)
{
    Istream& is(dict.lookup(entryName));
    word entryType(is);

    data_.read(is);

    expression_=exprString(
        data_.lookup("expression"),
        data_
    );
    independentVariableName_=word(data_.lookup("independentVariableName"));
}


template<class Type>
swakDataEntry<Type>::swakDataEntry(const swakDataEntry<Type>& cnst)
:
    DataEntry<Type>(cnst),
    data_(cnst.data_),
    //    driver_(cnst.driver_->clone()),
    expression_(cnst.expression_),
    independentVariableName_(cnst.independentVariableName_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
swakDataEntry<Type>::~swakDataEntry()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
CommonValueExpressionDriver &swakDataEntry<Type>::driver()
{
    if(!driver_.valid()) {
        driver_=CommonValueExpressionDriver::New(
            data_
        );
    }

    return driver_();
}

template<class Type>
Type swakDataEntry<Type>::value(const scalar x) const
{
    CommonValueExpressionDriver &theDriver=const_cast<swakDataEntry<Type> &>(
        *this
    ).driver();

    theDriver.clearVariables();
    theDriver.addUniformVariable(
        independentVariableName_,
        x
    );

    return theDriver.evaluateUniform<Type>(expression_);
}

template<class Type>
Type swakDataEntry<Type>::integrate(const scalar x1,const scalar x2) const
{
    CommonValueExpressionDriver &theDriver=const_cast<swakDataEntry<Type> &>(
        *this
    ).driver();

    theDriver.clearVariables();
    label intervalls=readLabel(data_.lookup("integrationIntervalls"));
    scalar dx=(x2-x1)/intervalls;

    scalar x=x1;
    theDriver.addUniformVariable(
        independentVariableName_,
        x
    );
    Type valOld=theDriver.evaluateUniform<Type>(expression_);
    Type result=pTraits<Type>::zero;
    while((x+SMALL)<x2) {
        x+=dx;
        theDriver.addUniformVariable(
            independentVariableName_,
            x
        );
        Type valNew=theDriver.evaluateUniform<Type>(expression_);
        result+=0.5*(valOld+valNew)*dx;
        valOld=valNew;
    }
    // Info << "Integrate " << expression_ << " from " << x1 << " to "
    //     << x2 << " -> " << result << endl;

    return result;
}


template<class Type>
tmp<Field<Type>> swakDataEntry<Type>::value
(
    const scalarField& x
) const
{
    tmp<Field<Type>> tfld(new Field<Type>(x.size()));
    Field<Type>& fld = const_cast<Field<Type>&>(tfld());

    forAll(x, i)
    {
        fld[i] = this->value(x[i]);
    }
    return tfld;
}


template<class Type>
tmp<Field<Type>> swakDataEntry<Type>::integrate
(
    const scalarField& x1,
    const scalarField& x2
) const
{
    tmp<Field<Type>> tfld(new Field<Type>(x1.size()));
    Field<Type>& fld = const_cast<Field<Type>&>(tfld());

    forAll(x1, i)
    {
        fld[i] = this->integrate(x1[i], x2[i]);
    }
    return tfld;
}

// * * * * * * * * * * * * * *  IOStream operators * * * * * * * * * * * * * //


template<class Type>
Ostream& operator<<
(
    Ostream& os,
    const swakDataEntry<Type>& cnst
)
{
    if (os.format() == IOstream::ASCII)
    {
        os  << static_cast<const DataEntry<Type>& >(cnst)
            << token::SPACE;
    }
    else
    {
        os  << static_cast<const DataEntry<Type>& >(cnst);
    }
    cnst.data_.write(os,true);

    // Check state of Ostream
    os.check
    (
        "Ostream& operator<<(Ostream&, const swakDataEntry<Type>&)"
    );

    return os;
}

template<class Type>
void swakDataEntry<Type>::writeData(Ostream& os) const
{
    DataEntry<Type>::writeData(os);

    os  << token::SPACE;
    data_.write(os,true);
    os  << token::END_STATEMENT << nl;
}

} // End namespace DataEntryTypes
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#endif

// ************************************************************************* //
