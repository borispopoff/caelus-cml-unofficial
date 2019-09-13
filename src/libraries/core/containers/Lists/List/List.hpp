/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2016 OpenFOAM Foundation
Copyright (C) 2015 Applied CCM
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
    CML::List

Description
    A 1D array of objects of type \<T\>, where the size of the vector
    is known and used for subscript bounds checking, etc.

    Storage is allocated on free-store during construction.


\*---------------------------------------------------------------------------*/

#ifndef List_H
#define List_H

#include "UList.hpp"
#include "autoPtr.hpp"
#include "Xfer.hpp"
#include <initializer_list>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes

class Istream;
class Ostream;

// Forward declaration of friend functions and operators
template<class T> class List;

template<class T> Istream& operator>>(Istream&, List<T>&);

template<class T, unsigned Size> class FixedList;
template<class T> class PtrList;
template<class T> class SLList;

template<class T, unsigned SizeInc, unsigned SizeMult, unsigned SizeDiv>
class DynamicList;

template<class T> class SortableList;
template<class T> class IndirectList;
template<class T> class UIndirectList;
template<class T> class BiIndirectList;

typedef UList<label> unallocLabelList;

/*---------------------------------------------------------------------------*\
                           Class List Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class List
:
    public UList<T>
{
    // Private member functions

        //- Copy list of given type
        template<class List2>
        void CopyList(const List2&);

        //- Construct given start and end iterators and number of elements
        template<class InputIterator>
        List(InputIterator first, InputIterator last, const label s);


protected:

    //- Override size to be inconsistent with allocated storage.
    //  Use with care
    inline void size(const label);


public:

    // Static Member Functions

        //- Return a null List
        inline static const List<T>& null();


    // Constructors

        //- Null constructor
        inline List();

        //- Construct with given size
        explicit List(const label);

        //- Construct with given size and value for all elements
        List(const label, const T&);

        //- Construct with given size initializing all elements to zero
        List(const label, const zero);

        //- Copy constructor
        List(const List<T>&);

        //- Copy constructor from list containing another type
        template<class T2>
        explicit List(const List<T2>&);

        //- Construct by transferring the parameter contents
        List(const Xfer<List<T>>&);

        //- Construct as copy or re-use as specified
        List(List<T>&, bool reuse);

        //- Construct as subset
        List(const UList<T>&, const labelUList& mapAddressing);

        //- Construct given start and end iterators
        template<class InputIterator>
        List(InputIterator first, InputIterator last);

        //- Construct as copy of FixedList<T, Size>
        template<unsigned Size>
        explicit List(const FixedList<T, Size>&);

        //- Construct as copy of PtrList<T>
        explicit List(const PtrList<T>&);

        //- Construct as copy of SLList<T>
        explicit List(const SLList<T>&);

        //- Construct as copy of UIndirectList<T>
        explicit List(const UIndirectList<T>&);

        //- Construct as copy of BiIndirectList<T>
        explicit List(const BiIndirectList<T>&);

        //- Construct from an initializer list
        List(std::initializer_list<T> lst);

        //- Construct from Istream
        List(Istream&);

        //- Clone
        inline autoPtr<List<T>> clone() const;


    //- Destructor
    ~List();


    // Related types

        //- Declare type of subList
        typedef SubList<T> subList;


    // Member Functions

        //- Return the number of elements in the UList
        inline label size() const;


        // Edit

            //- Alias for setSize(const label)
            inline void resize(const label);

            //- Alias for setSize(const label, const T&)
            inline void resize(const label, const T&);

            //- Reset size of List
            void setSize(const label);

            //- Reset size of List and value for new elements
            void setSize(const label, const T&);

            //- Clear the list, i.e. set size to zero
            void clear();

            //- Append an element at the end of the list
            inline void append(const T&);

            //- Append a List at the end of this list
            inline void append(const UList<T>&);

            //- Append a UIndirectList at the end of this list
            inline void append(const UIndirectList<T>&);

            //- Transfer the contents of the argument List into this list
            //  and annul the argument list
            void transfer(List<T>&);

            //- Transfer the contents of the argument List into this list
            //  and annul the argument list
            template<unsigned SizeInc, unsigned SizeMult, unsigned SizeDiv>
            void transfer(DynamicList<T, SizeInc, SizeMult, SizeDiv>&);

            //- Transfer the contents of the argument List into this list
            //  and annul the argument list
            void transfer(SortableList<T>&);

            //- Transfer contents to the Xfer container
            inline Xfer<List<T>> xfer();

            //- Return subscript-checked element of UList
            inline T& newElmt(const label);


        //- Disallow implicit shallowCopy
        void shallowCopy(const UList<T>&) = delete;


    // Member Operators

        //- Assignment to UList operator. Takes linear time
        void operator=(const UList<T>&);

        //- Assignment operator. Takes linear time
        void operator=(const List<T>&);

        //- Assignment to SLList operator. Takes linear time
        void operator=(const SLList<T>&);

        //- Assignment to UIndirectList operator. Takes linear time
        void operator=(const UIndirectList<T>&);

        //- Assignment to BiIndirectList operator. Takes linear time
        void operator=(const BiIndirectList<T>&);

        //- Assignment of all entries to the given value
        inline void operator=(const T&);

        //- Assignment of all entries to zero
        inline void operator=(const zero);


    // Istream operator

        //- Read List from Istream, discarding contents of existing List
        friend Istream& operator>> <T>
        (Istream&, List<T>&);
};


//- Read a bracket-delimited list, or handle a single value as list of size 1
//  For example,
//  \code
//      wList = readList<word>(IStringStream("(patch1 patch2 patch3)")());
//      wList = readList<word>(IStringStream("patch0")());
//  \endcode
//  Mostly useful for handling command-line arguments
template<class T>
List<T> readList(Istream&);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
inline CML::List<T>::List()
{}


template<class T>
inline CML::autoPtr<CML::List<T>> CML::List<T>::clone() const
{
    return autoPtr<List<T>>(new List<T>(*this));
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
inline const CML::List<T>& CML::List<T>::null()
{
    return NullSingletonRef<List<T>>();
}


template<class T>
inline void CML::List<T>::resize(const label newSize)
{
    this->setSize(newSize);
}


template<class T>
inline void CML::List<T>::resize(const label newSize, const T& a)
{
    this->setSize(newSize, a);
}


template<class T>
inline T& CML::List<T>::newElmt(const label i)
{
    if (i >= this->size())
    {
        setSize(2*this->size());
    }

    return UList<T>::operator[](i);
}


template<class T>
inline void CML::List<T>::size(const label n)
{
    UList<T>::size_ = n;
}


template<class T>
inline CML::label CML::List<T>::size() const
{
    return UList<T>::size_;
}


template<class T>
inline CML::Xfer<CML::List<T>> CML::List<T>::xfer()
{
    return xferMove(*this);
}


template<class T>
inline void CML::List<T>::append(const T& t)
{
    setSize(size()+1, t);
}


template<class T>
inline void CML::List<T>::append(const UList<T>& lst)
{
    if (this == &lst)
    {
        FatalErrorInFunction
            << "attempted appending to self" << abort(FatalError);
    }

    label nextFree = this->size();
    setSize(nextFree + lst.size());

    forAll(lst, elemI)
    {
        this->operator[](nextFree++) = lst[elemI];
    }
}


template<class T>
inline void CML::List<T>::append(const UIndirectList<T>& lst)
{
    label nextFree = this->size();
    setSize(nextFree + lst.size());

    forAll(lst, elemI)
    {
        this->operator[](nextFree++) = lst[elemI];
    }
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T>
inline void CML::List<T>::operator=(const T& t)
{
    UList<T>::operator=(t);
}


template<class T>
inline void CML::List<T>::operator=(const zero)
{
    UList<T>::operator=(Zero);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ListLoopM.hpp"
#include "FixedList.hpp"
#include "PtrList.hpp"
#include "SLList.hpp"
#include "IndirectList.hpp"
#include "UIndirectList.hpp"
#include "BiIndirectList.hpp"
#include "contiguous.hpp"


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class T>
template<class List2>
void CML::List<T>::CopyList(const List2& lst)
{
    if (this->size_)
    {
        this->v_ = new T[this->size_];

        forAll(*this, i)
        {
            this->operator[](i) = lst[i];
        }
    }
}


template<class T>
template<class InputIterator>
CML::List<T>::List(InputIterator first, InputIterator last, const label s)
:
    UList<T>(NULL, s)
{
    if (this->size_)
    {
        this->v_ = new T[this->size_];

        InputIterator iter = first;
        forAll(*this, i)
        {
            this->operator[](i) = *iter++;
        }
    }
}


// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class T>
CML::List<T>::List(const label s)
:
    UList<T>(nullptr, s)
{
    if (this->size_ < 0)
    {
        FatalErrorInFunction
            << "bad size " << this->size_
            << abort(FatalError);
    }

    if (this->size_)
    {
        this->v_ = new T[this->size_];
    }
}


template<class T>
CML::List<T>::List(const label s, const T& a)
:
    UList<T>(nullptr, s)
{
    if (this->size_ < 0)
    {
        FatalErrorInFunction
            << "bad size " << this->size_
            << abort(FatalError);
    }

    if (this->size_)
    {
        this->v_ = new T[this->size_];

        List_ACCESS(T, (*this), vp);
        List_FOR_ALL((*this), i)
            List_ELEM((*this), vp, i) = a;
        List_END_FOR_ALL
    }
}


template<class T>
CML::List<T>::List(const label s, const zero)
:
    UList<T>(NULL, s)
{
    if (this->size_ < 0)
    {
        FatalErrorInFunction
            << "bad size " << this->size_
            << abort(FatalError);
    }

    if (this->size_)
    {
        this->v_ = new T[this->size_];

        List_ACCESS(T, (*this), vp);
        List_FOR_ALL((*this), i)
            List_ELEM((*this), vp, i) = Zero;
        List_END_FOR_ALL
    }
}


template<class T>
CML::List<T>::List(const List<T>& a)
:
    UList<T>(nullptr, a.size_)
{
    if (this->size_)
    {
        this->v_ = new T[this->size_];

        #ifdef USEMEMCPY
        if (contiguous<T>())
        {
            memcpy(this->v_, a.v_, this->byteSize());
        }
        else
        #endif
        {
            List_ACCESS(T, (*this), vp);
            List_CONST_ACCESS(T, a, ap);
            List_FOR_ALL((*this), i)
                List_ELEM((*this), vp, i) = List_ELEM(a, ap, i);
            List_END_FOR_ALL
        }
    }
}


template<class T>
template<class T2>
CML::List<T>::List(const List<T2>& a)
:
    UList<T>(NULL, a.size())
{
    if (this->size_)
    {
        this->v_ = new T[this->size_];

        List_ACCESS(T, (*this), vp);
        List_CONST_ACCESS(T2, a, ap);
        List_FOR_ALL((*this), i)
            List_ELEM((*this), vp, i) = T(List_ELEM(a, ap, i));
        List_END_FOR_ALL
    }
}


template<class T>
CML::List<T>::List(const Xfer<List<T>>& lst)
{
    transfer(lst());
}


template<class T>
CML::List<T>::List(List<T>& a, bool reuse)
:
    UList<T>(nullptr, a.size_)
{
    if (reuse)
    {
        this->v_ = a.v_;
        a.v_ = 0;
        a.size_ = 0;
    }
    else if (this->size_)
    {
        this->v_ = new T[this->size_];

        #ifdef USEMEMCPY
        if (contiguous<T>())
        {
            memcpy(this->v_, a.v_, this->byteSize());
        }
        else
        #endif
        {
            List_ACCESS(T, (*this), vp);
            List_CONST_ACCESS(T, a, ap);
            List_FOR_ALL((*this), i)
                List_ELEM((*this), vp, i) = List_ELEM(a, ap, i);
            List_END_FOR_ALL
        }
    }
}


template<class T>
CML::List<T>::List(const UList<T>& a, const labelUList& map)
:
    UList<T>(nullptr, map.size())
{
    if (this->size_)
    {
        // Note:cannot use List_ELEM since third argument has to be index.

        this->v_ = new T[this->size_];

        forAll(*this, i)
        {
            this->operator[](i) = a[map[i]];
        }
    }
}


template<class T>
template<class InputIterator>
CML::List<T>::List(InputIterator first, InputIterator last)
:
    List<T>(first, last, std::distance(first, last))
{}


template<class T>
template<unsigned Size>
CML::List<T>::List(const FixedList<T, Size>& lst)
:
    UList<T>(nullptr, Size)
{
    CopyList(lst);
}


template<class T>
CML::List<T>::List(const PtrList<T>& lst)
:
    UList<T>(nullptr, lst.size())
{
    CopyList(lst);
}


template<class T>
CML::List<T>::List(const SLList<T>& lst)
:
    List<T>(lst.first(), lst.last(), lst.size())
{}


template<class T>
CML::List<T>::List(const UIndirectList<T>& lst)
:
    UList<T>(NULL, lst.size())
{
    CopyList(lst);
}


template<class T>
CML::List<T>::List(const BiIndirectList<T>& lst)
:
    UList<T>(nullptr, lst.size())
{
    CopyList(lst);
}


template<class T>
CML::List<T>::List(std::initializer_list<T> lst)
:
    List<T>(lst.begin(), lst.end())
{}


// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

template<class T>
CML::List<T>::~List()
{
    if (this->v_) delete[] this->v_;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void CML::List<T>::setSize(const label newSize)
{
    if (newSize < 0)
    {
        FatalErrorInFunction
            << "bad set size " << newSize
            << abort(FatalError);
    }

    if (newSize != this->size_)
    {
        if (newSize > 0)
        {
            T* nv = new T[label(newSize)];

            if (this->size_)
            {
                label i = min(this->size_, newSize);

                #ifdef USEMEMCPY
                if (contiguous<T>())
                {
                    memcpy(nv, this->v_, i*sizeof(T));
                }
                else
                #endif
                {
                    T* vv = &this->v_[i];
                    T* av = &nv[i];
                    while (i--) *--av = *--vv;
                }
            }
            if (this->v_) delete[] this->v_;

            this->size_ = newSize;
            this->v_ = nv;
        }
        else
        {
            clear();
        }
    }
}


template<class T>
void CML::List<T>::setSize(const label newSize, const T& a)
{
    label oldSize = label(this->size_);
    this->setSize(newSize);

    if (newSize > oldSize)
    {
        label i = newSize - oldSize;
        T* vv = &this->v_[newSize];
        while (i--) *--vv = a;
    }
}


template<class T>
void CML::List<T>::clear()
{
    if (this->v_) delete[] this->v_;
    this->size_ = 0;
    this->v_ = 0;
}


template<class T>
void CML::List<T>::transfer(List<T>& a)
{
    if (this->v_) delete[] this->v_;
    this->size_ = a.size_;
    this->v_ = a.v_;

    a.size_ = 0;
    a.v_ = 0;
}


template<class T>
template<unsigned SizeInc, unsigned SizeMult, unsigned SizeDiv>
void CML::List<T>::transfer(DynamicList<T, SizeInc, SizeMult, SizeDiv>& a)
{
    // Shrink the allocated space to the number of elements used
    a.shrink();
    transfer(static_cast<List<T>&>(a));
    a.clearStorage();
}


template<class T>
void CML::List<T>::transfer(SortableList<T>& a)
{
    // Shrink away the sort indices
    a.shrink();
    transfer(static_cast<List<T>&>(a));
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T>
void CML::List<T>::operator=(const UList<T>& a)
{
    if (a.size_ != this->size_)
    {
        if (this->v_) delete[] this->v_;
        this->v_ = 0;
        this->size_ = a.size_;
        if (this->size_) this->v_ = new T[this->size_];
    }

    if (this->size_)
    {
        #ifdef USEMEMCPY
        if (contiguous<T>())
        {
            memcpy(this->v_, a.v_, this->byteSize());
        }
        else
        #endif
        {
            List_ACCESS(T, (*this), vp);
            List_CONST_ACCESS(T, a, ap);
            List_FOR_ALL((*this), i)
                List_ELEM((*this), vp, i) = List_ELEM(a, ap, i);
            List_END_FOR_ALL
        }
    }
}


template<class T>
void CML::List<T>::operator=(const List<T>& a)
{
    if (this == &a)
    {
        FatalErrorInFunction
            << "attempted assignment to self"
            << abort(FatalError);
    }

    operator=(static_cast<const UList<T>&>(a));
}


template<class T>
void CML::List<T>::operator=(const SLList<T>& lst)
{
    if (lst.size() != this->size_)
    {
        if (this->v_) delete[] this->v_;
        this->v_ = 0;
        this->size_ = lst.size();
        if (this->size_) this->v_ = new T[this->size_];
    }

    if (this->size_)
    {
        label i = 0;
        for
        (
            typename SLList<T>::const_iterator iter = lst.begin();
            iter != lst.end();
            ++iter
        )
        {
            this->operator[](i++) = iter();
        }
    }
}


template<class T>
void CML::List<T>::operator=(const UIndirectList<T>& lst)
{
    if (lst.size() != this->size_)
    {
        if (this->v_) delete[] this->v_;
        this->v_ = 0;
        this->size_ = lst.size();
        if (this->size_) this->v_ = new T[this->size_];
    }

    forAll(*this, i)
    {
        this->operator[](i) = lst[i];
    }
}


template<class T>
void CML::List<T>::operator=(const BiIndirectList<T>& lst)
{
    if (lst.size() != this->size_)
    {
        if (this->v_) delete[] this->v_;
        this->v_ = 0;
        this->size_ = lst.size();
        if (this->size_) this->v_ = new T[this->size_];
    }

    forAll(*this, i)
    {
        this->operator[](i) = lst[i];
    }
}


// * * * * * * * * * * * * * * * *  IOStream operators * * * * * * * * * * * //

#include "Istream.hpp"
#include "token.hpp"
#include "SLList.hpp"
#include "contiguous.hpp"

// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template<class T>
CML::List<T>::List(Istream& is)
:
    UList<T>(nullptr, 0)
{
    operator>>(is, *this);
}


template<class T>
CML::Istream& CML::operator>>(Istream& is, List<T>& L)
{
    // Anull list
    L.setSize(0);

    is.fatalCheck("operator>>(Istream&, List<T>&)");

    token firstToken(is);

    is.fatalCheck("operator>>(Istream&, List<T>&) : reading first token");

    if (firstToken.isCompound())
    {
        L.transfer
        (
            dynamicCast<token::Compound<List<T>>>
            (
                firstToken.transferCompoundToken(is)
            )
        );
    }
    else if (firstToken.isLabel())
    {
        label s = firstToken.labelToken();

        // Set list length to that read
        L.setSize(s);

        // Read list contents depending on data format

        if (is.format() == IOstream::ASCII || !contiguous<T>())
        {
            // Read beginning of contents
            char delimiter = is.readBeginList("List");

            if (s)
            {
                if (delimiter == token::BEGIN_LIST)
                {
                    for (label i=0; i<s; i++)
                    {
                        is >> L[i];

                        is.fatalCheck
                        (
                            "operator>>(Istream&, List<T>&) : reading entry"
                        );
                    }
                }
                else
                {
                    T element;
                    is >> element;

                    is.fatalCheck
                    (
                        "operator>>(Istream&, List<T>&) : "
                        "reading the single entry"
                    );

                    for (label i=0; i<s; i++)
                    {
                        L[i] = element;
                    }
                }
            }

            // Read end of contents
            is.readEndList("List");
        }
        else
        {
            if (s)
            {
                is.read(reinterpret_cast<char*>(L.data()), s*sizeof(T));

                is.fatalCheck
                (
                    "operator>>(Istream&, List<T>&) : reading the binary block"
                );
            }
        }
    }
    else if (firstToken.isPunctuation())
    {
        if (firstToken.pToken() != token::BEGIN_LIST)
        {
            FatalIOErrorInFunction(is)
                << "incorrect first token, expected '(', found "
                << firstToken.info()
                << exit(FatalIOError);
        }

        // Putback the opening bracket
        is.putBack(firstToken);

        // Now read as a singly-linked list
        SLList<T> sll(is);

        // Convert the singly-linked list to this list
        L = sll;
    }
    else
    {
        FatalIOErrorInFunction(is)
            << "incorrect first token, expected <int> or '(', found "
            << firstToken.info()
            << exit(FatalIOError);
    }

    return is;
}


template<class T>
CML::List<T> CML::readList(Istream& is)
{
    List<T> L;
    token firstToken(is);
    is.putBack(firstToken);

    if (firstToken.isPunctuation())
    {
        if (firstToken.pToken() != token::BEGIN_LIST)
        {
            FatalIOErrorInFunction(is)
                << "incorrect first token, expected '(', found "
                << firstToken.info()
                << exit(FatalIOError);
        }

        // Read via a singly-linked list
        L = SLList<T>(is);
    }
    else
    {
        // Create list with a single item
        L.setSize(1);

        is >> L[0];
    }

    return L;
}


#endif

// ************************************************************************* //
