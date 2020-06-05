/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
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
    CML::fv::interRegionOption

Description
    Base class for inter region exchange.

\*---------------------------------------------------------------------------*/

#ifndef interRegionOption_HPP
#define interRegionOption_HPP

#include "fvOption.hpp"
#include "volFields.hpp"
#include "autoPtr.hpp"
#include "meshToMesh.hpp"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                Class interRegionOption Declaration
\*---------------------------------------------------------------------------*/

class interRegionOption
:
    public option
{
protected:

    // Protected data

        //- Master or slave region
        bool master_;

        //- Name of the neighbour region to map
        word nbrRegionName_;

        //- Mesh to mesh interpolation object
        autoPtr<meshToMesh> meshInterpPtr_;


    // Protected member functions

        //- Set the mesh to mesh interpolation object
        void setMapper();


public:

    //- Runtime type information
    TypeName("interRegionOption");


    // Constructors

        //- Construct from dictionary
        interRegionOption
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~interRegionOption();


    // Member Functions

        // Access

            //- Return const access to the neighbour region name
            inline const word& nbrRegionName() const;

            //- Return const access to the mapToMap pointer
            inline const meshToMesh& meshInterp() const;


        // IO

            //- Read dictionary
            virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

inline const CML::word&
CML::fv::interRegionOption::nbrRegionName() const
{
    return nbrRegionName_;
}


inline const CML::meshToMesh&
CML::fv::interRegionOption::meshInterp() const
{
    if (!meshInterpPtr_.valid())
    {
        FatalErrorInFunction  << "Interpolation object not set"
            << abort(FatalError);
    }

    return meshInterpPtr_();
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
