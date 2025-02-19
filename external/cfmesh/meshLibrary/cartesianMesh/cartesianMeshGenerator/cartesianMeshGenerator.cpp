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

#include "cartesianMeshGenerator.hpp"
#include "triSurf.hpp"
#include "triSurfacePatchManipulator.hpp"
#include "demandDrivenData.hpp"
#include "Time.hpp"
#include "meshOctreeCreator.hpp"
#include "cartesianMeshExtractor.hpp"
#include "meshSurfaceEngine.hpp"
#include "meshSurfaceMapper.hpp"
#include "edgeExtractor.hpp"
#include "meshSurfaceEdgeExtractorNonTopo.hpp"
#include "meshOptimizer.hpp"
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

void cartesianMeshGenerator::createCartesianMesh()
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

void cartesianMeshGenerator::surfacePreparation()
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

void cartesianMeshGenerator::mapMeshToSurface()
{
    //- calculate mesh surface
    meshSurfaceEngine mse(mesh_);

    //- pre-map mesh surface
    meshSurfaceMapper mapper(mse, *octreePtr_);
    mapper.preMapVertices();

    //- map mesh surface on the geometry surface
    mapper.mapVerticesOntoSurface();

    //- untangle surface faces
    meshSurfaceOptimizer(mse, *octreePtr_).untangleSurface();
}

void cartesianMeshGenerator::extractPatches()
{
    edgeExtractor extractor(mesh_, *octreePtr_);

    Info << "Extracting edges" << endl;
    extractor.extractEdges();

    extractor.updateMeshPatches();
}

void cartesianMeshGenerator::mapEdgesAndCorners()
{
    meshSurfaceEdgeExtractorNonTopo(mesh_, *octreePtr_);
}

void cartesianMeshGenerator::optimiseMeshSurface()
{
    meshSurfaceEngine mse(mesh_);
    meshSurfaceOptimizer(mse, *octreePtr_).optimizeSurface();
}

void cartesianMeshGenerator::generateBoundaryLayers()
{
    //- add boundary layers
    boundaryLayers bl(mesh_);
    bl.addLayerForAllPatches();
}

void cartesianMeshGenerator::refBoundaryLayers()
{
    if( meshDict_.isDict("boundaryLayers") )
    {
        refineBoundaryLayers refLayers(mesh_);

        refineBoundaryLayers::readSettings(meshDict_, refLayers);

        refLayers.refineLayers();

        labelLongList pointsInLayer;
        refLayers.pointsInBndLayer(pointsInLayer);

        meshOptimizer mOpt(mesh_);
        mOpt.lockPoints(pointsInLayer);
        mOpt.untangleBoundaryLayer();
    }
}

void cartesianMeshGenerator::optimiseFinalMesh()
{
    //- untangle the surface if needed
    bool enforceConstraints(false);
    if( meshDict_.found("enforceGeometryConstraints") )
    {
        enforceConstraints =
            readBool(meshDict_.lookup("enforceGeometryConstraints"));
    }

    if( true )
    {
        meshSurfaceEngine mse(mesh_);
        meshSurfaceOptimizer surfOpt(mse, *octreePtr_);

        if( enforceConstraints )
            surfOpt.enforceConstraints();

        surfOpt.optimizeSurface();
    }

    deleteDemandDrivenData(octreePtr_);

    //- final optimisation
    meshOptimizer optimizer(mesh_);
    if( enforceConstraints )
        optimizer.enforceConstraints();

    optimizer.optimizeMeshFV();
    optimizer.optimizeLowQualityFaces();
    optimizer.optimizeBoundaryLayer(modSurfacePtr_==nullptr);
    optimizer.untangleMeshFV();

    mesh_.clearAddressingData();

    if( modSurfacePtr_ )
    {
        polyMeshGenGeometryModification meshMod(mesh_, meshDict_);

        //- revert the mesh into the original space
        meshMod.revertGeometryModification();

        //- delete modified surface mesh
        deleteDemandDrivenData(modSurfacePtr_);
    }
}

void cartesianMeshGenerator::projectSurfaceAfterBackScaling()
{
    if( !meshDict_.found("anisotropicSources") )
        return;

    deleteDemandDrivenData(octreePtr_);
    octreePtr_ = new meshOctree(*surfacePtr_);

    meshOctreeCreator
    (
        *octreePtr_,
        meshDict_
    ).createOctreeWithRefinedBoundary(20, 30);

    //- calculate mesh surface
    meshSurfaceEngine mse(mesh_);

    //- pre-map mesh surface
    meshSurfaceMapper mapper(mse, *octreePtr_);

    //- map mesh surface on the geometry surface
    mapper.mapVerticesOntoSurface();

    optimiseFinalMesh();
}

void cartesianMeshGenerator::replaceBoundaries()
{
    renameBoundaryPatches rbp(mesh_, meshDict_);
}

void cartesianMeshGenerator::renumberMesh()
{
    polyMeshGenModifier(mesh_).renumberMesh();
}

void cartesianMeshGenerator::generateMesh()
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
            optimiseFinalMesh();

            projectSurfaceAfterBackScaling();
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
            "void cartesianMeshGenerator::generateMesh()"
        ) << "Meshing process terminated!" << endl;
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

cartesianMeshGenerator::cartesianMeshGenerator(const Time& time)
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
    }

    if( surfacePtr_->featureEdges().size() != 0 )
    {
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

        octreePtr_ = new meshOctree(*modSurfacePtr_);
    }
    else
    {
        octreePtr_ = new meshOctree(*surfacePtr_);
    }

    meshOctreeCreator(*octreePtr_, meshDict_).createOctreeBoxes();

    generateMesh();
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

cartesianMeshGenerator::~cartesianMeshGenerator()
{
    deleteDemandDrivenData(surfacePtr_);
    deleteDemandDrivenData(modSurfacePtr_);
    deleteDemandDrivenData(octreePtr_);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void cartesianMeshGenerator::writeMesh() const
{
    mesh_.write();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// ************************************************************************* //
