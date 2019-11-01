/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2019 OpenFOAM Foundation
Copyright (C) 2016 OpenCFD Ltd
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
    CML::fv::optionList

Description
    List of finite volume options

SourceFile
    optionList.cpp

\*---------------------------------------------------------------------------*/

#ifndef fvOptionList_HPP
#define fvOptionList_HPP

#include "fvOption.hpp"
#include "PtrList.hpp"
#include "GeometricField.hpp"
#include "geometricOneField.hpp"
#include "fvPatchField.hpp"
#include "profiling.hpp"

namespace CML
{

// Forward declaration of friend functions and operators

namespace fv
{
    class optionList;
}

Ostream& operator<<(Ostream& os, const fv::optionList& options);

namespace fv
{

/*---------------------------------------------------------------------------*\
                         Class optionList Declaration
\*---------------------------------------------------------------------------*/

class optionList
:
    public PtrList<option>
{
protected:

    // Protected data

        //- Reference to the mesh database
        const fvMesh& mesh_;

        //- Time index to check that all defined sources have been applied
        label checkTimeIndex_;


    // Protected Member Functions

        //- Return the "options" sub-dictionary if present otherwise return dict
        const dictionary& optionsDict(const dictionary& dict) const;

        //- Read options dictionary
        bool readOptions(const dictionary& dict);

        //- Check that all sources have been applied
        void checkApplied() const;

        //- Return source for equation with specified name and dimensions
        template<class Type>
        tmp<fvMatrix<Type>> source
        (
            GeometricField<Type, fvPatchField, volMesh>& field,
            const word& fieldName,
            const dimensionSet& ds
        );


public:

    //- Runtime type information
    TypeName("optionList");


    // Constructors

        //- Construct null
        optionList(const fvMesh& mesh);

        //- Construct from mesh and dictionary
        optionList(const fvMesh& mesh, const dictionary& dict);

        //- Disallow default bitwise copy construct
        optionList(const optionList&) = delete;


    //- Destructor
    virtual ~optionList()
    {}


    // Member Functions

        //- Reset the source list
        void reset(const dictionary& dict);

        //- Return whether there is something to apply to the field
        bool appliesToField(const word& fieldName) const;


        // Sources

            //- Return source for equation
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation with specified name
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                GeometricField<Type, fvPatchField, volMesh>& field,
                const word& fieldName
            );

            //- Return source for equation
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const volScalarField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation with specified name
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const volScalarField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field,
                const word& fieldName
            );

            //- Return source for equation
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const volScalarField& alpha,
                const volScalarField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation with specified name
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const volScalarField& alpha,
                const volScalarField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field,
                const word& fieldName
            );

            //- Return source for equation
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const volScalarField& alpha,
                const geometricOneField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const geometricOneField& alpha,
                const volScalarField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation
            template<class Type>
            tmp<fvMatrix<Type>> operator()
            (
                const geometricOneField& alpha,
                const geometricOneField& rho,
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation with second time derivative
            template<class Type>
            tmp<fvMatrix<Type>> d2dt2
            (
                GeometricField<Type, fvPatchField, volMesh>& field
            );

            //- Return source for equation with second time derivative
            template<class Type>
            tmp<fvMatrix<Type>> d2dt2
            (
                GeometricField<Type, fvPatchField, volMesh>& field,
                const word& fieldName
            );


        // Constraints

            //- Apply constraints to equation
            template<class Type>
            void constrain(fvMatrix<Type>& eqn);


        // Correction

            //- Apply correction to field
            template<class Type>
            void correct(GeometricField<Type, fvPatchField, volMesh>& field);


        // IO

            //- Read dictionary
            virtual bool read(const dictionary& dict);

            //- Write data to Ostream
            virtual bool writeData(Ostream& os) const;

            //- Ostream operator
            friend Ostream& operator<<
            (
                Ostream& os,
                const optionList& options
            );


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const optionList&) = delete;
};

} // End namespace fv
} // End namespace CML



template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::source
(
    GeometricField<Type, fvPatchField, volMesh>& field,
    const word& fieldName,
    const dimensionSet& ds
)
{
    checkApplied();

    tmp<fvMatrix<Type>> tmtx(new fvMatrix<Type>(field, ds));
    fvMatrix<Type>& mtx = tmtx();

    forAll(*this, i)
    {
        option& source = this->operator[](i);

        label fieldi = source.applyToField(fieldName);

        if (fieldi != -1)
        {
            addProfiling(fvopt, "fvOption()." + source.name());

            source.setApplied(fieldi);

            if (source.isActive())
            {
                if (debug)
                {
                    Info<< "Applying source " << source.name() << " to field "
                        << fieldName << endl;
                }

                source.addSup(mtx, fieldi);
            }
        }
    }

    return tmtx;
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    return this->operator()(field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    GeometricField<Type, fvPatchField, volMesh>& field,
    const word& fieldName
)
{
    return source(field, fieldName, field.dimensions()/dimTime*dimVolume);
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const volScalarField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    return this->operator()(rho, field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const volScalarField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field,
    const word& fieldName
)
{
    checkApplied();

    const dimensionSet ds
    (
        rho.dimensions()*field.dimensions()/dimTime*dimVolume
    );

    tmp<fvMatrix<Type>> tmtx(new fvMatrix<Type>(field, ds));
    fvMatrix<Type>& mtx = tmtx();

    forAll(*this, i)
    {
        option& source = this->operator[](i);

        label fieldi = source.applyToField(fieldName);

        if (fieldi != -1)
        {
            addProfiling(fvopt, "fvOption()." + source.name());

            source.setApplied(fieldi);

            if (source.isActive())
            {
                if (debug)
                {
                    Info<< "Applying source " << source.name() << " to field "
                        << fieldName << endl;
                }

                source.addSup(rho, mtx, fieldi);
            }
        }
    }

    return tmtx;
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const volScalarField& alpha,
    const volScalarField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    return this->operator()(alpha, rho, field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const volScalarField& alpha,
    const volScalarField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field,
    const word& fieldName
)
{
    checkApplied();

    const dimensionSet ds
    (
        alpha.dimensions()*rho.dimensions()*field.dimensions()
       /dimTime*dimVolume
    );

    tmp<fvMatrix<Type>> tmtx(new fvMatrix<Type>(field, ds));
    fvMatrix<Type>& mtx = tmtx();

    forAll(*this, i)
    {
        option& source = this->operator[](i);

        label fieldi = source.applyToField(fieldName);

        if (fieldi != -1)
        {
            addProfiling(fvopt, "fvOption()." + source.name());

            source.setApplied(fieldi);

            if (source.isActive())
            {
                if (debug)
                {
                    Info<< "Applying source " << source.name() << " to field "
                        << fieldName << endl;
                }

                source.addSup(alpha, rho, mtx, fieldi);
            }
        }
    }

    return tmtx;
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const geometricOneField& alpha,
    const geometricOneField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    return this->operator()(field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const volScalarField& alpha,
    const geometricOneField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    volScalarField one
    (
        IOobject
        (
            "one",
            this->mesh_.time().timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            false
        ),
        this->mesh_,
        dimensionedScalar("one", dimless, 1.0)
    );

    return this->operator()(alpha, one, field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::operator()
(
    const geometricOneField& alpha,
    const volScalarField& rho,
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    return this->operator()(rho, field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::d2dt2
(
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    return this->d2dt2(field, field.name());
}


template<class Type>
CML::tmp<CML::fvMatrix<Type>> CML::fv::optionList::d2dt2
(
    GeometricField<Type, fvPatchField, volMesh>& field,
    const word& fieldName
)
{
    return source(field, fieldName, field.dimensions()/sqr(dimTime)*dimVolume);
}


template<class Type>
void CML::fv::optionList::constrain(fvMatrix<Type>& eqn)
{
    checkApplied();

    forAll(*this, i)
    {
        option& source = this->operator[](i);

        label fieldi = source.applyToField(eqn.psi().name());

        if (fieldi != -1)
        {
            addProfiling(fvopt, "fvOption::constrain." + eqn.psi().name());

            source.setApplied(fieldi);

            if (source.isActive())
            {
                if (debug)
                {
                    Info<< "Applying constraint " << source.name()
                        << " to field " << eqn.psi().name() << endl;
                }

                source.constrain(eqn, fieldi);
            }
        }
    }
}


template<class Type>
void CML::fv::optionList::correct
(
    GeometricField<Type, fvPatchField, volMesh>& field
)
{
    const word& fieldName = field.name();

    forAll(*this, i)
    {
        option& source = this->operator[](i);

        label fieldi = source.applyToField(fieldName);

        if (fieldi != -1)
        {
            source.setApplied(fieldi);

            if (source.isActive())
            {
                if (debug)
                {
                    Info<< "Correcting source " << source.name()
                        << " for field " << fieldName << endl;
                }

                source.correct(field);
            }
        }
    }
}
#endif


