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

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const CML::word& CML::fv::option::name() const
{
    return name_;
}


inline const CML::fvMesh& CML::fv::option::mesh() const
{
    return mesh_;
}


inline const CML::dictionary& CML::fv::option::coeffs() const
{
    return coeffs_;
}


inline bool CML::fv::option::active() const
{
    return active_;
}


inline void CML::fv::option::setApplied(const label fieldi)
{
    applied_[fieldi] = true;
}


inline CML::Switch& CML::fv::option::active()
{
    return active_;
}



