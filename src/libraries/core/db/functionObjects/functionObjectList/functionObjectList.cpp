/*---------------------------------------------------------------------------*\
Copyright (C) 2011-2018 OpenFOAM Foundation
Copyright (C) 2016 OpenCFD Ltd

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

\*---------------------------------------------------------------------------*/

#include "functionObjectList.hpp"
#include "Time.hpp"
#include "profiling.hpp"

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

CML::functionObject* CML::functionObjectList::remove
(
    const word& key,
    label& oldIndex
)
{
    functionObject* ptr = 0;

    // Find index of existing functionObject
    HashTable<label>::iterator fnd = indices_.find(key);

    if (fnd != indices_.end())
    {
        oldIndex = fnd();

        // Retrieve the pointer and remove it from the old list
        ptr = this->set(oldIndex, 0).ptr();
        indices_.erase(fnd);
    }
    else
    {
        oldIndex = -1;
    }

    return ptr;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::functionObjectList::functionObjectList
(
    const Time& t,
    const bool execution
)
:
    PtrList<functionObject>(),
    digests_(),
    indices_(),
    time_(t),
    parentDict_(t.controlDict()),
    execution_(execution),
    updated_(false)
{}


CML::functionObjectList::functionObjectList
(
    const Time& t,
    const dictionary& parentDict,
    const bool execution
)
:
    PtrList<functionObject>(),
    digests_(),
    indices_(),
    time_(t),
    parentDict_(parentDict),
    execution_(execution),
    updated_(false)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CML::functionObjectList::~functionObjectList()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::functionObjectList::clear()
{
    PtrList<functionObject>::clear();
    digests_.clear();
    indices_.clear();
    updated_ = false;
}


CML::label CML::functionObjectList::findObjectID(const word& name) const
{
    forAll(*this, objectI)
    {
        if (operator[](objectI).name() == name)
        {
            return objectI;
        }
    }

    return -1;
}


void CML::functionObjectList::on()
{
    execution_ = true;
}


void CML::functionObjectList::off()
{
    // For safety, also force a read() when execution is turned back on
    updated_ = execution_ = false;
}


bool CML::functionObjectList::status() const
{
    return execution_;
}


bool CML::functionObjectList::start()
{
    return read();
}


bool CML::functionObjectList::execute(const bool forceWrite)
{
    bool ok = true;

    if (execution_)
    {
        if (!updated_)
        {
            read();
        }

        forAll(*this, objectI)
        {
            addProfiling
            (
                fo,
                "functionObject::" + operator[](objectI).name() + "::execute"
            );

            ok = operator[](objectI).execute(forceWrite) && ok;
        }
    }

    return ok;
}


bool CML::functionObjectList::end()
{
    bool ok = true;

    if (execution_)
    {
        if (!updated_)
        {
            read();
        }

        forAll(*this, objectI)
        {
            addProfiling
            (
                fo,
                "functionObject::" + operator[](objectI).name() + "::end"
            );

            ok = operator[](objectI).end() && ok;
        }
    }

    return ok;
}


bool CML::functionObjectList::setTimeStep()
{
    bool set = true;

    if (execution_)
    {
        if (!updated_)
        {
            read();
        }

        wordList names;

        forAll(*this, objectI)
        {
            if (operator[](objectI).setTimeStep())
            {
                names.append(operator[](objectI).name());
                set = true;
            }
        }

        if (names.size() > 1)
        {
            WarningInFunction << "Multiple function objects (" << names[0];
            for (label i = 1; i < names.size(); ++ i)
            {
                WarningInFunction << ", " << names[i];
            }
            WarningInFunction << ") are setting the time step." << endl;
        }
    }

    return set;
}


CML::scalar CML::functionObjectList::timeToNextWrite()
{
    scalar result = VGREAT;

    if (execution_)
    {
        if (!updated_)
        {
            read();
        }

        forAll(*this, objectI)
        {
            result = min(result, operator[](objectI).timeToNextWrite());
        }
    }

    return result;
}


bool CML::functionObjectList::read()
{
    bool ok = true;
    updated_ = execution_;

    // Avoid reading/initializing if execution is off
    if (!execution_)
    {
        return true;
    }

    // Update existing and add new functionObjects
    const entry* entryPtr = parentDict_.lookupEntryPtr
    (
        "functions",
        false,
        false
    );

    if (entryPtr)
    {
        PtrList<functionObject> newPtrs;
        List<SHA1Digest> newDigs;
        HashTable<label> newIndices;

        label nFunc = 0;

        addProfiling(fo,"functionObjects::read");

        if (entryPtr->isDict())
        {
            // a dictionary of functionObjects
            const dictionary& functionDicts = entryPtr->dict();

            newPtrs.setSize(functionDicts.size());
            newDigs.setSize(functionDicts.size());

            forAllConstIter(dictionary, functionDicts, iter)
            {
                // safety:
                if (!iter().isDict())
                {
                    continue;
                }
                const word& key = iter().keyword();
                const dictionary& dict = iter().dict();

                newDigs[nFunc] = dict.digest();

                label oldIndex;
                functionObject* objPtr = remove(key, oldIndex);
                if (objPtr)
                {
                    // an existing functionObject, and dictionary changed
                    if (newDigs[nFunc] != digests_[oldIndex])
                    {
                        addProfiling
                        (
                            fo2,
                            "functionObject::" + objPtr->name() + "::read"
                        );

                        ok = objPtr->read(dict) && ok;
                    }
                }
                else
                {
                    // New functionObject
                    addProfiling
                    (
                        fo2,
                        "functionObject::" + key + "::start"
                    );

                    objPtr = functionObject::New(key, time_, dict).ptr();
                    ok = objPtr->start() && ok;
                }

                newPtrs.set(nFunc, objPtr);
                newIndices.insert(key, nFunc);
                nFunc++;
            }
        }
        else
        {
            // a list of functionObjects
            PtrList<entry> functionDicts(entryPtr->stream());

            newPtrs.setSize(functionDicts.size());
            newDigs.setSize(functionDicts.size());

            forAllIter(PtrList<entry>, functionDicts, iter)
            {
                // safety:
                if (!iter().isDict())
                {
                    continue;
                }
                const word& key = iter().keyword();
                const dictionary& dict = iter().dict();

                newDigs[nFunc] = dict.digest();

                label oldIndex;
                functionObject* objPtr = remove(key, oldIndex);
                if (objPtr)
                {
                    // an existing functionObject, and dictionary changed
                    if (newDigs[nFunc] != digests_[oldIndex])
                    {
                        ok = objPtr->read(dict) && ok;
                    }
                }
                else
                {
                    // new functionObject
                    objPtr = functionObject::New(key, time_, dict).ptr();
                    ok = objPtr->start() && ok;
                }

                newPtrs.set(nFunc, objPtr);
                newIndices.insert(key, nFunc);
                nFunc++;
            }
        }

        // safety:
        newPtrs.setSize(nFunc);
        newDigs.setSize(nFunc);

        // Updating the PtrList of functionObjects deletes any
        // existing unused functionObjects
        PtrList<functionObject>::transfer(newPtrs);
        digests_.transfer(newDigs);
        indices_.transfer(newIndices);
    }
    else
    {
        PtrList<functionObject>::clear();
        digests_.clear();
        indices_.clear();
    }

    return ok;
}


// ************************************************************************* //
