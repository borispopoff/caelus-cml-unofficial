/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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
    CML::fieldMinMax

Group
    grpFieldFunctionObjects

Description
    Calculates the value and location of scalar minimum and maximum for a list
    of user-specified fields.

    For variables with a rank greater than zero, either the min/max of a
    component value or the magnitude is reported.  When operating in parallel,
    the processor owning the value is also given.

    Example of function object specification:
    \verbatim
    fieldMinMax1
    {
        type        fieldMinMax;
        functionObjectLibs ("libfieldFunctionObjects.so");
        ...
        write       yes;
        log         yes;
        location    yes;
        mode        magnitude;
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
        type         | type name: fieldMinMax  | yes         |
        write        | write min/max data to file |  no      | yes
        log          | write min/max data to standard output | no | no
        location     | write location of the min/max value | no | yes
        mode         | calculation mode: magnitude or component | no | magnitude
    \endtable

    Output data is written to the file \<timeDir\>/fieldMinMax.dat

See also
    CML::functionObject
    CML::OutputFilterFunctionObject

SourceFiles
    fieldMinMax.cpp
    IOfieldMinMax.hpp

\*---------------------------------------------------------------------------*/

#ifndef fieldMinMax_H
#define fieldMinMax_H

#include "pointFieldFwd.hpp"
#include "functionObjectFile.hpp"
#include "Switch.hpp"
#include "vector.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;

/*---------------------------------------------------------------------------*\
                       Class fieldMinMax Declaration
\*---------------------------------------------------------------------------*/

class fieldMinMax
:
    public functionObjectFile
{
public:

    enum class modeType
    {
        mag,
        cmpt
    };

protected:

    // Protected data

        //- Mode type names
        static const NamedEnum<modeType, 2> modeTypeNames_;

        //- Name of this set of field min/max
        //  Also used as the name of the output directory
        word name_;

        const objectRegistry& obr_;

        //- on/off switch
        bool active_;

        //- Switch to send output to Info as well
        Switch log_;

        //- Switch to write location of min/max values
        Switch location_;

        //- Mode for min/max - only applicable for ranks > 0
        modeType mode_;

        //- Fields to assess min/max
        wordList fieldSet_;


    // Protected Member Functions

        //- Helper function to write the output
        template<class Type>
        void output
        (
            const word& fieldName,
            const word& outputName,
            const label minCell,
            const label maxCell,
            const vector& minC,
            const vector& maxC,
            const label minProci,
            const label maxProci,
            const Type& minValue,
            const Type& maxValue
        );

        //- Disallow default bitwise copy construct
        fieldMinMax(const fieldMinMax&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const fieldMinMax&) = delete;

        //- Output file header information
        virtual void writeFileHeader(const label i);


public:

    //- Runtime type information
    TypeName("fieldMinMax");


    // Constructors

        //- Construct for given objectRegistry and dictionary.
        //  Allow the possibility to load fields from files
        fieldMinMax
        (
            const word& name,
            const objectRegistry&,
            const dictionary&,
            const bool loadFromFiles = false
        );


    //- Destructor
    virtual ~fieldMinMax();


    // Member Functions

        //- Return name of the set of field min/max
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

        //- Calculate the field min/max
        template<class Type>
        void calcMinMaxFields
        (
            const word& fieldName,
            const modeType& mode
        );

        //- Write the fieldMinMax
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

#include "fieldMinMax.hpp"
#include "volFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void CML::fieldMinMax::output
(
    const word& fieldName,
    const word& outputName,
    const label minCell,
    const label maxCell,
    const vector& minC,
    const vector& maxC,
    const label minProci,
    const label maxProci,
    const Type& minValue,
    const Type& maxValue
)
{
    OFstream& file = this->file();

    if (location_)
    {
        file<< obr_.time().value();

        writeTabbed(file, fieldName);

        file<< token::TAB << minValue
            << token::TAB << minC;

        if (Pstream::parRun())
        {
            file<< token::TAB << minProci;
        }

        file<< token::TAB << maxValue
            << token::TAB << maxC;

        if (Pstream::parRun())
        {
            file<< token::TAB << maxProci;
        }

        file<< endl;

        Info(log_) << "    min(" << outputName << ") = " << minValue
            << " in cell " << minCell
            << " at location " << minC;

        if (Pstream::parRun())
        {
            Info(log_) << " on processor " << minProci;
        }

        Info(log_) << nl << "    max(" << outputName << ") = " << maxValue
            << " in cell " << maxCell
            << " at location " << maxC;

        if (Pstream::parRun())
        {
            Info(log_)<< " on processor " << maxProci;
        }
    }
    else
    {
        file<< token::TAB << minValue << token::TAB << maxValue;

        Info(log_) << "    min/max(" << outputName << ") = "
            << minValue << ' ' << maxValue;
    }

    Info(log_)<< endl;
}


template<class Type>
void CML::fieldMinMax::calcMinMaxFields
(
    const word& fieldName,
    const modeType& mode
)
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    if (obr_.foundObject<fieldType>(fieldName))
    {
        const label proci = Pstream::myProcNo();

        const fieldType& field = obr_.lookupObject<fieldType>(fieldName);
        const fvMesh& mesh = field.mesh();

        const volVectorField::GeometricBoundaryField& CfBoundary =
            mesh.C().boundaryField();

        switch (mode)
        {
            case modeType::mag:
            {
                const volScalarField magField(mag(field));
                const volScalarField::GeometricBoundaryField& magFieldBoundary =
                    magField.boundaryField();

                scalarList minVs(Pstream::nProcs());
                labelList minCells(Pstream::nProcs());
                List<vector> minCs(Pstream::nProcs());
                label minProci = findMin(magField);
                minVs[proci] = magField[minProci];
                minCells[proci] = minProci;
                minCs[proci] = field.mesh().C()[minProci];

                scalarList maxVs(Pstream::nProcs());
                labelList maxCells(Pstream::nProcs());
                List<vector> maxCs(Pstream::nProcs());
                label maxProci = findMax(magField);
                maxVs[proci] = magField[maxProci];
                maxCells[proci] = maxProci;
                maxCs[proci] = field.mesh().C()[maxProci];

                forAll(magFieldBoundary, patchi)
                {
                    const scalarField& mfp = magFieldBoundary[patchi];
                    if (mfp.size())
                    {
                        const vectorField& Cfp = CfBoundary[patchi];

                        const labelList& faceCells =
                            magFieldBoundary[patchi].patch().faceCells();

                        label minPI = findMin(mfp);
                        if (mfp[minPI] < minVs[proci])
                        {
                            minVs[proci] = mfp[minPI];
                            minCells[proci] = faceCells[minPI];
                            minCs[proci] = Cfp[minPI];
                        }

                        label maxPI = findMax(mfp);
                        if (mfp[maxPI] > maxVs[proci])
                        {
                            maxVs[proci] = mfp[maxPI];
                            maxCells[proci] = faceCells[maxPI];
                            maxCs[proci] = Cfp[maxPI];
                        }
                    }
                }

                Pstream::gatherList(minVs);
                Pstream::gatherList(minCells);
                Pstream::gatherList(minCs);

                Pstream::gatherList(maxVs);
                Pstream::gatherList(maxCells);
                Pstream::gatherList(maxCs);

                if (Pstream::master())
                {
                    const label minI = findMin(minVs);
                    const scalar minValue = minVs[minI];
                    const label minCell = minCells[minI];
                    const vector& minC = minCs[minI];

                    const label maxI = findMax(maxVs);
                    const scalar maxValue = maxVs[maxI];
                    const label maxCell = maxCells[maxI];
                    const vector& maxC = maxCs[maxI];

                    output
                    (
                        fieldName,
                        word("mag(" + fieldName + ")"),
                        minCell,
                        maxCell,
                        minC,
                        maxC,
                        minI,
                        maxI,
                        minValue,
                        maxValue
                    );
                }
                break;
            }
            case modeType::cmpt:
            {
                const typename fieldType::GeometricBoundaryField&
                    fieldBoundary = field.boundaryField();

                List<Type> minVs(Pstream::nProcs());
                labelList minCells(Pstream::nProcs());
                List<vector> minCs(Pstream::nProcs());
                label minProci = findMin(field);
                minVs[proci] = field[minProci];
                minCells[proci] = minProci;
                minCs[proci] = field.mesh().C()[minProci];

                List<Type> maxVs(Pstream::nProcs());
                labelList maxCells(Pstream::nProcs());
                List<vector> maxCs(Pstream::nProcs());
                label maxProci = findMax(field);
                maxVs[proci] = field[maxProci];
                maxCells[proci] = maxProci;
                maxCs[proci] = field.mesh().C()[maxProci];

                forAll(fieldBoundary, patchi)
                {
                    const Field<Type>& fp = fieldBoundary[patchi];

                    if (fp.size())
                    {
                        const vectorField& Cfp = CfBoundary[patchi];

                        const labelList& faceCells =
                            fieldBoundary[patchi].patch().faceCells();

                        label minPI = findMin(fp);
                        if (fp[minPI] < minVs[proci])
                        {
                            minVs[proci] = fp[minPI];
                            minCells[proci] = faceCells[minPI];
                            minCs[proci] = Cfp[minPI];
                        }

                        label maxPI = findMax(fp);
                        if (fp[maxPI] > maxVs[proci])
                        {
                            maxVs[proci] = fp[maxPI];
                            maxCells[proci] = faceCells[maxPI];
                            maxCs[proci] = Cfp[maxPI];
                        }
                    }
                }

                Pstream::gatherList(minVs);
                Pstream::gatherList(minCells);
                Pstream::gatherList(minCs);

                Pstream::gatherList(maxVs);
                Pstream::gatherList(maxCells);
                Pstream::gatherList(maxCs);

                if (Pstream::master())
                {
                    label minI = findMin(minVs);
                    Type minValue = minVs[minI];
                    const label minCell = minCells[minI];
                    const vector& minC = minCs[minI];

                    label maxI = findMax(maxVs);
                    Type maxValue = maxVs[maxI];
                    const label maxCell = maxCells[maxI];
                    const vector& maxC = maxCs[maxI];

                    output
                    (
                        fieldName,
                        fieldName,
                        minCell,
                        maxCell,
                        minC,
                        maxC,
                        minI,
                        maxI,
                        minValue,
                        maxValue
                    );
                }
                break;
            }
            default:
            {
                FatalErrorInFunction
                    << "Unknown min/max mode: " << modeTypeNames_[mode_]
                    << exit(FatalError);
            }
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
