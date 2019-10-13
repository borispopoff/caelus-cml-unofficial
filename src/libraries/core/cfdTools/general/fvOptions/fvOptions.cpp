/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "fvOptions.hpp"
#include "fvMesh.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    namespace fv
    {
        defineTypeNameAndDebug(options, 0);
    }
}


// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

CML::IOobject CML::fv::options::createIOobject
(
    const fvMesh& mesh
) const
{
    IOobject io
    (
        typeName,
        mesh.time().constant(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    );

    if (io.headerOk())
    {
        Info<< "Creating finite volume options from "
            << io.instance()/io.name() << nl
            << endl;

        io.readOpt() = IOobject::MUST_READ_IF_MODIFIED;
        return io;
    }
    else
    {
        // Check if the fvOptions file is in system
        io.instance() = mesh.time().system();

        if (io.headerOk())
        {
            Info<< "Creating finite volume options from "
                << io.instance()/io.name() << nl
                << endl;

            io.readOpt() = IOobject::MUST_READ_IF_MODIFIED;
            return io;
        }
        else
        {
            Info<< "No finite volume options present" << nl << endl;

            io.readOpt() = IOobject::NO_READ;
            return io;
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::options::options
(
    const fvMesh& mesh
)
:
    IOdictionary(createIOobject(mesh)),
    optionList(mesh, *this)
{}


CML::fv::options& CML::fv::options::New(const fvMesh& mesh)
{
    if (mesh.thisDb().foundObject<options>(typeName))
    {
        return const_cast<options&>
        (
            mesh.lookupObject<options>(typeName)
        );
    }
    else
    {
        if (debug)
        {
            InfoInFunction
                << "Constructing " << typeName
                << " for region " << mesh.name() << endl;
        }

        options* objectPtr = new options(mesh);
        regIOobject::store(objectPtr);
        return *objectPtr;
    }
}


bool CML::fv::options::read()
{
    if (IOdictionary::regIOobject::read())
    {
        optionList::read(*this);
        return true;
    }
    else
    {
        return false;
    }
}



