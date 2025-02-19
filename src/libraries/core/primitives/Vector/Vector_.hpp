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
    CML::Vector

Description
    Templated 3D Vector derived from VectorSpace adding construction from
    3 components, element access using x(), y() and z() member functions and
    the inner-product (dot-product) and cross product operators.

    A centre() member function which returns the Vector for which it is called
    is defined so that point which is a typedef to Vector\<scalar\> behaves as
    other shapes in the shape hierarchy.

SourceFiles
    VectorI.hpp

\*---------------------------------------------------------------------------*/

#ifndef Vector__H
#define Vector__H

#include "VectorSpace.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

template<class T> class List;

/*---------------------------------------------------------------------------*\
                           Class Vector Declaration
\*---------------------------------------------------------------------------*/

template<class Cmpt>
class Vector
:
    public VectorSpace<Vector<Cmpt>, Cmpt, 3>
{

public:

    //- Equivalent type of labels used for valid component indexing
    typedef Vector<label> labelType;


    // Member constants

        //- Rank of Vector is 1
        static const direction rank = 1;


    //- Component labeling enumeration
    enum components { X, Y, Z };


    // Constructors

        //- Construct null
        inline Vector();

        //- Construct initialized to zero
        inline Vector(const CML::zero);

        //- Construct given VectorSpace of the same rank
        template<class Cmpt2>
        inline Vector(const VectorSpace<Vector<Cmpt2>, Cmpt2, 3>&);

        //- Construct given three components
        inline Vector(const Cmpt& vx, const Cmpt& vy, const Cmpt& vz);

        //- Construct from Istream
        inline Vector(Istream&);


    // Member Functions

        // Access

            inline const Cmpt& x() const;
            inline const Cmpt& y() const;
            inline const Cmpt& z() const;

            inline Cmpt& x();
            inline Cmpt& y();
            inline Cmpt& z();

            //- Return i-th component.  Consistency with hyperVector
            inline const Cmpt& operator()
            (
                const direction i
            ) const;

            //- Return i-th component.  Consistency with hyperVector
            inline  Cmpt& operator()
            (
                const direction i
            );

        //- Normalise the vector by its magnitude
        inline Vector<Cmpt>& normalise();


        //- Return *this (used for point which is a typedef to Vector<scalar>.
        inline const Vector<Cmpt>& centre
        (
            const CML::List<Vector<Cmpt> >&
        ) const;

};


template<class Cmpt>
class typeOfRank<Cmpt, 1>
{
public:

    typedef Vector<Cmpt> type;
};


template<class Cmpt>
class symmTypeOfRank<Cmpt, 1>
{
public:

    typedef Vector<Cmpt> type;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "VectorI.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
