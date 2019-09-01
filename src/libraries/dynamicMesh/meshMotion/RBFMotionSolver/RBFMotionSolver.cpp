/*---------------------------------------------------------------------------*\
Copyright (C) 2009 Frank Bos
Copyright (C) 2016-2019 Applied CCM Pty Ltd
-------------------------------------------------------------------------------
License
    This file is part of CAELUS.

    CAELUS is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    CAELUS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with CAELUS.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "RBFMotionSolver.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "transformField.hpp"
#include "ListListOps.hpp"
#include "mergePoints.hpp"


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    defineTypeNameAndDebug(RBFMotionSolver, 0);

    addToRunTimeSelectionTable
    (
        motionSolver,
        RBFMotionSolver,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::RBFMotionSolver::mergeControlPoints()
{
    // Collect all control points from all processors
    if (Pstream::parRun())
    {
        // Initialise label to hold final size of allControlPoints list
        label allControlPointsSize(0);

        // Gather data from all processors
        List<vectorField> gatheredData(Pstream::nProcs());
        gatheredData[Pstream::myProcNo()] = controlPoints_;
        Pstream::gatherList(gatheredData);

        // Assemble a single list of all control points on the master.
        // May contain duplicate entries due to processor boundaries.
        if (Pstream::master())
        {
            vectorField allControlPointsMaster
            (
                ListListOps::combine<vectorField>
                (
                    gatheredData,
                    CML::accessOp<vectorField>()
                )
            );

            // Merge duplicates, sort and update field on master
            labelList oldToNew;
            bool hasMerged = mergePoints
            (
                allControlPointsMaster, // combined field before merging
                SMALL,                  // tolerance for merge
                false,                  // verbosity
                oldToNew,               // mapping - not used
                allControlPoints_       // updated field
            );

            // Make reverse map. A labelListList because duplicates in the 
            // original list will result in multiple mapping.
            labelListList newToOldLL
            (
                invertOneToMany(allControlPoints_.size(), oldToNew)
            );

            // Reverse map that removes duplicate mapping by selecting the
            // lowest entry from duplicates
            map_.setSize(allControlPoints_.size());

            forAll(map_, i)
            {
                labelList oldIndicies = newToOldLL[i];
                map_[i] = oldIndicies[findMin(oldIndicies)];
            }

            // Sort map to keep origianl order before merging of duplicates
            map_.sort();

            forAll(map_, i)
            {
                allControlPoints_[i] = allControlPointsMaster[map_[i]];
            }

            if (debug)
            {
                Info<<"Control points have been merged  = "<< hasMerged << endl;            
            }

            // Parallel data exchange - send size of control points
            for
            (
                int slave=Pstream::firstSlave();
                slave<=Pstream::lastSlave();
                slave++
            )
            {
                OPstream toSlaveSize(Pstream::blocking, slave, sizeof(label));
                toSlaveSize << allControlPoints_.size();
            }

            // Parallel data exchange - send control points
            for
            (
                int slave=Pstream::firstSlave();
                slave<=Pstream::lastSlave();
                slave++
            )
            {
                OPstream toSlaveData
                (
                    Pstream::blocking,
                    slave,
                    allControlPoints_.size()*sizeof(vector)
                );
                toSlaveData << allControlPoints_;
            }
        }
        else
        {
            // Parallel data exchange - receive size of control points
            IPstream fromMasterSize
            (
                Pstream::blocking, Pstream::masterNo(), sizeof(label)
            );
            fromMasterSize >> allControlPointsSize;

            // Parallel data exchange - receive all control points
            IPstream fromMasterData
            (
                Pstream::blocking,
                Pstream::masterNo(),
                allControlPointsSize*sizeof(vector)
            );
            fromMasterData >> allControlPoints_;
        }
    }
    else
    {
        allControlPoints_ = controlPoints_;
    }

}


void CML::RBFMotionSolver::makeControlIDs()
{
    // Points that are neither on moving nor on static patches
    // will be marked with 0
    labelList markedPoints(mesh().nPoints(), 0);

    // Mark all points on moving patches with 1
    label nMovingPoints = 0;

    forAll (movingPatches_, patchi)
    {
        // Find the patch in boundary
        label patchIndex =
            mesh().boundaryMesh().findPatchID(movingPatches_[patchi]);

        if (patchIndex < 0)
        {
            FatalErrorInFunction
                << "Patch " << movingPatches_[patchi] << " not found.  "
                << "valid patch names: " << mesh().boundaryMesh().names()
                << abort(FatalError);
        }

        const labelList& mp = mesh().boundaryMesh()[patchIndex].meshPoints();

        forAll (mp, i)
        {
            markedPoints[mp[i]] = 1;
            nMovingPoints++;
        }
    }

    // Mark moving points and select control points from moving patches
    movingIDs_.setSize(nMovingPoints);
    label totalnMovingPoints = nMovingPoints;

    reduce(totalnMovingPoints, sumOp<label>());
    Info<< "Total points on moving boundaries: " << totalnMovingPoints << endl;

    const pointField& points = mesh().points();

    // Re-use counter to count moving points
    // Note: the control points also hold static points in the second part
    // of the list if static patches are included in the RBF
    nMovingPoints = 0;

    // Count moving points first
    forAll (markedPoints, i)
    {
        if (markedPoints[i] == 1)
        {
            // Grab internal point
            movingIDs_[nMovingPoints] = i;
            nMovingPoints++;
        }
    }

    // Allocate size
    movingIDs_.setSize(nMovingPoints);

    // Actual location of moving points will be set later on request
    movingPoints_.setSize(nMovingPoints, Zero);

    // Mark all points on static patches with -1
    label nStaticPoints = 0;

    forAll (staticPatches_, patchi)
    {
        // Find the patch in boundary
        label patchIndex =
            mesh().boundaryMesh().findPatchID(staticPatches_[patchi]);

        if (patchIndex < 0)
        {
            FatalErrorInFunction
                << "Patch " << staticPatches_[patchi] << " not found.  "
                << "valid patch names: " << mesh().boundaryMesh().names()
                << abort(FatalError);
        }

        const labelList& mp = mesh().boundaryMesh()[patchIndex].meshPoints();

        forAll (mp, i)
        {
            markedPoints[mp[i]] = -1;
            nStaticPoints++;
        }
    }

    // Calculate total number of static points. Note: could contain duplicates
    // from processor boundaries
    label totalnStaticPoints = nStaticPoints;
    reduce(totalnStaticPoints, sumOp<label>());

    Info<< "Total points on static boundaries: " << totalnStaticPoints << endl;
    staticIDs_.setSize(nStaticPoints);

    // Re-use counter
    nStaticPoints = 0;

    // Count total number of control points
    forAll (markedPoints, i)
    {
        if (markedPoints[i] == -1)
        {
            staticIDs_[nStaticPoints] = i;
            nStaticPoints++;
        }
    }

    staticIDs_.setSize(nStaticPoints);

    // Control IDs also potentially include points on static patches
    controlIDs_.setSize(movingIDs_.size() + staticIDs_.size());
    motion_.setSize(controlIDs_.size(), Zero);

    label nControlPoints = 0;

    forAll (movingPatches_, patchi)
    {
        // Find the patch in boundary
        label patchIndex =
            mesh().boundaryMesh().findPatchID(movingPatches_[patchi]);

        const labelList& mp = mesh().boundaryMesh()[patchIndex].meshPoints();

        for
        (
            label pickedPoint = 0;
            pickedPoint < mp.size();
            pickedPoint += coarseningRatio_
        )
        {
            // Pick point as control point
            controlIDs_[nControlPoints] = mp[pickedPoint];

            // Mark the point as picked
            markedPoints[mp[pickedPoint]] = 2;
            nControlPoints++;
        }
    }

    // Calculate total number of control points. Note: Could contain duplicates
    // because of processor boundaries
    label totalnControlPoints =  nControlPoints;
    reduce(totalnControlPoints, sumOp<label>());

    Info<< "Selected " << totalnControlPoints
        << " control points on moving boundaries" << endl;

    if (includeStaticPatches_)
    {
        forAll (staticPatches_, patchi)
        {
            // Find the patch in boundary
            label patchIndex =
                mesh().boundaryMesh().findPatchID(staticPatches_[patchi]);

            const labelList& mp =
                mesh().boundaryMesh()[patchIndex].meshPoints();

            for
            (
                label pickedPoint = 0;
                pickedPoint < mp.size();
                pickedPoint += coarseningRatio_
            )
            {
                // Pick point as control point
                controlIDs_[nControlPoints] = mp[pickedPoint];

                // Mark the point as picked
                markedPoints[mp[pickedPoint]] = 2;
                nControlPoints++;
            }
        }

        // Calculate total number of control points. Note: Could contain
        // duplicates because of processor boundaries
        totalnControlPoints =  nControlPoints;
        reduce(totalnControlPoints, sumOp<label>());

        Info<< "Selected " << nControlPoints
            << " total control points" << endl;
    }

    // Resize control IDs
    controlIDs_.setSize(nControlPoints);

    // Pick up point locations
    controlPoints_.setSize(nControlPoints);

    // Set control points
    forAll (controlIDs_, i)
    {
        controlPoints_[i] = points[controlIDs_[i]];
    }

    // Merge control points. Handles duplicate points that may exisit in
    // parallel simulations
    mergeControlPoints();

    // Pick up all internal points
    internalIDs_.setSize(points.size());
    internalPoints_.setSize(points.size());

    // Count internal points
    label nInternalPoints = 0;

    forAll (markedPoints, i)
    {
        if (markedPoints[i] == 0)
        {
            // Grab internal point
            internalIDs_[nInternalPoints] = i;
            internalPoints_[nInternalPoints] = points[i];
            nInternalPoints++;
        }
    }

    label totalnInternalPoints  =  nInternalPoints ;
    reduce(totalnInternalPoints , sumOp<label>());
    Info << "Number of internal points: " << totalnInternalPoints << endl;

    // Resize the lists
    internalIDs_.setSize(nInternalPoints);
    internalPoints_.setSize(nInternalPoints);
}


void CML::RBFMotionSolver::setMovingPoints() const
{
    const pointField& points = mesh().points();

    // Set moving points
    forAll (movingIDs_, i)
    {
        movingPoints_[i] = points[movingIDs_[i]];
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::RBFMotionSolver::RBFMotionSolver
(
    const polyMesh& mesh,
    const IOdictionary& dict
)
:
    motionSolver(mesh,dict,typeName),
    SBMFPtr_(solidBodyMotionFunction::New(coeffDict(), mesh.time(), false)),
    movingPatches_(coeffDict().lookup("movingPatches")),
    staticPatches_(coeffDict().lookup("staticPatches")),
    coarseningRatio_(readLabel(coeffDict().lookup("coarseningRatio"))),
    includeStaticPatches_(coeffDict().lookup("includeStaticPatches")),
    frozenInterpolation_(coeffDict().lookup("frozenInterpolation")),
    movingIDs_(0),
    movingPoints_(0),
    staticIDs_(0),
    controlIDs_(0),
    map_(0),
    controlPoints_(0),
    allControlPoints_(0),
    internalIDs_(0),
    internalPoints_(0),
    motion_(0),
    interpolation_
    (
        coeffDict().subDict("interpolation"),
        allControlPoints_,
        internalPoints_
    )
{
    makeControlIDs();

    undisplacedPoints_ = movingPoints();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::RBFMotionSolver::~RBFMotionSolver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::RBFMotionSolver::setMotion(const vectorField& m)
{
    if (m.size() != movingIDs_.size())
    {
        FatalErrorInFunction
            << "Incorrect size of motion points: m = " << m.size()
            << " movingIDs = " << movingIDs_.size()
            << abort(FatalError);
    }

    // Motion of static points is zero and moving points are first
    // in the list.
    motion_ = Zero;

    forAll (m, i)
    {
        motion_[i] = m[i];
    }

    if (!frozenInterpolation_)
    {
        // Set control points
        const pointField& points = mesh().points();

        forAll (controlIDs_, i)
        {
            controlPoints_[i] = points[controlIDs_[i]];
        }

        // Merge control points. Handles duplicate points that may exisit in
        // parallel simulations
        mergeControlPoints();

        // Re-calculate interpolation
        interpolation_.movePoints();
    }
}


const CML::vectorField& CML::RBFMotionSolver::movingPoints() const
{
    // Update moving points based on current mesh
    setMovingPoints();

    return movingPoints_;
}


CML::tmp<CML::pointField> CML::RBFMotionSolver::curPoints() const
{
    // Prepare new points: same as old point
    tmp<pointField> tcurPoints
    (
        new vectorField(mesh().nPoints(), Zero)
    );
    pointField& curPoints = tcurPoints();

    // Add motion to existing points

    // 1. Insert prescribed motion of moving points
    forAll (movingIDs_, i)
    {
        curPoints[movingIDs_[i]] = motion_[i];
    }

    // 2. Insert zero motion of static points
    forAll (staticIDs_, i)
    {
        curPoints[staticIDs_[i]] = Zero;
    }

    // Set motion of control points
    vectorField motionOfControl(controlIDs_.size());

    // 3. Capture positions of control points
    forAll (controlIDs_, i)
    {
        motionOfControl[i] = curPoints[controlIDs_[i]];
    }

    // Motion of all control points
    vectorField motionOfAllControl(motionOfControl);

    // Collect motion of all control points from all CPUs
    if (Pstream::parRun())
    {
        // Size of list 
        label motionOfAllControlSize(motionOfAllControl.size());

        // Gather motion lists from all processors
        List<vectorField> gatheredData(Pstream::nProcs());
        gatheredData[Pstream::myProcNo()] = motionOfControl;
        Pstream::gatherList(gatheredData);

        if (Pstream::master())
        {
            // Assemble single list on master
            vectorField motionOfAllControlMaster
            (
                ListListOps::combine<vectorField>
                (
                    gatheredData,
                    CML::accessOp<vectorField>()
                )
            );

            // Allocate final combined list size. Should be the size of the map
            // from the control points
            motionOfAllControl.setSize(map_.size());

            // Map original to final, removing duplicates.
            forAll(map_, i)
            {
                motionOfAllControl[i] = motionOfAllControlMaster[map_[i]];
            }

            // Parallel data exchange - send size of motion data
            for
            (
                int slave=Pstream::firstSlave();
                slave<=Pstream::lastSlave();
                slave++
            )
            {
                OPstream toSlaveSize(Pstream::blocking, slave, sizeof(label));
                toSlaveSize << motionOfAllControl.size();
            }

            // Parallel data exchange - send motion data
            for
            (
                int slave=Pstream::firstSlave();
                slave<=Pstream::lastSlave();
                slave++
            )
            {
                OPstream toSlaveData
                (
                    Pstream::blocking,
                    slave,
                    motionOfAllControl.size()*sizeof(vector)
                );
                toSlaveData << motionOfAllControl;
            }
        }
        else
        {
            // Parallel data exchange - receive size of motion data
            IPstream fromMasterSize
            (
                Pstream::blocking, Pstream::masterNo(), sizeof(label)
            );
            fromMasterSize >> motionOfAllControlSize;

            // Parallel data exchange - receive motion data
            IPstream fromMasterData
            (
                Pstream::blocking,
                Pstream::masterNo(),
                motionOfAllControlSize*sizeof(vector)
            );
            fromMasterData >> motionOfAllControl;
        }
    }

    // Call interpolation
    vectorField interpolatedMotion
    (
        interpolation_.interpolate(motionOfAllControl)
    );

    // 4. Insert RBF interpolated motion
    forAll (internalIDs_, i)
    {
        curPoints[internalIDs_[i]] = interpolatedMotion[i];
    }

    // 5. Add old point positions
    curPoints += mesh().points();

    twoDCorrectPoints(tcurPoints());

    return tcurPoints;
}


void CML::RBFMotionSolver::solve()
{
    // Motion is a vectorField of all moving boundary points
    vectorField motion(movingPoints().size(), Zero);

    vectorField oldPoints = movingPoints();
    vectorField newPoints = oldPoints;

    // Transform undisplacedPoints into newPoints
    CML::transformPoints(newPoints, SBMFPtr_().transformation(), undisplacedPoints_);

    // Work out change in position
    motion = newPoints - oldPoints;

    setMotion(motion);

}


void CML::RBFMotionSolver::movePoints(const pointField&)
{
    // No local data to update
}


void CML::RBFMotionSolver::updateMesh(const mapPolyMesh&)
{
    // Recalculate control point IDs
    makeControlIDs();
}
