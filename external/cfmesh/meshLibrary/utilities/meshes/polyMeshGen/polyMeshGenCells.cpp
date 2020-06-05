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

#include "polyMeshGenCells.hpp"
#include "polyMeshGenAddressing.hpp"
#include "IOobjectList.hpp"
#include "cellSet.hpp"
#include "demandDrivenData.hpp"

#include "labelPair.hpp"

# ifdef USE_OMP
#include <omp.h>
# endif

namespace CML
{

// * * * * * * * * * * Private member functions * * * * * * * * * * * * * * * //

void polyMeshGenCells::calculateOwnersAndNeighbours() const
{
    if( ownerPtr_ || neighbourPtr_ )
        FatalErrorInFunction
          << "Owners and neighbours are already allocated" << abort(FatalError);

    //- allocate owners
    ownerPtr_ =
        new labelIOList
        (
            IOobject
            (
                "owner",
                runTime_.constant(),
                "polyMesh",
                runTime_
            ),
            faces_.size()
        );
    labelIOList& own = *ownerPtr_;

    //- allocate neighbours
    neighbourPtr_ =
        new labelIOList
        (
            IOobject
            (
                "neighbour",
                runTime_.constant(),
                "polyMesh",
                runTime_
            ),
            faces_.size()
        );
    labelIOList& nei = *neighbourPtr_;

    //- start calculating owners and neighbours
    nIntFaces_ = 0;

    # ifdef USE_OMP
    const label nThreads = 3 * omp_get_num_procs();
    const label chunkSize = faces_.size() / nThreads + 1;
    # else
    const label nThreads = 1;
    const label chunkSize = faces_.size();
    # endif

    label nInternalFaces(0);

    List<List<LongList<labelPair>>> dataForOtherThreads(nThreads);

    # ifdef USE_OMP
    # pragma omp parallel num_threads(nThreads) reduction(+ : nInternalFaces)
    # endif
    {
        # ifdef USE_OMP
        const label threadI = omp_get_thread_num();
        # else
        const label threadI(0);
        # endif

        const label startingFace = threadI * chunkSize;
        const label endFace =
            CML::min(startingFace + chunkSize, faces_.size());

        List<LongList<labelPair>>& dot = dataForOtherThreads[threadI];
        dot.setSize(nThreads);

        for(label facei=startingFace;facei<endFace;++facei)
        {
            own[facei] = -1;
            nei[facei] = -1;
        }

        # ifdef USE_OMP
        # pragma omp for schedule(static)
        # endif
        forAll(cells_, celli)
        {
            const cell& c = cells_[celli];

            forAll(c, fI)
            {
                const label facei = c[fI];

                const label threadNo = facei / chunkSize;

                if( threadNo == threadI )
                {
                    if( own[facei] == -1 )
                    {
                        own[facei] = celli;
                    }
                    else if( nei[facei] == -1 )
                    {
                        nei[facei] = celli;
                        ++nInternalFaces;
                    }
                    else
                    {
                        Serr << "Face " << faces_[facei] << endl;
                        Serr << "Owner " << own[facei] << endl;
                        Serr << "Neighbour " << nei[facei] << endl;
                        Serr << "Current cell " << celli << endl;
                        FatalErrorInFunction
                            << Pstream::myProcNo() << "Face " << facei
                            << " appears in more than 2 cells!!"
                            << abort(FatalError);
                    }
                }
                else
                {
                    dot[threadNo].append(labelPair(facei, celli));
                }
            }
        }

        # ifdef USE_OMP
        # pragma omp barrier

        # pragma omp critical
        # endif
        for(label i=0;i<nThreads;++i)
        {
            const LongList<labelPair>& data =
                dataForOtherThreads[i][threadI];

            forAll(data, j)
            {
                const label facei = data[j].first();
                const label celli = data[j].second();

                if( own[facei] == -1 )
                {
                    own[facei] = celli;
                }
                else if( own[facei] > celli )
                {
                    if( nei[facei] == -1 )
                    {
                        nei[facei] = own[facei];
                        own[facei] = celli;
                        ++nInternalFaces;
                    }
                    else
                    {
                        Serr << "Face " << faces_[facei] << endl;
                        Serr << "Owner " << own[facei] << endl;
                        Serr << "Neighbour " << nei[facei] << endl;
                        Serr << "Current cell " << celli << endl;
                        FatalErrorInFunction
                            << Pstream::myProcNo() << "Face " << facei
                            << " appears in more than 2 cells!!"
                            << abort(FatalError);
                    }
                }
                else if( nei[facei] == -1 )
                {
                    nei[facei] = celli;
                    ++nInternalFaces;
                }
                else
                {
                    Serr << "Face " << faces_[facei] << endl;
                    Serr << "Owner " << own[facei] << endl;
                    Serr << "Neighbour " << nei[facei] << endl;
                    Serr << "Current cell " << celli << endl;
                    FatalErrorInFunction
                        << Pstream::myProcNo() << "Face " << facei
                        << " appears in more than 2 cells!!"
                        << abort(FatalError);
                }
            }
        }
    }

    nIntFaces_ = nInternalFaces;
}

void polyMeshGenCells::calculateAddressingData() const
{
    if( !ownerPtr_ || !neighbourPtr_ )
    {
        # ifdef USE_OMP
        if( omp_in_parallel() )
            FatalErrorInFunction
                << "Calculating addressing inside a parallel region."
                << " This is not thread safe" << exit(FatalError);
        # endif

        calculateOwnersAndNeighbours();
    }

    addressingDataPtr_ = new polyMeshGenAddressing(*this);
}

void polyMeshGenCells::clearOut() const
{
    polyMeshGenFaces::clearOut();
    deleteDemandDrivenData(addressingDataPtr_);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Constructors
//- Null constructor
polyMeshGenCells::polyMeshGenCells(const Time& runTime)
:
    polyMeshGenFaces(runTime),
    cells_(),
    cellSubsets_(),
    addressingDataPtr_(nullptr)
{
}

//- Construct from components without the boundary
polyMeshGenCells::polyMeshGenCells
(
    const Time& runTime,
    const pointField& points,
    const faceList& faces,
    const cellList& cells
)
:
    polyMeshGenFaces(runTime, points, faces),
    cells_(),
    cellSubsets_(),
    addressingDataPtr_(nullptr)
{
    cells_ = cells;
}

//- Construct from components with the boundary
polyMeshGenCells::polyMeshGenCells
(
    const Time& runTime,
    const pointField& points,
    const faceList& faces,
    const cellList& cells,
    const wordList& patchNames,
    const labelList& patchStart,
    const labelList& nFacesInPatch
)
:
    polyMeshGenFaces
    (
        runTime,
        points,
        faces,
        patchNames,
        patchStart,
        nFacesInPatch
    ),
    cells_(),
    cellSubsets_(),
    addressingDataPtr_(nullptr)
{
    cells_ = cells;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Destructor
polyMeshGenCells::~polyMeshGenCells()
{
    clearOut();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//- return addressing which may be needed
const polyMeshGenAddressing& polyMeshGenCells::addressingData() const
{
    if( !addressingDataPtr_ )
    {
        # ifdef USE_OMP
        if( omp_in_parallel() )
            FatalErrorInFunction
                << "Calculating addressing inside a parallel region."
                << " This is not thread safe" << exit(FatalError);
        # endif

        calculateAddressingData();
    }

    return *addressingDataPtr_;
}

void polyMeshGenCells::clearAddressingData() const
{
    deleteDemandDrivenData(addressingDataPtr_);
}

label polyMeshGenCells::addCellSubset(const word& selName)
{
    label id = cellSubsetIndex(selName);
    if( id >= 0 )
    {
        Warning << "Cell subset " << selName << " already exists!" << endl;
        return id;
    }

    id = 0;
    for
    (
        std::map<label, meshSubset>::const_iterator it=cellSubsets_.begin();
        it!=cellSubsets_.end();
        ++it
    )
        id = CML::max(id, it->first+1);

    cellSubsets_.insert
    (
        std::make_pair
        (
            id,
            meshSubset(selName, meshSubset::CELLSUBSET)
        )
    );

    return id;
}

void polyMeshGenCells::removeCellSubset(const label setI)
{
    if( cellSubsets_.find(setI) == cellSubsets_.end() )
        return;

    cellSubsets_.erase(setI);
}

word polyMeshGenCells::cellSubsetName(const label setI) const
{
    std::map<label, meshSubset>::const_iterator it =
        cellSubsets_.find(setI);
    if( it == cellSubsets_.end() )
    {
        Warning << "Subset " << setI << " is not a cell subset" << endl;
        return word();
    }

    return it->second.name();
}

label polyMeshGenCells::cellSubsetIndex(const word& selName) const
{
    std::map<label, meshSubset>::const_iterator it;
    for(it=cellSubsets_.begin();it!=cellSubsets_.end();++it)
    {
        if( it->second.name() == selName )
            return it->first;
    }

    return -1;
}

void polyMeshGenCells::read()
{
    polyMeshGenFaces::read();

    Info << "Starting creating cells" << endl;
    //- count the number of cells and create the cells
    label nCells(0);
    const labelList& own = this->owner();
    const labelList& nei = this->neighbour();

    forAll(own, facei)
    {
        if( own[facei] >= nCells )
            nCells = own[facei] + 1;

        if( nei[facei] >= nCells )
            nCells = nei[facei] + 1;
    }

    List<label> nFacesInCell(nCells, label(0));
    forAll(own, facei)
        ++nFacesInCell[own[facei]];

    forAll(nei, facei)
        if( nei[facei] != -1 )
            ++nFacesInCell[nei[facei]];

    cells_.setSize(nCells);
    forAll(cells_, celli)
        cells_[celli].setSize(nFacesInCell[celli]);

    nFacesInCell = 0;
    forAll(own, facei)
    {
        cells_[own[facei]][nFacesInCell[own[facei]]++] = facei;
        if( nei[facei] != -1 )
            cells_[nei[facei]][nFacesInCell[nei[facei]]++] = facei;
    }

    // read cell subsets
    IOobjectList allSets
    (
        runTime_,
        runTime_.constant(),
        "polyMesh/sets"
    );

    wordList setNames = allSets.names("cellSet");
    forAll(setNames, setI)
    {
        IOobject* obj = allSets.lookup(setNames[setI]);

        cellSet cSet(*obj);

        const labelList content = cSet.toc();
        const label id = addCellSubset(setNames[setI]);

        cellSubsets_[id].updateSubset(content);
    }
}

void polyMeshGenCells::write() const
{
    polyMeshGenFaces::write();

    //- write cell subsets
    std::map<label, meshSubset>::const_iterator setIt;
    for(setIt=cellSubsets_.begin();setIt!=cellSubsets_.end();++setIt)
    {
        cellSet set
        (
            IOobject
            (
                setIt->second.name(),
                runTime_.constant(),
                "polyMesh/sets",
                runTime_,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            )
        );

        labelLongList containedElements;
        setIt->second.containedElements(containedElements);

        forAll(containedElements, i)
            set.insert(containedElements[i]);
        set.write();
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
