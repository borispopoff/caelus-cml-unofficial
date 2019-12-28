/*---------------------------------------------------------------------------*\
Copyright (C) 2012-2019 OpenFOAM Foundation
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
    CML::meshToMesh

Description
    Class to calculate the cell-addressing between two overlapping meshes

    Mapping is performed using a run-time selectable interpolation mothod

SeeAlso
    meshToMeshMethod

SourceFiles
    meshToMesh.cpp
    meshToMeshParallelOps.cpp

\*---------------------------------------------------------------------------*/

#ifndef meshToMesh_H
#define meshToMesh_H

#include "polyMesh.hpp"
#include "boundBox.hpp"
#include "mapDistribute.hpp"
#include "volFieldsFwd.hpp"
#include "NamedEnum.hpp"
#include "AMIInterpolation.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                          Class meshToMesh Declaration
\*---------------------------------------------------------------------------*/

class meshToMesh
{
public:

    // Public data types

        //- Enumeration specifying interpolation method
        enum interpolationMethod
        {
            imDirect,
            imMapNearest,
            imCellVolumeWeight
        };

        static const NamedEnum<interpolationMethod, 3>
            interpolationMethodNames_;

private:

    // Private data

        //- Reference to the source mesh
        const polyMesh& srcRegion_;

        //- Reference to the target mesh
        const polyMesh& tgtRegion_;

        //- List of target patch IDs per source patch (local index)
        List<label> srcPatchID_;

        //- List of source patch IDs per target patch (local index)
        List<label> tgtPatchID_;

        //- List of AMIs between source and target patches
        PtrList<AMIInterpolation> patchAMIs_;

        //- Cutting patches whose values are set using a zero-gradient condition
        List<label> cuttingPatches_;

        //- Source to target cell addressing
        labelListList srcToTgtCellAddr_;

        //- Target to source cell addressing
        labelListList tgtToSrcCellAddr_;

        //- Source to target cell interplation weights
        scalarListList srcToTgtCellWght_;

        //- Target to source cell interpolation weights
        scalarListList tgtToSrcCellWght_;

        //- Cell total volume in overlap region [m3]
        scalar V_;

        //- Index of processor that holds all of both sides. -1 in all other
        //  cases
        label singleMeshProc_;

        //- Source map pointer - parallel running only
        autoPtr<mapDistribute> srcMapPtr_;

        //- Target map pointer - parallel running only
        autoPtr<mapDistribute> tgtMapPtr_;


    // Private Member Functions

        //- Helper function to add a constant offset to a list
        template<class Type>
        void add(UList<Type>& fld, const label offset) const;

        //- Return src cell IDs for the overlap region
        labelList maskCells(const polyMesh& src, const polyMesh& tgt) const;

        //- Normalise the interpolation weights
        void normaliseWeights
        (
            const word& descriptor,
            const labelListList& addr,
            scalarListList& wght
        ) const;

        //- Calculate the addressing between overlapping regions of src and tgt
        //  meshes
        void calcAddressing
        (
            const word& methodName,
            const polyMesh& src,
            const polyMesh& tgt
        );

        //- Calculate - main driver function
        void calculate(const word& methodName);

        //- Calculate patch overlap
        void calculatePatchAMIs(const word& amiMethodName);

        //- Constructor helper
        void constructNoCuttingPatches
        (
            const word& methodName,
            const word& AMIMethodName,
            const bool interpAllPatches
        );

        //- Constructor helper
        void constructFromCuttingPatches
        (
            const word& methodName,
            const word& AMIMethodName,
            const HashTable<word>& patchMap,
            const wordList& cuttingPatches
        );

        //- Return the list of AMIs between source and target patches
        inline const PtrList<AMIInterpolation>&
        patchAMIs() const;


        // Parallel operations

            //- Determine whether the meshes are split across multiple pocessors
            label calcDistribution
            (
                const polyMesh& src,
                const polyMesh& tgt
            ) const;

            //- Determine which processor bounding-boxes overlap
            label calcOverlappingProcs
            (
                const List<boundBox>& procBb,
                const boundBox& bb,
                boolList& overlaps
            ) const;

            //- Calculate the mapping between processors
            autoPtr<mapDistribute> calcProcMap
            (
                const polyMesh& src,
                const polyMesh& tgt
            ) const;

            //- Distribute mesh info from 'my' processor to others
            void distributeCells
            (
                const mapDistribute& map,
                const polyMesh& tgtMesh,
                const globalIndex& globalI,
                List<pointField>& points,
                List<label>& nInternalFaces,
                List<faceList>& faces,
                List<labelList>& faceOwner,
                List<labelList>& faceNeighbour,
                List<labelList>& cellIDs,
                List<labelList>& nbrProcIDs,
                List<labelList>& procLocalFaceIDs
            ) const;

            //- Collect pieces of tgt mesh from other procssors and restructure
            void distributeAndMergeCells
            (
                const mapDistribute& map,
                const polyMesh& tgt,
                const globalIndex& globalI,
                pointField& tgtPoints,
                faceList& tgtFaces,
                labelList& tgtFaceOwners,
                labelList& tgtFaceNeighbours,
                labelList& tgtCellIDs
            ) const;


        //- Disallow default bitwise copy construct
        meshToMesh(const meshToMesh&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const meshToMesh&) = delete;


public:

    //- Run-time type information
    TypeName("meshToMesh");


    //- Construct from source and target meshes
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const interpolationMethod& method,
        const bool interpAllPatches = true
    );

    //- Construct from source and target meshes, generic mapping methods
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const word& methodName,     // internal mapping
        const word& AMIMethodName,  // boundary mapping
        const bool interpAllPatches = true
    );

    //- Construct from source and target meshes
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const interpolationMethod& method,
        const HashTable<word>& patchMap,
        const wordList& cuttingPatches
    );


    //- Construct from source and target meshes, generic mapping methods
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const word& methodName,     // internal mapping
        const word& AMIMethodName,  // boundary mapping
        const HashTable<word>& patchMap,
        const wordList& cuttingPatches
    );


    //- Destructor
    virtual ~meshToMesh();


    // Member Functions

        // Access

            //- Return const access to the source mesh
            inline const polyMesh& srcRegion() const;

            //- Return const access to the target mesh
            inline const polyMesh& tgtRegion() const;

            //- Return const access to the source to target cell addressing
            inline const labelListList& srcToTgtCellAddr() const;

            //- Return const access to the target to source cell addressing
            inline const labelListList& tgtToSrcCellAddr() const;

            //- Return const access to the source to target cell weights
            inline const scalarListList& srcToTgtCellWght() const;

            //- Return const access to the target to source cell weights
            inline const scalarListList& tgtToSrcCellWght() const;

            //- Return const access to the overlap volume
            inline scalar V() const;

            //- Conversion between mesh and patch interpolation methods
            static AMIInterpolation::interpolationMethod
            interpolationMethodAMI(const interpolationMethod method);


        // Evaluation

            // Source-to-target field mapping

                //- Map field from src to tgt mesh with defined operation
                //  Values passed in via 'result' are used to initialise the
                //  return value
                template<class Type, class CombineOp>
                void mapSrcToTgt
                (
                    const UList<Type>& srcFld,
                    const CombineOp& cop,
                    List<Type>& result
                ) const;

                //- Return the src field mapped to the tgt mesh with a defined
                //  operation.  Initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const Field<Type>& srcFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a tmp field to the tgt mesh
                //  with a defined operation
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const tmp<Field<Type>>& tsrcFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a field to the tgt mesh with a
                //  default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const Field<Type>& srcFld
                ) const;

                //- Convenience function to map a tmp field to the tgt mesh
                //  with a default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const tmp<Field<Type>>& tsrcFld
                ) const;


            // Target-to-source field mapping

                //- Map field from tgt to src mesh with defined operation
                //  Values passed in via 'result' are used to initialise the
                //  return value
                template<class Type, class CombineOp>
                void mapTgtToSrc
                (
                    const UList<Type>& tgtFld,
                    const CombineOp& cop,
                    List<Type>& result
                ) const;

                //- Return the tgt field mapped to the src mesh with a defined
                //  operation.  Initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const Field<Type>& tgtFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a tmp field to the src mesh
                //  with a defined operation
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const tmp<Field<Type>>& ttgtFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a field to the src mesh with a
                //  default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const Field<Type>& tgtFld
                ) const;

                //- Convenience function to map a tmp field to the src mesh
                //  with a default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const tmp<Field<Type>>& ttgtFld
                ) const;


            // Source-to-target volume field mapping

                //- Interpolate a field with a defined operation.  Values
                //  passed in via 'result' are used to initialise the return
                //  value
                template<class Type, class CombineOp>
                void mapSrcToTgt
                (
                    const GeometricField<Type, fvPatchField, volMesh>& field,
                    const CombineOp& cop,
                    GeometricField<Type, fvPatchField, volMesh>& result
                ) const;

                //- Interpolate a field with a defined operation.  The initial
                //  values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapSrcToTgt
                (
                    const GeometricField<Type, fvPatchField, volMesh>& field,
                    const CombineOp& cop
                ) const;

                //- Interpolate a tmp field with a defined operation.  The
                //  initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapSrcToTgt
                (
                    const tmp<GeometricField<Type, fvPatchField, volMesh>>&
                        tfield,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapSrcToTgt
                (
                    const GeometricField<Type, fvPatchField, volMesh>& field
                ) const;

                //- Convenience function to map a tmp field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapSrcToTgt
                (
                    const tmp<GeometricField<Type, fvPatchField, volMesh>>&
                        tfield
                ) const;


            // Target-to-source volume field mapping

                //- Interpolate a field with a defined operation.  Values
                //  passed in via 'result' are used to initialise the return
                //  value
                template<class Type, class CombineOp>
                void mapTgtToSrc
                (
                    const GeometricField<Type, fvPatchField, volMesh>& field,
                    const CombineOp& cop,
                    GeometricField<Type, fvPatchField, volMesh>& result
                ) const;

                //- Interpolate a field with a defined operation.  The initial
                //  values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapTgtToSrc
                (
                    const GeometricField<Type, fvPatchField, volMesh>& field,
                    const CombineOp& cop
                ) const;

                //- Interpolate a tmp field with a defined operation.  The
                //  initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapTgtToSrc
                (
                    const tmp<GeometricField<Type, fvPatchField, volMesh>>&
                        tfield,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapTgtToSrc
                (
                    const GeometricField<Type, fvPatchField, volMesh>& field
                ) const;

                //- Convenience function to map a tmp field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<GeometricField<Type, fvPatchField, volMesh>> mapTgtToSrc
                (
                    const tmp<GeometricField<Type, fvPatchField, volMesh>>&
                        tfield
                ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "meshToMeshI.hpp"
#include "fvMesh.hpp"
#include "volFields.hpp"
#include "directFvPatchFieldMapper.hpp"
#include "calculatedFvPatchField.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace CML
{
    //- Helper class for list
    template<class Type>
    class ListPlusEqOp
    {
        public:
        void operator()(List<Type>& x, const List<Type> y) const
        {
            if (y.size())
            {
                if (x.size())
                {
                    label sz = x.size();
                    x.setSize(sz + y.size());
                    forAll(y, i)
                    {
                        x[sz++] = y[i];
                    }
                }
                else
                {
                    x = y;
                }
            }
        }
    };
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class Type>
void CML::meshToMesh::add
(
    UList<Type>& fld,
    const label offset
) const
{
    forAll(fld, i)
    {
        fld[i] += offset;
    }
}


template<class Type, class CombineOp>
void CML::meshToMesh::mapSrcToTgt
(
    const UList<Type>& srcField,
    const CombineOp& cop,
    List<Type>& result
) const
{
    if (result.size() != tgtToSrcCellAddr_.size())
    {
        FatalErrorInFunction
            << "Supplied field size is not equal to target mesh size" << nl
            << "    source mesh    = " << srcToTgtCellAddr_.size() << nl
            << "    target mesh    = " << tgtToSrcCellAddr_.size() << nl
            << "    supplied field = " << result.size()
            << abort(FatalError);
    }

    multiplyWeightedOp<Type, CombineOp> cbop(cop);

    if (singleMeshProc_ == -1)
    {
        const mapDistribute& map = srcMapPtr_();

        List<Type> work(srcField);
        map.distribute(work);

        forAll(result, celli)
        {
            const labelList& srcAddress = tgtToSrcCellAddr_[celli];
            const scalarList& srcWeight = tgtToSrcCellWght_[celli];

            if (srcAddress.size())
            {
                result[celli] *= (1.0 - sum(srcWeight));
                forAll(srcAddress, i)
                {
                    label srcI = srcAddress[i];
                    scalar w = srcWeight[i];
                    cbop(result[celli], celli, work[srcI], w);
                }
            }
        }
    }
    else
    {
        forAll(result, celli)
        {
            const labelList& srcAddress = tgtToSrcCellAddr_[celli];
            const scalarList& srcWeight = tgtToSrcCellWght_[celli];

            if (srcAddress.size())
            {
                result[celli] *= (1.0 - sum(srcWeight));
                forAll(srcAddress, i)
                {
                    label srcI = srcAddress[i];
                    scalar w = srcWeight[i];
                    cbop(result[celli], celli, srcField[srcI], w);
                }
            }
        }
    }
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapSrcToTgt
(
    const Field<Type>& srcField,
    const CombineOp& cop
) const
{
    tmp<Field<Type>> tresult
    (
        new Field<Type>
        (
            tgtToSrcCellAddr_.size(),
            Zero
        )
    );

    mapSrcToTgt(srcField, cop, tresult.ref());

    return tresult;
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapSrcToTgt
(
    const tmp<Field<Type>>& tsrcField,
    const CombineOp& cop
) const
{
    return mapSrcToTgt(tsrcField(), cop);
}


template<class Type>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapSrcToTgt
(
    const Field<Type>& srcField
) const
{
    return mapSrcToTgt(srcField, plusEqOp<Type>());
}


template<class Type>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapSrcToTgt
(
    const tmp<Field<Type>>& tsrcField
) const
{
    return mapSrcToTgt(tsrcField());
}


template<class Type, class CombineOp>
void CML::meshToMesh::mapTgtToSrc
(
    const UList<Type>& tgtField,
    const CombineOp& cop,
    List<Type>& result
) const
{
    if (result.size() != srcToTgtCellAddr_.size())
    {
        FatalErrorInFunction
            << "Supplied field size is not equal to source mesh size" << nl
            << "    source mesh    = " << srcToTgtCellAddr_.size() << nl
            << "    target mesh    = " << tgtToSrcCellAddr_.size() << nl
            << "    supplied field = " << result.size()
            << abort(FatalError);
    }

    multiplyWeightedOp<Type, CombineOp> cbop(cop);

    if (singleMeshProc_ == -1)
    {
        const mapDistribute& map = tgtMapPtr_();

        List<Type> work(tgtField);
        map.distribute(work);

        forAll(result, celli)
        {
            const labelList& tgtAddress = srcToTgtCellAddr_[celli];
            const scalarList& tgtWeight = srcToTgtCellWght_[celli];

            if (tgtAddress.size())
            {
                result[celli] *= (1.0 - sum(tgtWeight));
                forAll(tgtAddress, i)
                {
                    label tgtI = tgtAddress[i];
                    scalar w = tgtWeight[i];
                    cbop(result[celli], celli, work[tgtI], w);
                }
            }
        }
    }
    else
    {
        forAll(result, celli)
        {
            const labelList& tgtAddress = srcToTgtCellAddr_[celli];
            const scalarList& tgtWeight = srcToTgtCellWght_[celli];

            if (tgtAddress.size())
            {
                result[celli] *= (1.0 - sum(tgtWeight));
                forAll(tgtAddress, i)
                {
                    label tgtI = tgtAddress[i];
                    scalar w = tgtWeight[i];
                    cbop(result[celli], celli, tgtField[tgtI], w);
                }
            }
        }
    }
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapTgtToSrc
(
    const Field<Type>& tgtField,
    const CombineOp& cop
) const
{
    tmp<Field<Type>> tresult
    (
        new Field<Type>
        (
            srcToTgtCellAddr_.size(),
            Zero
        )
    );

    mapTgtToSrc(tgtField, cop, tresult.ref());

    return tresult;
}


template<class Type, class CombineOp>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapTgtToSrc
(
    const tmp<Field<Type>>& ttgtField,
    const CombineOp& cop
) const
{
    return mapTgtToSrc(ttgtField(), cop);
}


template<class Type>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapTgtToSrc
(
    const Field<Type>& tgtField
) const
{
    return mapTgtToSrc(tgtField, plusEqOp<Type>());
}


template<class Type>
CML::tmp<CML::Field<Type>> CML::meshToMesh::mapTgtToSrc
(
    const tmp<Field<Type>>& ttgtField
) const
{
    return mapTgtToSrc(ttgtField(), plusEqOp<Type>());
}


template<class Type, class CombineOp>
void CML::meshToMesh::mapSrcToTgt
(
    const GeometricField<Type, fvPatchField, volMesh>& field,
    const CombineOp& cop,
    GeometricField<Type, fvPatchField, volMesh>& result
) const
{
    mapSrcToTgt(field, cop, result.internalFieldRef());

    const PtrList<AMIInterpolation>& AMIList = patchAMIs();

    forAll(AMIList, i)
    {
        label srcPatchi = srcPatchID_[i];
        label tgtPatchi = tgtPatchID_[i];

        const Field<Type>& srcField = field.boundaryField()[srcPatchi];
        Field<Type>& tgtField = result.boundaryFieldRef()[tgtPatchi];

        tgtField = Type(Zero);

        AMIList[i].interpolateToTarget
        (
            srcField,
            multiplyWeightedOp<Type, CombineOp>(cop),
            tgtField,
            UList<Type>::null()
        );
    }

    forAll(cuttingPatches_, i)
    {
        label patchi = cuttingPatches_[i];
        fvPatchField<Type>& pf = result.boundaryFieldRef()[patchi];
        pf == pf.patchInternalField();
    }
}


template<class Type, class CombineOp>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapSrcToTgt
(
    const GeometricField<Type, fvPatchField, volMesh>& field,
    const CombineOp& cop
) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    const fvMesh& tgtMesh = static_cast<const fvMesh&>(tgtRegion_);

    const fvBoundaryMesh& tgtBm = tgtMesh.boundary();
    const typename fieldType::Boundary& srcBfld =
        field.boundaryField();

    PtrList<fvPatchField<Type>> tgtPatchFields(tgtBm.size());

    // construct tgt boundary patch types as copy of 'field' boundary types
    // note: this will provide place holders for fields with additional
    // entries, but these values will need to be reset
    forAll(tgtPatchID_, i)
    {
        label srcPatchi = srcPatchID_[i];
        label tgtPatchi = tgtPatchID_[i];

        if (!tgtPatchFields.set(tgtPatchi))
        {
            tgtPatchFields.set
            (
                tgtPatchi,
                fvPatchField<Type>::New
                (
                    srcBfld[srcPatchi],
                    tgtMesh.boundary()[tgtPatchi],
                    DimensionedField<Type, volMesh>::null(),
                    directFvPatchFieldMapper
                    (
                        labelList(tgtMesh.boundary()[tgtPatchi].size(), -1)
                    )
                )
            );
        }
    }

    // Any unset tgtPatchFields become calculated
    forAll(tgtPatchFields, tgtPatchi)
    {
        if (!tgtPatchFields.set(tgtPatchi))
        {
            // Note: use factory New method instead of direct generation of
            //       calculated so we keep constraints
            tgtPatchFields.set
            (
                tgtPatchi,
                fvPatchField<Type>::New
                (
                    calculatedFvPatchField<Type>::typeName,
                    tgtMesh.boundary()[tgtPatchi],
                    DimensionedField<Type, volMesh>::null()
                )
            );
        }
    }

    tmp<fieldType> tresult
    (
        new fieldType
        (
            IOobject
            (
                type() + ":interpolate(" + field.name() + ")",
                tgtMesh.time().timeName(),
                tgtMesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            tgtMesh,
            field.dimensions(),
            Field<Type>(tgtMesh.nCells(), Zero),
            tgtPatchFields
        )
    );

    mapSrcToTgt(field, cop, tresult.ref());

    return tresult;
}


template<class Type, class CombineOp>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapSrcToTgt
(
    const tmp<GeometricField<Type, fvPatchField, volMesh>>& tfield,
    const CombineOp& cop
) const
{
    return mapSrcToTgt(tfield(), cop);
}


template<class Type>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapSrcToTgt
(
    const GeometricField<Type, fvPatchField, volMesh>& field
) const
{
    return mapSrcToTgt(field, plusEqOp<Type>());
}


template<class Type>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapSrcToTgt
(
    const tmp<GeometricField<Type, fvPatchField, volMesh>>& tfield
) const
{
    return mapSrcToTgt(tfield(), plusEqOp<Type>());
}


template<class Type, class CombineOp>
void CML::meshToMesh::mapTgtToSrc
(
    const GeometricField<Type, fvPatchField, volMesh>& field,
    const CombineOp& cop,
    GeometricField<Type, fvPatchField, volMesh>& result
) const
{
    mapTgtToSrc(field, cop, result.internalFieldRef());

    const PtrList<AMIInterpolation>& AMIList = patchAMIs();

    forAll(AMIList, i)
    {
        label srcPatchi = srcPatchID_[i];
        label tgtPatchi = tgtPatchID_[i];

        Field<Type>& srcField = result.boundaryField()[srcPatchi];
        const Field<Type>& tgtField = field.boundaryField()[tgtPatchi];

        srcField = Type(Zero);

        AMIList[i].interpolateToSource
        (
            tgtField,
            multiplyWeightedOp<Type, CombineOp>(cop),
            srcField
        );
    }

    forAll(cuttingPatches_, i)
    {
        label patchi = cuttingPatches_[i];
        fvPatchField<Type>& pf = result.boundaryFieldRef()[patchi];
        pf == pf.patchInternalField();
    }
}


template<class Type, class CombineOp>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapTgtToSrc
(
    const GeometricField<Type, fvPatchField, volMesh>& field,
    const CombineOp& cop
) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    const fvMesh& srcMesh = static_cast<const fvMesh&>(srcRegion_);

    const fvBoundaryMesh& srcBm = srcMesh.boundary();
    const typename fieldType::Boundary& tgtBfld =
        field.boundaryField();

    PtrList<fvPatchField<Type>> srcPatchFields(srcBm.size());

    // constuct src boundary patch types as copy of 'field' boundary types
    // note: this will provide place holders for fields with additional
    // entries, but these values will need to be reset
    forAll(srcPatchID_, i)
    {
        label srcPatchi = srcPatchID_[i];
        label tgtPatchi = tgtPatchID_[i];

        if (!srcPatchFields.set(tgtPatchi))
        {
            srcPatchFields.set
            (
                srcPatchi,
                fvPatchField<Type>::New
                (
                    tgtBfld[srcPatchi],
                    srcMesh.boundary()[tgtPatchi],
                    DimensionedField<Type, volMesh>::null(),
                    directFvPatchFieldMapper
                    (
                        labelList(srcMesh.boundary()[srcPatchi].size(), -1)
                    )
                )
            );
        }
    }

    // Any unset srcPatchFields become calculated
    forAll(srcPatchFields, srcPatchi)
    {
        if (!srcPatchFields.set(srcPatchi))
        {
            // Note: use factory New method instead of direct generation of
            //       calculated so we keep constraints
            srcPatchFields.set
            (
                srcPatchi,
                fvPatchField<Type>::New
                (
                    calculatedFvPatchField<Type>::typeName,
                    srcMesh.boundary()[srcPatchi],
                    DimensionedField<Type, volMesh>::null()
                )
            );
        }
    }

    tmp<fieldType> tresult
    (
        new fieldType
        (
            IOobject
            (
                type() + ":interpolate(" + field.name() + ")",
                srcMesh.time().timeName(),
                srcMesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            srcMesh,
            field.dimensions(),
            Field<Type>(srcMesh.nCells(), Zero),
            srcPatchFields
        )
    );

    mapTgtToSrc(field, cop, tresult.ref());

    return tresult;
}


template<class Type, class CombineOp>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapTgtToSrc
(
    const tmp<GeometricField<Type, fvPatchField, volMesh>>& tfield,
    const CombineOp& cop
) const
{
    return mapTgtToSrc(tfield(), cop);
}


template<class Type>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapTgtToSrc
(
    const GeometricField<Type, fvPatchField, volMesh>& field
) const
{
    return mapTgtToSrc(field, plusEqOp<Type>());
}


template<class Type>
CML::tmp<CML::GeometricField<Type, CML::fvPatchField, CML::volMesh>>
CML::meshToMesh::mapTgtToSrc
(
    const tmp<GeometricField<Type, fvPatchField, volMesh>>& tfield
) const
{
    return mapTgtToSrc(tfield(), plusEqOp<Type>());
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
