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
    CML::lduPrimitiveMesh

Description
    Simplest contrete lduMesh which stores the addressing needed by lduMatrix.

\*---------------------------------------------------------------------------*/

#ifndef lduPrimitiveMesh_H
#define lduPrimitiveMesh_H

#include "lduMesh.hpp"
#include "labelList.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                       Class lduPrimitiveMesh Declaration
\*---------------------------------------------------------------------------*/

class lduPrimitiveMesh
:
    public lduMesh,
    public lduAddressing
{
    // Private data

        //- Lower addressing
        labelList lowerAddr_;

        //- Upper addressing
        labelList upperAddr_;

        //- Patch to internal addressing
        labelListList patchAddr_;

        //- List of pointers for each patch
        //  with only those pointing to interfaces being set
        lduInterfacePtrsList interfaces_;

        //- Patch field evaluation schedule.
        lduSchedule patchSchedule_;


public:

    // Constructors

        //- Construct from components but without interfaces. Add interfaces
        //  separately using addInterfaces
        lduPrimitiveMesh
        (
            const label nCells,
            labelList& l,
            labelList& u,
            bool reuse
        )
        :
            lduAddressing(nCells),
            lowerAddr_(l, reuse),
            upperAddr_(u, reuse)
        {}

        //- Add interfaces to a mesh constructed without
        void addInterfaces
        (
            const lduInterfacePtrsList& interfaces,
            const labelListList& pa,
            const lduSchedule& ps
        )
        {
            interfaces_ = interfaces;
            patchAddr_ = pa;
            patchSchedule_ = ps;
        }

        //- Clear interfaces
        void clearInterfaces()
        {
            forAll (interfaces_, i)
            {
                if (interfaces_.set(i))
                {
                    delete interfaces_(i);
                }
            }
        }

        //- Construct from components as copies
        lduPrimitiveMesh
        (
            const label nCells,
            const labelUList& l,
            const labelUList& u,
            const labelListList& pa,
            lduInterfacePtrsList interfaces,
            const lduSchedule& ps
        )
        :
            lduAddressing(nCells),
            lowerAddr_(l),
            upperAddr_(u),
            patchAddr_(pa),
            interfaces_(interfaces),
            patchSchedule_(ps)
        {}


        //- Construct from components and  re-use storage as specified.
        lduPrimitiveMesh
        (
            const label nCells,
            labelList& l,
            labelList& u,
            labelListList& pa,
            lduInterfacePtrsList interfaces,
            const lduSchedule& ps,
            bool reuse
        )
        :
            lduAddressing(nCells),
            lowerAddr_(l, reuse),
            upperAddr_(u, reuse),
            patchAddr_(pa, reuse),
            interfaces_(interfaces, reuse),
            patchSchedule_(ps)
        {}

        //- Disallow default bitwise copy construct
        lduPrimitiveMesh(const lduPrimitiveMesh&) = delete;


    //- Destructor
    virtual ~lduPrimitiveMesh()
    {}


    // Member Functions

            //- Return number of interfaces
            virtual label nPatches() const
            {
                return interfaces_.size();
            }

        // Access

            //- Return ldu addressing
            virtual const lduAddressing& lduAddr() const
            {
                return *this;
            }

            //- Return a list of pointers for each patch
            //  with only those pointing to interfaces being set
            virtual lduInterfacePtrsList interfaces() const
            {
                return interfaces_;
            }

            //- Return Lower addressing
            virtual const labelUList& lowerAddr() const
            {
                return lowerAddr_;
            }

            //- Return Upper addressing
            virtual const labelUList& upperAddr() const
            {
                return upperAddr_;
            }

            //- Return patch addressing
            virtual const labelUList& patchAddr(const label i) const
            {
                return patchAddr_[i];
            }

            virtual const labelUList& getPatchAddr(const label i) const
            {
	            return patchAddr_[i];
            }

            //- Return patch evaluation schedule
            virtual const lduSchedule& patchSchedule() const
            {
                return patchSchedule_;
            }


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const lduPrimitiveMesh&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
