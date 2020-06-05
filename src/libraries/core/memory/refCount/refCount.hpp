/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
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
    CML::refCount

Description
    Reference counter for various CAELUS components.

\*---------------------------------------------------------------------------*/

#ifndef refCount_H
#define refCount_H

#include "bool.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                          Class refCount Declaration
\*---------------------------------------------------------------------------*/

class refCount
{
    // Private data

        int count_;


    // Private Member Functions

        //- Dissallow copy
        refCount(const refCount&);

        //- Dissallow bitwise assignment
        void operator=(const refCount&);


protected:

    // Constructors

        //- Construct null initializing count to 0
        refCount()
        :
            count_(0)
        {}


public:

    // Member Functions

        //- Return the current reference count
        int count() const
        {
            return count_;
        }

        //- Return true if the reference count is zero
        bool unique() const
        {
            return count_ == 0;
        }


    // Member Operators

        //- Increment the reference count
        void operator++()
        {
            count_++;
        }

        //- Increment the reference count
        void operator++(int)
        {
            count_++;
        }

        //- Decrement the reference count
        void operator--()
        {
            count_--;
        }

        //- Decrement the reference count
        void operator--(int)
        {
            count_--;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
