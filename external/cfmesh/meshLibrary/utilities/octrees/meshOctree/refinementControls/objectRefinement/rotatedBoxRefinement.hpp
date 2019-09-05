/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
Copyright (C) 2019 Applied CCM Pty Ltd.
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

Class
    rotatedBoxRefinement

Description
    Checks if a box is contained inside the box object

Author: Franjo Juretic (franjo.juretic@c-fields.com)
        Darrin Stephens, Applied CCM Pty Ltd

SourceFiles
    rotatedBoxRefinement.cpp

\*---------------------------------------------------------------------------*/

#ifndef rotatedBoxRefinement_HPP
#define rotatedBoxRefinement_HPP

#include "objectRefinement.hpp"
#include "point.hpp"
#include "typeInfo.hpp"
#include "treeBoundBox.hpp"
#include "boundBox.hpp"
#include "coordinateSystem.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class rotatedBoxRefinement Declaration
\*---------------------------------------------------------------------------*/

class rotatedBoxRefinement
:
    public objectRefinement
{
    // Private data

        //- box minium
        point min_;

        //- box maxium
        point max_;

        //- origin of rotation
        point origin_;

        //- rotation axis    
        vector axis_;

        //- rotation direction
        vector direction_;

        //- transformation from local to global co-ordinates
        coordinateSystem transform_;

        //- bound box in local co-ordinates
        boundBox box_;

public:

    //- Runtime type information
    TypeName("rotatedBox");


    // Constructors

        //- Null construct
        rotatedBoxRefinement();

        //- Construct from name, cell size, min, max and rotation
        rotatedBoxRefinement
        (
            const word& name,
            const scalar cellSize,
            const direction additionalRefLevels,
            const point& min,
            const point& max,
            const point& origin,
            const vector& axis,
            const vector& direction
        );

        //- Construct from dictionary
        rotatedBoxRefinement(const word& name, const dictionary& dict);
        
        //- Construct and return a clone
        virtual autoPtr<objectRefinement> clone
        (
            const rotatedBoxRefinement& sr
        ) const
        {
            return autoPtr<objectRefinement>
            (
                new rotatedBoxRefinement
                (
                    sr.name(),
                    sr.cellSize(),
                    sr.additionalRefinementLevels(),
                    sr.min_,
                    sr.max_,
                    sr.origin_,
                    sr.axis_,
                    sr.direction_
                )
            );
        }
    
    // Member Functions
   
        //- check if a boundBox intersects or is inside the object
        bool intersectsObject(const boundBox&) const;
        
        //- Return as dictionary of entries
        dictionary dict(bool ignoreType = false) const;

      // Write

        //- Write
        void write(Ostream&) const;

        //- Write dictionary
        void writeDict(Ostream&, bool subDict = true) const;
        
    // Member Operators

        //- assign from dictionary
        void operator=(const dictionary&);

    // IOstream Operators

        Ostream& operator<<(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
