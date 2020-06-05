/*---------------------------------------------------------------------------*\
Copyright (C) 2015-2019 OpenFOAM Foundation
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
    CML::PtrListDictionary

Description
    Template dictionary class which manages the storage associated with it.

    It is derived from DictionaryBase instantiated on the memory managed PtrList
    of \<T\> to provide ordered indexing in addition to the dictionary lookup.


\*---------------------------------------------------------------------------*/

#ifndef PtrListDictionary_HPP
#define PtrListDictionary_HPP

#include "DictionaryBase.hpp"
#include "PtrList.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                        Class PtrListDictionary Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class PtrListDictionary
:
    public DictionaryBase<PtrList<T>, T>
{

public:

    // Constructors

        //- Construct given initial list size
        PtrListDictionary(const label size);

        //- Copy constructor
        PtrListDictionary(const PtrListDictionary&);

        //- Move constructor
        PtrListDictionary(PtrListDictionary&&);

        //- Construct from Istream using given Istream constructor class
        template<class INew>
        PtrListDictionary(Istream&, const INew&);

        //- Construct from Istream
        PtrListDictionary(Istream&);


    // Member functions

        //- Set element to pointer provided and return old element
        autoPtr<T> set(const label, const word& key, T*);

        //- Set element to autoPtr value provided and return old element
        autoPtr<T> set(const label, const word& key, autoPtr<T>&);

        //- Set element to tmp value provided and return old element
        autoPtr<T> set(const label, const word& key, tmp<T>&);


    // Member operators

        using PtrList<T>::operator[];

        //- Find and return entry
        const T& operator[](const word& key) const
        {
            return *DictionaryBase<PtrList<T>, T>::operator[](key);
        }

        //- Find and return entry
        T& operator[](const word& key)
        {
            return *DictionaryBase<PtrList<T>, T>::operator[](key);
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
CML::PtrListDictionary<T>::PtrListDictionary(const label size)
:
    DictionaryBase<PtrList<T>, T>(2*size)
{
    PtrList<T>::setSize(size);
}


template<class T>
CML::PtrListDictionary<T>::PtrListDictionary(const PtrListDictionary& dict)
:
    DictionaryBase<PtrList<T>, T>(dict)
{}


template<class T>
CML::PtrListDictionary<T>::PtrListDictionary(PtrListDictionary&& dict)
:
    DictionaryBase<PtrList<T>, T>(move(dict))
{}


template<class T>
template<class INew>
CML::PtrListDictionary<T>::PtrListDictionary(Istream& is, const INew& iNew)
:
    DictionaryBase<PtrList<T>, T>(is, iNew)
{}


template<class T>
CML::PtrListDictionary<T>::PtrListDictionary(Istream& is)
:
    DictionaryBase<PtrList<T>, T>(is)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
inline CML::autoPtr<T> CML::PtrListDictionary<T>::set
(
    const label i,
    const word& key,
    T* ptr
)
{
    if (!DictionaryBase<PtrList<T>, T>::hashedTs_.insert(key, ptr))
    {
        FatalErrorInFunction
            << "Cannot insert with key '" << key << "' into hash-table"
            << abort(FatalError);
    }
    return PtrList<T>::set(i, ptr);
}


template<class T>
inline CML::autoPtr<T> CML::PtrListDictionary<T>::set
(
    const label i,
    const word& key,
    autoPtr<T>& aptr
)
{
    T* ptr = aptr.ptr();
    if (!DictionaryBase<PtrList<T>, T>::hashedTs_.insert(key, ptr))
    {
        FatalErrorInFunction
            << "Cannot insert with key '" << key << "' into hash-table"
            << abort(FatalError);
    }
    return PtrList<T>::set(i, ptr);
}


template<class T>
inline CML::autoPtr<T> CML::PtrListDictionary<T>::set
(
    const label i,
    const word& key,
    tmp<T>& t
)
{
    T* ptr = t.ptr();
    if (!DictionaryBase<PtrList<T>, T>::hashedTs_.insert(key, ptr))
    {
        FatalErrorInFunction
            << "Cannot insert with key '" << key << "' into hash-table"
            << abort(FatalError);
    }
    return PtrList<T>::set(i, ptr);
}


#endif

// ************************************************************************* //
