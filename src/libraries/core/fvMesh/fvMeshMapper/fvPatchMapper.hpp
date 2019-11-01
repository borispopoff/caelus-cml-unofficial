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
    CML::fvPatchMapper

Description
    Mapping class for a fvPatchField.

SourceFiles
    fvPatchMapper.cpp

\*---------------------------------------------------------------------------*/

#ifndef fvPatchMapper_H
#define fvPatchMapper_H

#include "fvPatchFieldMapper.hpp"
#include "faceMapper.hpp"
#include "fvPatch.hpp"
#include "primitiveFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes
class fvPatch;
class mapPolyMesh;

/*---------------------------------------------------------------------------*\
                       Class fvPatchMapper Declaration
\*---------------------------------------------------------------------------*/

class fvPatchMapper
:
    public fvPatchFieldMapper
{
    // Private data

        //- Reference to patch
        const fvPatch& patch_;

        //- Reference to face mapper
        const faceMapper& faceMap_;

        //- Size before mapping
        const label sizeBeforeMapping_;


    // Demand-driven private data

        mutable bool hasUnmapped_;

        //- Direct addressing (only one for of addressing is used)
        mutable labelList* directAddrPtr_;

        //- Interpolated addressing (only one for of addressing is used)
        mutable labelListList* interpolationAddrPtr_;

        //- Interpolation weights
        mutable scalarListList* weightsPtr_;


    // Private Member Functions

        //- Calculate addressing for mapping with inserted cells
        void calcAddressing() const;

        //- Clear out local storage
        void clearOut();


public:

    // Static data members

    // Constructors

        //- Construct from mappers
        fvPatchMapper
        (
            const fvPatch& patch,
            const faceMapper& faceMap
        );

        //- Disallow default bitwise copy construct
        fvPatchMapper
        (
            const fvPatchMapper&
        ) = delete;


    //- Destructor
    virtual ~fvPatchMapper();


    // Member Functions

        //- Return size
        virtual label size() const
        {
            return patch_.size();
        }

        //- Return size of field before mapping
        virtual label sizeBeforeMapping() const
        {
            return sizeBeforeMapping_;
        }

        //- Is the mapping direct
        virtual bool direct() const
        {
            return faceMap_.direct();
        }

        virtual bool hasUnmapped() const
        {
            return hasUnmapped_;
        }

        //- Return direct addressing
        virtual const labelUList& directAddressing() const;

        //- Return interpolated addressing
        virtual const labelListList& addressing() const;

        //- Return interpolaion weights
        virtual const scalarListList& weights() const;


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const fvPatchMapper&) = delete;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
