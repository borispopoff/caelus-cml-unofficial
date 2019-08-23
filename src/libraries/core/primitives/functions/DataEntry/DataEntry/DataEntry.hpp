/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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
    CML::DataEntry

Description
    Top level data entry class for use in dictionaries. Provides a mechanism
    to specify a variable as a certain type, e.g. constant or table, and
    provide functions to return the (interpolated) value, and integral between
    limits.


\*---------------------------------------------------------------------------*/

#ifndef DataEntry_HPP
#define DataEntry_HPP

#include "dictionary.hpp"
#include "Field.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declarations
class Time;

// Forward declaration of friend functions and operators
template<class Type> class DataEntry;
template<class Type> Ostream& operator<<(Ostream&, const DataEntry<Type>&);

/*---------------------------------------------------------------------------*\
                         Class DataEntry Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class DataEntry
:
    public refCount
{
    // Private Member Functions

        //- Disallow default bitwise assignment
        void operator=(const DataEntry<Type>&);


protected:

    // Protected data

        //- Name of entry
        const word name_;


public:

    typedef Type returnType;

    //- Runtime type information
    TypeName("DataEntry")

    //- Declare runtime constructor selection table
    declareRunTimeSelectionTable
    (
        autoPtr,
        DataEntry,
        dictionary,
        (
            const word& entryName,
            const dictionary& dict
        ),
        (entryName, dict)
    );


    // Constructors

        //- Construct from entry name
        DataEntry(const word& entryName);

        //- Copy constructor
        DataEntry(const DataEntry<Type>& de);

        //- Construct and return a clone
        virtual tmp<DataEntry<Type> > clone() const = 0;


    //- Selector
    static autoPtr<DataEntry<Type> > New
    (
        const word& entryName,
        const dictionary& dict
    );


    //- Destructor
    virtual ~DataEntry();


    // Member Functions

        // Access

            //- Return the name of the entry
            const word& name() const;


        // Manipulation

            //- Convert time
            virtual void convertTimeBase(const Time& t);


        // Evaluation

            //- Return value as a function of (scalar) independent variable
            virtual Type value(const scalar x) const;

            //- Return value as a function of (scalar) independent variable
            virtual tmp<Field<Type> > value(const scalarField& x) const = 0;

            //- Integrate between two (scalar) values
            virtual Type integrate(const scalar x1, const scalar x2) const;

            //- Integrate between two (scalar) values
            virtual tmp<Field<Type> > integrate
            (
                const scalarField& x1,
                const scalarField& x2
            ) const = 0;


        // I/O

            //- Ostream Operator
            friend Ostream& operator<< <Type>
            (
                Ostream& os,
                const DataEntry<Type>& de
            );

            //- Write in dictionary format
            virtual void writeData(Ostream& os) const;
};


/*---------------------------------------------------------------------------*\
                        Class FieldDataEntry Declaration
\*---------------------------------------------------------------------------*/

template<class DataEntryType>
class FieldDataEntry
:
    public DataEntryType
{

public:

    typedef typename DataEntryType::returnType Type;


    // Constructors

        //- Construct from entry name and dictionary
        FieldDataEntry(const word& entryName, const dictionary& dict);

        //- Construct and return a clone
        virtual tmp<DataEntry<Type> > clone() const;


    //- Destructor
    virtual ~FieldDataEntry()
    {}


    // Member Functions

        // Evaluation

            //- Return value as a function of (scalar) independent variable
            virtual tmp<Field<Type> > value(const scalarField& x) const;

            //- Integrate between two (scalar) values
            virtual tmp<Field<Type> > integrate
            (
                const scalarField& x1,
                const scalarField& x2
            ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeDataEntry(Type)                                                    \
                                                                               \
    defineNamedTemplateTypeNameAndDebug(DataEntry<Type>, 0);                   \
                                                                               \
    defineTemplateRunTimeSelectionTable                                        \
    (                                                                          \
        DataEntry<Type>,                                                       \
        dictionary                                                             \
    );


#define makeDataEntryType(SS, Type)                                            \
                                                                               \
    defineNamedTemplateTypeNameAndDebug(DataEntryTypes::SS<Type>, 0);          \
                                                                               \
    DataEntry<Type>::adddictionaryConstructorToTable                           \
        <FieldDataEntry<DataEntryTypes::SS<Type> > >                           \
        add##SS##Type##ConstructorToTable_;


#define makeScalarDataEntry(SS)                                                \
                                                                               \
    defineTypeNameAndDebug(SS, 0);                                             \
                                                                               \
    DataEntry<scalar>::adddictionaryConstructorToTable<FieldDataEntry<SS> >    \
        add##SS##ConstructorToTable_;

// * * * * * * * * * * * * * * * * Constructor * * * * * * * * * * * * * * * //

template<class Type>
CML::DataEntry<Type>::DataEntry(const word& entryName)
:
    name_(entryName)
{}


template<class Type>
CML::DataEntry<Type>::DataEntry(const DataEntry<Type>& de)
:
    refCount(),
    name_(de.name_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
CML::DataEntry<Type>::~DataEntry()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
const CML::word& CML::DataEntry<Type>::name() const
{
    return name_;
}


template<class Type>
void CML::DataEntry<Type>::convertTimeBase(const Time&)
{}


template<class Type>
Type CML::DataEntry<Type>::value(const scalar x) const
{
    FatalErrorInFunction
        << "Evaluation is not defined for " << type() << " functions"
        << exit(FatalError);

    return Zero;
}


template<class Type>
Type CML::DataEntry<Type>::integrate(const scalar x1, const scalar x2) const
{
    FatalErrorInFunction
        << "Integration is not defined for " << type() << " functions"
        << exit(FatalError);

    return Zero;
}


template<class DataEntryType>
CML::tmp<CML::Field<typename DataEntryType::returnType> >
CML::FieldDataEntry<DataEntryType>::value
(
    const scalarField& x
) const
{
    tmp<Field<Type> > tfld(new Field<Type>(x.size()));
    Field<Type>& fld = tfld();

    forAll(x, i)
    {
        fld[i] = DataEntryType::value(x[i]);
    }
    return tfld;
}


template<class DataEntryType>
CML::FieldDataEntry<DataEntryType>::FieldDataEntry
(
    const word& entryName,
    const dictionary& dict
)
:
    DataEntryType(entryName, dict)
{}


template<class DataEntryType>
CML::tmp<CML::DataEntry<typename DataEntryType::returnType> >
CML::FieldDataEntry<DataEntryType>::clone() const
{
    return tmp<DataEntry<Type> >
    (
        new FieldDataEntry<DataEntryType>(*this)
    );
}


template<class DataEntryType>
CML::tmp<CML::Field<typename DataEntryType::returnType> > 
CML::FieldDataEntry<DataEntryType>::integrate
(
    const scalarField& x1,
    const scalarField& x2
) const
{
    tmp<Field<Type> > tfld(new Field<Type>(x1.size()));
    Field<Type>& fld = tfld();

    forAll(x1, i)
    {
        fld[i] = DataEntryType::integrate(x1[i], x2[i]);
    }

    return tfld;
}


template<class Type>
void CML::DataEntry<Type>::writeData(Ostream& os) const
{
    os.writeKeyword(name_) << type();
}


// * * * * * * * * * * * * * *  IOStream operators * * * * * * * * * * * * * //

template<class Type>
CML::Ostream& CML::operator<<
(
    Ostream& os,
    const DataEntry<Type>& f1
)
{
    // Check state of Ostream
    os.check
    (
        "Ostream& operator<<(Ostream&, const DataEntry<Type>&)"
    );

    os  << f1.name_;
    f1.writeData(os);

    return os;
}


// ************************************************************************* //

#include "Constant.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
