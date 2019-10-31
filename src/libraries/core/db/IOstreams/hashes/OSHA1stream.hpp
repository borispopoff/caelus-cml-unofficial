/*---------------------------------------------------------------------------*\
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

Class
    CML::OSHA1stream

Description
    An output stream for calculating SHA1 digests.

SourceFiles
    OSHA1stream.cpp

\*---------------------------------------------------------------------------*/

#ifndef OSHA1stream_H
#define OSHA1stream_H

#include "OSstream.hpp"
#include "SHA1.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

class osha1stream;
class OSHA1stream;

/*---------------------------------------------------------------------------*\
                        Class sha1streambuf Declaration
\*---------------------------------------------------------------------------*/

//- A streambuf class for calculating SHA1 digests
class sha1streambuf
:
    public std::streambuf
{
    // Private data

    //- This does all the work and has its own buffering
    SHA1 sha1_;

    friend class osha1stream;

public:

    // Constructors

        //- Construct null
        sha1streambuf()
        {}

    // Member Functions

    // Write

        //- Process unbuffered
        virtual std::streamsize xsputn(const char* str, std::streamsize n)
        {
            sha1_.append(str, n);
            return n;
        }
};


/*---------------------------------------------------------------------------*\
                         Class osha1stream Declaration
\*---------------------------------------------------------------------------*/

//- A basic output stream for calculating SHA1 digests
class osha1stream
:
    virtual public std::ios,
    public std::ostream
{
    // Private data

    sha1streambuf sbuf_;

public:

    // Constructors

        //- Construct null
        osha1stream()
        :
            std::ostream(&sbuf_)
        {}

    // Member Functions

    // Access

        //- This hides both signatures of std::basic_ios::rdbuf()
        sha1streambuf* rdbuf()
        {
            return &sbuf_;
        }

        //- Full access to the sha1
        SHA1& sha1()
        {
            return sbuf_.sha1_;
        }

};


/*---------------------------------------------------------------------------*\
                         Class OSHA1stream Declaration
\*---------------------------------------------------------------------------*/

//- The output stream for calculating SHA1 digests
class OSHA1stream
:
    public OSstream
{

    // Private Member Functions

        //- Disallow default bitwise copy construct
        OSHA1stream(const OSHA1stream&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const OSHA1stream&) = delete;

public:

    // Constructors

        //- Construct and set stream status
        OSHA1stream
        (
            streamFormat format=ASCII,
            versionNumber version=currentVersion
        )
        :
            OSstream
            (
                *(new osha1stream),
                "OSHA1stream.sinkFile_",
                format,
                version
            )
        {}


    //- Destructor
    ~OSHA1stream()
    {
        delete &dynamic_cast<osha1stream&>(stdStream());
    }


    // Member functions

    // Access

        //- Full access to the sha1
        CML::SHA1& sha1()
        {
            return dynamic_cast<osha1stream&>(stdStream()).sha1();
        }

        //- Return SHA1::Digest for the data processed until now
        CML::SHA1Digest digest()
        {
            return sha1().digest();
        }

    // Edit

        //- Clear the SHA1 calculation
        void rewind()
        {
            sha1().clear();
        }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
