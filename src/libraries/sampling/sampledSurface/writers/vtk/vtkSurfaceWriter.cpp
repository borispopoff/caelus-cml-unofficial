/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2012 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "vtkSurfaceWriter.hpp"
#include "OSspecific.hpp"
#include "makeSurfaceWriterMethods.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    makeSurfaceWriterType(vtkSurfaceWriter);
    addToRunTimeSelectionTable(surfaceWriter, vtkSurfaceWriter, wordDict);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::vtkSurfaceWriter::writeGeometry
(
    ofstream& os,
    const bool binary,
    const pointField& points,
    const faceList& faces
)
{
    // Header
    writeFuns::writeHeader(os, binary, "sampleSurface");
    os << "DATASET POLYDATA" << std::endl;

    // Write vertex coords
    label nPoints = points.size();
    os << "POINTS " << nPoints << " float" << nl;

   DynamicList<floatScalar> ptField(3*nPoints);
   writeFuns::insert(points, ptField);
   writeFuns::write(os, binary, ptField);

    // Write faces
    label nNodes = 0;
    forAll(faces, faceI)
    {
        nNodes += faces[faceI].size();
    }

    os  << "POLYGONS " << faces.size() << ' ' << faces.size() + nNodes << nl;

    DynamicList<label> vertLabels(faces.size() + nNodes);

    forAll(faces, faceI)
    {
        const face& f = faces[faceI];
        vertLabels.append(f.size());
        writeFuns::insert(f, vertLabels);
    }
    writeFuns::write(os, binary, vertLabels);
}


namespace CML
{

    template<>
    void CML::vtkSurfaceWriter::writeData
    (
        ofstream& os,
        const bool binary,
        const Field<scalar>& values
    )
    {
        label nValues = values.size(); 

        os  << "1 " << nValues << " float" << std::endl;

        DynamicList<floatScalar> fField(nValues);

        writeFuns::insert(values, fField);
        writeFuns::write(os, binary, fField);
    }


    template<>
    void CML::vtkSurfaceWriter::writeData
    (
        ofstream& os,
        const bool binary,
        const Field<vector>& values
    )
    {
        label nValues = values.size(); 

        os  << "3 " << nValues << " float" << std::endl;

        DynamicList<floatScalar> fField(3*nValues);

        writeFuns::insert(values, fField);
        writeFuns::write(os, binary, fField);
    }


    template<>
    void CML::vtkSurfaceWriter::writeData
    (
        ofstream& os,
        const bool binary,
        const Field<sphericalTensor>& values
    )
    {
        label nValues = values.size(); 

        os  << "1 " << nValues << " float" << std::endl;

        DynamicList<floatScalar> fField(nValues);

        writeFuns::insert(values, fField);
        writeFuns::write(os, binary, fField);
    }


    template<>
    void CML::vtkSurfaceWriter::writeData
    (
        ofstream& os,
        const bool binary,
        const Field<symmTensor>& values
    )
    {

        label nValues = values.size(); 

        os  << "6 " << nValues << " float" << std::endl;

        DynamicList<floatScalar> fField(6*nValues);

        writeFuns::insert(values, fField);
        writeFuns::write(os, binary, fField);

        os  << "6 " << values.size() << " float" << nl;
    }


    template<>
    void CML::vtkSurfaceWriter::writeData
    (
        ofstream& os,
        const bool binary,
        const Field<tensor>& values
    )
    {
        label nValues = values.size(); 

        os  << "9 " << nValues << " float" << std::endl;

        DynamicList<floatScalar> fField(9*nValues);

        writeFuns::insert(values, fField);
        writeFuns::write(os, binary, fField);
    }

}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::vtkSurfaceWriter::vtkSurfaceWriter()
:
    surfaceWriter(),
    binary_(false)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::vtkSurfaceWriter::~vtkSurfaceWriter()
{}


CML::vtkSurfaceWriter::vtkSurfaceWriter(const dictionary& options)
:
    surfaceWriter(),
    binary_(false)
{
    // Choose ascii or binary format
    if (options.found("binary"))
    {
        binary_ = options.lookup("binary");
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::vtkSurfaceWriter::write
(
    const fileName& outputDir,
    const fileName& surfaceName,
    const pointField& points,
    const faceList& faces,
    const bool verbose
) const
{
    if (!isDir(outputDir))
    {
        mkDir(outputDir);
    }

    std::ofstream os(outputDir/surfaceName + ".vtk");

    if (verbose)
    {
        Info<< "Writing geometry to " << outputDir/surfaceName + ".vtk" << endl;
    }

    writeGeometry(os, binary_, points, faces);
}


// create write methods
defineSurfaceWriterWriteFields(CML::vtkSurfaceWriter);


// ************************************************************************* //
