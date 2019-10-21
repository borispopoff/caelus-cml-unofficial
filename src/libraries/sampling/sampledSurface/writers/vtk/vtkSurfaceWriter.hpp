/*---------------------------------------------------------------------------*\
Copyright (C) 2011 OpenFOAM Foundation
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
    CML::vtkSurfaceWriter

Description
    A surfaceWriter for VTK legacy format.

SourceFiles
    vtkSurfaceWriter.cpp

\*---------------------------------------------------------------------------*/

#ifndef vtkSurfaceWriter_H
#define vtkSurfaceWriter_H

#include "surfaceWriter.hpp"
#include "writeFuns.hpp"
#include "OFstream.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                      Class vtkSurfaceWriter Declaration
\*---------------------------------------------------------------------------*/

class vtkSurfaceWriter
:
    public surfaceWriter
{
    bool binary_;

    // Private Member Functions

        static void writeGeometry(ofstream&, const bool binary, const pointField&, const faceList&);

        template<class Type>
        static void writeData(ofstream&, const bool binary, const Field<Type>&);


        //- Templated write operation
        template<class Type>
        void writeTemplate
        (
            const fileName& outputDir,
            const fileName& surfaceName,
            const pointField& points,
            const faceList& faces,
            const word& fieldName,
            const Field<Type>& values,
            const bool isNodeValues,
            const bool verbose
        ) const;

public:

    //- Runtime type information
    TypeName("vtk");


    // Constructors

        //- Construct null
        vtkSurfaceWriter();

        //- Construct with some output options
        vtkSurfaceWriter(const dictionary& options);


    //- Destructor
    virtual ~vtkSurfaceWriter();


    // Member Functions

        //- Write single surface geometry to file.
        virtual void write
        (
            const fileName& outputDir,
            const fileName& surfaceName,
            const pointField& points,
            const faceList& faces,
            const bool verbose = false
        ) const;


        //- Write scalarField for a single surface to file.
        //  One value per face or vertex (isNodeValues = true)
        virtual void write
        (
            const fileName& outputDir,      // <case>/surface/TIME
            const fileName& surfaceName,    // name of surface
            const pointField& points,
            const faceList& faces,
            const word& fieldName,          // name of field
            const Field<scalar>& values,
            const bool isNodeValues,
            const bool verbose = false
        ) const;

        //- Write vectorField for a single surface to file.
        //  One value per face or vertex (isNodeValues = true)
        virtual void write
        (
            const fileName& outputDir,      // <case>/surface/TIME
            const fileName& surfaceName,    // name of surface
            const pointField& points,
            const faceList& faces,
            const word& fieldName,          // name of field
            const Field<vector>& values,
            const bool isNodeValues,
            const bool verbose = false
        ) const;

        //- Write sphericalTensorField for a single surface to file.
        //  One value per face or vertex (isNodeValues = true)
        virtual void write
        (
            const fileName& outputDir,      // <case>/surface/TIME
            const fileName& surfaceName,    // name of surface
            const pointField& points,
            const faceList& faces,
            const word& fieldName,          // name of field
            const Field<sphericalTensor>& values,
            const bool isNodeValues,
            const bool verbose = false
        ) const;

        //- Write symmTensorField for a single surface to file.
        //  One value per face or vertex (isNodeValues = true)
        virtual void write
        (
            const fileName& outputDir,      // <case>/surface/TIME
            const fileName& surfaceName,    // name of surface
            const pointField& points,
            const faceList& faces,
            const word& fieldName,          // name of field
            const Field<symmTensor>& values,
            const bool isNodeValues,
            const bool verbose = false
        ) const;

        //- Write tensorField for a single surface to file.
        //  One value per face or vertex (isNodeValues = true)
        virtual void write
        (
            const fileName& outputDir,      // <case>/surface/TIME
            const fileName& surfaceName,    // name of surface
            const pointField& points,
            const faceList& faces,
            const word& fieldName,          // name of field
            const Field<tensor>& values,
            const bool isNodeValues,
            const bool verbose = false
        ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// Write generic field in vtk format
template<class Type>
void CML::vtkSurfaceWriter::writeData
(
    ofstream& os,
    const bool binary,
    const Field<Type>& values
)
{
    label nValues = values.size(); 

    os  << "1 " << nValues << " float" << nl;

    DynamicList<floatScalar> fField(nValues);

    forAll(values, elemI)
    {
        writeFuns::insert(0, fField);
    }
    writeFuns::write(os, binary, fField);
}


template<class Type>
void CML::vtkSurfaceWriter::writeTemplate
(
    const fileName& outputDir,
    const fileName& surfaceName,
    const pointField& points,
    const faceList& faces,
    const word& fieldName,
    const Field<Type>& values,
    const bool isNodeValues,
    const bool verbose
) const
{
    if (!isDir(outputDir))
    {
        mkDir(outputDir);
    }

    std::ofstream os(outputDir/fieldName + '_' + surfaceName + ".vtk");

    if (verbose)
    {
        Info<< "Writing field " << fieldName << " to " 
            << outputDir/fieldName + '_' + surfaceName + ".vtk" << endl;
    }

    writeGeometry(os, binary_, points, faces);

    // start writing data
    if (isNodeValues)
    {
        os  << "POINT_DATA ";
    }
    else
    {
        os  << "CELL_DATA ";
    }

    os  << values.size() << nl
        << "FIELD attributes 1" << nl
        << fieldName << " ";

    // Write data
    writeData(os, binary_, values);
}

#endif

// ************************************************************************* //
