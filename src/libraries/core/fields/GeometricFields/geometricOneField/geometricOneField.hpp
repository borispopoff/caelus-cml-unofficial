/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2018 OpenFOAM Foundation
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
    CML::geometricOneField

Description
    A class representing the concept of a GeometricField of 1 used to avoid
    unnecessary manipulations for objects which are known to be one at
    compile-time.

    Used for example as the density argument to a function written for
    compressible to be used for incompressible flow.

\*---------------------------------------------------------------------------*/

#ifndef geometricOneField_H
#define geometricOneField_H

#include "oneFieldField.hpp"
#include "dimensionSet.hpp"
#include "scalar.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                      Class geometricOneField Declaration
\*---------------------------------------------------------------------------*/

class geometricOneField
:
    public one
{

public:

    // Public typedefs

        typedef oneField Internal;
        typedef oneField Patch;
        typedef oneFieldField Boundary;
        typedef one cmptType;

    // Constructors

        //- Construct null
        geometricOneField()
        {}


    // Member Operators

        inline const dimensionSet& dimensions() const;

        inline one operator[](const label) const;

        inline oneField field() const;

        inline oneField oldTime() const;

        inline Internal operator()() const;

        inline Internal v() const;

        inline typename Internal::FieldType internalField() const;

        inline Boundary boundaryField() const;
};


inline const geometricOneField& operator*
(
    const geometricOneField&,
    const geometricOneField&
);

inline const geometricOneField::Internal& operator*
(
    const geometricOneField::Internal&,
    const geometricOneField&
);

inline const geometricOneField::Internal& operator*
(
    const geometricOneField&,
    const geometricOneField::Internal&
);

inline const geometricOneField& operator/
(
    const geometricOneField&,
    const geometricOneField&
);

inline const geometricOneField::Internal& operator/
(
    const geometricOneField::Internal&,
    const geometricOneField&
);

inline const geometricOneField::Internal& operator/
(
    const geometricOneField&,
    const geometricOneField::Internal&
);


} // End namespace CML


inline const CML::dimensionSet& CML::geometricOneField::dimensions() const
{
    return dimless;
}


inline CML::one CML::geometricOneField::operator[](const label) const
{
    return one();
}


inline CML::oneField CML::geometricOneField::field() const
{
    return oneField();
}


inline CML::oneField CML::geometricOneField::oldTime() const
{
    return oneField();
}


inline CML::geometricOneField::Internal
CML::geometricOneField::operator()() const
{
    return Internal();
}


inline CML::geometricOneField::Internal
CML::geometricOneField::v() const
{
    return Internal();
}


inline typename CML::geometricOneField::Internal::FieldType
CML::geometricOneField::internalField() const
{
    return typename Internal::FieldType();
}

inline CML::geometricOneField::Boundary
CML::geometricOneField::boundaryField() const
{
    return Boundary();
}


inline const CML::geometricOneField& CML::operator*
(
    const geometricOneField& gof,
    const geometricOneField&
)
{
    return gof;
}


inline const CML::geometricOneField::Internal& CML::operator*
(
    const geometricOneField::Internal
& of,
    const geometricOneField&
)
{
    return of;
}


inline const CML::geometricOneField::Internal& CML::operator*
(
    const geometricOneField&,
    const geometricOneField::Internal& of
)
{
    return of;
}


inline const CML::geometricOneField& CML::operator/
(
    const geometricOneField& gof,
    const geometricOneField&
)
{
    return gof;
}


inline const CML::geometricOneField::Internal& CML::operator/
(
    const geometricOneField::Internal& of,
    const geometricOneField&
)
{
    return of;
}


inline const CML::geometricOneField::Internal& CML::operator/
(
    const geometricOneField&,
    const geometricOneField::Internal& of
)
{
    return of;
}


#endif
