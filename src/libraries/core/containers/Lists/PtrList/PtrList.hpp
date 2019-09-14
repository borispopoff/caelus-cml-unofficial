/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2016 OpenFOAM Foundation
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
    CML::PtrList

Description
    A templated 1D list of pointers to objects of type \<T\>, where the
    size of the array is known and used for subscript bounds checking, etc.

    The element operator [] returns a reference to the object rather than a
    pointer.

SourceFiles
    PtrList.cpp
    PtrListIO.cpp

\*---------------------------------------------------------------------------*/

#ifndef PtrList_H
#define PtrList_H

#include "UPtrList.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes

template<class T> class autoPtr;
template<class T> class tmp;

class SLListBase;
template<class LListBase, class T> class LPtrList;
template<class T>
using SLPtrList = LPtrList<SLListBase, T>;

// Forward declaration of friend functions and operators
template<class T> class PtrList;

template<class T>
Istream& operator>>(Istream&, PtrList<T>&);


/*---------------------------------------------------------------------------*\
                           Class PtrList Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class PtrList
:
    public UPtrList<T>
{

protected:

    // Protected Member Functions

        //- Read from Istream using given Istream constructor class
        template<class INew>
        void read(Istream&, const INew& inewt);


public:

    // Constructors

        //- Null Constructor
        PtrList();

        //- Construct with size specified
        explicit PtrList(const label);

        //- Copy constructor
        PtrList(const PtrList<T>&);

        //- Copy constructor with additional argument for clone
        template<class CloneArg>
        PtrList(const PtrList<T>&, const CloneArg&);

        //- Construct by transferring the parameter contents
        PtrList(const Xfer<PtrList<T>>&);

        //- Construct as copy or re-use as specified
        PtrList(PtrList<T>&, bool reuse);

        //- Construct as copy of SLPtrList<T>
        explicit PtrList(const SLPtrList<T>&);

        //- Construct from Istream using given Istream constructor class
        template<class INew>
        PtrList(Istream&, const INew&);

        //- Construct from Istream using default Istream constructor class
        PtrList(Istream&);


    //- Destructor
    ~PtrList();


    // Member functions

        // Edit

            //- Reset size of PtrList. If extending the PtrList, new entries are
            //  set to nullptr. If truncating the PtrList, removed entries are
            //  deleted
            void setSize(const label);

            //- Alias for setSize(const label)
            inline void resize(const label);

            //- Clear the PtrList, i.e. set size to zero deleting all the
            //  allocated entries
            void clear();

            //- Append an element at the end of the list
            inline void append(T*);
            inline void append(const autoPtr<T>&);
            inline void append(const tmp<T>&);

            //- Transfer the contents of the argument PtrList into this PtrList
            //  and annul the argument list
            void transfer(PtrList<T>&);

            //- Transfer contents to the Xfer container
            inline Xfer<PtrList<T>> xfer();

            //- Is element set
            inline bool set(const label) const;

            //- Set element to given T* and return old element (can be nullptr)
            inline autoPtr<T> set(const label, T*);

            //- Set element to given autoPtr<T> and return old element
            inline autoPtr<T> set(const label, const autoPtr<T>&);

            //- Set element to given tmp<T> and return old element
            inline autoPtr<T> set(const label, const tmp<T>&);

            //- Reorders elements. Ordering does not have to be done in
            //  ascending or descending order. Reordering has to be unique.
            //  (is shuffle)
            void reorder(const labelUList&);


    // Member operators

        //- Assignment
        void operator=(const PtrList<T>&);


    // IOstream operator

        //- Read PtrList from Istream, discarding contents of existing PtrList
        friend Istream& operator>> <T>(Istream&, PtrList<T>&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "autoPtr.hpp"
#include "tmp.hpp"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
inline void CML::PtrList<T>::resize(const label newSize)
{
    this->setSize(newSize);
}


template<class T>
inline void CML::PtrList<T>::append(T* ptr)
{
    label sz = this->size();
    this->setSize(sz+1);
    this->ptrs_[sz] = ptr;
}


template<class T>
inline void CML::PtrList<T>::append(const autoPtr<T>& aptr)
{
    return append(const_cast<autoPtr<T>&>(aptr).ptr());
}


template<class T>
inline void CML::PtrList<T>::append
(
    const tmp<T>& t
)
{
    return append(const_cast<tmp<T>&>(t).ptr());
}


template<class T>
inline bool CML::PtrList<T>::set(const label i) const
{
    return this->ptrs_[i] != nullptr;
}


template<class T>
inline CML::autoPtr<T> CML::PtrList<T>::set(const label i, T* ptr)
{
    autoPtr<T> old(this->ptrs_[i]);
    this->ptrs_[i] = ptr;
    return old;
}


template<class T>
inline CML::autoPtr<T> CML::PtrList<T>::set
(
    const label i,
    const autoPtr<T>& aptr
)
{
    return set(i, const_cast<autoPtr<T>&>(aptr).ptr());
}


template<class T>
inline CML::autoPtr<T> CML::PtrList<T>::set
(
    const label i,
    const tmp<T>& t
)
{
    return set(i, const_cast<tmp<T>&>(t).ptr());
}


template<class T>
inline CML::Xfer<CML::PtrList<T>> CML::PtrList<T>::xfer()
{
    return xferMove(*this);
}


#include "SLPtrList.hpp"

// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class T>
CML::PtrList<T>::PtrList()
:
    UPtrList<T>()
{}


template<class T>
CML::PtrList<T>::PtrList(const label s)
:
    UPtrList<T>(s)
{}


template<class T>
CML::PtrList<T>::PtrList(const PtrList<T>& a)
:
    UPtrList<T>(a.size())
{
    forAll(*this, i)
    {
        this->ptrs_[i] = (a[i]).clone().ptr();
    }
}


template<class T>
template<class CloneArg>
CML::PtrList<T>::PtrList(const PtrList<T>& a, const CloneArg& cloneArg)
:
    UPtrList<T>(a.size())
{
    forAll(*this, i)
    {
        this->ptrs_[i] = (a[i]).clone(cloneArg).ptr();
    }
}


template<class T>
CML::PtrList<T>::PtrList(const Xfer<PtrList<T>>& lst)
{
    transfer(lst());
}


template<class T>
CML::PtrList<T>::PtrList(PtrList<T>& a, bool reuse)
:
    UPtrList<T>(a, reuse)
{
    if (!reuse)
    {
        forAll(*this, i)
        {
            this->ptrs_[i] = (a[i]).clone().ptr();
        }
    }
}


template<class T>
CML::PtrList<T>::PtrList(const SLPtrList<T>& sll)
:
    UPtrList<T>(sll.size())
{
    if (sll.size())
    {
        label i = 0;
        for
        (
            typename SLPtrList<T>::const_iterator iter = sll.begin();
            iter != sll.end();
            ++iter
        )
        {
            this->ptrs_[i++] = (iter()).clone().ptr();
        }
    }
}


// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

template<class T>
CML::PtrList<T>::~PtrList()
{
    forAll(*this, i)
    {
        if (this->ptrs_[i])
        {
            delete this->ptrs_[i];
        }
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void CML::PtrList<T>::setSize(const label newSize)
{
    if (newSize < 0)
    {
        FatalErrorInFunction
            << "bad set size " << newSize
            << " for type " << typeid(T).name()
            << abort(FatalError);
    }

    label oldSize = this->size();

    if (newSize == 0)
    {
        clear();
    }
    else if (newSize < oldSize)
    {
        label i;
        for (i=newSize; i<oldSize; i++)
        {
            if (this->ptrs_[i])
            {
                delete this->ptrs_[i];
            }
        }

        this->ptrs_.setSize(newSize);
    }
    else // newSize > oldSize
    {
        this->ptrs_.setSize(newSize);

        label i;
        for (i=oldSize; i<newSize; i++)
        {
            this->ptrs_[i] = nullptr;
        }
    }
}


template<class T>
void CML::PtrList<T>::clear()
{
    forAll(*this, i)
    {
        if (this->ptrs_[i])
        {
            delete this->ptrs_[i];
        }
    }

    this->ptrs_.clear();
}


template<class T>
void CML::PtrList<T>::transfer(PtrList<T>& a)
{
    clear();
    this->ptrs_.transfer(a.ptrs_);
}


template<class T>
void CML::PtrList<T>::reorder(const labelUList& oldToNew)
{
    if (oldToNew.size() != this->size())
    {
        FatalErrorInFunction
            << "Size of map (" << oldToNew.size()
            << ") not equal to list size (" << this->size()
            << ") for type " << typeid(T).name()
            << abort(FatalError);
    }

    List<T*> newPtrs_(this->ptrs_.size(), reinterpret_cast<T*>(0));

    forAll(*this, i)
    {
        label newI = oldToNew[i];

        if (newI < 0 || newI >= this->size())
        {
            FatalErrorInFunction
                << "Illegal index " << newI << nl
                << "Valid indices are 0.." << this->size()-1
                << " for type " << typeid(T).name()
                << abort(FatalError);
        }

        if (newPtrs_[newI])
        {
            FatalErrorInFunction
                << "reorder map is not unique; element " << newI
                << " already set for type " << typeid(T).name()
                << abort(FatalError);
        }
        newPtrs_[newI] = this->ptrs_[i];
    }

    forAll(newPtrs_, i)
    {
        if (!newPtrs_[i])
        {
            FatalErrorInFunction
                << "Element " << i << " not set after reordering with type "
                << typeid(T).name() << nl << abort(FatalError);
        }
    }

    this->ptrs_.transfer(newPtrs_);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T>
void CML::PtrList<T>::operator=(const PtrList<T>& a)
{
    if (this == &a)
    {
        FatalErrorInFunction
            << "attempted assignment to self for type " << typeid(T).name()
            << abort(FatalError);
    }

    if (this->size() == 0)
    {
        setSize(a.size());

        forAll(*this, i)
        {
            this->ptrs_[i] = (a[i]).clone().ptr();
        }
    }
    else if (a.size() == this->size())
    {
        forAll(*this, i)
        {
            (*this)[i] = a[i];
        }
    }
    else
    {
        FatalErrorInFunction
            << "bad size: " << a.size()
            << " for type " << typeid(T).name()
            << abort(FatalError);
    }
}


#include "SLList.hpp"
#include "Istream.hpp"
#include "Ostream.hpp"
#include "INew.hpp"

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

template<class T>
template<class INew>
void CML::PtrList<T>::read(Istream& is, const INew& inewt)
{
    is.fatalCheck("PtrList<T>::read(Istream&, const INew&)");

    token firstToken(is);

    is.fatalCheck
    (
        "PtrList<T>::read(Istream&, const INew&) : "
        "reading first token"
    );

    if (firstToken.isLabel())
    {
        // Read size of list
        label s = firstToken.labelToken();

        setSize(s);

        // Read beginning of contents
        char delimiter = is.readBeginList("PtrList");

        if (s)
        {
            if (delimiter == token::BEGIN_LIST)
            {
                forAll(*this, i)
                {
                    set(i, inewt(is));

                    is.fatalCheck
                    (
                        "PtrList<T>::read(Istream&, const INew&) : "
                        "reading entry"
                    );
                }
            }
            else
            {
                T* tPtr = inewt(is).ptr();
                set(0, tPtr);

                is.fatalCheck
                (
                    "PtrList<T>::read(Istream&, const INew&) : "
                    "reading the single entry"
                );

                for (label i=1; i<s; i++)
                {
                    set(i, tPtr->clone());
                }
            }
        }

        // Read end of contents
        is.readEndList("PtrList");
    }
    else if (firstToken.isPunctuation())
    {
        if (firstToken.pToken() != token::BEGIN_LIST)
        {
            FatalIOErrorInFunction
            (
                is
            )   << "incorrect first token, '(', found " << firstToken.info()
                << exit(FatalIOError);
        }

        SLList<T*> sllPtrs;

        token lastToken(is);
        while
        (
           !(
                lastToken.isPunctuation()
             && lastToken.pToken() == token::END_LIST
            )
        )
        {
            is.putBack(lastToken);

            if (is.eof())
            {
                FatalIOErrorInFunction
                (
                    is
                )   << "Premature EOF after reading " << lastToken.info()
                    << exit(FatalIOError);
            }

            sllPtrs.append(inewt(is).ptr());
            is >> lastToken;
        }

        setSize(sllPtrs.size());

        label i = 0;
        for
        (
            typename SLList<T*>::iterator iter = sllPtrs.begin();
            iter != sllPtrs.end();
            ++iter
        )
        {
            set(i++, iter());
        }
    }
    else
    {
        FatalIOErrorInFunction
        (
            is
        )   << "incorrect first token, expected <int> or '(', found "
            << firstToken.info()
            << exit(FatalIOError);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
template<class INew>
CML::PtrList<T>::PtrList(Istream& is, const INew& inewt)
{
    read(is, inewt);
}


template<class T>
CML::PtrList<T>::PtrList(Istream& is)
{
    read(is, INew<T>());
}


// * * * * * * * * * * * * * * * Istream Operator  * * * * * * * * * * * * * //

template<class T>
CML::Istream& CML::operator>>(Istream& is, PtrList<T>& L)
{
    L.clear();
    L.read(is, INew<T>());

    return is;
}


#endif

// ************************************************************************* //
