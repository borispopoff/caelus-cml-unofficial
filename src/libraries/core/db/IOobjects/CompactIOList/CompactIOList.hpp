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
    CML::CompactIOList

Description
    A List of objects of type \<T\> with automated input and output using
    a compact storage. Behaves like IOList except when binary output in
    case it writes a CompactListList.

    Useful for lists of small sublists e.g. faceList, cellList.


\*---------------------------------------------------------------------------*/

#ifndef CompactIOList_H
#define CompactIOList_H

#include "ListCompactIO.hpp"
#include "regIOobject.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class CompactIOList Declaration
\*---------------------------------------------------------------------------*/

template<class T, class BaseType>
class CompactIOList
:
    public regIOobject,
    public ListCompactIO<T, BaseType>
{
    // Private Member Functions

        //- Read according to header type
        void readFromStream();

public:

    //- Runtime type information
    TypeName("CompactList");


    // Constructors

        //- Construct from IOobject
        CompactIOList(const IOobject&);

        //- Construct from IOobject and size of CompactIOList
        CompactIOList(const IOobject&, const label);

        //- Construct from IOobject and a List
        CompactIOList(const IOobject&, const List<T>&);

        //- Move construct by transferring the List contents
        CompactIOList(const IOobject&, List<T>&&);

        //- Move constructor
        CompactIOList(CompactIOList<T, BaseType>&&);


    // Destructor

        virtual ~CompactIOList();


    // Member functions

        virtual bool writeObject
        (
            IOstream::streamFormat,
            IOstream::versionNumber,
            IOstream::compressionType
        ) const;

        virtual bool writeData(Ostream&) const;


    // Member operators

        void operator=(const CompactIOList<T, BaseType>&);
        void operator=(CompactIOList<T, BaseType>&&);

        void operator=(const List<T>&);
        void operator=(List<T>&&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "IOList.hpp"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class T, class BaseType>
void CML::CompactIOList<T, BaseType>::readFromStream()
{
    Istream& is = readStream(word::null);

    if (headerClassName() == IOList<T>::typeName)
    {
        is >> static_cast<ListCompactIO<T, BaseType>&>(*this);
        close();
    }
    else if (headerClassName() == typeName)
    {
        is >> *this;
        close();
    }
    else
    {
        FatalIOErrorInFunction
        (
            is
        )   << "unexpected class name " << headerClassName()
            << " expected " << typeName << " or " << IOList<T>::typeName
            << endl
            << "    while reading object " << name()
            << exit(FatalIOError);
    }
}


// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class T, class BaseType>
CML::CompactIOList<T, BaseType>::CompactIOList(const IOobject& io)
:
    regIOobject(io)
{
    if
    (
        io.readOpt() == IOobject::MUST_READ
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readFromStream();
    }
}


template<class T, class BaseType>
CML::CompactIOList<T, BaseType>::CompactIOList
(
    const IOobject& io,
    const label size
)
:
    regIOobject(io)
{
    if
    (
        io.readOpt() == IOobject::MUST_READ
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readFromStream();
    }
    else
    {
        this->setSize(size);
    }
}


template<class T, class BaseType>
CML::CompactIOList<T, BaseType>::CompactIOList
(
    const IOobject& io,
    const List<T>& list
)
:
    regIOobject(io)
{
    if
    (
        io.readOpt() == IOobject::MUST_READ
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readFromStream();
    }
    else
    {
        ListCompactIO<T, BaseType>::operator=(list);
    }
}


template<class T, class BaseType>
CML::CompactIOList<T, BaseType>::CompactIOList
(
    const IOobject& io,
    List<T>&& list
)
:
    regIOobject(io),
    ListCompactIO<T, BaseType>(move(list))
{
    if
    (
        io.readOpt() == IOobject::MUST_READ
     || (io.readOpt() == IOobject::READ_IF_PRESENT && headerOk())
    )
    {
        readFromStream();
    }
}


template<class T, class BaseType>
CML::CompactIOList<T, BaseType>::CompactIOList
(
    CompactIOList<T, BaseType>&& list
)
:
    regIOobject(move(list)),
    List<T>(move(list))
{}


// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

template<class T, class BaseType>
CML::CompactIOList<T, BaseType>::~CompactIOList()
{}



// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T, class BaseType>
bool CML::CompactIOList<T, BaseType>::writeObject
(
    IOstream::streamFormat fmt,
    IOstream::versionNumber ver,
    IOstream::compressionType cmp
) const
{
    if (fmt == IOstream::ASCII)
    {
        // Change type to be non-compact format type
        const word oldTypeName = typeName;

        const_cast<word&>(typeName) = IOList<T>::typeName;

        bool good = regIOobject::writeObject(fmt, ver, cmp);

        // Change type back
        const_cast<word&>(typeName) = oldTypeName;

        return good;
    }
    else if (this->overflows())
    {
        WarningInFunction
            << "Overall number of elements of CompactIOList of size "
            << this->size() << " overflows the representation of a label"
            << endl << "    Switching to ascii writing" << endl;

        // Change type to be non-compact format type
        const word oldTypeName = typeName;

        const_cast<word&>(typeName) = IOList<T>::typeName;

        bool good = regIOobject::writeObject(IOstream::ASCII, ver, cmp);

        // Change type back
        const_cast<word&>(typeName) = oldTypeName;

        return good;
    }
    else
    {
        return regIOobject::writeObject(fmt, ver, cmp);
    }
}


template<class T, class BaseType>
bool CML::CompactIOList<T, BaseType>::writeData(Ostream& os) const
{
    return (os << *this).good();
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T, class BaseType>
void CML::CompactIOList<T, BaseType>::operator=
(
    const CompactIOList<T, BaseType>& rhs
)
{
    ListCompactIO<T, BaseType>::operator=(rhs);
}


template<class T, class BaseType>
void CML::CompactIOList<T, BaseType>::operator=
(
    CompactIOList<T, BaseType>&& rhs
)
{
    ListCompactIO<T, BaseType>::operator=(move(rhs));
}


template<class T, class BaseType>
void CML::CompactIOList<T, BaseType>::operator=
(
    const List<T>& rhs
)
{
    ListCompactIO<T, BaseType>::operator=(rhs);
}


template<class T, class BaseType>
void CML::CompactIOList<T, BaseType>::operator=
(
    List<T>&& rhs
)
{
    ListCompactIO<T, BaseType>::operator=(move(rhs));
}


#endif

// ************************************************************************* //
