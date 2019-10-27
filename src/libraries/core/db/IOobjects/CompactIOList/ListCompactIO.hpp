/*---------------------------------------------------------------------------*\
Copyright (C) 2019 OpenFOAM Foundation
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
    CML::ListCompactIO

Description
    A List of objects of type \<T\> with input and output using
    a compact storage.  Behaves like List except when binary output in
    case it writes a CompactListList.

    Useful for lists of small sublists e.g. faceList, cellList.


\*---------------------------------------------------------------------------*/

#ifndef ListCompactIO_HPP
#define ListCompactIO_HPP

#include "labelList.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

class Istream;
class Ostream;

// Forward declaration of friend functions and operators
template<class T, class BaseType> class ListCompactIO;

template<class T, class BaseType>
void writeEntry(Ostream& os, const ListCompactIO<T, BaseType>& l);

template<class T, class BaseType> Istream& operator>>
(
    Istream&,
    ListCompactIO<T, BaseType>&
);

template<class T, class BaseType> Ostream& operator<<
(
    Ostream&,
    const ListCompactIO<T, BaseType>&
);


/*---------------------------------------------------------------------------*\
                           Class ListCompactIO Declaration
\*---------------------------------------------------------------------------*/

template<class T, class BaseType>
class ListCompactIO
:
    public List<T>
{
protected:

    // Protected Member Functions

        //- Has too many elements in it?
        bool overflows() const;

        void convertToCompact
        (
            labelList& start,
            List<BaseType>& elems
        ) const;

        void convertFromCompact
        (
            const labelList& start,
            const List<BaseType>& elems
        );


public:

    // Constructors

        //- Construct null
        ListCompactIO();

        //- Construct from UList
        ListCompactIO(const UList<T>&);

        //- Construct from Istream
        ListCompactIO(Istream&);

        //- Move constructor
        ListCompactIO(ListCompactIO<T, BaseType>&&);

        //- Move constructor from List
        ListCompactIO(List<T>&&);


    // Member operators

        void operator=(const ListCompactIO<T, BaseType>&);
        void operator=(ListCompactIO<T, BaseType>&&);

        void operator=(const List<T>&);
        void operator=(List<T>&&);


    // IOstream functions

        friend void writeEntry <T, BaseType>
        (
            Ostream& os,
            const ListCompactIO<T, BaseType>& l
        );


    // IOstream operators

        //- Read List from Istream, discarding contents of existing List.
        friend Istream& operator>> <T, BaseType>
        (
            Istream&,
            ListCompactIO<T, BaseType>&
        );

        // Write List to Ostream.
        friend Ostream& operator<< <T, BaseType>
        (
            Ostream&,
            const ListCompactIO<T, BaseType>&
        );
};


} // End namespace CML


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class T, class BaseType>
bool CML::ListCompactIO<T, BaseType>::overflows() const
{
    label size = 0;
    forAll(*this, i)
    {
        label oldSize = size;
        size += this->operator[](i).size();
        if (size < oldSize)
        {
            return true;
        }
    }
    return false;
}


template<class T, class BaseType>
void CML::ListCompactIO<T, BaseType>::convertToCompact
(
    labelList& start,
    List<BaseType>& elems
) const
{
    start.setSize(this->size() + 1);

    start[0] = 0;
    for (label i = 1; i < start.size(); i++)
    {
        label prev = start[i-1];
        start[i] = prev + this->operator[](i-1).size();

        if (start[i] < prev)
        {
            FatalErrorInFunction
                << "Overall number of elements " << start[i]
                << " of ListCompactIO of size "
                << this->size() << " overflows the representation of a label"
                << endl << "Please recompile with a larger representation"
                << " for label" << exit(FatalError);
        }
    }

    elems.setSize(start[start.size() - 1]);

    label elemi = 0;
    forAll(*this, i)
    {
        const T& subList = this->operator[](i);

        forAll(subList, j)
        {
            elems[elemi++] = subList[j];
        }
    }
}


template<class T, class BaseType>
void CML::ListCompactIO<T, BaseType>::convertFromCompact
(
    const labelList& start,
    const List<BaseType>& elems
)
{
    this->setSize(start.size() - 1);

    forAll(*this, i)
    {
        T& subList = this->operator[](i);

        label index = start[i];
        subList.setSize(start[i+1] - index);

        forAll(subList, j)
        {
            subList[j] = elems[index++];
        }
    }
}


// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class T, class BaseType>
CML::ListCompactIO<T, BaseType>::ListCompactIO()
{}


template<class T, class BaseType>
CML::ListCompactIO<T, BaseType>::ListCompactIO(const UList<T>& l)
:
    List<T>(l)
{}


template<class T, class BaseType>
CML::ListCompactIO<T, BaseType>::ListCompactIO(Istream& is)
{
    is >> *this;
}


template<class T, class BaseType>
CML::ListCompactIO<T, BaseType>::ListCompactIO(ListCompactIO<T, BaseType>&& l)
:
    List<T>(move(l))
{}


template<class T, class BaseType>
CML::ListCompactIO<T, BaseType>::ListCompactIO(List<T>&& l)
:
    List<T>(move(l))
{}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T, class BaseType>
void CML::ListCompactIO<T, BaseType>::operator=
(
    const ListCompactIO<T, BaseType>& rhs
)
{
    List<T>::operator=(rhs);
}


template<class T, class BaseType>
void CML::ListCompactIO<T, BaseType>::operator=
(
    ListCompactIO<T, BaseType>&& rhs
)
{
    List<T>::operator=(move(rhs));
}


template<class T, class BaseType>
void CML::ListCompactIO<T, BaseType>::operator=(const List<T>& rhs)
{
    List<T>::operator=(rhs);
}


template<class T, class BaseType>
void CML::ListCompactIO<T, BaseType>::operator=(List<T>&& rhs)
{
    List<T>::operator=(move(rhs));
}


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

template<class T, class BaseType>
void CML::writeEntry(Ostream& os, const ListCompactIO<T, BaseType>& l)
{
    // Keep ascii writing same.
    if (os.format() == IOstream::ASCII)
    {
        os << static_cast<const List<T>&>(l);
    }
    else
    {
        labelList start;
        List<BaseType> elems;
        l.convertToCompact(start, elems);
        writeEntry(os, start);
        writeEntry(os, elems);
    }
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class T, class BaseType>
CML::Istream& CML::operator>>
(
    CML::Istream& is,
    CML::ListCompactIO<T, BaseType>& L
)
{
    if (is.format() == IOstream::ASCII)
    {
        is >> static_cast<List<T>&>(L);
    }
    else
    {
        labelList start(is);
        List<BaseType> elems(is);
        L.convertFromCompact(start, elems);
    }

    return is;
}


template<class T, class BaseType>
CML::Ostream& CML::operator<<
(
    CML::Ostream& os,
    const CML::ListCompactIO<T, BaseType>& L
)
{
    // Keep ascii writing same.
    if (os.format() == IOstream::ASCII)
    {
        os << static_cast<const List<T>&>(L);
    }
    else
    {
        labelList start;
        List<BaseType> elems;
        L.convertToCompact(start, elems);
        os << start << elems;
    }

    return os;
}


#endif
