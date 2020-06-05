/*---------------------------------------------------------------------------*\
Copyright (C) 2014 - 2017 Applied CCM
Copyright (C) 2011 OpenFOAM Foundation
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
    CML::solutionControl

Description
    Base class for solution control classes

\*---------------------------------------------------------------------------*/

#ifndef solutionControl_H
#define solutionControl_H

#include "fvMesh.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                       Class solutionControl Declaration
\*---------------------------------------------------------------------------*/

class solutionControl
{
public:

    struct fieldData
    {
        wordRe name;
        scalar absTol;
        scalar relTol;
        scalar initialResidual;
    };


protected:

    // Protected data

    //- Reference to the mesh database
    fvMesh& mesh_;

    //- List of residual data per field
    List<fieldData> residualControl_;

    //- The dictionary name, e.g. SIMPLE, PIMPLE
    const word algorithmName_;


    // Solution controls

    //- Maximum number of non-orthogonal correctors
    label nNonOrthCorr_;

    //- Flag to indicate to solve for momentum
    bool momentumPredictor_;

    //- Flag to indicate to solve using transonic algorithm
    bool transonic_;

    //- Flag to indicate to relax pressure using the
    //  "consistent" approach of SIMPLEC
    bool consistent_;

    //- Flag to indicate that the correction form of pressure
    //  equation is used.
    bool correctionForm_;


    // Evolution

    //- Current corrector loop index
    label corr_;

    //- Current non-orthogonal corrector loop index
    label corrNonOrtho_;


    // Protected Member Functions

    //- Read controls from fvSolution dictionary
    virtual void read(const bool absTolOnly);

    //- Read controls from fvSolution dictionary
    virtual void read();

    //- Return index of field in residualControl_ if present
    virtual label applyToField
    (
        const word& fieldName,
        const bool useRegEx = true
    ) const;

    //- Return true if all convergence checks are satisfied
    virtual bool criteriaSatisfied() = 0;

    //- Store previous iteration fields
    virtual void storePrevIterFields() const;

    //- Store previous iteration field for vol<Type>Fields
    template<class Type>
    void storePrevIter() const;

    //- Disallow default bitwise copy construct
    solutionControl(const solutionControl&) = delete;

    //- Disallow default bitwise assignment
    void operator=(const solutionControl&) = delete;


public:


    // Static Data Members

    //- Run-time type information
    TypeName("solutionControl");


    // Constructors

    //- Construct from mesh
    solutionControl(fvMesh& mesh, const word& algorithmName);


    //- Destructor
    virtual ~solutionControl();


    // Member Functions

    // Access

    //- Return the solution dictionary
    inline const dictionary& dict() const;

    //- Current corrector loop index
    inline label corr() const;

    //- Current non-orthogonal corrector index
    inline label corrNonOrtho() const;


    // Solution control

    //- Maximum number of non-orthogonal correctors
    inline label nNonOrthCorr() const;

    //- Helper function to identify final non-orthogonal iteration
    inline bool finalNonOrthogonalIter() const;

    //- Flag to indicate to solve for momentum
    inline bool momentumPredictor() const;

    //- Flag to indicate to solve using transonic algorithm
    inline bool transonic() const;

    //- Flag to indicate to relax pressure using the
    //  "consistent" approach of SIMPLEC
    inline bool consistent() const;

    inline bool correctionForm() const;


    // Evolution

    //- Main control loop
    virtual bool loop() = 0;

    //- Non-orthogonal corrector loop
    inline bool correctNonOrthogonal();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "GeometricField.hpp"
#include "volMesh.hpp"
#include "fvPatchField.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void CML::solutionControl::storePrevIter() const
{
    typedef GeometricField<Type, fvPatchField, volMesh> GeoField;

    HashTable<GeoField*>
        flds(mesh_.objectRegistry::lookupClass<GeoField>());

    forAllIter(typename HashTable<GeoField*>, flds, iter)
    {
        GeoField& fld = *iter();

        const word& fName = fld.name();

        size_t prevIterField = fName.find("PrevIter");

        if ((prevIterField == word::npos) && mesh_.relaxField(fName))
        {
            if (debug)
            {
                Info<< algorithmName_ << ": storing previous iter for "
                    << fName << endl;
            }

            fld.storePrevIter();
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const CML::dictionary& CML::solutionControl::dict() const
{
    return mesh_.solutionDict().subDict(algorithmName_);
}


inline CML::label CML::solutionControl::corr() const
{
    return corr_;
}


inline CML::label CML::solutionControl::corrNonOrtho() const
{
    return corrNonOrtho_;
}


inline CML::label CML::solutionControl::nNonOrthCorr() const
{
    return nNonOrthCorr_;
}


inline bool CML::solutionControl::finalNonOrthogonalIter() const
{
    return corrNonOrtho_ == nNonOrthCorr_ + 1;
}


inline bool CML::solutionControl::momentumPredictor() const
{
    return momentumPredictor_;
}


inline bool CML::solutionControl::transonic() const
{
    return transonic_;
}


inline bool CML::solutionControl::consistent() const
{
    return consistent_;
}

inline bool CML::solutionControl::correctionForm() const
{
    return correctionForm_;
}

inline bool CML::solutionControl::correctNonOrthogonal()
{
    corrNonOrtho_++;

    if (debug)
    {
        Info<< algorithmName_ << " correctNonOrthogonal: corrNonOrtho = "
            << corrNonOrtho_ << endl;
    }

    if (corrNonOrtho_ <= nNonOrthCorr_ + 1)
    {
        return true;
    }
    else
    {
        corrNonOrtho_ = 0;
        return false;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
