/*---------------------------------------------------------------------------*\
Copyright (C) 2014 Applied CCM
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

Namespace
    CML::debug

Description
    Namespace for handling debugging switches.


\*---------------------------------------------------------------------------*/

#ifndef debug_H
#define debug_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace CML
{

// Forward declaration of classes
class dictionary;
class Istream;
class Ostream;
class simpleRegIOobject;
class simpleObjectRegistry;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace debug
{
    //- The central control dictionary.
    //  Located in ~/.CAELUS/VERSION or $CAELUS_PROJECT_DIR/etc
    //  \sa CML::findEtcFile()
    dictionary& controlDict();

    //- The DebugSwitches sub-dictionary in the central controlDict.
    dictionary& debugSwitches();

    //- The InfoSwitches sub-dictionary in the central controlDict.
    dictionary& infoSwitches();

    //- The OptimisationSwitches sub-dictionary in the central controlDict.
    dictionary& optimisationSwitches();

    //- Lookup debug switch or add default value.
    int debugSwitch(const char* name, const int defaultValue=0);

    //- Lookup info switch or add default value.
    int infoSwitch(const char* name, const int defaultValue=0);

    //- Lookup optimisation switch or add default value.
    int optimisationSwitch(const char* name, const int defaultValue=0);

    //- Internal function to lookup a sub-dictionary from controlDict.
    dictionary& switchSet(const char* subDictName, dictionary*& subDictPtr);

    //- List debug switches
    void listSwitches(const bool unset);


    // Registered debug switches

        //- Register debug switch read/write object
        void addDebugObject(const char* name, simpleRegIOobject* obj);

        //- Register info switch read/write object
        void addInfoObject(const char* name, simpleRegIOobject* obj);

        //- Register optimisation switch read/write object
        void addOptimisationObject(const char* name, simpleRegIOobject* obj);

        //- Register DimensionSets read/write object
        void addDimensionSetObject(const char* name, simpleRegIOobject* obj);

        //- Register DimensionedConstant read/write object
        void addDimensionedConstantObject(const char* name, simpleRegIOobject*);


        //- Get access to registered debug switch objects
        simpleObjectRegistry& debugObjects();

        //- Get access to registered info switch objects
        simpleObjectRegistry& infoObjects();

        //- Get access to registered optimisation switch objects
        simpleObjectRegistry& optimisationObjects();

        //- Get access to registered dimensionSets switch objects
        simpleObjectRegistry& dimensionSetObjects();

        //- Get access to registered dimensionedConstant switch objects
        simpleObjectRegistry& dimensionedConstantObjects();

        //- List registered debug switches
        void listRegisteredSwitches(const bool unset);

} // End namespace debug


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace CML

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
