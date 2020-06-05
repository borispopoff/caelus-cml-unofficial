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
    CML::LangmuirHinshelwoodReactionRate

Description
    Langmuir-Hinshelwood reaction rate for gaseous reactions on surfaces.

    Reference:
    \verbatim
        Hinshelwood, C.N. (1940).
        The Kinetics of Chemical Change.
        Oxford Clarendon Press
    \endverbatim


\*---------------------------------------------------------------------------*/

#ifndef LangmuirHinshelwoodReactionRate_HPP
#define LangmuirHinshelwoodReactionRate_HPP

#include "scalarField.hpp"
#include "typeInfo.hpp"
#include "FixedList.hpp"
#include "Tuple2.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of friend functions and operators

class LangmuirHinshelwoodReactionRate;

Ostream& operator<<(Ostream&, const LangmuirHinshelwoodReactionRate&);


/*---------------------------------------------------------------------------*\
               Class LangmuirHinshelwoodReactionRate Declaration
\*---------------------------------------------------------------------------*/

class LangmuirHinshelwoodReactionRate
{
    // Private data

        //- List of species present in reaction system
        const speciesTable& species_;

        //- Reactant names
        FixedList<word, 2> reactantNames_;

        //- Reactant indices
        FixedList<label, 3> r_;

        scalar a_;
        FixedList<scalar, 3> A_;
        FixedList<scalar, 3> Ta_;
        FixedList<scalar, 3> beta_;
        FixedList<scalar, 3> m_;


public:

    // Constructors

        //- Construct from dictionary
        inline LangmuirHinshelwoodReactionRate
        (
            const speciesTable& st,
            const dictionary& dict
        )
        :
        species_(st),
        reactantNames_(dict.lookup("reactants")),
        a_(dict.lookupOrDefault<scalar>("a", 1)),
        A_(dict.lookup("A")),
        Ta_(dict.lookup("Ta")),
        beta_
        (
            dict.lookupOrDefault<FixedList<scalar, 3>>
            (
                "beta",
                FixedList<scalar, 3>({0, 0, 0})
            )
        ),
        m_
        (
            dict.lookupOrDefault<FixedList<scalar, 3>>
            (
                "m",
                FixedList<scalar, 3>({1, 1, 2})
            )
        )
    {
        forAll(reactantNames_, i)
        {
            r_[i] = st[reactantNames_[i]];
        }
    }


    // Member Functions

        //- Return the type name
        static word type()
        {
            return "LangmuirHinshelwood";
        }

        inline scalar operator()
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const
        {
            const scalar c0m = pow(c[r_[0]], m_[0]);
            const scalar c1m = pow(c[r_[1]], m_[1]);

            const scalar TaByT0 = Ta_[0]/T;
            const scalar TaByT1 = Ta_[1]/T;
            const scalar TaByT2 = Ta_[2]/T;

            const scalar k0 = A_[0]*pow(T, beta_[0])*exp(-TaByT0);
            const scalar k1 = A_[1]*pow(T, beta_[1])*exp(-TaByT1);
            const scalar k2 = A_[2]*pow(T, beta_[2])*exp(-TaByT2);

            return k0/pow(a_ + k1*c0m + k2*c1m, m_[2]);
        }

        inline scalar ddT
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const
        {
            const scalar c0m = pow(c[r_[0]], m_[0]);
            const scalar c1m = pow(c[r_[1]], m_[1]);

            const scalar TaByT0 = Ta_[0]/T;
            const scalar TaByT1 = Ta_[1]/T;
            const scalar TaByT2 = Ta_[2]/T;

            const scalar k0 = A_[0]*pow(T, beta_[0])*exp(-TaByT0);
            const scalar k1 = A_[1]*pow(T, beta_[1])*exp(-TaByT1);
            const scalar k2 = A_[2]*pow(T, beta_[2])*exp(-TaByT2);

            return
            (
                (beta_[0] + TaByT0)*k0
               - m_[2]*k0*((beta_[1] + TaByT1)*k1*c0m + (beta_[2] + TaByT2)*k2*c1m)
                /(a_ + k1*c0m + k2*c1m)
            )/(pow(a_ + k1*c0m + k2*c1m, m_[2])*T);
        }

        //- Third-body efficiencies (beta = 1-alpha)
        //  non-empty only for third-body reactions
        //  with enhanced molecularity (alpha != 1)
        inline const List<Tuple2<label, scalar>>& beta() const
        {
            return NullSingletonRef<List<Tuple2<label, scalar>>>();
        }

        //- Species concentration derivative of the pressure dependent term
        inline void dcidc
        (
            const scalar p,
            const scalar T,
            const scalarField& c,
            scalarField& dcidc
        ) const
        {}

        //- Temperature derivative of the pressure dependent term
        inline scalar dcidT
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const
        {
            return 0;
        }

        //- Write to stream
        inline void write(Ostream& os) const
        {
            writeEntry(os, "reactants", reactantNames_);
            writeEntry(os, "a", a_);
            writeEntry(os, "A", A_);
            writeEntry(os, "Ta", Ta_);
            writeEntry(os, "beta", beta_);
            writeEntry(os, "m", m_);
        }


    // Ostream Operator

        inline friend Ostream& operator<<
        (
            Ostream& os,
            const LangmuirHinshelwoodReactionRate& lhrr
        )
        {
            lhrr.write(os);
            return os;
        }

};


} // End namespace CML


#endif
