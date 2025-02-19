/*---------------------------------------------------------------------------*\
Copyright (C) Creative Fields, Ltd.
-------------------------------------------------------------------------------
License
    This file is part of cfMesh.

    cfMesh is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    cfMesh is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with cfMesh.  If not, see <http://www.gnu.org/licenses/>.

Author: Franjo Juretic (franjo.juretic@c-fields.com)

\*---------------------------------------------------------------------------*/

#include "cartesian2DMeshGenerator.hpp"
#include "triSurface2DCheck.hpp"
#include "polyMeshGen2DEngine.hpp"
#include "triSurf.hpp"
#include "triSurfacePatchManipulator.hpp"
#include "triSurfaceCleanupDuplicateTriangles.hpp"
#include "Time.hpp"
#include "demandDrivenData.hpp"
#include "meshOctreeCreator.hpp"
#include "cartesianMeshExtractor.hpp"
#include "meshSurfaceEngine.hpp"
#include "meshSurfaceMapper2D.hpp"
#include "meshSurfaceEdgeExtractor2D.hpp"
#include "meshSurfaceOptimizer.hpp"
#include "topologicalCleaner.hpp"
#include "boundaryLayers.hpp"
#include "refineBoundaryLayers.hpp"
#include "renameBoundaryPatches.hpp"
#include "checkMeshDict.hpp"
#include "checkCellConnectionsOverFaces.hpp"
#include "checkIrregularSurfaceConnections.hpp"
#include "checkNonMappableCellConnections.hpp"
#include "checkBoundaryFacesSharingTwoEdges.hpp"
#include "triSurfaceMetaData.hpp"
#include "polyMeshGenGeometryModification.hpp"
#include "surfaceMeshGeometryModification.hpp"

//#define DEBUG

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * Private member functions  * * * * * * * * * * * * //

void cartesian2DMeshGenerator::createCartesianMesh()
{
    //- create polyMesh from octree boxes
    cartesianMeshExtractor cme(*octreePtr_, meshDict_, mesh_);

    if( meshDict_.found("decomposePolyhedraIntoTetsAndPyrs") )
    {
        if( readBool(meshDict_.lookup("decomposePolyhedraIntoTetsAndPyrs")) )
            cme.decomposeSplitHexes();
    }

    cme.createMesh();
}

void cartesian2DMeshGenerator::surfacePreparation()
{
    //- removes unnecessary cells and morph the boundary
    //- such that there is only one boundary face per cell
    //- It also checks topology of cells after morphing is performed
    bool changed;

    do
    {
        changed = false;

        checkIrregularSurfaceConnections checkConnections(mesh_);
        if( checkConnections.checkAndFixIrregularConnections() )
            changed = true;

        if( checkNonMappableCellConnections(mesh_).removeCells() )
            changed = true;

        if( checkCellConnectionsOverFaces(mesh_).checkCellGroups() )
            changed = true;
    } while( changed );

    checkBoundaryFacesSharingTwoEdges(mesh_).improveTopology();
}

void cartesian2DMeshGenerator::mapMeshToSurface()
{
    //- calculate mesh surface
    meshSurfaceEngine* msePtr = new meshSurfaceEngine(mesh_);

    //- pre-map mesh surface
    meshSurfaceMapper2D mapper(*msePtr, *octreePtr_);

    mapper.adjustZCoordinates();

    mapper.preMapVertices();

    //- map mesh surface on the geometry surface
    mapper.mapVerticesOntoSurface();

    deleteDemandDrivenData(msePtr);
}

void cartesian2DMeshGenerator::extractPatches()
{
    meshSurfaceEdgeExtractor2D(mesh_, *octreePtr_).distributeBoundaryFaces();
}

void cartesian2DMeshGenerator::mapEdgesAndCorners()
{
    meshSurfaceEdgeExtractor2D(mesh_, *octreePtr_).remapBoundaryPoints();
}

void cartesian2DMeshGenerator::optimiseMeshSurface()
{
    meshSurfaceEngine mse(mesh_);
    meshSurfaceOptimizer optimizer(mse, *octreePtr_);
    optimizer.optimizeSurface2D();
    optimizer.untangleSurface2D();
}

void cartesian2DMeshGenerator::generateBoundaryLayers()
{
    boundaryLayers bl(mesh_);

    bl.activate2DMode();

    bl.addLayerForAllPatches();

    if( modSurfacePtr_ )
    {
        polyMeshGenGeometryModification meshMod(mesh_, meshDict_);

        //- revert the mesh into the original space
        meshMod.revertGeometryModification();

        //- delete modified surface mesh
        deleteDemandDrivenData(modSurfacePtr_);

        //- delete the octree
        deleteDemandDrivenData(octreePtr_);

        //- contruct a new octree from the input surface
        octreePtr_ = new meshOctree(*surfacePtr_, true);
        meshOctreeCreator(*octreePtr_).createOctreeWithRefinedBoundary(20);

        mapEdgesAndCorners();

        optimiseMeshSurface();
    }
}

void cartesian2DMeshGenerator::refBoundaryLayers()
{
    if( meshDict_.isDict("boundaryLayers") )
    {
        refineBoundaryLayers refLayers(mesh_);

        refineBoundaryLayers::readSettings(meshDict_, refLayers);

        refLayers.activate2DMode();

        refLayers.refineLayers();

        meshSurfaceEngine mse(mesh_);
        meshSurfaceOptimizer optimizer(mse, *octreePtr_);

        optimizer.untangleSurface2D();
    }
}

void cartesian2DMeshGenerator::replaceBoundaries()
{
    renameBoundaryPatches rbp(mesh_, meshDict_);
}

void cartesian2DMeshGenerator::renumberMesh()
{
    polyMeshGenModifier(mesh_).renumberMesh();
}

void cartesian2DMeshGenerator::generateMesh()
{
    try
    {
        if( controller_.runCurrentStep("templateGeneration") )
        {
            createCartesianMesh();
        }

        if( controller_.runCurrentStep("surfaceTopology") )
        {
            surfacePreparation();
        }

        if( controller_.runCurrentStep("surfaceProjection") )
        {
            mapMeshToSurface();
        }

        if( controller_.runCurrentStep("patchAssignment") )
        {
            extractPatches();
        }

        if( controller_.runCurrentStep("edgeExtraction") )
        {
            mapEdgesAndCorners();

            optimiseMeshSurface();
        }

        if( controller_.runCurrentStep("boundaryLayerGeneration") )
        {
            generateBoundaryLayers();
        }

        if( controller_.runCurrentStep("meshOptimisation") )
        {
            optimiseMeshSurface();
        }

        if( controller_.runCurrentStep("boundaryLayerRefinement") )
        {
            refBoundaryLayers();
        }

        renumberMesh();

        replaceBoundaries();

        controller_.workflowCompleted();
    }
    catch(const std::string& message)
    {
        Info << message << endl;
    }
    catch(...)
    {
        WarningIn
        (
            "void cartesian2DMeshGenerator::generateMesh()"
        ) << "Meshing process terminated!" << endl;
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

cartesian2DMeshGenerator::cartesian2DMeshGenerator(const Time& time)
:
    db_(time),
    surfacePtr_(nullptr),
    modSurfacePtr_(nullptr),
    meshDict_
    (
        IOobject
        (
            "meshDict",
            db_.system(),
            db_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    ),
    octreePtr_(nullptr),
    mesh_(time),
    controller_(mesh_)
{
    if( true )
    {
        checkMeshDict cmd(meshDict_);
    }

    fileName surfaceFile = meshDict_.lookup("surfaceFile");
    if( Pstream::parRun() )
        surfaceFile = ".."/surfaceFile;

    surfacePtr_ = new triSurf(db_.path()/surfaceFile);

    if( true )
    {
        //- save meta data with the mesh (surface mesh + its topology info)
        triSurfaceMetaData sMetaData(*surfacePtr_);
        const dictionary& surfMetaDict = sMetaData.metaData();

        mesh_.metaData().add("surfaceFile", surfaceFile, true);
        mesh_.metaData().add("surfaceMeta", surfMetaDict, true);

        triSurface2DCheck surfCheck(*surfacePtr_);
        if( !surfCheck.is2DSurface() )
        {
            surfCheck.createSubsets();

            Info << "Writting surface with subsets to file "
                 << "badSurfaceWithSubsets.fms" << endl;
            surfacePtr_->writeSurface("badSurfaceWithSubsets.fms");
        }
    }

    if( surfacePtr_->featureEdges().size() != 0 )
    {
        //- get rid of duplicate triangles as they cause strange problems
        triSurfaceCleanupDuplicateTriangles(const_cast<triSurf&>(*surfacePtr_));

        //- create surface patches based on the feature edges
        //- and update the meshDict based on the given data
        triSurfacePatchManipulator manipulator(*surfacePtr_);

        const triSurf* surfaceWithPatches =
            manipulator.surfaceWithPatches(&meshDict_);

        //- delete the old surface and assign the new one
        deleteDemandDrivenData(surfacePtr_);
        surfacePtr_ = surfaceWithPatches;
    }

    if( meshDict_.found("anisotropicSources") )
    {
        surfaceMeshGeometryModification surfMod(*surfacePtr_, meshDict_);

        modSurfacePtr_ = surfMod.modifyGeometry();

        octreePtr_ = new meshOctree(*modSurfacePtr_, true);
    }
    else
    {
        octreePtr_ = new meshOctree(*surfacePtr_, true);
    }

    meshOctreeCreator(*octreePtr_, meshDict_).createOctreeBoxes();

    generateMesh();
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

cartesian2DMeshGenerator::~cartesian2DMeshGenerator()
{
    deleteDemandDrivenData(surfacePtr_);
    deleteDemandDrivenData(modSurfacePtr_);
    deleteDemandDrivenData(octreePtr_);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void cartesian2DMeshGenerator::writeMesh() const
{
    mesh_.write();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
