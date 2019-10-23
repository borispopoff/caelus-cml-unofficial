/*---------------------------------------------------------------------------*\
Copyright (C) 2014 Applied CCM
Copyright (C) 2011-2019 OpenFOAM Foundation
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

Typedef
    CML::label

Description
    A label is an int/long/long long depending on the range desired.

    A readLabel function is defined so that label can be constructed from
    Istream.

\*---------------------------------------------------------------------------*/

#ifndef label_H
#define label_H

#include <climits>
#include <cstdlib>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#if CAELUS_LABEL64
#    define CAELUS_LABEL_MAX 9000000000000000000
#else
#    define CAELUS_LABEL_MAX 2000000000
#endif

#include "pTraits.hpp"
#include "direction.hpp"


#if INT_MAX > CAELUS_LABEL_MAX

// Define label as an int

# undef  CAELUS_LABEL_MAX
# define CAELUS_LABEL_MAX INT_MAX

# include "int.hpp"

namespace CML
{
    typedef int label;

    static const label labelMin = INT_MIN;
    static const label labelMax = INT_MAX;

    inline label readLabel(Istream& is)
    {
        return readInt(is);
    }

    inline void writeEntry(Ostream& os, const label value)
    {
        os << value;
    }

} // End namespace CML


#elif LONG_MAX > CAELUS_LABEL_MAX
// Define label as a long

# undef  CAELUS_LABEL_MAX
# define CAELUS_LABEL_MAX LONG_MAX

# include "int.hpp"
# include "long.hpp"

namespace CML
{
    typedef long label;

    static const label labelMin = LONG_MIN;
    static const label labelMax = LONG_MAX;

    inline label readLabel(Istream& is)
    {
        return readLong(is);
    }

} // End namespace CML


#elif LLONG_MAX > CAELUS_LABEL_MAX

// Define label as a long long

# undef  CAELUS_LABEL_MAX
# define CAELUS_LABEL_MAX LLONG_MAX

# include "int.hpp"
# include "long.hpp"
# include "longLong.hpp"

namespace CML
{
    typedef long long label;

    static const label labelMin = LLONG_MIN;
    static const label labelMax = LLONG_MAX;

    inline label readLabel(Istream& is)
    {
        return readLongLong(is);
    }

    inline bool readLabel(const char* buf, label& s)
    {
        return readLongLong(buf, s);
    }

} // End namespace CML

#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//#include "pTraits.hpp"
//#include "direction.hpp"

namespace CML
{

//- template specialization for pTraits<label>
template<>
class pTraits<label>
{
    label p_;

public:

    //- Component type
    typedef label cmptType;

    // Member constants

        enum
        {
            dim = 3,         //!< Dimensionality of space
            rank = 0,        //!< Rank of label is 0
            nComponents = 1  //!< Number of components in label is 1
        };


    // Static data members

        static const char* const typeName;
        static const char* componentNames[];
        static const label zero;
        static const label one;
        static const label min;
        static const label max;
        static const label rootMin;
        static const label rootMax;


    // Constructors

        //- Construct from primitive
        explicit pTraits(const label&);

        //- Construct from Istream
        pTraits(Istream&);


    // Member Functions

        //- Access to the label value
        operator label() const
        {
            return p_;
        }

        //- Access to the label value
        operator label&()
        {
            return p_;
        }
};


// For convenience's sake always define pTraits<int> (so even if label != int)
#if (CAELUS_LABEL_MAX != INT_MAX)
//- template specialization for pTraits<int>
template<>
class pTraits<int>
{
    int p_;

public:

    //- Component type
    typedef int cmptType;

    // Member constants

        enum
        {
            dim = 3,         //!< Dimensionality of space
            rank = 0,        //!< Rank of int is 0
            nComponents = 1  //!< Number of components in int is 1
        };


    // Static data members

        static const char* const typeName;
        static const char* componentNames[];
        static const int zero;
        static const int one;
        static const int min;
        static const int max;
        static const int rootMax;
        static const int rootMin;


    // Constructors

        //- Construct from primitive
        explicit pTraits(const int&);

        //- Construct from Istream
        pTraits(Istream&);


    // Member Functions

        //- Access to the int value
        operator int() const
        {
            return p_;
        }

        //- Access to the int value
        operator int&()
        {
            return p_;
        }
};
#endif



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//- Raise one label to the power of another
label pow(label a, label b);

//- Evaluate n! : 0 < n <= 12
label factorial(label n);


#define MAXMIN(retType, type1, type2)                                     \
                                                                          \
inline retType max(const type1 s1, const type2 s2)                        \
{                                                                         \
    return (s1 > s2)? static_cast<retType>(s1): static_cast<retType>(s2); \
}                                                                         \
                                                                          \
inline retType min(const type1 s1, const type2 s2)                        \
{                                                                         \
    return (s1 < s2)? static_cast<retType>(s1): static_cast<retType>(s2); \
}


MAXMIN(char, char, char)
MAXMIN(short, short, short)
MAXMIN(int, int, int)
MAXMIN(long, long, long)
MAXMIN(long, long, int)
MAXMIN(long long, long long, long long)

MAXMIN(unsigned char, unsigned char, unsigned char)
MAXMIN(unsigned short, unsigned short, unsigned short)
MAXMIN(unsigned int, unsigned int, unsigned int)
MAXMIN(unsigned long, unsigned long, unsigned long)
MAXMIN(unsigned long long, unsigned long long, unsigned long long)

MAXMIN(long, int, long)
MAXMIN(long long, int, long long)
MAXMIN(long long, long long, int)

inline label& setComponent(label& l, const direction)
{
    return l;
}

inline label component(const label l, const direction)
{
    return l;
}

inline label mag(const label l)
{
    return ::abs(l);
}

inline label sign(const label s)
{
    return (s >= 0)? 1: -1;
}

inline label pos(const label s)
{
    return (s >= 0)? 1: 0;
}

inline label neg(const label s)
{
    return (s < 0)? 1: 0;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
