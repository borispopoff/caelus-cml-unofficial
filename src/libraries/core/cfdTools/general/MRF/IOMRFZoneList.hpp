/*---------------------------------------------------------------------------*\
Copyright (C) 2012-2019 OpenFOAM Foundation
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
    CML::IOMRFZoneList

Description
    List of MRF zones with IO functionality.  MRF zones are specified by a list
    of dictionary entries, e.g.

    \verbatim
    zone1
    {
        cellZone    rotor1;
        active      yes;
        ...
    }

    zone2
    {
        cellZone    rotor2;
        active      yes;
        ...
    }
    \endverbatim

\*---------------------------------------------------------------------------*/

#ifndef IOMRFZoneList_H
#define IOMRFZoneList_H

#include "IOdictionary.hpp"
#include "MRFZoneList.hpp"

namespace CML
{

class IOMRFZoneList
:
    public IOdictionary,
    public MRFZoneList
{
private:

    // Private Member Functions

        //- Create IO object if dictionary is present
        IOobject createIOobject(const fvMesh& mesh) const;

        //- Disallow default bitwise copy construct
        IOMRFZoneList(const IOMRFZoneList&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const IOMRFZoneList&) = delete;


public:

    // Constructors

        //- Construct from mesh
        IOMRFZoneList(const fvMesh& mesh);


        //- Destructor
        virtual ~IOMRFZoneList()
        {}


    // Member Functions

        //- Read dictionary
        virtual bool read();
};


} // End namespace CML


#endif


