/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
-------------------------------------------------------------------------------
License
    This file is part of cfMesh.

    cfMesh is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    cfMesh is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with cfMesh.  If not, see <http://www.gnu.org/licenses/>.

Author: Franjo Juretic (franjo.juretic@c-fields.com)

\*---------------------------------------------------------------------------*/

template<class T, CML::label staticSize>
inline T* CML::DynList<T, staticSize>::data()
{
    return dataPtr_;
}


template<class T, CML::label staticSize>
inline const T* CML::DynList<T, staticSize>::data() const
{
    return dataPtr_;
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::allocateSize(const label s)
{
    checkAllocation();

    if (s > staticSize)
    {
        if (s > nAllocated_)
        {
            //- allocates enough space for the elements
            T* newData = new T[s];

            for (label i = 0; i < nextFree_; ++i)
            {
                newData[i] = this->operator[](i);
            }

            if (nAllocated_ > staticSize)
            {
                delete[] dataPtr_;
            }

            dataPtr_ = newData;
            nAllocated_ = s;
        }
        else if (s < nAllocated_)
        {
            //- shrinks the list
            T* newData = new T[s];

            for (label i = 0; i < s; ++i)
            {
                newData[i] = this->operator[](i);
            }

            delete[] dataPtr_;

            dataPtr_ = newData;
            nAllocated_ = s;
        }
    }
    else
    {
        if (nAllocated_ > staticSize)
        {
            //- delete dynamically allocated data
            for (label i = 0; i < s; ++i)
            {
                staticData_[i] = dataPtr_[i];
            }

            delete[] dataPtr_;
        }

        dataPtr_ = staticData_;
        nAllocated_ = staticSize;
    }
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::checkIndex(const label i) const
{
    if ((i < 0) || (i >= nextFree_))
    {
        FatalErrorInFunction
            << "Index " << i << " is not in range " << 0
            << " and " << nextFree_ << abort(FatalError);
    }
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::checkAllocation() const
{
    if (nextFree_ > nAllocated_)
    {
        FatalErrorInFunction
            << "nextFree_ is out of scope 0 " << " and " << nAllocated_
            << abort(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T, CML::label staticSize>
inline CML::DynList<T, staticSize>::DynList()
:
    dataPtr_(nullptr),
    nAllocated_(0),
    staticData_(),
    nextFree_(0)
{
    setSize(0);

    # ifdef DEBUG
    checkAllocation();
    # endif

}


template<class T, CML::label staticSize>
inline CML::DynList<T, staticSize>::DynList(const label s)
:
    dataPtr_(nullptr),
    nAllocated_(0),
    staticData_(),
    nextFree_(0)
{
    setSize(s);

    # ifdef DEBUG
    checkAllocation();
    # endif

}


template<class T, CML::label staticSize>
inline CML::DynList<T, staticSize>::DynList(const label s, const T& val)
:
    dataPtr_(nullptr),
    nAllocated_(0),
    staticData_(),
    nextFree_(0)
{
    setSize(s);

    for (label i = 0; i < s; ++i)
    {
        this->operator[](i) = val;
    }

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
inline CML::DynList<T, staticSize>::DynList(const UList<T>& ul)
:
    dataPtr_(nullptr),
    nAllocated_(0),
    staticData_(),
    nextFree_(0)
{
    setSize(ul.size());

    forAll(ul, i)
    {
        this->operator[](i) = ul[i];
    }

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
template<class ListType>
inline CML::DynList<T, staticSize>::DynList(const ListType& l)
:
    dataPtr_(nullptr),
    nAllocated_(0),
    staticData_(),
    nextFree_(0)
{
    setSize(l.size());
    for (label i = 0; i < nextFree_; ++i)
    {
        this->operator[](i) = l[i];
    }

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
inline CML::DynList<T, staticSize>::DynList
(
    const DynList<T, staticSize>& dl
)
:
    dataPtr_(nullptr),
    nAllocated_(0),
    staticData_(),
    nextFree_(0)
{
    setSize(dl.size());
    for (label i = 0; i < nextFree_; ++i)
    {
        this->operator[](i) = dl[i];
    }

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
inline CML::DynList<T, staticSize>::~DynList()
{
    allocateSize(0);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T, CML::label staticSize>
inline CML::label CML::DynList<T, staticSize>::size() const
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    return nextFree_;
}


template<class T, CML::label staticSize>
inline CML::label CML::DynList<T, staticSize>::byteSize() const
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    if (!contiguous<T>())
    {
        FatalErrorInFunction
            << "Cannot return the binary size of a list of "
            << "non - primitive elements"
            << abort(FatalError);
    }

    return nextFree_*sizeof(T);
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::setSize(const label s)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    allocateSize(s);
    nextFree_ = s;

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::clear()
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    nextFree_ = 0;
}


template<class T, CML::label staticSize>
void CML::DynList<T, staticSize>::shrink()
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    allocateSize(nextFree_);

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::append(const T& e)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    if (nextFree_ >= nAllocated_)
    {
        const label newSize = 2*nAllocated_ + 2;
        allocateSize(newSize);
    }

    # ifdef DEBUG
    checkAllocation();
    # endif

    this->operator[](nextFree_++) = e;
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::appendIfNotIn(const T& e)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    if (!contains(e))
    {
         append(e);
    }

    # ifdef DEBUG
    checkAllocation();
    # endif
}


template<class T, CML::label staticSize>
inline bool CML::DynList<T, staticSize>::contains(const T& e) const
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    for (label i = 0; i < nextFree_; ++i)
    {
        if (this->operator[](i) == e)
        {
            return true;
        }
    }

    return false;
}


template<class T, CML::label staticSize>
inline CML::label CML::DynList<T, staticSize>::containsAtPosition
(
    const T& e
) const
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    for (label i = 0; i < nextFree_; ++i)
    {
        if (this->operator[](i) == e)
        {
            return i;
        }
    }

    return -1;
}


template<class T, CML::label staticSize>
inline const T& CML::DynList<T, staticSize>::lastElement() const
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    return this->operator[](nextFree_ - 1);
}


template<class T, CML::label staticSize>
inline T CML::DynList<T, staticSize>::removeLastElement()
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    if (nextFree_ == 0)
    {
        FatalErrorInFunction
            << "List is empty" << abort(FatalError);
    }

    T el = this->operator[](nextFree_ - 1);
    --nextFree_;
    return el;
}


template<class T, CML::label staticSize>
inline T CML::DynList<T, staticSize>::removeElement(const label i)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    if (nextFree_ == 0)
    {
        FatalErrorInFunction
            << "List is empty" << abort(FatalError);
    }

    T el = this->operator[](i);
    this->operator[](i) = this->operator[](nextFree_ - 1);
    --nextFree_;

    # ifdef DEBUG
    checkAllocation();
    # endif

    return el;
}


template<class T, CML::label staticSize>
inline T& CML::DynList<T, staticSize>::newElmt(const label i)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    return this->operator()(i);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T, CML::label staticSize>
inline T& CML::DynList<T, staticSize>::operator()(const label i)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    nextFree_ = CML::max(nextFree_, i + 1);

    if (nextFree_ >= nAllocated_)
    {
        allocateSize(2*nextFree_ + 1);
    }

    # ifdef DEBUG
    checkAllocation();
    # endif

    return this->operator[](i);
}


template<class T, CML::label staticSize>
inline const T& CML::DynList<T, staticSize>::operator[](const label i) const
{
    # ifdef FULLDEBUG
    checkAllocation();
    checkIndex(i);
    # endif

    return dataPtr_[i];
}


template<class T, CML::label staticSize>
inline T& CML::DynList<T, staticSize>::operator[](const label i)
{
    # ifdef FULLDEBUG
    checkAllocation();
    checkIndex(i);
    # endif

    return dataPtr_[i];
}


template<class T, CML::label staticSize>
inline CML::label CML::DynList<T, staticSize>::fcIndex
(
    const label index,
    const label offset
) const
{
    return (index + offset) % nextFree_;
}


template<class T, CML::label staticSize>
inline CML::label CML::DynList<T, staticSize>::rcIndex
(
    const label index,
    const label offset
) const
{
    return (index + nextFree_ - offset) % nextFree_;
}


template<class T, CML::label staticSize>
inline const T& CML::DynList<T, staticSize>::fcElement
(
    const label index,
    const label offset
) const
{
    return operator[](fcIndex(index, offset));
}


template<class T, CML::label staticSize>
inline const T& CML::DynList<T, staticSize>::rcElement
(
    const label index,
    const label offset
) const
{
    return operator[](rcIndex(index, offset));
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::operator=(const T& t)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    for (label i = 0; i < nextFree_; ++i)
    {
        operator[](i) = t;
    }
}


template<class T, CML::label staticSize>
inline void CML::DynList<T, staticSize>::operator=
(
    const DynList<T, staticSize>& dl
)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    allocateSize(dl.size());
    nextFree_ = dl.size();

    # ifdef DEBUG
    checkAllocation();
    # endif

    for (label i = 0; i < nextFree_; ++i)
    {
        this->operator[](i) = dl[i];
    }
}


template<class T, CML::label staticSize>
template<class ListType>
inline void CML::DynList<T, staticSize>::operator=(const ListType& l)
{
    # ifdef DEBUG
    checkAllocation();
    # endif

    allocateSize(l.size());
    nextFree_ = l.size();

    # ifdef DEBUG
    checkAllocation();
    # endif

    for (label i = 0; i < nextFree_; ++i)
    {
        this->operator[](i) = l[i];
    }
}


template<class T, CML::label staticSize>
inline bool CML::DynList<T, staticSize>::operator==
(
    const DynList<T, staticSize>& DL
) const
{
    if (nextFree_ != DL.nextFree_)
    {
        return false;
    }

    forAll(DL, i)
    {
        if (this->operator[](i) != DL[i])
        {
            return false;
        }
    }

    return true;
}


template<class T, CML::label staticSize>
inline bool CML::DynList<T, staticSize>::operator!=
(
    const DynList<T, staticSize>& DL
) const
{
    return !operator==(DL);
}


// ************************************************************************* //
