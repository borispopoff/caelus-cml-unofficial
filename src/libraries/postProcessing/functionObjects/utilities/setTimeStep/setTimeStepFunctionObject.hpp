/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2018 OpenFOAM Foundation
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
    CML::setTimeStepFunctionObject

Description
    Overrides the timeStep. Can only be used with
    solvers with adjustTimeStep control (e.g. pimpleSolver). Makes no attempt
    to cooperate with other timeStep 'controllers' (maxCo, other
    functionObjects). Supports 'enabled' flag but none of the other ones
    'timeStart', 'timeEnd', 'writeControl' etc.

SourceFiles
    setTimeStepFunctionObject.cpp

\*---------------------------------------------------------------------------*/

#ifndef setTimeStepFunctionObject_H
#define setTimeStepFunctionObject_H

#include "functionObject.hpp"
#include "dictionary.hpp"
#include "DataEntry.hpp"
#include "pointFieldFwd.hpp"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
// Forward declaration of classes
class mapPolyMesh;
/*---------------------------------------------------------------------------*\
                    Class setTimeStepFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class setTimeStepFunctionObject
:
    public functionObject
{
    // Private data

        //- Reference to the time database
        const Time& time_;

        //- Time step
        autoPtr<DataEntry<scalar>> timeStepPtr_;

        //- Switch for the execution - defaults to 'yes/on'
        bool enabled_;

    // Private member functions

        //- Disallow default bitwise copy construct
        setTimeStepFunctionObject(const setTimeStepFunctionObject&);

        //- Disallow default bitwise assignment
        void operator=(const setTimeStepFunctionObject&);


public:

    //- Runtime type information
    TypeName("setTimeStep");


    // Constructors

        //- Construct from components
        setTimeStepFunctionObject
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );


    // Destructor
    virtual ~setTimeStepFunctionObject();


    // Member Functions

        // Access

            //- Return time database
            virtual const Time& time() const
            {
                return time_;
            }

            //- Return the enabled flag
            virtual bool enabled() const
            {
                return enabled_;
            }


        // Function object control

            //- Switch the function object on
            virtual void on();

            //- Switch the function object off
            virtual void off();


            //- Called at the start of the time-loop
            virtual bool start();

            //- Called at each ++ or += of the time-loop
            virtual bool execute(const bool forceWrite);

            //- Called when Time::run() determines that the time-loop exits
            virtual bool end();

            //- Override the time-step value
            virtual bool setTimeStep();

            //- Read and set the function object if its data have changed
            virtual bool read(const dictionary&);

            //- Update for changes of mesh
            virtual void updateMesh(const mapPolyMesh& mpm);

            //- Update for changes of mesh
            virtual void movePoints(const pointField& mesh);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
