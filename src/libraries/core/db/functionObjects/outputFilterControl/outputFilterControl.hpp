/*---------------------------------------------------------------------------* \
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
    CML::outputFilterControl

Description
    An output control for function objects.
    The default is time-step execution at every interval.

SourceFiles
    outputFilterControl.cpp

\*---------------------------------------------------------------------------*/

#ifndef outputFilterControl_H
#define outputFilterControl_H

#include "dictionary.hpp"
#include "Time.hpp"
#include "NamedEnum.hpp"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

/*---------------------------------------------------------------------------*\
                  Class outputFilterControl Declaration
\*---------------------------------------------------------------------------*/

class outputFilterControl
{
public:

    //- The output control options
    enum class timeControls
    {
        timeStep,           //!< execution is coupled to the time-step
        writeTime,          //!< execution is coupled to the write-time
        outputTime,         //!< execution is coupled to the output-time
        adjustableRunTime,  //!< Adjust time step for execution
        runTime,            //!< run time for execution
        clockTime,          //!< clock time for execution
        cpuTime,            //!< cpu time for execution
        none                //!< no execution
    };


private:

    // Private data

        //- Time object
        const Time& time_;

        //- Prefix
        const word prefix_;

        //- String representation of timeControls enums
        static const NamedEnum<timeControls, 8> timeControlNames_;

        //- Type of output
        timeControls timeControl_;

        //- Execution interval steps for timeStep mode
        //  a value <= 1 means execute at every time step
        label intervalSteps_;

        //- Execution interval
        scalar interval_;

        //- Index of previous execution
        label executionIndex_;


    // Private Member Functions

        //- Disallow default bitwise copy construct and assignment
        outputFilterControl(const outputFilterControl&) = delete;

        //- Disallow default bitwise assignment
        void operator=(const outputFilterControl&) = delete;


public:

    // Constructors

        //- Construct from Time object and dictionary
        outputFilterControl
        (
            const Time&,
            const dictionary&,
            const word prefix = "write"
        );


    //- Destructor
    ~outputFilterControl();


    // Member Functions

        //- Read from dictionary
        void read(const dictionary&);

        //- Return Time
        inline const Time& time() const
        {
            return time_;
        }

        //- Flag to indicate whether to execute
        bool execute();

        //- Return control
        inline timeControls control() const
        {
            return timeControl_;
        }

        //- Return interval
        inline scalar interval() const
        {
            return interval_;
        }

        //- Return the index of the previous execution
        inline label executionIndex() const
        {
            return executionIndex_;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
