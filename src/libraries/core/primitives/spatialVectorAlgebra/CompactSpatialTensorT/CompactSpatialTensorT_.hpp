/*---------------------------------------------------------------------------*\
Copyright (C) 2016-2018 OpenFOAM Foundation
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
    CML::CompactSpatialTensorT

Description
    Templated 3D transposed compact spatial tensor derived from MatrixSpace
    used to represent transformations of spatial vectors of rigid bodies.

    Reference:
    \verbatim
        Featherstone, R. (2008).
        Rigid body dynamics algorithms.
        Springer.
    \endverbatim

SourceFiles
    CompactSpatialTensorTI.hpp

See also
    CML::MatrixSpace
    CML::CompactSpatialTensor

\*---------------------------------------------------------------------------*/

#ifndef CompactSpatialTensorT_HPP
#define CompactSpatialTensorT_HPP

#include "CompactSpatialTensor_.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                     Class CompactSpatialTensor Declaration
\*---------------------------------------------------------------------------*/

template<class Cmpt>
class CompactSpatialTensorT
:
    public MatrixSpace<CompactSpatialTensorT<Cmpt>, Cmpt, 3, 6>
{

public:

    // Constructors

        //- Construct null
        inline CompactSpatialTensorT();

        inline CompactSpatialTensorT(const CML::zero);

        //- Construct given MatrixSpace of the same rank
        inline CompactSpatialTensorT
        (
            const typename CompactSpatialTensorT::msType&
        );

        //- Construct given 18 components
        inline CompactSpatialTensorT
        (
            const Cmpt& t00, const Cmpt& t01, const Cmpt& t02,
            const Cmpt& t10, const Cmpt& t11, const Cmpt& t12,
            const Cmpt& t20, const Cmpt& t21, const Cmpt& t22,
            const Cmpt& t30, const Cmpt& t31, const Cmpt& t32,
            const Cmpt& t40, const Cmpt& t41, const Cmpt& t42,
            const Cmpt& t50, const Cmpt& t51, const Cmpt& t52
        );

        //- Construct from Istream
        inline CompactSpatialTensorT(Istream&);
};


template<class Cmpt>
class typeOfTranspose<Cmpt, CompactSpatialTensor<Cmpt> >
{
public:

    typedef CompactSpatialTensorT<Cmpt> type;
};


template<class Cmpt>
class typeOfTranspose<Cmpt, CompactSpatialTensorT<Cmpt> >
{
public:

    typedef CompactSpatialTensor<Cmpt> type;
};


template<class Cmpt>
class typeOfInnerProduct
<
    Cmpt,
    CompactSpatialTensor<Cmpt>,
    CompactSpatialTensorT<Cmpt>
>
{
public:

    typedef SpatialTensor<Cmpt> type;
};


template<class Cmpt>
class typeOfInnerProduct
<
    Cmpt,
    CompactSpatialTensorT<Cmpt>,
    CompactSpatialTensor<Cmpt>
>
{
public:

    typedef Tensor<Cmpt> type;
};


template<class Cmpt>
class typeOfInnerProduct
<
    Cmpt,
    CompactSpatialTensorT<Cmpt>,
    SpatialVector<Cmpt>
>
{
public:

    typedef Vector<Cmpt> type;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Include inline implementations
#include "CompactSpatialTensorTI.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
