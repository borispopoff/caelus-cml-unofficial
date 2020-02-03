/*---------------------------------------------------------------------------*\
Copyright (C) 2013 OpenFOAM Foundation
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
    CML::directFieldMapper

Description
    FieldMapper with direct mapping.

\*---------------------------------------------------------------------------*/

#ifndef directFieldMapper_HPP
#define directFieldMapper_HPP

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                           Class directFieldMapper Declaration
\*---------------------------------------------------------------------------*/

class directFieldMapper
:
    public FieldMapper
{
    const labelUList& directAddressing_;

    bool hasUnmapped_;

public:

    // Constructors

        //- Construct given addressing
        directFieldMapper(const labelUList& directAddressing)
        :
            directAddressing_(directAddressing),
            hasUnmapped_(false)
        {
            if (directAddressing_.size() && min(directAddressing_) < 0)
            {
                hasUnmapped_ = true;
            }
        }

    //- Destructor
    virtual ~directFieldMapper()
    {}


    // Member Functions

        label size() const
        {
            return directAddressing_.size();
        }

        bool direct() const
        {
            return true;
        }

        bool hasUnmapped() const
        {
            return hasUnmapped_;
        }

        const labelUList& directAddressing() const
        {
            return directAddressing_;
        }
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
