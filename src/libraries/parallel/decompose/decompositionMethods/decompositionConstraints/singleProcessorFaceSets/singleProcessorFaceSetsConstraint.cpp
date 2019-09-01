/*---------------------------------------------------------------------------*\
Copyright (C) 2015 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "singleProcessorFaceSetsConstraint.hpp"
#include "addToRunTimeSelectionTable.hpp"
#include "syncTools.hpp"
#include "faceSet.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace decompositionConstraints
{
defineTypeName(singleProcessorFaceSetsConstraint);
addToRunTimeSelectionTable
(
    decompositionConstraint,
    singleProcessorFaceSetsConstraint,
    dictionary
);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::decompositionConstraints::singleProcessorFaceSetsConstraint::
singleProcessorFaceSetsConstraint
(
    const dictionary& constraintsDict,
    const word& modelType
)
:
    decompositionConstraint(constraintsDict, typeName),
    setNameAndProcs_(coeffDict_.lookup("singleProcessorFaceSets"))
{
    if (decompositionConstraint::debug)
    {
        Info<< type()
            << " : adding constraints to keep" << endl;

        forAll(setNameAndProcs_, setI)
        {
            Info<< "    all cells connected to faceSet "
                << setNameAndProcs_[setI].first()
                << " on processor " << setNameAndProcs_[setI].second() << endl;
        }
    }
}


CML::decompositionConstraints::singleProcessorFaceSetsConstraint::
singleProcessorFaceSetsConstraint
(
    const List<Tuple2<word, label>>& setNameAndProcs
)
:
    decompositionConstraint(dictionary(), typeName),
    setNameAndProcs_(setNameAndProcs)
{
    if (decompositionConstraint::debug)
    {
        Info<< type()
            << " : adding constraints to keep" << endl;

        forAll(setNameAndProcs_, setI)
        {
            Info<< "    all cells connected to faceSet "
                << setNameAndProcs_[setI].first()
                << " on processor " << setNameAndProcs_[setI].second() << endl;
        }
    }
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void CML::decompositionConstraints::singleProcessorFaceSetsConstraint::add
(
    const polyMesh& mesh,
    boolList& blockedFace,
    PtrList<labelList>& specifiedProcessorFaces,
    labelList& specifiedProcessor,
    List<labelPair>& explicitConnections
) const
{
    blockedFace.setSize(mesh.nFaces(), true);

    // Mark faces already in set
    labelList faceToSet(mesh.nFaces(), -1);
    forAll(specifiedProcessorFaces, setI)
    {
        const labelList& faceLabels = specifiedProcessorFaces[setI];
        forAll(faceLabels, i)
        {
            faceToSet[faceLabels[i]] = setI;
        }
    }


    forAll(setNameAndProcs_, setI)
    {
        //Info<< "Keeping all cells connected to faceSet "
        //    << setNameAndProcs_[setI].first()
        //    << " on processor " << setNameAndProcs_[setI].second() << endl;

        const label destProcI = setNameAndProcs_[setI].second();

        // Read faceSet
        const faceSet fz(mesh, setNameAndProcs_[setI].first());

        // Check that it does not overlap with existing specifiedProcessorFaces
        labelList nMatch(specifiedProcessorFaces.size(), 0);
        forAllConstIter(faceSet, fz, iter)
        {
            label setI = faceToSet[iter.key()];
            if (setI != -1)
            {
                nMatch[setI]++;
            }
        }


        // Only store if all faces are not yet in specifiedProcessorFaces
        // (on all processors)
        bool store = true;

        forAll(nMatch, setI)
        {
            if (nMatch[setI] == fz.size())
            {
                // full match
                store = false;
                break;
            }
            else if (nMatch[setI] > 0)
            {
                // partial match
                store = false;
                break;
            }
        }

        reduce(store, andOp<bool>());


        if (store)
        {
            specifiedProcessorFaces.append(new labelList(fz.sortedToc()));
            specifiedProcessor.append(destProcI);
        }
    }


    // Unblock all point connected faces
    // 1. Mark all points on specifiedProcessorFaces
    boolList procFacePoint(mesh.nPoints(), false);
    forAll(specifiedProcessorFaces, setI)
    {
        const labelList& set = specifiedProcessorFaces[setI];
        forAll(set, fI)
        {
            const face& f = mesh.faces()[set[fI]];
            forAll(f, fp)
            {
                procFacePoint[f[fp]] = true;
            }
        }
    }
    syncTools::syncPointList(mesh, procFacePoint, orEqOp<bool>(), false);

    // 2. Unblock all faces on procFacePoint

    label nUnblocked = 0;

    forAll(procFacePoint, pointi)
    {
        if (procFacePoint[pointi])
        {
            const labelList& pFaces = mesh.pointFaces()[pointi];
            forAll(pFaces, i)
            {
                if (blockedFace[pFaces[i]])
                {
                    blockedFace[pFaces[i]] = false;
                    nUnblocked++;
                }
            }
        }
    }

    if (decompositionConstraint::debug & 2)
    {
        reduce(nUnblocked, sumOp<label>());
        Info<< type() << " : unblocked " << nUnblocked << " faces" << endl;
    }

    syncTools::syncFaceList(mesh, blockedFace, andEqOp<bool>());
}


void CML::decompositionConstraints::singleProcessorFaceSetsConstraint::apply
(
    const polyMesh& mesh,
    const boolList& blockedFace,
    const PtrList<labelList>& specifiedProcessorFaces,
    const labelList& specifiedProcessor,
    const List<labelPair>& explicitConnections,
    labelList& decomposition
) const
{
    // For specifiedProcessorFaces rework the cellToProc to enforce
    // all on one processor since we can't guarantee that the input
    // to regionSplit was a single region.
    // E.g. faceSet 'a' with the cells split into two regions
    // by a notch formed by two walls
    //
    //          \   /
    //           \ /
    //    ---a----+-----a-----
    //
    //
    // Note that reworking the cellToProc might make the decomposition
    // unbalanced.
    label nChanged = 0;

    forAll(specifiedProcessorFaces, setI)
    {
        const labelList& set = specifiedProcessorFaces[setI];

        // Get the processor to use for the set
        label proci = specifiedProcessor[setI];
        if (proci == -1)
        {
            // If no processor specified use the one from the
            // 0th element
            if (set.size())
            {
                proci = decomposition[mesh.faceOwner()[set[0]]];
            }
            reduce(proci, maxOp<label>());
        }

        // Get all points on the sets
        boolList procFacePoint(mesh.nPoints(), false);
        forAll(set, fI)
        {
            const face& f = mesh.faces()[set[fI]];
            forAll(f, fp)
            {
                procFacePoint[f[fp]] = true;
            }
        }
        syncTools::syncPointList(mesh, procFacePoint, orEqOp<bool>(), false);

        // 2. Unblock all faces on procFacePoint
        forAll(procFacePoint, pointi)
        {
            if (procFacePoint[pointi])
            {
                const labelList& pFaces = mesh.pointFaces()[pointi];
                forAll(pFaces, i)
                {
                    label facei = pFaces[i];

                    label own = mesh.faceOwner()[facei];
                    if (decomposition[own] != proci)
                    {
                        decomposition[own] = proci;
                        nChanged++;
                    }
                    if (mesh.isInternalFace(facei))
                    {
                        label nei = mesh.faceNeighbour()[facei];
                        if (decomposition[nei] != proci)
                        {
                            decomposition[nei] = proci;
                            nChanged++;
                        }
                    }
                }
            }
        }
    }

    if (decompositionConstraint::debug & 2)
    {
        reduce(nChanged, sumOp<label>());
        Info<< type() << " : changed decomposition on " << nChanged
            << " cells" << endl;
    }
}


// ************************************************************************* //
