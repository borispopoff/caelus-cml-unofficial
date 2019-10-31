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
    CML::ensightParts

Description
    A collection of several ensightPart elements

SourceFiles
    ensightParts.cpp
    ensightPartsTemplates.cpp

\*---------------------------------------------------------------------------*/

#ifndef ensightParts_H
#define ensightParts_H

#include "ensightPart.hpp"
#include "ensightPartFaces.hpp"
#include "ensightPartCells.hpp"
#include "volFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of friend functions and operators

class ensightParts;

ensightGeoFile& operator<<(ensightGeoFile&, const ensightParts&);


/*---------------------------------------------------------------------------*\
                       Class ensightParts Declaration
\*---------------------------------------------------------------------------*/

class ensightParts
{
    // Private Data

        //- List of parts
        PtrList<ensightPart> partsList_;

    // Private Member Functions

        //- Disallow default bitwise copy construct
        ensightParts(const ensightParts&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const ensightParts&) = delete;


public:

    // Constructors

        //- Construct from polyMesh
        ensightParts(const polyMesh&);

        //- Construct from IOobject
        ensightParts(const IOobject&);


    //- Destructor
    ~ensightParts();


    // Member functions

        //- Clear old information and construct anew from polyMesh
        void recalculate(const polyMesh&);

        //- Renumber elements
        void renumber
        (
            const labelUList& origCellId,
            const labelUList& origFaceId
        );

        //- Number of parts
        label size() const
        {
            return partsList_.size();
        }

        //- Write the geometry
        void writeGeometry(ensightGeoFile&) const;

        //- Write summary information about the objects
        bool writeSummary(Ostream&) const;

        //- Write the lists
        void writeData(Ostream&) const;

        //- Write (volume) scalar field
        //  optionally write data for face parts
        //  optionally write data per node
        void writeScalarField
        (
            ensightFile&,
            const List<scalar>& field,
            const bool useFaceData = false,
            const bool perNode = false
        ) const;

        //- Write (volume) vector field components
        //  optionally write data for face parts
        //  optionally write data per node
        void writeVectorField
        (
            ensightFile&,
            const List<scalar>& field0,
            const List<scalar>& field1,
            const List<scalar>& field2,
            const bool useFaceData = false,
            const bool perNode = false
        ) const;


        //- Write generalized volume field components
        template<class Type>
        void writeField
        (
            ensightFile&,
            const GeometricField<Type, fvPatchField, volMesh>&
        ) const;


    // Friend Operators

        //- Write geometry
        friend ensightGeoFile& operator<<(ensightGeoFile&, const ensightParts&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void CML::ensightParts::writeField
(
    ensightFile& os,
    const GeometricField<Type, fvPatchField, volMesh>& field
) const
{
    // find offset to patch parts (ie, the first face data)
    label patchOffset = 0;
    forAll(partsList_, partI)
    {
        if (partsList_[partI].isFaceData())
        {
            patchOffset = partI;
            break;
        }
    }

    forAll(partsList_, partI)
    {
        label patchi = partI - patchOffset;

        if (partsList_[partI].isCellData())
        {
            partsList_[partI].writeField
            (
                os,
                field
            );
        }
        else if (patchi < field.boundaryField().size())
        {
            partsList_[partI].writeField
            (
                os,
                field.boundaryField()[patchi]
            );
        }
    }
}


// ************************************************************************* //

#endif

// ************************************************************************* //
