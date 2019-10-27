/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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
    CML::IOField

Description
    A primitive field of type \<T\> with automated input and output.


\*---------------------------------------------------------------------------*/

#ifndef IOField_H
#define IOField_H

#include "regIOobject.hpp"
#include "Field.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class IOField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class IOField
:
    public regIOobject,
    public Field<Type>
{

public:

    TypeName("Field");


    // Constructors

        //- Default copy construct
        IOField(const IOField&) = default;

        //- Construct from IOobject
        IOField(const IOobject&);

        //- Construct from IOobject and size (does not set values)
        IOField(const IOobject&, const label size);

        //- Construct from components
        IOField(const IOobject&, const Field<Type>&);

        //- Move construct by transferring the Field contents
        IOField(const IOobject&, Field<Type>&&);

        //- Move construct by transferring the Field contents
        IOField(const IOobject&, const tmp<Field<Type>>&);

        //- Move constructor
        IOField(IOField<Type>&&);


    //- Destructor
    virtual ~IOField();


    // Member functions

        bool writeData(Ostream&) const;


    // Member operators

        void operator=(const IOField<Type>&);
        void operator=(IOField<Type>&&);

        void operator=(const Field<Type>&);
        void operator=(Field<Type>&&);
        void operator=(const tmp<Field<Type>>&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
CML::IOField<Type>::IOField(const IOobject& io)
:
    regIOobject(io)
{
    // Temporary warning
    if (io.readOpt() == IOobject::MUST_READ_IF_MODIFIED)
    {
        WarningInFunction
            << "IOField " << name()
            << " constructed with IOobject::MUST_READ_IF_MODIFIED"
            " but IOField does not support automatic rereading."
            << endl;
    }

    if
    (
        (
            io.readOpt() == IOobject::MUST_READ
         || io.readOpt() == IOobject::MUST_READ_IF_MODIFIED
        )
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readStream(typeName) >> *this;
        close();
    }
}


template<class Type>
CML::IOField<Type>::IOField(const IOobject& io, const label size)
:
    regIOobject(io)
{
    // Temporary warning
    if (io.readOpt() == IOobject::MUST_READ_IF_MODIFIED)
    {
        WarningInFunction
            << "IOField " << name()
            << " constructed with IOobject::MUST_READ_IF_MODIFIED"
            " but IOField does not support automatic rereading."
            << endl;
    }

    if
    (
        (
            io.readOpt() == IOobject::MUST_READ
         || io.readOpt() == IOobject::MUST_READ_IF_MODIFIED
        )
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readStream(typeName) >> *this;
        close();
    }
    else
    {
        Field<Type>::setSize(size);
    }
}


template<class Type>
CML::IOField<Type>::IOField(const IOobject& io, const Field<Type>& f)
:
    regIOobject(io),
    Field<Type>(f)
{
    // Temporary warning
    if (io.readOpt() == IOobject::MUST_READ_IF_MODIFIED)
    {
        WarningInFunction
            << "IOField " << name()
            << " constructed with IOobject::MUST_READ_IF_MODIFIED"
            " but IOField does not support automatic rereading."
            << endl;
    }

    if
    (
        (
            io.readOpt() == IOobject::MUST_READ
         || io.readOpt() == IOobject::MUST_READ_IF_MODIFIED
        )
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readStream(typeName) >> *this;
        close();
    }
}


template<class Type>
CML::IOField<Type>::IOField(const IOobject& io, Field<Type>&& f)
:
    regIOobject(io),
    Field<Type>(move(f))
{
    // Temporary warning
    if (io.readOpt() == IOobject::MUST_READ_IF_MODIFIED)
    {
        WarningInFunction
            << "IOField " << name()
            << " constructed with IOobject::MUST_READ_IF_MODIFIED"
            " but IOField does not support automatic rereading."
            << endl;
    }

    if
    (
        (
            io.readOpt() == IOobject::MUST_READ
         || io.readOpt() == IOobject::MUST_READ_IF_MODIFIED
        )
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readStream(typeName) >> *this;
        close();
    }
}


template<class Type>
CML::IOField<Type>::IOField(const IOobject& io, const tmp<Field<Type>>& f)
:
    regIOobject(io),
    Field<Type>(f)
{
    // Temporary warning
    if (io.readOpt() == IOobject::MUST_READ_IF_MODIFIED)
    {
        WarningInFunction
            << "IOField " << name()
            << " constructed with IOobject::MUST_READ_IF_MODIFIED"
            " but IOField does not support automatic rereading."
            << endl;
    }

    if
    (
        (
            io.readOpt() == IOobject::MUST_READ
         || io.readOpt() == IOobject::MUST_READ_IF_MODIFIED
        )
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readStream(typeName) >> *this;
        close();
    }
}


template<class Type>
CML::IOField<Type>::IOField(IOField<Type>&& f)
:
    regIOobject(move(f)),
    Field<Type>(move(f))
{}


// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

template<class Type>
CML::IOField<Type>::~IOField()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
bool CML::IOField<Type>::writeData(Ostream& os) const
{
    return (os << static_cast<const Field<Type>&>(*this)).good();
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Type>
void CML::IOField<Type>::operator=(const IOField<Type>& rhs)
{
    Field<Type>::operator=(rhs);
}


template<class Type>
void CML::IOField<Type>::operator=(IOField<Type>&& rhs)
{
    Field<Type>::operator=(move(rhs));
}


template<class Type>
void CML::IOField<Type>::operator=(const Field<Type>& rhs)
{
    Field<Type>::operator=(rhs);
}


template<class Type>
void CML::IOField<Type>::operator=(Field<Type>&& rhs)
{
    Field<Type>::operator=(move(rhs));
}


template<class Type>
void CML::IOField<Type>::operator=(const tmp<Field<Type>>& rhs)
{
    Field<Type>::operator=(rhs);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
