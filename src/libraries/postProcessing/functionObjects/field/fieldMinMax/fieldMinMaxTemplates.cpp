/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2015 OpenFOAM Foundation
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

#include "fieldMinMax.hpp"
#include "volFields.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void CML::fieldMinMax::output
(
    const word& fieldName,
    const word& outputName,
    const vector& minC,
    const vector& maxC,
    const label minProcI,
    const label maxProcI,
    const Type& minValue,
    const Type& maxValue
)
{
    OFstream& file = this->file();

    if (location_)
    {
        file<< obr_.time().value();

        writeTabbed(file, fieldName);

        file<< token::TAB << minValue
            << token::TAB << minC;

        if (Pstream::parRun())
        {
            file<< token::TAB << minProcI;
        }

        file<< token::TAB << maxValue
            << token::TAB << maxC;

        if (Pstream::parRun())
        {
            file<< token::TAB << maxProcI;
        }

        file<< endl;

        Info(log_)
            << "    min(" << outputName << ") = " << minValue
            << " at location " << minC;

        if (Pstream::parRun())
        {
            Info(log_)<< " on processor " << minProcI;
        }

        Info(log_)
            << nl << "    max(" << outputName << ") = " << maxValue
            << " at location " << maxC;

        if (Pstream::parRun())
        {
            Info(log_)<< " on processor " << maxProcI;
        }
    }
    else
    {
        file<< token::TAB << minValue << token::TAB << maxValue;

        Info(log_)
            << "    min/max(" << outputName << ") = "
            << minValue << ' ' << maxValue;
    }

    Info(log_)<< endl;
}


template<class Type>
void CML::fieldMinMax::calcMinMaxFields
(
    const word& fieldName,
    const modeType& mode
)
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    if (obr_.foundObject<fieldType>(fieldName))
    {
        const label proci = Pstream::myProcNo();

        const fieldType& field = obr_.lookupObject<fieldType>(fieldName);
        const fvMesh& mesh = field.mesh();

        const volVectorField::GeometricBoundaryField& CfBoundary =
            mesh.C().boundaryField();

        switch (mode)
        {
            case mdMag:
            {
                const volScalarField magField(mag(field));
                const volScalarField::GeometricBoundaryField& magFieldBoundary =
                    magField.boundaryField();

                scalarList minVs(Pstream::nProcs());
                List<vector> minCs(Pstream::nProcs());
                label minProcI = findMin(magField);
                minVs[proci] = magField[minProcI];
                minCs[proci] = field.mesh().C()[minProcI];


                labelList maxIs(Pstream::nProcs());
                scalarList maxVs(Pstream::nProcs());
                List<vector> maxCs(Pstream::nProcs());
                label maxProcI = findMax(magField);
                maxVs[proci] = magField[maxProcI];
                maxCs[proci] = field.mesh().C()[maxProcI];

                forAll(magFieldBoundary, patchi)
                {
                    const scalarField& mfp = magFieldBoundary[patchi];
                    if (mfp.size())
                    {
                        const vectorField& Cfp = CfBoundary[patchi];

                        label minPI = findMin(mfp);
                        if (mfp[minPI] < minVs[proci])
                        {
                            minVs[proci] = mfp[minPI];
                            minCs[proci] = Cfp[minPI];
                        }

                        label maxPI = findMax(mfp);
                        if (mfp[maxPI] > maxVs[proci])
                        {
                            maxVs[proci] = mfp[maxPI];
                            maxCs[proci] = Cfp[maxPI];
                        }
                    }
                }

                Pstream::gatherList(minVs);
                Pstream::gatherList(minCs);

                Pstream::gatherList(maxVs);
                Pstream::gatherList(maxCs);

                if (Pstream::master())
                {
                    label minI = findMin(minVs);
                    scalar minValue = minVs[minI];
                    const vector& minC = minCs[minI];

                    label maxI = findMax(maxVs);
                    scalar maxValue = maxVs[maxI];
                    const vector& maxC = maxCs[maxI];

                    output
                    (
                        fieldName,
                        word("mag(" + fieldName + ")"),
                        minC,
                        maxC,
                        minI,
                        maxI,
                        minValue,
                        maxValue
                    );
                }
                break;
            }
            case mdCmpt:
            {
                const typename fieldType::GeometricBoundaryField&
                    fieldBoundary = field.boundaryField();

                List<Type> minVs(Pstream::nProcs());
                List<vector> minCs(Pstream::nProcs());
                label minProcI = findMin(field);
                minVs[proci] = field[minProcI];
                minCs[proci] = field.mesh().C()[minProcI];

                Pstream::gatherList(minVs);
                Pstream::gatherList(minCs);

                List<Type> maxVs(Pstream::nProcs());
                List<vector> maxCs(Pstream::nProcs());
                label maxProcI = findMax(field);
                maxVs[proci] = field[maxProcI];
                maxCs[proci] = field.mesh().C()[maxProcI];

                forAll(fieldBoundary, patchi)
                {
                    const Field<Type>& fp = fieldBoundary[patchi];
                    if (fp.size())
                    {
                        const vectorField& Cfp = CfBoundary[patchi];

                        label minPI = findMin(fp);
                        if (fp[minPI] < minVs[proci])
                        {
                            minVs[proci] = fp[minPI];
                            minCs[proci] = Cfp[minPI];
                        }

                        label maxPI = findMax(fp);
                        if (fp[maxPI] > maxVs[proci])
                        {
                            maxVs[proci] = fp[maxPI];
                            maxCs[proci] = Cfp[maxPI];
                        }
                    }
                }

                Pstream::gatherList(minVs);
                Pstream::gatherList(minCs);

                Pstream::gatherList(maxVs);
                Pstream::gatherList(maxCs);

                if (Pstream::master())
                {
                    label minI = findMin(minVs);
                    Type minValue = minVs[minI];
                    const vector& minC = minCs[minI];

                    label maxI = findMax(maxVs);
                    Type maxValue = maxVs[maxI];
                    const vector& maxC = maxCs[maxI];

                    output
                    (
                        fieldName,
                        fieldName,
                        minC,
                        maxC,
                        minI,
                        maxI,
                        minValue,
                        maxValue
                    );
                }
                break;
            }
            default:
            {
                FatalErrorInFunction
                    << "Unknown min/max mode: " << modeTypeNames_[mode_]
                    << exit(FatalError);
            }
        }
    }
}


// ************************************************************************* //
