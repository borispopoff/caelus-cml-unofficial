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

InClass
    CML::makeSolidReaction

Description
    Macros for instantiating reactions for solid

\*---------------------------------------------------------------------------*/

#ifndef makeSolidReaction_HPP
#define makeSolidReaction_HPP

#include "SolidReaction.hpp"
#include "IrreversibleReaction.hpp"
#include "Reaction.hpp"

#include "addToRunTimeSelectionTable.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeSolidReaction(ReactionType, Thermo, ReactionRate)                  \
                                                                               \
    typedef SolidReaction<Thermo> SolidReaction##Thermo;                       \
                                                                               \
    typedef Reaction<Thermo> Reaction##Thermo;                                 \
                                                                               \
    typedef ReactionType<SolidReaction, Thermo, ReactionRate>                  \
        ReactionType##Thermo##ReactionRate;                                    \
                                                                               \
    defineTemplateRunTimeSelectionTable(Reaction##Thermo, dictionary);         \
                                                                               \
    defineTemplateTypeNameAndDebug(SolidReaction##Thermo, 0);                  \
    defineTemplateTypeNameAndDebug(Reaction##Thermo, 0);                       \
                                                                               \
    template<>                                                                 \
    const word ReactionType##Thermo##ReactionRate::typeName                    \
    (                                                                          \
        ReactionType::typeName_()                                              \
      + ReactionRate::type()                                                   \
      + SolidReaction##Thermo::typeName_()                                     \
    );                                                                         \
                                                                               \
    addToRunTimeSelectionTable                                                 \
    (                                                                          \
        Reaction##Thermo,                                                      \
        ReactionType##Thermo##ReactionRate,                                    \
        dictionary                                                             \
    );

#define makeSolidIRReactions(Thermo, ReactionRate)                             \
                                                                               \
    makeSolidReaction(IrreversibleReaction, Thermo, ReactionRate)


} // End namespace CML


#endif
