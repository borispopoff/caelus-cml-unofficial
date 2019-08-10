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

\*---------------------------------------------------------------------------*/

#include "writePointSet_setSet.hpp"
#include "OFstream.hpp"
#include "writeFuns_setSet.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

void writePointSet
(
    const bool binary,
    const primitiveMesh& mesh,
    const topoSet& set,
    const fileName& fileName
)
{
    std::ofstream pStream(fileName.c_str());

    pStream
        << "# vtk DataFile Version 2.0" << std::endl
        << set.name() << std::endl;
    if (binary)
    {
        pStream << "BINARY" << std::endl;
    }
    else
    {
        pStream << "ASCII" << std::endl;
    }
    pStream << "DATASET POLYDATA" << std::endl;


    //------------------------------------------------------------------
    //
    // Write topology
    //
    //------------------------------------------------------------------


    labelList pointLabels(set.toc());

    pointField setPoints(mesh.points(), pointLabels);

    // Write points

    pStream << "POINTS " << pointLabels.size() << " float" << std::endl;

    DynamicList<floatScalar> ptField(3*pointLabels.size());

    writeFuns::insert(setPoints, ptField);

    writeFuns::write(pStream, binary, ptField);


    //-----------------------------------------------------------------
    //
    // Write data
    //
    //-----------------------------------------------------------------

    // Write pointID

    pStream
        << "POINT_DATA " << pointLabels.size() << std::endl
        << "FIELD attributes 1" << std::endl;

    // Cell ids first
    pStream << "pointID 1 " << pointLabels.size() << " int" << std::endl;

    writeFuns::write(pStream, binary, pointLabels);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
