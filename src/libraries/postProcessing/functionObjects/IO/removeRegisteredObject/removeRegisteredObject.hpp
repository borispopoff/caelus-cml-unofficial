/*---------------------------------------------------------------------------*\
Copyright (C) 2013-2019 OpenFOAM Foundation
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
    CML::removeRegisteredObject

Group
    grpIOFunctionObjects

Description
    This function object removes registered objects if present in the database

    Example of function object specification:
    \verbatim
    removeRegisteredObject1
    {
        type        removeRegisteredObject;
        functionObjectLibs ("libIOFunctionObjects.so");
        ...
        objectNames (obj1 obj2);
    }
    \endverbatim

    \heading Function object usage
    \table
        Property     | Description             | Required    | Default value
        type         | type name: removeRegisteredObject | yes |
        objectNames  | objects to remove       | yes         |
    \endtable

SeeAlso
    CML::functionObject
    CML::OutputFilterFunctionObject

SourceFiles
    removeRegisteredObject.cpp
    IOremoveRegisteredObject.hpp

\*---------------------------------------------------------------------------*/

#ifndef removeRegisteredObject_H
#define removeRegisteredObject_H

#include "pointFieldFwd.hpp"
#include "wordList.hpp"
#include "runTimeSelectionTables.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;

/*---------------------------------------------------------------------------*\
                   Class removeRegisteredObject Declaration
\*---------------------------------------------------------------------------*/

class removeRegisteredObject
{
protected:

    // Protected data

        //- Name of this set of removeRegisteredObject
        word name_;

        const objectRegistry& obr_;

        // Read from dictionary

            //- Names of objects to control
            wordList objectNames_;


private:

    // Private Member Functions


        //- Disallow default bitwise copy construct
        removeRegisteredObject(const removeRegisteredObject&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const removeRegisteredObject&) = delete;


public:

    //- Runtime type information
    TypeName("removeRegisteredObject");


    // Constructors

        //- Construct for given objectRegistry and dictionary.
        //  Allow the possibility to load fields from files
        removeRegisteredObject
        (
            const word& name,
            const objectRegistry&,
            const dictionary&,
            const bool loadFromFiles = false
        );


    //- Destructor
    virtual ~removeRegisteredObject();


    // Member Functions

        //- Return name of the removeRegisteredObject
        virtual const word& name() const
        {
            return name_;
        }

        //- Read the removeRegisteredObject data
        virtual void read(const dictionary&);

        //- Execute, currently does nothing
        virtual void execute();

        //- Execute at the final time-loop, currently does nothing
        virtual void end();

        //- Called when time was set at the end of the Time::operator++
        virtual void timeSet();

        //- Write the removeRegisteredObject
        virtual void write();

        //- Update for changes of mesh
        virtual void updateMesh(const mapPolyMesh&)
        {}

        //- Update for changes of mesh
        virtual void movePoints(const pointField&)
        {}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
