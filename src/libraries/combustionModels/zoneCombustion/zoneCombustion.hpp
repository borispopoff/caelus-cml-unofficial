/*---------------------------------------------------------------------------*\
Copyright (C) 2016-2019 OpenFOAM Foundation
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

Class
    CML::combustionModels::zoneCombustion

Description
    Zone-filtered combustion model.

    Enable the reactions within the specified list of cell-zones and set
    to zero elsewhere.


\*---------------------------------------------------------------------------*/

#ifndef zoneCombustion_HPP
#define zoneCombustion_HPP

#include "CombustionModel_.hpp"
#include "fvmSup.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{
namespace combustionModels
{

/*---------------------------------------------------------------------------*\
                            Class zoneCombustion Declaration
\*---------------------------------------------------------------------------*/

template<class ReactionThermo>
class zoneCombustion
:
    public CombustionModel<ReactionThermo>
{
    // Private Data

        //- The combustion model to be zone-filtered
        autoPtr<CombustionModel<ReactionThermo>> combustionModelPtr_;

        //- List of zone names in which the reactions are active
        wordList zoneNames_;


    // Private Member Functions

        //- Filter the reaction-rate matrix on the cellZones
        tmp<fvScalarMatrix> filter(const tmp<fvScalarMatrix>& tR) const;

        //- Filter the given field on the cellZones
        tmp<volScalarField> filter(const tmp<volScalarField>& tS) const;


public:

    //- Runtime type information
    TypeName("zoneCombustion");


    // Constructors

        //- Construct from components
        zoneCombustion
        (
            const word& modelType,
            ReactionThermo& thermo,
            const compressible::turbulenceModel& turb,
            const word& combustionProperties
        );

        //- Disallow default bitwise copy construction
        zoneCombustion(const zoneCombustion&);


    //- Destructor
    virtual ~zoneCombustion();


    // Member Functions

        //- Return access to the thermo package
        virtual ReactionThermo& thermo();

        //- Return const access to the thermo package
        virtual const ReactionThermo& thermo() const;

        //- Correct combustion rate
        virtual void correct();

        //- Fuel consumption rate matrix.
        virtual tmp<fvScalarMatrix> R(volScalarField& Y) const;

        //- Heat release rate [kg/m/s^3]
        virtual tmp<volScalarField> Qdot() const;

        //- Update properties from given dictionary
        virtual bool read();


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const zoneCombustion&) = delete;
};


} // End namespace combustionModels
} // End namespace CML


// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

template<class ReactionThermo>
CML::tmp<CML::fvScalarMatrix>
CML::combustionModels::zoneCombustion<ReactionThermo>::filter
(
    const tmp<fvScalarMatrix>& tR
) const
{
    fvScalarMatrix& R = tR.ref();
    scalarField& Su = R.source();
    scalarField filteredField(Su.size(), 0);

    forAll(zoneNames_, zonei)
    {
        const labelList& cells = this->mesh().cellZones()[zoneNames_[zonei]];

        forAll(cells, i)
        {
            filteredField[cells[i]] = Su[cells[i]];
        }
    }

    Su = filteredField;

    if (R.hasDiag())
    {
        scalarField& Sp = R.diag();

        forAll(zoneNames_, zonei)
        {
            const labelList& cells =
                this->mesh().cellZones()[zoneNames_[zonei]];

            forAll(cells, i)
            {
                filteredField[cells[i]] = Sp[cells[i]];
            }
        }

        Sp = filteredField;
    }

    return tR;
}


template<class ReactionThermo>
CML::tmp<CML::volScalarField>
CML::combustionModels::zoneCombustion<ReactionThermo>::filter
(
    const tmp<volScalarField>& tS
) const
{
    scalarField& S = tS.ref();
    scalarField filteredField(S.size(), 0);

    forAll(zoneNames_, zonei)
    {
        const labelList& cells = this->mesh().cellZones()[zoneNames_[zonei]];

        forAll(cells, i)
        {
            filteredField[cells[i]] = S[cells[i]];
        }
    }

    S = filteredField;

    return tS;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ReactionThermo>
CML::combustionModels::zoneCombustion<ReactionThermo>::zoneCombustion
(
    const word& modelType,
    ReactionThermo& thermo,
    const compressible::turbulenceModel& turb,
    const word& combustionProperties
)
:
    CombustionModel<ReactionThermo>
    (
        modelType,
        thermo,
        turb,
        combustionProperties
    ),
    combustionModelPtr_
    (
        CombustionModel<ReactionThermo>::New
        (
            thermo,
            turb,
            "zoneCombustionProperties"
        )
    ),
    zoneNames_(this->coeffs().lookup("zones"))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class ReactionThermo>
CML::combustionModels::zoneCombustion<ReactionThermo>::~zoneCombustion()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class ReactionThermo>
ReactionThermo& CML::combustionModels::zoneCombustion<ReactionThermo>::thermo()
{
    return combustionModelPtr_->thermo();
}


template<class ReactionThermo>
const ReactionThermo&
CML::combustionModels::zoneCombustion<ReactionThermo>::thermo() const
{
    return combustionModelPtr_->thermo();
}


template<class ReactionThermo>
void CML::combustionModels::zoneCombustion<ReactionThermo>::correct()
{
    combustionModelPtr_->correct();
}


template<class ReactionThermo>
CML::tmp<CML::fvScalarMatrix>
CML::combustionModels::zoneCombustion<ReactionThermo>::R
(
    volScalarField& Y
) const
{
    return filter(combustionModelPtr_->R(Y));
}


template<class ReactionThermo>
CML::tmp<CML::volScalarField>
CML::combustionModels::zoneCombustion<ReactionThermo>::Qdot() const
{
    return filter(combustionModelPtr_->Qdot());
}


template<class ReactionThermo>
bool CML::combustionModels::zoneCombustion<ReactionThermo>::read()
{
    if (CombustionModel<ReactionThermo>::read())
    {
        combustionModelPtr_->read();
        return true;
    }
    else
    {
        return false;
    }
}


#endif
