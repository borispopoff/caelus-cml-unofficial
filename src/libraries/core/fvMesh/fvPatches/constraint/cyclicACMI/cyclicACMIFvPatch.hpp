/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2018 OpenFOAM Foundation
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
    CML::cyclicACMIFvPatch

Description
    Cyclic patch for Arbitrarily Coupled Mesh Interface (ACMI)

SourceFiles
    cyclicACMIFvPatch.cpp

\*---------------------------------------------------------------------------*/

#ifndef cyclicACMIFvPatch_H
#define cyclicACMIFvPatch_H

#include "cyclicAMIFvPatch.hpp"
#include "cyclicACMIPolyPatch.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                      Class cyclicACMIFvPatch Declaration
\*---------------------------------------------------------------------------*/

class cyclicACMIFvPatch
:
    public cyclicAMIFvPatch
{
private:

    // Private member functions

        //- Update the patch areas after AMI update
        void updateAreas() const;


protected:

    // Protected Member functions

        //- Make patch weighting factors
        virtual void makeWeights(scalarField&) const;


public:

    //- Runtime type information
    TypeName(cyclicACMIPolyPatch::typeName_());


    // Constructors

        //- Construct from polyPatch
        cyclicACMIFvPatch(const polyPatch& patch, const fvBoundaryMesh& bm)
        :
           cyclicAMIFvPatch(patch, bm)
        {}


    // Member functions

        // Access

            //- Return a reference to the AMI interpolators
            virtual const PtrList<AMIInterpolation>& AMIs() const
            {
                const PtrList<AMIInterpolation>& AMIs =
                    cyclicAMIFvPatch::AMIs();

                updateAreas();

                return AMIs;
            }

            //- Return a reference to the AMI transformations
            virtual const List<vectorTensorTransform>& AMITransforms() const
            {
                const List<vectorTensorTransform>& AMITransforms =
                    cyclicAMIFvPatch::AMITransforms();

                updateAreas();

                return AMITransforms;
            }

            //- Return the poly patch
            const cyclicACMIPolyPatch& cyclicACMIPatch() const
            {
                return refCast<const cyclicACMIPolyPatch>(this->patch());
            }

            //- Return the neighbour patch
            const cyclicACMIFvPatch& neighbFvPatch() const
            {
                return refCast<const cyclicACMIFvPatch>
                (
                    this->boundaryMesh()[this->cyclicAMIPatch().neighbPatchID()]
                );
            }

            //- Return the non-overlap patch
            const fvPatch& nonOverlapFvPatch() const
            {
                return this->boundaryMesh()
                [
                    cyclicACMIPatch().nonOverlapPatchID()
                ];
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
