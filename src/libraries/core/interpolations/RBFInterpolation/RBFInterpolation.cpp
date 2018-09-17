/*---------------------------------------------------------------------------*\
 Copyright 2009 TU Delft
 Copyright 2009 FSB Zagreb
 Copyright 2018 Applied CCM Pty Ltd
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

Author
    Frank Bos, TU Delft.
    Dubravko Matijasevic, FSB Zagreb.

\*---------------------------------------------------------------------------*/

#include "RBFInterpolation.hpp"
#include "demandDrivenData.hpp"
#include "ListListOps.hpp"
#include "OPstream.hpp"
#include "IPstream.hpp"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void CML::RBFInterpolation::updateAllControlPoints() const 
{ 
    allControlPoints_ = controlPoints_;

    // Collect ALL control points from ALL CPUs
    // Create an identical inverse for all CPUs
    if (Pstream::parRun())
    {
        label totalControlPoints = controlPoints_.size();
        reduce(totalControlPoints, sumOp<label>());

        allControlPoints_.resize(totalControlPoints);

        // Collect data from all processors
        List<vectorField> gatheredData(Pstream::nProcs());
        gatheredData[Pstream::myProcNo()] = controlPoints_;
        Pstream::gatherList(gatheredData);

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

            // Update field on master
            allControlPoints_ = allControlPointsMaster;

            // Parallel data exchange - send
            for
            (
                int slave=Pstream::firstSlave();
                slave<=Pstream::lastSlave();
                slave++
            )
            {
                OPstream toSlave(Pstream::blocking, slave, allControlPoints_.size()*sizeof(vector));
                toSlave << allControlPoints_;
            }
        }
        else
        {
            // Parallel data exchange - receive
            IPstream fromMaster(Pstream::blocking, Pstream::masterNo(), allControlPoints_.size()*sizeof(vector));
            fromMaster >> allControlPoints_;
        }
    }
}


const CML::scalarSquareMatrix& CML::RBFInterpolation::B() const
{
    if (!BPtr_)
    {
        calcB();
    }

    return *BPtr_;
}


void CML::RBFInterpolation::calcB() const
{ 
    // Update allControlPoints
    updateAllControlPoints();

    // Determine inverse of boundary connectivity matrix
    label polySize(4);

    if (!polynomials_)
    {
        polySize = 0;
    }

    // Fill Nb x Nb matrix
    simpleMatrix<scalar> A(allControlPoints_.size()+polySize);

    const label nControlPoints = allControlPoints_.size();
    for (label i = 0; i < nControlPoints; i++)
    {
        scalarField weights(RBF_->weights(allControlPoints_, allControlPoints_[i]));

        for (label col = 0; col < nControlPoints; col++)
        {
            A[i][col] = weights[col];
        }
    }

    if (polynomials_)
    {
        for(label row = nControlPoints; row < (nControlPoints + 1); row++)
        {
            for (label col = 0; col < nControlPoints; col++)
            {
                A[col][row] = 1.0;
                A[row][col] = 1.0;
            }
        }

        // Fill in X components of polynomial part of matrix
        for(label row = (nControlPoints + 1); row < (nControlPoints + 2); row++)
        {
            for (label col = 0; col < nControlPoints; col++)
            {
                A[col][row] = allControlPoints_[col].x();
                A[row][col] = allControlPoints_[col].x();
            }
        }

        // Fill in Y components of polynomial part of matrix
        for(label row = (nControlPoints + 2); row < (nControlPoints + 3); row++)
        {
            for (label col = 0; col < nControlPoints; col++)
            {
                A[col][row] = allControlPoints_[col].y();
                A[row][col] = allControlPoints_[col].y();
            }
        }
        // Fill in Z components of polynomial part of matrix
        for(label row = (nControlPoints + 3); row < (nControlPoints + 4); row++)
        {
            for (label col = 0; col < nControlPoints; col++)
            {
                A[col][row] = allControlPoints_[col].z();
                A[row][col] = allControlPoints_[col].z();
            }
        }

        // Fill 4x4 zero part of matrix
        for(label row = nControlPoints; row < (nControlPoints + 4); row++)
        {
            for(label col = nControlPoints; col < (nControlPoints + 4); col++)
            {
                A[row][col] = 0.0;
            }
        }
    }

    Info<< "Inverting RBF motion matrix" << endl;

    BPtr_ = new scalarSquareMatrix(A.LUinvert());
}


void CML::RBFInterpolation::clearOut()
{
    deleteDemandDrivenData(BPtr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::RBFInterpolation::RBFInterpolation
(
    const dictionary& dict,
    const vectorField& controlPoints,
    const vectorField& allPoints
)
:
    dict_(dict),
    controlPoints_(controlPoints),
    allPoints_(allPoints),
    RBF_(RBFFunction::New(word(dict.lookup("RBF")), dict)),
    BPtr_(NULL),
    focalPoint_(dict.lookup("focalPoint")),
    innerRadius_(readScalar(dict.lookup("innerRadius"))),
    outerRadius_(readScalar(dict.lookup("outerRadius"))),
    polynomials_(dict.lookup("polynomials"))
{
    updateAllControlPoints();
}


CML::RBFInterpolation::RBFInterpolation
(
    const RBFInterpolation& rbf
)
:
    dict_(rbf.dict_),
    controlPoints_(rbf.controlPoints_),
    allPoints_(rbf.allPoints_),
    RBF_(rbf.RBF_->clone()),
    BPtr_(NULL),
    focalPoint_(rbf.focalPoint_),
    innerRadius_(rbf.innerRadius_),
    outerRadius_(rbf.outerRadius_),
    polynomials_(rbf.polynomials_)
{
    updateAllControlPoints();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::RBFInterpolation::~RBFInterpolation()
{
    clearOut();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::RBFInterpolation::movePoints()
{
    clearOut();
}


// ************************************************************************* //
