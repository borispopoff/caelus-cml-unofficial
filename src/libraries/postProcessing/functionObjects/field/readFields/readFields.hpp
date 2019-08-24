/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2014 OpenFOAM Foundation
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
    CML::readFields

Group
    grpFieldFunctionObjects

Description
    This function object reads fields from the time directories and adds them to
    the mesh database for further post-processing.

    Example of function object specification:
    \verbatim
    readFields1
    {
        type        readFields;
        functionObjectLibs ("libfieldFunctionObjects.so");
        ...
        fields
        (
            U
            p
        );
    }
    \endverbatim

    \heading Function object usage
    \table
        Property     | Description             | Required    | Default value
        type         | type name: readFields   | yes         |
        fields       | list of fields to read  |  no         |
    \endtable

SeeAlso
    CML::functionObject
    CML::OutputFilterFunctionObject

SourceFiles
    readFields.cpp
    IOreadFields.hpp

\*---------------------------------------------------------------------------*/

#ifndef readFields_H
#define readFields_H

#include "OFstream.hpp"
#include "pointFieldFwd.hpp"
#include "volFields.hpp"
#include "surfaceFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;

/*---------------------------------------------------------------------------*\
                         Class readFields Declaration
\*---------------------------------------------------------------------------*/

class readFields
{
protected:

    // Protected data

        //- Name of this set of readFields object
        word name_;

        const objectRegistry& obr_;

        //- on/off switch
        bool active_;

        //- Fields to load
        wordList fieldSet_;

        //- Loaded fields
        PtrList<volScalarField> vsf_;
        PtrList<volVectorField> vvf_;
        PtrList<volSphericalTensorField> vSpheretf_;
        PtrList<volSymmTensorField> vSymmtf_;
        PtrList<volTensorField> vtf_;

        PtrList<surfaceScalarField> ssf_;
        PtrList<surfaceVectorField> svf_;
        PtrList<surfaceSphericalTensorField> sSpheretf_;
        PtrList<surfaceSymmTensorField> sSymmtf_;
        PtrList<surfaceTensorField> stf_;


    // Protected Member Functions

        template<class Type>
        void loadField
        (
            const word&,
            PtrList<GeometricField<Type, fvPatchField, volMesh> >&,
            PtrList<GeometricField<Type, fvsPatchField, surfaceMesh> >&
        ) const;

private:

    // Private member functions

        //- Disallow default bitwise copy construct
        readFields(const readFields&);

        //- Disallow default bitwise assignment
        void operator=(const readFields&);
public:

    //- Runtime type information
    TypeName("readFields");


    // Constructors

        //- Construct for given objectRegistry and dictionary.
        //  Allow the possibility to load fields from files
        readFields
        (
            const word& name,
            const objectRegistry&,
            const dictionary&,
            const bool loadFromFiles = false
        );


    //- Destructor
    virtual ~readFields();


    // Member Functions

        //- Return name of the readFields object
        virtual const word& name() const
        {
            return name_;
        }

        //- Read the field min/max data
        virtual void read(const dictionary&);

        //- Execute, currently does nothing
        virtual void execute();

        //- Execute at the final time-loop, currently does nothing
        virtual void end();

        //- Called when time was set at the end of the Time::operator++
        virtual void timeSet();

        //- Write
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

#include "readFields.hpp"
#include "volFields.hpp"
#include "surfaceFields.hpp"
#include "Time.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void CML::readFields::loadField
(
    const word& fieldName,
    PtrList<GeometricField<Type, fvPatchField, volMesh> >& vflds,
    PtrList<GeometricField<Type, fvsPatchField, surfaceMesh> >& sflds
) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> vfType;
    typedef GeometricField<Type, fvsPatchField, surfaceMesh> sfType;

    if (obr_.foundObject<vfType>(fieldName))
    {
        if (debug)
        {
            Info<< "readFields : Field " << fieldName << " already in database"
                << endl;
        }
    }
    else if (obr_.foundObject<sfType>(fieldName))
    {
        if (debug)
        {
            Info<< "readFields : Field " << fieldName << " already in database"
                << endl;
        }
    }
    else
    {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        IOobject fieldHeader
        (
            fieldName,
            mesh.time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );

        if
        (
            fieldHeader.headerOk()
         && fieldHeader.headerClassName() == vfType::typeName
        )
        {
            // store field locally
            Info<< "    Reading " << fieldName << endl;
            label sz = vflds.size();
            vflds.setSize(sz+1);
            vflds.set(sz, new vfType(fieldHeader, mesh));
        }
        else if
        (
            fieldHeader.headerOk()
         && fieldHeader.headerClassName() == sfType::typeName
        )
        {
            // store field locally
            Info<< "    Reading " << fieldName << endl;
            label sz = sflds.size();
            sflds.setSize(sz+1);
            sflds.set(sz, new sfType(fieldHeader, mesh));
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
