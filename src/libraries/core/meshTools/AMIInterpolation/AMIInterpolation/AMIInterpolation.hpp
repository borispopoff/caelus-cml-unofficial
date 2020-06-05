/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
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
    CML::AMIInterpolation

Description
    Interpolation class dealing with transfer of data between two
    primitive patches with an arbitrary mesh interface (AMI).

    Based on the algorithm given in:

        Conservative interpolation between volume meshes by local Galerkin
        projection, Farrell PE and Maddison JR, 2011, Comput. Methods Appl.
        Mech Engrg, Volume 200, Issues 1-4, pp 89-100

    Interpolation requires that the two patches should have opposite
    orientations (opposite normals).  The 'reverseTarget' flag can be used to
    reverse the orientation of the target patch.


SourceFiles
    AMIInterpolation.cpp
    AMIInterpolationName.cpp

\*---------------------------------------------------------------------------*/

#ifndef AMIInterpolation_H
#define AMIInterpolation_H

#include "className.hpp"
#include "searchableSurface.hpp"
#include "treeBoundBoxList.hpp"
#include "boolList.hpp"
#include "primitivePatch.hpp"
#include "faceAreaIntersect.hpp"
#include "globalIndex.hpp"
#include "ops.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                  Class AMIInterpolation Declaration
\*---------------------------------------------------------------------------*/

class AMIInterpolation
{
public:

    // Public data types

        //- Enumeration specifying interpolation method
        enum interpolationMethod
        {
            imDirect,
            imMapNearest,
            imFaceAreaWeight,
            imPartialFaceAreaWeight,
            imSweptFaceAreaWeight
        };

        //- Convert interpolationMethod to word representation
        static word interpolationMethodToWord
        (
            const interpolationMethod& method
        );

        //- Convert word to interpolationMethod
        static interpolationMethod wordTointerpolationMethod
        (
            const word& method
        );

        //- Calculate the patch face magnitudes for the given tri-mode
        static tmp<scalarField> patchMagSf
        (
            const primitivePatch& patch,
            const faceAreaIntersect::triangulationMode triMode
        );


private:

    // Private data

        //- Interpolation method
        const word methodName_;

        //- Flag to indicate that the two patches are co-directional and
        //  that the orientation of the target patch should be reversed
        const bool reverseTarget_;

        //- Flag to indicate that the two patches must be matched/an overlap
        //  exists between them
        const bool requireMatch_;

        //- Index of processor that holds all of both sides. -1 in all other
        //  cases
        label singlePatchProc_;

        //- Threshold weight below which interpolation is deactivated
        scalar lowWeightCorrection_;


        // Source patch

            //- Source face areas
            scalarField srcMagSf_;

            //- Addresses of target faces per source face
            labelListList srcAddress_;

            //- Weights of target faces per source face
            scalarListList srcWeights_;

            //- Sum of weights of target faces per source face
            scalarField srcWeightsSum_;


        // Target patch

            //- Target face areas
            scalarField tgtMagSf_;

            //- Addresses of source faces per target face
            labelListList tgtAddress_;

            //- Weights of source faces per target face
            scalarListList tgtWeights_;

            //- Sum of weights of source faces per target face
            scalarField tgtWeightsSum_;


        //- Face triangulation mode
        const faceAreaIntersect::triangulationMode triMode_;

        //- Source map pointer - parallel running only
        autoPtr<mapDistribute> srcMapPtr_;

        //- Target map pointer - parallel running only
        autoPtr<mapDistribute> tgtMapPtr_;


    // Private Member Functions

        // Parallel functionality

            //- Calculate if patches are on multiple processors
            label calcDistribution
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch
            ) const;

            label calcOverlappingProcs
            (
                const List<treeBoundBoxList>& procBb,
                const treeBoundBox& bb,
                boolList& overlaps
            ) const;

            void distributePatches
            (
                const mapDistribute& map,
                const primitivePatch& pp,
                const globalIndex& gi,
                List<faceList>& faces,
                List<pointField>& points,
                List<labelList>& tgtFaceIDs
            ) const;

            void distributeAndMergePatches
            (
                const mapDistribute& map,
                const primitivePatch& tgtPatch,
                const globalIndex& gi,
                faceList& tgtFaces,
                pointField& tgtPoints,
                labelList& tgtFaceIDs
            ) const;

            autoPtr<mapDistribute> calcProcMap
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch
            ) const;


        // Initialisation

            //- Project points to surface
            void projectPointsToSurface
            (
                const searchableSurface& surf,
                pointField& pts
            ) const;


        // Manipulation

            //- Sum the weights for each face
            static void sumWeights
            (
                const scalarListList& wght,
                scalarField& wghtSum
            );

            //- As above, but for multiple sets of weights
            static void sumWeights
            (
                const UPtrList<scalarListList>& wghts,
                scalarField& wghtSum
            );

            //- Print out information relating to the weights sum. Values close
            //  to one are ideal. This information acts as a measure of the
            //  quality of the AMI.
            static void reportSumWeights
            (
                const scalarField& patchAreas,
                const word& patchName,
                const scalarField& wghtSum,
                const scalar lowWeightTol
            );

            //- Normalise the weights so that they sum to one for each face.
            //  This may stabilise the solution at the expense of accuracy.
            static void normaliseWeights
            (
                scalarListList& wght,
                const scalarField& wghtSum
            );

            //- As above but for multiple sets of weights
            static void normaliseWeights
            (
                UPtrList<scalarListList>& wghts,
                const scalarField& wghtSum
            );


        // Constructor helpers

            static void agglomerate
            (
                const autoPtr<mapDistribute>& targetMap,
                const scalarField& fineSrcMagSf,
                const labelListList& fineSrcAddress,
                const scalarListList& fineSrcWeights,

                const labelList& sourceRestrictAddressing,
                const labelList& targetRestrictAddressing,

                scalarField& srcMagSf,
                labelListList& srcAddress,
                scalarListList& srcWeights,
                scalarField& srcWeightsSum,
                autoPtr<mapDistribute>& tgtMap
            );

            void constructFromSurface
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch,
                const autoPtr<searchableSurface>& surfPtr,
                const bool report
            );

public:

    //- Runtime type information
    TypeName("cyclicAMI");


    // Constructors

        //- Construct from components
        AMIInterpolation
        (
            const primitivePatch& srcPatch,
            const primitivePatch& tgtPatch,
            const faceAreaIntersect::triangulationMode& triMode,
            const bool requireMatch = true,
            const interpolationMethod& method = imFaceAreaWeight,
            const scalar lowWeightCorrection = -1,
            const bool reverseTarget = false,
            const bool report = true
        );

        //- Construct from components
        AMIInterpolation
        (
            const primitivePatch& srcPatch,
            const primitivePatch& tgtPatch,
            const faceAreaIntersect::triangulationMode& triMode,
            const bool requireMatch = true,
            const word& methodName =
                interpolationMethodToWord(imFaceAreaWeight),
            const scalar lowWeightCorrection = -1,
            const bool reverseTarget = false,
            const bool report = true
        );

        //- Construct from components, with projection surface
        AMIInterpolation
        (
            const primitivePatch& srcPatch,
            const primitivePatch& tgtPatch,
            const autoPtr<searchableSurface>& surf,
            const faceAreaIntersect::triangulationMode& triMode,
            const bool requireMatch = true,
            const interpolationMethod& method = imFaceAreaWeight,
            const scalar lowWeightCorrection = -1,
            const bool reverseTarget = false,
            const bool report = true
        );

        //- Construct from components, with projection surface
        AMIInterpolation
        (
            const primitivePatch& srcPatch,
            const primitivePatch& tgtPatch,
            const autoPtr<searchableSurface>& surf,
            const faceAreaIntersect::triangulationMode& triMode,
            const bool requireMatch = true,
            const word& methodName =
                interpolationMethodToWord(imFaceAreaWeight),
            const scalar lowWeightCorrection = -1,
            const bool reverseTarget = false,
            const bool report = true
        );

        //- Construct from agglomeration of AMIInterpolation.
        //  Agglomeration passed in as new coarse size and addressing from fine
        //  from coarse
        AMIInterpolation
        (
            const AMIInterpolation& fineAMI,
            const labelList& sourceRestrictAddressing,
            const labelList& neighbourRestrictAddressing,
            const bool report = false
        );

        //- Disallow default bitwise copy construct
        AMIInterpolation(const AMIInterpolation&) = delete;


    //- Destructor
    virtual ~AMIInterpolation();


    // Member Functions

        // Access

            //- Set to -1, or the processor holding all faces (both sides) of
            //  the AMI
            inline label singlePatchProc() const;

            //- Threshold weight below which interpolation is deactivated
            inline scalar lowWeightCorrection() const;

            //- Return true if employing a 'lowWeightCorrection'
            inline bool applyLowWeightCorrection() const;


            // Source patch

                //- Return const access to source patch face areas
                inline const scalarField& srcMagSf() const;

                //- Return const access to source patch addressing
                inline const labelListList& srcAddress() const;

                //- Return const access to source patch weights
                inline const scalarListList& srcWeights() const;

                //- Return access to source patch weights
                inline scalarListList& srcWeights();

                //- Return const access to normalisation factor of source
                //  patch weights (i.e. the sum before normalisation)
                inline const scalarField& srcWeightsSum() const;

                //- Return access to normalisation factor of source
                //  patch weights (i.e. the sum before normalisation)
                inline scalarField& srcWeightsSum();

                //- Source map pointer - valid only if singlePatchProc = -1
                //  This gets source data into a form to be consumed by
                //  tgtAddress, tgtWeights
                inline const mapDistribute& srcMap() const;


            // Target patch

                //- Return const access to target patch face areas
                inline const scalarField& tgtMagSf() const;

                //- Return const access to target patch addressing
                inline const labelListList& tgtAddress() const;

                //- Return const access to target patch weights
                inline const scalarListList& tgtWeights() const;

                //- Return access to target patch weights
                inline scalarListList& tgtWeights();

                //- Return const access to normalisation factor of target
                //  patch weights (i.e. the sum before normalisation)
                inline const scalarField& tgtWeightsSum() const;

                //- Return access to normalisation factor of target
                //  patch weights (i.e. the sum before normalisation)
                inline scalarField& tgtWeightsSum();

                //- Target map pointer -  valid only if singlePatchProc=-1.
                //  This gets target data into a form to be consumed by
                //  srcAddress, srcWeights
                inline const mapDistribute& tgtMap() const;


        // Manipulation

            //- Update addressing and weights
            void update
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch,
                const bool report
            );

            //- Sum the weights on both sides of an AMI
            static void sumWeights(AMIInterpolation& AMI);

            //- As above, but for multiple AMI-s
            static void sumWeights(PtrList<AMIInterpolation>& AMIs);

            //- Print out information relating to the weights sum. Values close
            //  to one are ideal. This information acts as a measure of the
            //  quality of the AMI.
            static void reportSumWeights(AMIInterpolation& AMI);

            //- Normalise the weights on both sides of an AMI
            static void normaliseWeights(AMIInterpolation& AMI);

            //- As above, but for multiple AMI-s
            static void normaliseWeights(UPtrList<AMIInterpolation>& AMIs);


        // Evaluation

            // Low-level

                //- Interpolate from target to source with supplied op
                //  to combine existing value with remote value and weight
                template<class Type, class CombineOp>
                void interpolateToSource
                (
                    const UList<Type>& fld,
                    const CombineOp& cop,
                    List<Type>& result,
                    const UList<Type>& defaultValues = UList<Type>::null()
                ) const;

                //- Interpolate from source to target with supplied op
                //  to combine existing value with remote value and weight
                template<class Type, class CombineOp>
                void interpolateToTarget
                (
                    const UList<Type>& fld,
                    const CombineOp& cop,
                    List<Type>& result,
                    const UList<Type>& defaultValues = UList<Type>::null()
                ) const;


            //- Interpolate from target to source with supplied op
            template<class Type, class CombineOp>
            tmp<Field<Type>> interpolateToSource
            (
                const Field<Type>& fld,
                const CombineOp& cop,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from target tmp field to source with supplied op
            template<class Type, class CombineOp>
            tmp<Field<Type>> interpolateToSource
            (
                const tmp<Field<Type>>& tFld,
                const CombineOp& cop,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from source to target with supplied op
            template<class Type, class CombineOp>
            tmp<Field<Type>> interpolateToTarget
            (
                const Field<Type>& fld,
                const CombineOp& cop,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from source tmp field to target with supplied op
            template<class Type, class CombineOp>
            tmp<Field<Type>> interpolateToTarget
            (
                const tmp<Field<Type>>& tFld,
                const CombineOp& cop,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from target to source
            template<class Type>
            tmp<Field<Type>> interpolateToSource
            (
                const Field<Type>& fld,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from target tmp field
            template<class Type>
            tmp<Field<Type>> interpolateToSource
            (
                const tmp<Field<Type>>& tFld,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from source to target
            template<class Type>
            tmp<Field<Type>> interpolateToTarget
            (
                const Field<Type>& fld,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;

            //- Interpolate from source tmp field
            template<class Type>
            tmp<Field<Type>> interpolateToTarget
            (
                const tmp<Field<Type>>& tFld,
                const UList<Type>& defaultValues = UList<Type>::null()
            ) const;


        // Point intersections

            //- Return source patch face index of point on target patch face
            label srcPointFace
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch,
                const vector& n,
                const label tgtFacei,
                point& tgtPoint
            )
            const;

            //- Return target patch face index of point on source patch face
            label tgtPointFace
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch,
                const vector& n,
                const label srcFacei,
                point& srcPoint
            )
            const;


        // Checks

            //- Write face connectivity as OBJ file
            void writeFaceConnectivity
            (
                const primitivePatch& srcPatch,
                const primitivePatch& tgtPatch,
                const labelListList& srcAddress
            ) const;


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const AMIInterpolation&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

inline CML::label CML::AMIInterpolation::singlePatchProc() const
{
    return singlePatchProc_;
}


inline CML::scalar CML::AMIInterpolation::lowWeightCorrection() const
{
    return lowWeightCorrection_;
}


inline bool CML::AMIInterpolation::applyLowWeightCorrection() const
{
    return lowWeightCorrection_ > 0;
}


inline const CML::scalarField& CML::AMIInterpolation::srcMagSf() const
{
    return srcMagSf_;
}


inline const CML::labelListList& CML::AMIInterpolation::srcAddress() const
{
    return srcAddress_;
}


inline const CML::scalarListList& CML::AMIInterpolation::srcWeights() const
{
    return srcWeights_;
}


inline CML::scalarListList& CML::AMIInterpolation::srcWeights()
{
    return srcWeights_;
}


inline const CML::scalarField& CML::AMIInterpolation::srcWeightsSum() const
{
    return srcWeightsSum_;
}


inline CML::scalarField& CML::AMIInterpolation::srcWeightsSum()
{
    return srcWeightsSum_;
}


inline const CML::mapDistribute& CML::AMIInterpolation::srcMap() const
{
    return srcMapPtr_();
}


inline const CML::scalarField& CML::AMIInterpolation::tgtMagSf() const
{
    return tgtMagSf_;
}


inline const CML::labelListList& CML::AMIInterpolation::tgtAddress() const
{
    return tgtAddress_;
}


inline const CML::scalarListList& CML::AMIInterpolation::tgtWeights() const
{
    return tgtWeights_;
}


inline CML::scalarListList& CML::AMIInterpolation::tgtWeights()
{
    return tgtWeights_;
}


inline const CML::scalarField& CML::AMIInterpolation::tgtWeightsSum() const
{
    return tgtWeightsSum_;
}


inline CML::scalarField& CML::AMIInterpolation::tgtWeightsSum()
{
    return tgtWeightsSum_;
}


inline const CML::mapDistribute& CML::AMIInterpolation::tgtMap() const
{
    return tgtMapPtr_();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "AMIMethod.hpp"
#include "meshTools.hpp"
#include "mapDistribute.hpp"
#include "flipOp.hpp"

template<class Type, class CombineOp>
void CML::AMIInterpolation::interpolateToTarget
(
    const UList<Type>& fld,
    const CombineOp& cop,
    List<Type>& result,
    const UList<Type>& defaultValues
) const
{
    if (fld.size() != srcAddress_.size())
    {
        FatalErrorInFunction
            << "Supplied field size is not equal to source patch size" << nl
            << "    source patch   = " << srcAddress_.size() << nl
            << "    target patch   = " << tgtAddress_.size() << nl
            << "    supplied field = " << fld.size()
            << abort(FatalError);
    }

    if (lowWeightCorrection_ > 0)
    {
        if (defaultValues.size() != tgtAddress_.size())
        {
            FatalErrorInFunction
                << "Employing default values when sum of weights falls below "
                << lowWeightCorrection_
                << " but supplied default field size is not equal to target "
                << "patch size" << nl
                << "    default values = " << defaultValues.size() << nl
                << "    target patch   = " << tgtAddress_.size() << nl
                << abort(FatalError);
        }
    }

    result.setSize(tgtAddress_.size());

    if (singlePatchProc_ == -1)
    {
        const mapDistribute& map = srcMapPtr_();

        List<Type> work(fld);
        map.distribute(work);

        forAll(result, facei)
        {
            if (tgtWeightsSum_[facei] < lowWeightCorrection_)
            {
                result[facei] = defaultValues[facei];
            }
            else
            {
                const labelList& faces = tgtAddress_[facei];
                const scalarList& weights = tgtWeights_[facei];

                forAll(faces, i)
                {
                    cop(result[facei], facei, work[faces[i]], weights[i]);
                }
            }
        }
    }
    else
    {
        forAll(result, facei)
        {
            if (tgtWeightsSum_[facei] < lowWeightCorrection_)
            {
                result[facei] = defaultValues[facei];
            }
            else
            {
                const labelList& faces = tgtAddress_[facei];
                const scalarList& weights = tgtWeights_[facei];

                forAll(faces, i)
                {
                    cop(result[facei], facei, fld[faces[i]], weights[i]);
                }
            }
        }
    }
}


template<class Type, class CombineOp>
void CML::AMIInterpolation::interpolateToSource
(
    const UList<Type>& fld,
    const CombineOp& cop,
    List<Type>& result,
    const UList<Type>& defaultValues
) const
{
    if (fld.size() != tgtAddress_.size())
    {
        FatalErrorInFunction
            << "Supplied field size is not equal to target patch size" << nl
            << "    source patch   = " << srcAddress_.size() << nl
            << "    target patch   = " << tgtAddress_.size() << nl
            << "    supplied field = " << fld.size()
            << abort(FatalError);
    }

    if (lowWeightCorrection_ > 0)
    {
        if (defaultValues.size() != srcAddress_.size())
        {
            FatalErrorInFunction
                << "Employing default values when sum of weights falls below "
                << lowWeightCorrection_
                << " but supplied default field size is not equal to target "
                << "patch size" << nl
                << "    default values = " << defaultValues.size() << nl
                << "    source patch   = " << srcAddress_.size() << nl
                << abort(FatalError);
        }
    }

    result.setSize(srcAddress_.size());

    if (singlePatchProc_ == -1)
    {
        const mapDistribute& map = tgtMapPtr_();

        List<Type> work(fld);
        map.distribute(work);

        forAll(result, facei)
        {
            if (srcWeightsSum_[facei] < lowWeightCorrection_)
            {
                result[facei] = defaultValues[facei];
            }
            else
            {
                const labelList& faces = srcAddress_[facei];
                const scalarList& weights = srcWeights_[facei];

                forAll(faces, i)
                {
                    cop(result[facei], facei, work[faces[i]], weights[i]);
                }
            }
        }
    }
    else
    {
        forAll(result, facei)
        {
            if (srcWeightsSum_[facei] < lowWeightCorrection_)
            {
                result[facei] = defaultValues[facei];
            }
            else
            {
                const labelList& faces = srcAddress_[facei];
                const scalarList& weights = srcWeights_[facei];

                forAll(faces, i)
                {
                    cop(result[facei], facei, fld[faces[i]], weights[i]);
                }
            }
        }
    }
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToSource
(
    const Field<Type>& fld,
    const CombineOp& cop,
    const UList<Type>& defaultValues
) const
{
    tmp<Field<Type>> tresult
    (
        new Field<Type>
        (
            srcAddress_.size(),
            Zero
        )
    );

    interpolateToSource
    (
        fld,
        multiplyWeightedOp<Type, CombineOp>(cop),
        tresult.ref(),
        defaultValues
    );

    return tresult;
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToSource
(
    const tmp<Field<Type>>& tFld,
    const CombineOp& cop,
    const UList<Type>& defaultValues
) const
{
    return interpolateToSource(tFld(), cop, defaultValues);
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToTarget
(
    const Field<Type>& fld,
    const CombineOp& cop,
    const UList<Type>& defaultValues
) const
{
    tmp<Field<Type>> tresult
    (
        new Field<Type>
        (
            tgtAddress_.size(),
            Zero
        )
    );

    interpolateToTarget
    (
        fld,
        multiplyWeightedOp<Type, CombineOp>(cop),
        tresult.ref(),
        defaultValues
    );

    return tresult;
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToTarget
(
    const tmp<Field<Type>>& tFld,
    const CombineOp& cop,
    const UList<Type>& defaultValues
) const
{
    return interpolateToTarget(tFld(), cop, defaultValues);
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToSource
(
    const Field<Type>& fld,
    const UList<Type>& defaultValues
) const
{
    return interpolateToSource(fld, plusEqOp<Type>(), defaultValues);
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToSource
(
    const tmp<Field<Type>>& tFld,
    const UList<Type>& defaultValues
) const
{
    return interpolateToSource(tFld(), plusEqOp<Type>(), defaultValues);
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToTarget
(
    const Field<Type>& fld,
    const UList<Type>& defaultValues
) const
{
    return interpolateToTarget(fld, plusEqOp<Type>(), defaultValues);
}


template<class Type>
CML::tmp<CML::Field<Type>>
CML::AMIInterpolation::interpolateToTarget
(
    const tmp<Field<Type>>& tFld,
    const UList<Type>& defaultValues
) const
{
    return interpolateToTarget(tFld(), plusEqOp<Type>(), defaultValues);
}


#endif

// ************************************************************************* //
