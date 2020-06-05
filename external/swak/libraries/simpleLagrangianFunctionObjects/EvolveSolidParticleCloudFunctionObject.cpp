/*---------------------------------------------------------------------------*\
Copyright: ICE Stroemungsfoschungs GmbH
Copyright  held by original author
-------------------------------------------------------------------------------
License
    This file is based on CAELUS.

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

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

\*---------------------------------------------------------------------------*/

#include "EvolveSolidParticleCloudFunctionObject.hpp"
#include "addToRunTimeSelectionTable.hpp"

#include "polyMesh.hpp"
#include "IOmanip.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{

    // Specialization because solidParticleCloud has no evolve
template<>
bool EvolveCloudFunctionObject<solidParticleCloud>::execute(bool forceWrite)
{
    Info << "Moving solidParticeCloud:" << cloud_->name()
        << " with " << cloud_->size() << " particles" << endl;
    cloud_->move(g());
    Info << tab << cloud_->size() << " particles after moving"
        << endl;

    if(
        obr().time().writeTime()
        ||
        forceWrite
    ) {
        Info << "Writing cloud " << cloud_->name() << endl;
        cloud_->write();
    }

    return true;
}


    defineTypeNameAndDebug(EvolveSolidParticleCloudFunctionObject, 0);

    addNamedToRunTimeSelectionTable
    (
        functionObject,
        EvolveSolidParticleCloudFunctionObject,
        dictionary,
        evolveSolidParticleCloud
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

EvolveSolidParticleCloudFunctionObject::EvolveSolidParticleCloudFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    EvolveCloudFunctionObject<solidParticleCloud>(
        name,
        t,
        dict
    )
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool EvolveSolidParticleCloudFunctionObject::start()
{
    cloud().set(
        new solidParticleCloud(
            //            dynamicCast<const fvMesh &>(
            dynamic_cast<const fvMesh &>(
                obr()
            ),
            cloudName()
        )
    );

    return true;
}


} // namespace CML

// ************************************************************************* //
