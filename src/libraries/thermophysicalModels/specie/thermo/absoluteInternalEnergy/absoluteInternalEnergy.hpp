/*---------------------------------------------------------------------------*\
Copyright (C) 2012-2018 OpenFOAM Foundation
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
    CML::absoluteInternalEnergy

Description
    Thermodynamics mapping class to expose the absolute internal energy
    functions.

\*---------------------------------------------------------------------------*/

#ifndef absoluteInternalEnergy_HPP
#define absoluteInternalEnergy_HPP

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                       Class absoluteInternalEnergy Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo>
class absoluteInternalEnergy
{

public:


    //- Construct
    absoluteInternalEnergy()
    {}


    // Member Functions

    //- Return the instantiated type name
    static word typeName()
    {
        return "absoluteInternalEnergy";
    }

    // Fundamental properties

    static word energyName()
    {
        return "ea";
    }

    // Heat capacity at constant volume [J/(kg K)]
    scalar Cpv
    (
        const Thermo& thermo,
        const scalar p,
        const scalar T
    ) const
    {
        return thermo.Cv(p, T);
    }

    //- Cp/Cv []
    scalar CpByCpv
    (
        const Thermo& thermo,
        const scalar p,
        const scalar T
    ) const
    {
        return thermo.gamma(p, T);
    }

    // Absolute internal energy [J/kg]
    scalar HE
    (
        const Thermo& thermo,
        const scalar p,
        const scalar T
    ) const
    {
        return thermo.Ea(p, T);
    }

    //- Temperature from absolute internal energy
    //  given an initial temperature T0
    scalar THE
    (
        const Thermo& thermo,
        const scalar e,
        const scalar p,
        const scalar T0
    ) const
    {
        return thermo.TEa(e, p, T0);
    }
};


} // End namespace CML

#endif
