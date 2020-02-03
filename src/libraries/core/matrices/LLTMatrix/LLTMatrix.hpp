/*---------------------------------------------------------------------------*\
Copyright (C) 2016 OpenFOAM Foundation
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
    CML::LLTMatrix

Description
    Templated class to perform the Cholesky decomposition on a
    symmetric positive-definite matrix.

    Member functions are provided to solve linear systems using the LLT
    decomposition.


\*---------------------------------------------------------------------------*/

#ifndef LLTMatrix_HPP
#define LLTMatrix_HPP

#include "SquareMatrix.hpp"
#include "Field.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                         Class LLTMatrix Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class LLTMatrix
:
    public SquareMatrix<Type>
{

public:

    // Constructors

        //- Construct null
        LLTMatrix();

        //- Construct from a square matrix and perform the decomposition
        LLTMatrix(const SquareMatrix<Type>& M);


    // Member Functions

        //- Perform the Cholesky decomposition of the matrix
        void decompose(const SquareMatrix<Type>& M);

        //- Solve the linear system with the given source
        //  and returning the solution in the Field argument x.
        //  This function may be called with the same field for x and source.
        void solve(Field<Type>& x, const Field<Type>& source) const;

        //- Solve the linear system with the given source
        //  returning the solution
        tmp<Field<Type>> solve(const Field<Type>& source) const;
};


} // End namespace CML


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
CML::LLTMatrix<Type>::LLTMatrix()
{}


template<class Type>
CML::LLTMatrix<Type>::LLTMatrix(const SquareMatrix<Type>& M)
{
    decompose(M);
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class Type>
void CML::LLTMatrix<Type>::decompose(const SquareMatrix<Type>& M)
{
    SquareMatrix<Type>& LLT = *this;

    // Initialize the LLT decomposition matrix to M
    LLT = M;

    const label m = LLT.m();

    for (label i=0; i<m; i++)
    {
        for (label j=0; j<m; j++)
        {
            if (j > i)
            {
                LLT(i, j) = Zero;
                continue;
            }

            Type sum = LLT(i, j);

            for (label k=0; k<j; k++)
            {
                sum -= LLT(i, k)*LLT(j, k);
            }

            if (i > j)
            {
                LLT(i, j) = sum/LLT(j, j);
            }
            else if (sum > 0)
            {
                LLT(i, i) = sqrt(sum);
            }
            else
            {
                FatalErrorInFunction
                    << "Cholesky decomposition failed, "
                       "matrix is not symmetric positive definite"
                    << abort(FatalError);
            }
        }
    }
}


template<class Type>
void CML::LLTMatrix<Type>::solve
(
    Field<Type>& x,
    const Field<Type>& source
) const
{
    // If x and source are different initialize x = source
    if (&x != &source)
    {
        x = source;
    }

    const SquareMatrix<Type>& LLT = *this;
    const label m = LLT.m();

    for (label i=0; i<m; i++)
    {
        Type sum = source[i];

        for (label j=0; j<i; j++)
        {
            sum = sum - LLT(i, j)*x[j];
        }

        x[i] = sum/LLT(i, i);
    }

    for (int i=m - 1; i >= 0; i--)
    {
        Type sum = x[i];

        for (label j=i + 1; j<m; j++)
        {
            sum = sum - LLT(j, i)*x[j];
        }

        x[i] = sum/LLT(i, i);
    }

}


template<class Type>
CML::tmp<CML::Field<Type>> CML::LLTMatrix<Type>::solve
(
    const Field<Type>& source
) const
{
    tmp<Field<Type>> tx(new Field<Type>(this->m()));
    Field<Type>& x = tx.ref();

    solve(x, source);

    return tx;
}


#endif
