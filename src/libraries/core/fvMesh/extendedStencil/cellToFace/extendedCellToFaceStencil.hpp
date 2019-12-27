/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2018 OpenFOAM Foundation
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
    CML::extendedCellToFaceStencil

Description
    Calculates/constains the extended cell-to-face stencil.

    The stencil is a list of indices into either cells or boundary faces
    in a compact way. (element 0 is owner, 1 is neighbour). The index numbering
    is
    - cells first
    - then all (non-empty patch) boundary faces

    When used in evaluation is a two stage process:
    - collect the data (cell data and non-empty boundaries) into a
    single field
    - (parallel) distribute the field
    - sum the weights*field.

SourceFiles
    extendedCellToFaceStencil.cpp

\*---------------------------------------------------------------------------*/

#ifndef extendedCellToFaceStencil_H
#define extendedCellToFaceStencil_H

#include "mapDistribute.hpp"
#include "volFields.hpp"
#include "surfaceFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

class globalIndex;

/*---------------------------------------------------------------------------*\
                  Class extendedCellToFaceStencil Declaration
\*---------------------------------------------------------------------------*/

class extendedCellToFaceStencil
{
protected:

    // Protected data

        const polyMesh& mesh_;


protected:

        //- Write some statistics about stencil
        static void writeStencilStats
        (
            Ostream& os,
            const labelListList& stencil,
            const mapDistribute& map
        );

public:

    // Declare name of the class and its debug switch
    ClassName("extendedCellToFaceStencil");


    // Constructors

        //- Construct from mesh
        explicit extendedCellToFaceStencil(const polyMesh&);


    // Member Functions

        //- Use map to get the data into stencil order
        template<class T>
        static void collectData
        (
            const mapDistribute& map,
            const labelListList& stencil,
            const GeometricField<T, fvPatchField, volMesh>& fld,
            List<List<T>>& stencilFld
        );

        //- Sum vol field contributions to create face values
        template<class Type>
        static tmp<GeometricField<Type, fvsPatchField, surfaceMesh>>
        weightedSum
        (
            const mapDistribute& map,
            const labelListList& stencil,
            const GeometricField<Type, fvPatchField, volMesh>& fld,
            const List<List<scalar>>& stencilWeights
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void CML::extendedCellToFaceStencil::collectData
(
    const mapDistribute& map,
    const labelListList& stencil,
    const GeometricField<Type, fvPatchField, volMesh>& fld,
    List<List<Type>>& stencilFld
)
{
    // 1. Construct cell data in compact addressing
    List<Type> compactFld(map.constructSize(), Zero);

    // Insert my internal values
    forAll(fld, celli)
    {
        compactFld[celli] = fld[celli];
    }
    // Insert my boundary values
    label nCompact = fld.size();
    forAll(fld.boundaryField(), patchi)
    {
        const fvPatchField<Type>& pfld = fld.boundaryField()[patchi];

        forAll(pfld, i)
        {
            compactFld[nCompact++] = pfld[i];
        }
    }

    // Do all swapping
    map.distribute(compactFld);

    // 2. Pull to stencil
    stencilFld.setSize(stencil.size());

    forAll(stencil, facei)
    {
        const labelList& compactCells = stencil[facei];

        stencilFld[facei].setSize(compactCells.size());

        forAll(compactCells, i)
        {
            stencilFld[facei][i] = compactFld[compactCells[i]];
        }
    }
}


template<class Type>
CML::tmp<CML::GeometricField<Type, CML::fvsPatchField, CML::surfaceMesh>>
CML::extendedCellToFaceStencil::weightedSum
(
    const mapDistribute& map,
    const labelListList& stencil,
    const GeometricField<Type, fvPatchField, volMesh>& fld,
    const List<List<scalar>>& stencilWeights
)
{
    const fvMesh& mesh = fld.mesh();

    // Collect internal and boundary values
    List<List<Type>> stencilFld;
    collectData(map, stencil, fld, stencilFld);

    tmp<GeometricField<Type, fvsPatchField, surfaceMesh>> tsfCorr
    (
        new GeometricField<Type, fvsPatchField, surfaceMesh>
        (
            IOobject
            (
                fld.name(),
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            mesh,
            dimensioned<Type>
            (
                fld.name(),
                fld.dimensions(),
                Zero
            )
        )
    );
    GeometricField<Type, fvsPatchField, surfaceMesh>& sf = tsfCorr();

    // Internal faces
    for (label facei = 0; facei < mesh.nInternalFaces(); facei++)
    {
        const List<Type>& stField = stencilFld[facei];
        const List<scalar>& stWeight = stencilWeights[facei];

        forAll(stField, i)
        {
            sf[facei] += stField[i]*stWeight[i];
        }
    }

    // Boundaries. Either constrained or calculated so assign value
    // directly (instead of nicely using operator==)
    typename GeometricField<Type, fvsPatchField, surfaceMesh>::
        GeometricBoundaryField& bSfCorr = sf.boundaryFieldRef();

    forAll(bSfCorr, patchi)
    {
        fvsPatchField<Type>& pSfCorr = bSfCorr[patchi];

        if (pSfCorr.coupled())
        {
            label facei = pSfCorr.patch().start();

            forAll(pSfCorr, i)
            {
                const List<Type>& stField = stencilFld[facei];
                const List<scalar>& stWeight = stencilWeights[facei];

                forAll(stField, j)
                {
                    pSfCorr[i] += stField[j]*stWeight[j];
                }

                facei++;
            }
        }
    }

    return tsfCorr;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
