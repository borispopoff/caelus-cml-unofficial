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
    CML::cyclicAMIPointPatch

Description
    Cyclic AMI point patch - place holder only

SourceFiles
    cyclicAMIPointPatch.cpp

\*---------------------------------------------------------------------------*/

#ifndef cyclicAMIPointPatch_H
#define cyclicAMIPointPatch_H

#include "coupledFacePointPatch.hpp"
#include "cyclicAMIPolyPatch.hpp"
#include "pointBoundaryMesh.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                      Class cyclicAMIPointPatch Declaration
\*---------------------------------------------------------------------------*/

class cyclicAMIPointPatch
:
    public coupledFacePointPatch
{
    // Private data

        //- Local reference cast into the cyclic AMI patch
        const cyclicAMIPolyPatch& cyclicAMIPolyPatch_;


    // Private Member Functions

        //- Disallow default construct as copy
        cyclicAMIPointPatch(const cyclicAMIPointPatch&);

        //- Disallow default assignment
        void operator=(const cyclicAMIPointPatch&);


protected:

    // Protected Member Functions

        //- Initialise the calculation of the patch geometry
        virtual void initGeometry(PstreamBuffers&);

        //- Calculate the patch geometry
        virtual void calcGeometry(PstreamBuffers&);

        //- Initialise the patches for moving points
        virtual void initMovePoints(PstreamBuffers&, const pointField&);

        //- Correct patches after moving points
        virtual void movePoints(PstreamBuffers&, const pointField&);

        //- Initialise the update of the patch topology
        virtual void initUpdateMesh(PstreamBuffers&);

        //- Update of the patch topology
        virtual void updateMesh(PstreamBuffers&);


public:

    //- Runtime type information
    TypeName(cyclicAMIPolyPatch::typeName_());


    // Constructors

        //- Construct from components
        cyclicAMIPointPatch
        (
            const polyPatch& patch,
            const pointBoundaryMesh& bm
        );


    //- Destructor
    virtual ~cyclicAMIPointPatch();


    // Member Functions

        //- Is patch 'coupled'. Note that on AMI the geometry is not
        //  coupled but the fields are!
        virtual bool coupled() const
        {
            return false;
        }

        //- Return the constraint type this pointPatch implements.
        virtual const word& constraintType() const
        {
            return type();
        }

        //- Return the underlying cyclicAMIPolyPatch
        const cyclicAMIPolyPatch& cyclicAMIPatch() const
        {
            return cyclicAMIPolyPatch_;
        }

        //- Return neighbour point patch
        const cyclicAMIPointPatch& neighbPatch() const
        {
            label patchi = cyclicAMIPolyPatch_.neighbPatchID();
            const pointPatch& pp = this->boundaryMesh()[patchi];
            return refCast<const cyclicAMIPointPatch>(pp);
        }

        //- Are the cyclic planes parallel
        bool parallel() const
        {
            return cyclicAMIPolyPatch_.parallel();
        }

        //- Return face transformation tensor
        const tensorField& forwardT() const
        {
            return cyclicAMIPolyPatch_.forwardT();
        }

        //- Return neighbour-cell transformation tensor
        const tensorField& reverseT() const
        {
            return cyclicAMIPolyPatch_.reverseT();
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
