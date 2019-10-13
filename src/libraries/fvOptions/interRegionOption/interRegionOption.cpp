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

\*---------------------------------------------------------------------------*/

#include "interRegionOption.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
namespace fv
{
    defineTypeNameAndDebug(interRegionOption, 0);
}
}


// * * * * * * * * * * * *  Protected member functions * * * * * * * * * * * //

void CML::fv::interRegionOption::setMapper()
{
    if (master_)
    {
        Info<< indent << "- selecting inter region mapping" << endl;

        const fvMesh& nbrMesh =
            mesh_.time().lookupObject<fvMesh>(nbrRegionName_);

        if (mesh_.name() == nbrMesh.name())
        {
            FatalErrorIn("interRegionOption::setCellIds()")
                << "Inter-region model selected, but local and "
                << "neighbour regions are the same: " << nl
                << "    local region: " << mesh_.name() << nl
                << "    secondary region: " << nbrMesh.name() << nl
                << exit(FatalError);
        }

        if (mesh_.bounds().overlaps(nbrMesh.bounds()))
        {
            meshInterpPtr_.reset
            (
                new meshToMesh
                (
                    mesh_,
                    nbrMesh,
                    meshToMesh::interpolationMethodNames_.read
                    (
                        coeffs_.lookup("interpolationMethod")
                    ),
                    false // not interpolating patches
                )
            );
        }
        else
        {
            FatalErrorIn("interRegionOption::setCellSet()")
                << "regions " << mesh_.name() << " and "
                << nbrMesh.name() <<  " do not intersect"
                << exit(FatalError);
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::fv::interRegionOption::interRegionOption
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    option
    (
        name,
        modelType,
        dict,
        mesh
    ),
    master_(coeffs_.lookupOrDefault<bool>("master", true)),
    nbrRegionName_(coeffs_.lookup("nbrRegionName")),
    meshInterpPtr_()
{
    if (active())
    {
        setMapper();
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::fv::interRegionOption::~interRegionOption()
{}


// ************************************************************************* //
