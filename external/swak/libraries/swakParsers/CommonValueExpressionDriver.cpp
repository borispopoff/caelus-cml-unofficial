/*---------------------------------------------------------------------------*\
Copyright: ICE Stroemungsfoschungs GmbH
Copyright (C) 1991-2008 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is based on CAELUS.

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

Contributors/Copyright:
    2010-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
    2012 Bruno Santos <wyldckat@gmail.com>
    2014 Hrvoje Jasak <h.jasak@wikki.co.uk>

\*---------------------------------------------------------------------------*/

#include "CommonValueExpressionDriver.hpp"
#include "GlobalVariablesRepository.hpp"

#include "ExpressionDriverWriter.hpp"

#include "Random.hpp"

#include "entryToExpression.hpp"

namespace CML {


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CommonValueExpressionDriver,0);

defineRunTimeSelectionTable(CommonValueExpressionDriver, dictionary);
defineRunTimeSelectionTable(CommonValueExpressionDriver, idName);

    // Currently not working
bool CommonValueExpressionDriver::cacheSets_=true;

const fvMesh *CommonValueExpressionDriver::defaultMeshPtr_=nullptr;

const fvMesh &CommonValueExpressionDriver::getDefaultMesh()
{
    if(defaultMeshPtr_==nullptr) {
        FatalErrorInFunction
            << "No default mesh set (value is NULL)" << endl
            << "Try using the 'initSwakFunctionObject' to work   around this"
            << endl
            << abort(FatalError);
    }
    return *defaultMeshPtr_;
}

bool CommonValueExpressionDriver::resetDefaultMesh(const fvMesh &mesh)
{
    bool wasSet=defaultMeshPtr_!=nullptr;

    defaultMeshPtr_=&mesh;

    return wasSet;
}

bool CommonValueExpressionDriver::setUnsetDefaultMesh(const fvMesh &mesh)
{
    if(defaultMeshPtr_==nullptr) {
        Info << "swak: Setting default mesh" << endl;
        defaultMeshPtr_=&mesh;

        return true;
    } else {
        return false;
    }
}

dictionary CommonValueExpressionDriver::emptyData_("noDictionary");

    // this should be sufficient
const label CommonValueExpressionDriver::maxVariableRecursionDepth_=100;

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


CommonValueExpressionDriver::CommonValueExpressionDriver(
    const CommonValueExpressionDriver& orig
)
:
    dict_(orig.dict_),
    variableStrings_(orig.variableStrings_),
    contextString_(orig.contextString_),
    aliases_(orig.aliases_),
    result_(orig.result_),
    variables_(orig.variables_),
    storedVariables_(orig.storedVariables_),
    delayedVariables_(orig.delayedVariables_),
    specialVariablesIndex_(orig.specialVariablesIndex_),
    globalVariableScopes_(orig.globalVariableScopes_),
    lines_(orig.lines_),
    lookup_(orig.lookup_),
    content_(""),
    trace_scanning_ (orig.trace_scanning_),
    trace_parsing_ (orig.trace_parsing_),
    variableNameIdenticalToField_(orig.variableNameIdenticalToField_),
    scanner_(nullptr),
    prevIterIsOldTime_(orig.prevIterIsOldTime_)
{
    if(debug) {
        Info << "CommonValueExpressionDriver - copy constructor" << endl;
    }
    setSearchBehaviour(
        orig.cacheReadFields_,
        orig.searchInMemory_,
        orig.searchOnDisc_
    );
}

CommonValueExpressionDriver::CommonValueExpressionDriver(
    const dictionary& dict
)
:
    dict_(dict),
    variableStrings_(readVariableStrings(dict)),
    contextString_("- From dictionary: "+dict.name()),
    specialVariablesIndex_(-1),
    globalVariableScopes_(dict.lookupOrDefault("globalScopes",wordList())),
    content_(""),
    trace_scanning_ (dict.lookupOrDefault("traceScanning",false)),
    trace_parsing_ (dict.lookupOrDefault("traceParsing",false)),
    variableNameIdenticalToField_(
        dict.lookupOrDefault("variableNameIdenticalToField",false)
    ),
    scanner_(nullptr),
    prevIterIsOldTime_(dict.lookupOrDefault("prevIterIsOldTime",false))
{
    debug=dict.lookupOrDefault<label>("debugCommonDriver",debug);

    if(debug) {
        Pout << "CommonValueExpressionDriver::CommonValueExpressionDriver(const dictionary& dict)" << endl;
    }

    if(dict.found("storedVariables")) {
        storedVariables_=List<StoredExpressionResult>(
            dict.lookup("storedVariables")
        );
        if(debug) {
            Info << "Read stored variables:" << storedVariables_ << endl;
        }
    }

    if(dict.found("delayedVariables")) {
        List<DelayedExpressionResult> readDelays(
            dict.lookup("delayedVariables")
        );
        forAll(readDelays,i)
        {
            delayedVariables_.insert(readDelays[i].name(),readDelays[i]);
        }
        if(debug) {
            Info << "Read delayed variables:" << delayedVariables_ << endl;
        }
    }

    setSearchBehaviour(
        dict.lookupOrDefault("cacheReadFields",false),
        dict.lookupOrDefault("searchInMemory",true),
        dict.lookupOrDefault("searchOnDisc",false)
    );

    readTables(dict);
}

CommonValueExpressionDriver::CommonValueExpressionDriver(
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
:
    dict_(emptyData_),
    variableStrings_(),
    contextString_("- Driver constructed from scratch"),
    specialVariablesIndex_(-1),
    globalVariableScopes_(),
    content_(""),
    trace_scanning_ (false),
    trace_parsing_ (false),
    variableNameIdenticalToField_(false),
    scanner_(nullptr),
    prevIterIsOldTime_(false)
{
    setSearchBehaviour(
        cacheReadFields,
        searchInMemory,
        searchOnDisc
    );
}

void CommonValueExpressionDriver::readVariablesAndTables(const dictionary &dict)
{
    debug=dict.lookupOrDefault<label>("debugCommonDriver",debug);

    if(dict.found("globalScopes")) {
        setGlobalScopes(wordList(dict.lookup("globalScopes")));
    }

    if(dict.found("storedVariables")) {
        if(
            writer_.valid()
            &&
            storedVariables_.size()>0
        ) {
            WarningInFunction
                << "Context: " << contextString() << endl
                << "Seems like 'storedVariables' was already read."
                << " No update from "
                << dict.lookup("storedVariables")
                << endl;
        } else {
            storedVariables_=List<StoredExpressionResult>(
                dict.lookup(
                    "storedVariables"
                )
            );
        }
    }

    if(dict.found("delayedVariables")) {
        if(
            writer_.valid()
            &&
            delayedVariables_.size()>0
        ) {
            WarningInFunction
                << "Context: " << contextString() << endl
                << "Seems like 'delayedVariables' was already read."
                << " No update from "
                << dict.lookup("delayedVariables")
                << endl;
        } else {
            List<DelayedExpressionResult> readDelays(
                dict.lookup("delayedVariables")
            );
            forAll(readDelays,i)
            {
                delayedVariables_[readDelays[i].name()]=readDelays[i];
            }
        }
    }

    setVariableStrings(dict);

    readTables(dict);
}

label CommonValueExpressionDriver::readForeignMeshInfo(
        const dictionary &dict,
        bool verbose
) {
    if(!dict.found("foreignMeshes")) {
        if(verbose) {
            Info << "No information about foreign meshes in "
                << dict.name() << nl << endl;
        }
        return 0;
    }

    label cnt=0;

    const dictionary &foreignMeshInfo=dict.subDict("foreignMeshes");
    const wordList names(foreignMeshInfo.toc());
    forAll(names,i) {
        const word &name=names[i];
        if(verbose) {
            Info << "Adding foreign mesh " << name << flush;
        }
        const dictionary &info=foreignMeshInfo.subDict(name);
        const fileName cDir(info.lookup("case"));
        word region=polyMesh::defaultRegion;
        if(info.found("region")) {
            region=word(info.lookup("region"));
        }
        const scalar time(readScalar(info.lookup("time")));
        const meshToMeshOrder interpolationOrder
            =
            meshToMeshInterpolationNames[
                word(info.lookup("interpolationOrder"))
            ];

        if(verbose) {
            Info << " case " << cDir << " region " << region
                << " at t=" << time << " with interpolation order "
                << " interpolation order "
                << word(info.lookup("interpolationOrder")) << endl;
        }

        MeshesRepository::getRepository().addMesh(
            name,
            cDir,
            region
        );
        MeshesRepository::getRepository().setInterpolationOrder(
            name,
            interpolationOrder
        );
        scalar t=MeshesRepository::getRepository().setTime(
            name,
            time
        );
        Info << "Actual mesh time t=" << t << nl << endl;

        cnt++;
    }

    return cnt;
}

void CommonValueExpressionDriver::readTables(const dictionary &dict)
{
    if(dict.found("timelines")) {
        readTables(dict.lookup("timelines"),lines_);
    }

    if(dict.found("lookuptables")) {
        readTables(dict.lookup("lookuptables"),lookup_);
    }

    if(dict.found("aliases")) {
        dictionary aliasDict(dict.subDict("aliases"));
        wordList toc(aliasDict.toc());
        forAll(toc,i) {
            aliases_.insert(
                toc[i],
                word(aliasDict[toc[i]])
            );
        }
        if(debug) {
            Info << "Reading aliases: " << aliases_ << endl;
        }
    }
}

void CommonValueExpressionDriver::setSearchBehaviour(
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
{
    cacheReadFields_=cacheReadFields;
    searchInMemory_=
        searchInMemory
        ||
        cacheReadFields;
    searchOnDisc_=searchOnDisc;
}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const dictionary& dict
)
{
    return CommonValueExpressionDriver::New(
        dict,
        getDefaultMesh()
    );
}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const dictionary& dict,
    const fvMesh& mesh
)
{
    setUnsetDefaultMesh(mesh);

    word driverType(dict.lookup("valueType"));
    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(driverType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown  CommonValueExpressionDriver type " << driverType
            << endl << endl
            << "Valid valueTypes are :" << endl
            << dictionaryConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    if(debug) {
        Pout << "Creating driver of type " << driverType << endl;
    }

    return autoPtr<CommonValueExpressionDriver>
    (
        cstrIter()(dict,mesh)
    );
}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const word& driverType,
    const word& id,
    const fvMesh& mesh
)
{
    setUnsetDefaultMesh(mesh);

    idNameConstructorTable::iterator cstrIter =
        idNameConstructorTablePtr_->find(driverType);

    if (cstrIter == idNameConstructorTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown  CommonValueExpressionDriver type " << driverType
            << endl << endl
            << "Valid valueTypes are :" << endl
            << idNameConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    if(debug) {
        Pout << "Creating driver of type " << driverType << endl;
    }

    return autoPtr<CommonValueExpressionDriver>
    (
        cstrIter()(id,mesh)
    );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CommonValueExpressionDriver::~CommonValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

List<exprString> CommonValueExpressionDriver::readVariableStrings(
    const dictionary &dict,
    const word &name,
    const label recursionDepth
)
{
    Sbug << "::readVariableStrings " << name
        << " depth " << recursionDepth << endl;

    if(!dict.found(name)) {
        return List<exprString>();
    }

    if(recursionDepth>maxVariableRecursionDepth_) {
        FatalErrorInFunction
            << "While reading variable list " << name << " in "
            << dict.name() << "the maximum recursion depth "
            << maxVariableRecursionDepth_ << " was exceeded." << nl
            << "Probable cause is that variable lists are referencing "
            << "each other in a circular fashion"
            << endl
            << exit(FatalError);
    }

    const entry &e=dict.lookupEntry(
        name,
        true, // recursive
        true //  pattern matching
    );

    ITstream data(e.stream());
    token nextToken;
    data.read(nextToken);

    List<string> result;

    if(nextToken.isString()) {
        Sbug << name << " is a single string" << endl;
        data.rewind();
        result=expandIncludeStringList(
            List<string>(1,string(data)),
            dict,
            recursionDepth+1
        );
    } else if(
        nextToken.type()==token::PUNCTUATION
        &&
        nextToken.pToken()==token::BEGIN_LIST
    ) {
        Sbug << name << " is a list of strings" << endl;
        data.rewind();
        result=expandIncludeStringList(
            List<string>(data),
            dict,
            recursionDepth+1
        );
    } else if(nextToken.isLabel()) {
        Sbug << name << " is a list of strings with prefix" << endl;
        token anotherToken;
        data.read(anotherToken);
        if(
            anotherToken.type()==token::PUNCTUATION
            &&
            anotherToken.pToken()==token::BEGIN_LIST
        ) {
            data.rewind();
            result=expandIncludeStringList(
                List<string>(data),
                dict,
                recursionDepth+1
            );
        }
    } else {
        FatalErrorInFunction
            << " Entry '"<< name << "' must in dictionary "
            << dict.name() << " either be a string or a list of strings"
            << endl
            << exit(FatalError);

        return List<exprString>();
    }

    List<exprString> toReturn(result.size());

    forAll(result,i) {
        toReturn[i]=exprString(
            result[i],
            dict
        );
    }
    Sbug << "Returns: " << toReturn;
    return toReturn;
}

List<string> CommonValueExpressionDriver::expandIncludeStringList(
    const List<string> &orig,
    const dictionary &dict,
    const label recursionDepth
) {
    Sbug << "::expandIncludeStringList " << orig << endl;

    DynamicList<string> strings;

    forAll(orig,i) {
	string o=orig[i];
        Sbug << "Checking " << o << endl;

        o.removeTrailing(' ');

        std::string::size_type start=0;
        std::string::size_type end=0;
        while(start<o.length()) {
            end=o.find(';',start);
            if(end==std::string::npos) {
            FatalErrorInFunction
                << "No terminating ';' found in expression '"
                << o.substr(start) << "' of dictionary "
                << dict.name() << "\n"
                << endl
                << exit(FatalError);
            }
            string sub=o.substr(start,end-start+1);

            Sbug << "Found: " << sub << endl;

            if(sub[0]=='#') {
                std::string::size_type semiPos=sub.find(';');
                assert(semiPos!=std::string::npos);
                string inList=sub.substr(1,semiPos-1);
                Sbug << "Include " << inList << endl;
                List<exprString> expansion(
                    // expansion shoujld happen implicitly here
                    readVariableStrings(
                        dict,
                        inList,
                        recursionDepth
                    )
                );
                Sbug << "Got expansion from " << inList << ": "
                    << expansion << endl;
                forAll(expansion,k){
                    // strings.append(
                    //     expandDictVariables(
                    //         expansion[k],
                    //         dict)
                    // );
                    strings.append(expansion[k]);
                }
            } else {
                strings.append(
                    expandDictVariables(
                        sub,
                        dict
                    )
                );
            }
            start=end+1;
        }
    }

    strings.shrink();
    return List<string>(strings);
}

inline bool is_valid(char c)
{
    return (isalpha(c) || isdigit(c) || (c == '_'));
}

string getEntryString(
    const dictionary &dict,
    const string &replace
) {
    const entry &e=dict.lookupEntry(
        replace,
        true, // recursive
        true //  pattern matching
    );
    if(e.isDict()) {
        FatalErrorInFunction
            << "Entry " << replace << " found in dictionary "
            << dict.name() << " but is a dictionary"
            << endl
            << exit(FatalError);
    }
    return entryToExpression::fromEntry(e);
}

exprString CommonValueExpressionDriver::expandDictVariables(
    const string &orig,
    const dictionary &dict
) {
    Sbug << "::expandDictVariables " << orig << endl;

    string result=orig;

    while(result.find('$')!=std::string::npos) {
        std::string::size_type dollarPos=result.find('$');
        if(dollarPos+1>=result.size()) {
            FatalErrorInFunction
                << "'$' found at end in " << result << "(originally "
                << orig << ")"
                << endl
                << exit(FatalError);
        }
        std::string::size_type endPos=std::string::npos;

        if(result[dollarPos+1]=='[') {
            // "protected pattern"
            endPos=result.find(']',dollarPos+1);
            if(endPos==std::string::npos) {
                FatalErrorInFunction
                    << "No correct terminating ']' found in " << result
                    << " (originally " << orig << ")"
                    << endl
                    << exit(FatalError);
            }
        } else {
            // 'pure' name ... word
            endPos=dollarPos;
            while(
                endPos+1<result.size()
                &&
                is_valid(result[endPos+1])
            ) {
                endPos++;
            }
            if(endPos==dollarPos) {
                FatalErrorInFunction
                    << "Not a valid character after the $ in " <<result
                    << "(originally " << orig << ")"
                    << endl
                    << exit(FatalError);
            }
        }

        Sbug << "DollarPos: " << label(dollarPos)
            << " endPos " << label(endPos) << endl;
        string replace=result.substr(dollarPos+1,endPos-dollarPos);
        string replacement="";

        if(replace[0]=='[') {
            string castTo="";
            string entryName="";
            if(replace[1]=='(') {
                std::string::size_type closePos=replace.find(')');
                if(closePos==std::string::npos) {
                    FatalErrorInFunction
                        << "No closing ')' found in " << replace
                        << " (" << orig << ")"
                        << endl
                        << exit(FatalError);
                }
                castTo=replace.substr(2,closePos-2);
                entryName=replace.substr(closePos+1,replace.length()-closePos-2);
            } else {
                entryName=replace.substr(1,replace.length()-2);
            }
            if(castTo=="") {
                replacement=getEntryString(
                    dict,
                    entryName
                );
            } else {
                autoPtr<entryToExpression> e2e=entryToExpression::New(castTo);
                const entry &e=dict.lookupEntry(
                    entryName,
                    true,
                    true
                );
                replacement=e2e->toExpr(e);
            }
        } else {
            replacement=getEntryString(
                dict,
                replace
            );
        }
        Sbug << replace << " -> " << replacement << endl;

        string pre=result.substr(0,dollarPos);
        string post=result.substr(endPos+1);

        Sbug << "Pre: " << pre << " Post: " << post << endl;
        result= pre
            + replacement
            + post;
        Sbug << "Resulting: " << result << endl;
    }

    Sbug << orig << " expanded to " << result << endl;

    exprString ret(result.c_str());

    return ret;
}

exprString CommonValueExpressionDriver::readExpression(
    const word &name,
    const dictionary &dict
) {
    Sbug << "::readExpression " << name << endl;

    string result=dict.lookup(name);

    return expandDictVariables(
        result,
        dict
    );
}


exprString CommonValueExpressionDriver::readExpression(
    const word &name
) {
    Dbug << "::readExpression " << name << endl;

    return readExpression(
        name,
        dict()
    );
}

void CommonValueExpressionDriver::setVariableStrings(const dictionary &dict)
{
    variableStrings_=readVariableStrings(dict);
}

Ostream &CommonValueExpressionDriver::writeVariableStrings(Ostream &out) const
{
    if(variableStrings_.size()==0) {
        out << exprString("");
    } else if(variableStrings_.size()==1) {
        out << variableStrings_[0];
    } else {
        out << variableStrings_;
    }

    return out;
}

Ostream &CommonValueExpressionDriver::writeCommon(Ostream &os,bool debug) const
{
    os.writeKeyword("variables");
    writeVariableStrings(os) << token::END_STATEMENT << nl;

    os.writeKeyword("timelines");
    writeTables(os,lines_);
    os << token::END_STATEMENT << nl;

    os.writeKeyword("lookuptables");
    writeTables(os,lookup_);
    os << token::END_STATEMENT << nl;

    if(debug) {
        os.writeKeyword("variableValues");
        os << variables_ << endl;
        os << token::END_STATEMENT << nl;
    }

    if(
        storedVariables_.size()>0
        ||
        delayedVariables_.size()>0
    ) {
        const_cast<CommonValueExpressionDriver&>(
            *this
        ).updateSpecialVariables(true);
    }

    if(storedVariables_.size()>0) {
        os.writeKeyword("storedVariables");
        os << storedVariables_ << endl;
        os << token::END_STATEMENT << nl;
    }

    if(delayedVariables_.size()>0) {
        List<DelayedExpressionResult> writeDelays(delayedVariables_.size());

        label cnt=0;
        typedef HashTable<DelayedExpressionResult,word> tableType;
        forAllConstIter(tableType,delayedVariables_,iter)
        {
            writeDelays[cnt]=iter();
            cnt++;
        }

        os.writeKeyword("delayedVariables");
        os << writeDelays << endl;
        os << token::END_STATEMENT << nl;
    }

    if(globalVariableScopes_.size()>0) {
        os.writeKeyword("globalScopes");
        os << globalVariableScopes_ << endl;
        os << token::END_STATEMENT << nl;
    }

    if(aliases_.size()>0) {
        os.writeKeyword("aliases");
        os << token::BEGIN_BLOCK << endl;
        wordList toc(aliases_.toc());
        forAll(toc,i) {
            os.writeKeyword(toc[i]);
            os << aliases_[toc[i]] << token::END_STATEMENT << nl;
        }
        os << token::END_BLOCK << endl;
    }
    return os;
}

word CommonValueExpressionDriver::getResultType()
{
    return result_.valueType();
}

void CommonValueExpressionDriver::clearResult()
{
    result_.clearResult();
}

tmp<vectorField> CommonValueExpressionDriver::composeVectorField(
    const scalarField &x,
    const scalarField &y,
    const scalarField &z
)
{
    if(
        x.size() != y.size()
        ||
        x.size() != z.size()
    ) {
        FatalErrorInFunction
            << "Context: " << contextString() << endl
            << "Sizes " << x.size() << " " << y.size() << " "
            << z-size() << " of the components do not agree"
            << endl
            << abort(FatalError);
    }

    tmp<vectorField> result(
        new vectorField(x.size())
    );

    forAll(result(),faceI) {
        result()[faceI]=CML::vector(x[faceI],y[faceI],z[faceI]);
    }

    return result;
}

tmp<tensorField> CommonValueExpressionDriver::composeTensorField(
    const scalarField &xx,const scalarField &xy,const scalarField &xz,
    const scalarField &yx,const scalarField &yy,const scalarField &yz,
    const scalarField &zx,const scalarField &zy,const scalarField &zz
)
{
    if(
        xx.size() != xy.size()
        ||
        xx.size() != xz.size()
        ||
        xx.size() != yx.size()
        ||
        xx.size() != yy.size()
        ||
        xx.size() != yz.size()
        ||
        xx.size() != zx.size()
        ||
        xx.size() != zy.size()
        ||
        xx.size() != zz.size()
    ) {
        FatalErrorInFunction
            << "Context: " << contextString() << endl
            << "Sizes of the components do not agree"
            << endl
            << abort(FatalError);
    }

    tmp<tensorField> result(
        new tensorField(xx.size())
    );

    forAll(result(),faceI) {
        result()[faceI]=CML::tensor(
            xx[faceI],xy[faceI],xz[faceI],
            yx[faceI],yy[faceI],yz[faceI],
            zx[faceI],zy[faceI],zz[faceI]
        );

    }

    return result;
}

tmp<symmTensorField> CommonValueExpressionDriver::composeSymmTensorField(
    const scalarField &xx,const scalarField &xy,const scalarField &xz,
    const scalarField &yy,const scalarField &yz,
    const scalarField &zz
)
{
    if(
        xx.size() != xy.size()
        ||
        xx.size() != xz.size()
        ||
        xx.size() != yy.size()
        ||
        xx.size() != yz.size()
        ||
        xx.size() != zz.size()
    ) {
        FatalErrorInFunction
            << "Context: " << contextString() << endl
            << "Sizes of the components do not agree"
            << endl
            << abort(FatalError);
    }

    tmp<symmTensorField> result(
        new symmTensorField(xx.size())
    );

    forAll(result(),faceI) {
        result()[faceI]=CML::symmTensor(
            xx[faceI],xy[faceI],xz[faceI],
            yy[faceI],yz[faceI],
            zz[faceI]
        );

    }

    return result;
}

tmp<sphericalTensorField> CommonValueExpressionDriver::composeSphericalTensorField(
    const scalarField &ii
)
{
    tmp<sphericalTensorField> result(
        new sphericalTensorField(ii.size())
    );

    forAll(result(),faceI) {
        result()[faceI]=CML::sphericalTensor(
            ii[faceI]
        );

    }

    return result;
}

    /* void CommonValueExpressionDriver::parse (const std::string& f)
{
    content_ = f;
    scan_begin ();
    parserCommon::CommonValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing);
    parser.parse ();
    scan_end ();
}
        */

ExpressionResult CommonValueExpressionDriver::getUniform(
    label size,
    bool noWarning
)
{
    return result_.getUniform(size,noWarning);
}

void CommonValueExpressionDriver::error (const std::string& m)
{
    FatalErrorInFunction
        << " Parser Error: " << m
        << CML::exit(CML::FatalError);
}

const Time &CommonValueExpressionDriver::runTime() const
{
    return this->mesh().time();
}

tmp<scalarField> CommonValueExpressionDriver::makeModuloField(
    const scalarField &a,
    const scalarField &b
) const
{
    assert(a.size()==b.size());

    tmp<scalarField> result(
        new scalarField(this->size())
    );

    forAll(result(),i) {
        scalar val=fmod(a[i],b[i]);
        if(fabs(val)>(b[i]/2)) {
            if(val>0) {
                val -= b[i];
            } else {
                val += b[i];
            }
        }
        result()[i]=val;
    }

    return result;
}

tmp<scalarField> CommonValueExpressionDriver::makeRandomField(label seed) const
{
    tmp<scalarField> result(
        new scalarField(this->size())
    );

    if(seed<=0) {
        seed=runTime().timeIndex()-seed;
    }

    CML::Random rnd(seed);
    forAll(result(),i) {
        const_cast<scalar&>(result()[i]) = rnd.sample01<scalar>();
    }

    return result;
}

tmp<scalarField> CommonValueExpressionDriver::getLine(
    const word &name,
    scalar t
)
{
    return tmp<scalarField>(
        new scalarField(this->size(),lines_[name](t))
    );
}

tmp<scalarField> CommonValueExpressionDriver::getLookup(
    const word &name,
    const scalarField &val
)
{
    tmp<scalarField> result(
        new scalarField(val.size())
    );
    const interpolationTable<scalar> &table=lookup_[name];

    forAll(val,i) {
        result()[i]=table(val[i]);
    }

    return tmp<scalarField>(result);
}

scalar CommonValueExpressionDriver::getLineValue(const word &name,scalar t)
{
    return lines_[name](t);
}

tmp<scalarField> CommonValueExpressionDriver::makeGaussRandomField(
    label seed
) const
{
    tmp<scalarField> result(
        new scalarField(this->size())
    );

    if(seed<=0) {
        seed=runTime().timeIndex()-seed;
    }

    CML::Random rnd(seed);
    forAll(result(),i) {
        const_cast<scalar&>(result()[i]) = rnd.sampleNormal<scalar>();
    }

    return result;
}

bool CommonValueExpressionDriver::update()
{
    return true;
}

void CommonValueExpressionDriver::updateSpecialVariables(bool force)
{
    if(debug) {
        Info << "CommonValueExpressionDriver::updateSpecialVariables(bool force)"
            << " Force: " << force << endl;
    }
    bool updated=this->update();
    if(debug) {
        Info << "Updated: " << updated << endl;
    }

    if(specialVariablesIndex_<0) {
        if(debug) {
            Pout << "First update: " << mesh().time().timeIndex() << endl;
        }
        specialVariablesIndex_=mesh().time().timeIndex();
        forAll(storedVariables_,i) {
            StoredExpressionResult &v=storedVariables_[i];
            if(!v.hasValue()) {
                if(debug) {
                    Pout << "First value: " << v.initialValueExpression()
                        << " -> " << v.name() << endl;
                }
                parse(v.initialValueExpression());
                v=result_;
                if(debug) {
                    Info << "Parser size: " << this->size() << endl;
                    Info << "Calculated: " << result_ << endl;
                    Info << "Stored: " << v << endl;
                }
            }
        }
    }

    if(
        force
        ||
        specialVariablesIndex_!=mesh().time().timeIndex()
    ) {
        if(debug) {
            Pout << "Store variables: " << force << " "
                << specialVariablesIndex_ << " "
                << mesh().time().timeIndex() << endl;
        }
        forAll(storedVariables_,i) {
            StoredExpressionResult &v=storedVariables_[i];
            if(variables_.found(v.name())) {
                if(debug) {
                    Pout << "Storing variable: " << v.name() << " "
                        << variables_[v.name()] << endl;
                }
                v=variables_[v.name()];
            }
        }
        specialVariablesIndex_=mesh().time().timeIndex();
    }

    typedef HashTable<DelayedExpressionResult,word> tableType;
    forAllIter(tableType,delayedVariables_,iter)
    {
        if(debug) {
            Pout << "Updating delayed variable " << iter().name() << endl;
        }
        if(!iter().updateReadValue(mesh().time().value())) {
            const exprString &expr=iter().startupValueExpression();
            if(debug) {
                Pout << "Evaluate: " << expr << endl;
            }
            parse(expr);
            iter().setReadValue(result_);
            if(debug) {
                Pout << "Value " << iter() << endl;
                Pout << "Type " << iter().valueType() << "("
                    << result_.valueType() << ")" << endl;

            }
        } else {
            if(debug) {
                Pout << iter().name() << " updated without problem" << endl;
            }
        }
    }
}

void CommonValueExpressionDriver::clearVariables()
{
    if(debug) {
        Pout << "Clearing variables" << endl;
    }

    this->update();

    updateSpecialVariables();
    variables_.clear();
    forAll(storedVariables_,i) {
        StoredExpressionResult &v=storedVariables_[i];
        variables_.insert(v.name(),v);
    }

    addVariables(variableStrings_,false);

    typedef HashTable<DelayedExpressionResult,word> tableType;
    forAllIter(tableType,delayedVariables_,iter)
    {
        iter().storeValue(mesh().time().value());
    }
}

void CommonValueExpressionDriver::evaluateVariable(
    const word &name,
    const exprString &expr
)
{
    if(
        mesh().foundObject<regIOobject>(name)
        &&
        !variableNameIdenticalToField_
    ) {
        const regIOobject &ob=mesh().lookupObject<regIOobject>(name);

        WarningInFunction
            << "Context: " << contextString() << endl
            << "There is a field named " << name << " of type "
            << ob.headerClassName() << " found which may be shadowed "
            << "by the variable of the same name." << nl
            << "This may lead to trouble" << nl
            << "If this is OK set 'variableNameIdenticalToField'"
            << " in the relevant parser" << nl
            << endl;

    }

    parse(expr);

    if(debug) {
        Pout << "Evaluating: " << expr << " -> " << name << endl;
        Pout << result_;
    }

    if(delayedVariables_.found(name)) {
        if(debug) {
            Pout << name << " is delayed" << endl;
        }
        delayedVariables_[name]=result_;
    } else {
        variables_.set(name,ExpressionResult(result_));
    }
    if(debug>1) {
        Pout << "Value stored: " << variables_[name] << endl;
    }
}

void CommonValueExpressionDriver::evaluateVariableRemote(
    const exprString &remoteExpr,
    const word &name,
    const exprString &expr
)
{
    if(debug) {
        Pout << "Evaluating remote " << remoteExpr
            << " : " << expr << " -> " << name << endl;
    }

    exprString remote=remoteExpr;
    word regionName="";
    word id="";
    word type="patch";

    std::string::size_type slashPos=remote.find('/');

    if(slashPos!=std::string::npos) {
        regionName=string::validate<word>(
            remote.substr(slashPos+1)
        );
        remote=remote.substr(0,slashPos);
    }

    std::string::size_type quotePos=remote.find('\'');
    if(quotePos!=std::string::npos) {
        id=string::validate<word>(
            remote.substr(quotePos+1)
        );
        type=string::validate<word>(
            remote.substr(0,quotePos)
        );
    } else {
        id=string::validate<word>(
            remote
        );
    }

    if(
        type=="patch"
        &&
        (id=="internalField" || id=="")
    ) {
        type="internalField";
    }

    const fvMesh *pRegion=&(this->mesh());

    if(regionName!="") {
        //        pRegion=&(dynamicCast<const fvMesh&>( // doesn't work with gcc 4.2
        pRegion=&(dynamic_cast<const fvMesh&>(
                      pRegion->time().lookupObject<objectRegistry>(regionName))
        );
    }

    const fvMesh &region=*pRegion;

    autoPtr<CommonValueExpressionDriver> otherDriver=
        CommonValueExpressionDriver::New(
            type,
            id,
            region
        );

    otherDriver->setSearchBehaviour(
        this->cacheReadFields(),
        this->searchInMemory(),
        this->searchOnDisc()
    );
    otherDriver->setGlobalScopes(
        this->globalVariableScopes_
    );
    otherDriver->setAliases(
        this->aliases()
    );

    otherDriver->parse(expr);

    autoPtr<ExpressionResult> otherResult(this->getRemoteResult(otherDriver()));

    if(debug) {
        Pout << "Remote result: "
            << otherResult() << endl;
    }

    if(delayedVariables_.found(name)) {
        if(debug) {
            Pout << name << " is delayed - setting" << endl;
        }
        delayedVariables_[name]=otherResult();
    } else {
        variables_.insert(name,otherResult());
    }
}

autoPtr<ExpressionResult> CommonValueExpressionDriver::getRemoteResult(
        CommonValueExpressionDriver &otherDriver
)
{
    return autoPtr<ExpressionResult>(
        new ExpressionResult(
            otherDriver.getUniform(this->size(),false)
        )
    );
}

void CommonValueExpressionDriver::addVariables(
    const List<exprString> &exprList,
    bool clear
)
{
    if(clear) {
        clearVariables();
    }
    forAll(exprList,i) {
        addVariables(exprList[i],false);
    }
}

void CommonValueExpressionDriver::addVariables(
    const exprString &exprListIn,
    bool clear
)
{
    exprString exprList(exprListIn);
    exprList.removeTrailing(' ');

    if(clear) {
        clearVariables();
    }

    std::string::size_type start=0;
    std::string::size_type end=0;

    while(start<exprList.length()) {
        end=exprList.find(';',start);
        if(end==std::string::npos) {
            FatalErrorInFunction
                << "Context: " << contextString() << endl
                << "No terminating ';' found in expression '"
                << exprList.substr(start) << "'\n"
                << endl
                << exit(FatalError);
        }
        std::string::size_type  eqPos=exprList.find('=',start);
        if(eqPos==std::string::npos || eqPos > end) {
            FatalErrorInFunction
                << "Context: " << contextString() << endl
                << "No '=' found in expression '"
                << exprList.substr(start,end-start) << "'\n"
                << endl
                << exit(FatalError);
        }
        exprString expr(
            exprString::toExpr(
                exprList.substr(eqPos+1,end-eqPos-1)
            )
        );

        std::string::size_type  startPos=exprList.find('{',start);
        if(startPos!=std::string::npos && startPos<eqPos) {
            std::string::size_type  endPos=exprList.find('}',start);
            if(endPos>=eqPos) {
                FatalErrorInFunction
                    << "Context: " << contextString() << endl
                    << "No closing '}' found in "
                    << exprList.substr(start,eqPos-start)
                    << endl
                    << exit(FatalError);
            }
            word name(
                string::validate<word>(
                    exprList.substr(start,startPos-start)
                )
            );
            exprString remoteExpr(
                exprString::toExpr(
                    exprList.substr(startPos+1,endPos-startPos-1)
                )
            );
            evaluateVariableRemote(remoteExpr,name,expr);
        } else {
            word name(exprList.substr(start,eqPos-start));
            evaluateVariable(name,expr);
        }
        start=end+1;
    }
}

void CommonValueExpressionDriver::readTables(
    Istream &is,
    HashTable<interpolationTable<scalar> > &tables,
    bool clear
)
{
    if(clear) {
        tables.clear();
    }
    List<dictionary> lines(is);

    forAll(lines,i) {
        const dictionary &dict=lines[i];
        tables.insert(dict.lookup("name"),interpolationTable<scalar>(dict));
    }
}

void CommonValueExpressionDriver::writeTables(
    Ostream &os,
    const HashTable<interpolationTable<scalar> > &tables
) const
{
    os << token::BEGIN_LIST << nl;
    forAllConstIter(HashTable<interpolationTable<scalar> >,tables,it) {
        os << token::BEGIN_BLOCK << nl;
        os.writeKeyword("name") << it.key() << token::END_STATEMENT << nl;
        (*it).write(os);
        os << token::END_BLOCK << nl;
    }
    os << token::END_LIST;
}

const fvMesh &CommonValueExpressionDriver::regionMesh
(
    const dictionary &dict,
    const fvMesh &mesh,
    bool readIfNecessary
)
{
    if(!dict.found("region")) {
        if(debug) {
            Pout << "Using original mesh " << endl;
        }

        return mesh;
    }

    if(debug) {
        Pout << "Using mesh " << dict.lookup("region")  << endl;
    }

    if(
        !mesh.time().foundObject<objectRegistry>(
            dict.lookup("region")
        )
        &&
        readIfNecessary
    ) {
        WarningInFunction
            << "Region " << dict.lookup("region")
            << " not in memory. Trying to register it"
            << endl;

        autoPtr<polyMesh> temporary(
            new fvMesh
            (
                IOobject(
                    word(dict.lookup("region")),
                    mesh.time().constant(),
                    mesh.time(),
                    IOobject::MUST_READ
                )
            )
        );
        //        Info << "Hepp: "<< temporary->polyMesh::ownedByRegistry() << endl;
        temporary->polyMesh::store(temporary.ptr());
    }

    //     return dynamicCast<const fvMesh&>( // soesn't work with gcc 3.2
    return dynamic_cast<const fvMesh&>(
        mesh.time().lookupObject<objectRegistry>(
            dict.lookup("region")
        )
    );
}

word CommonValueExpressionDriver::getTypeOfField(const word &name) const
{
    return getTypeOfFieldInternal(mesh(),name);
}

word CommonValueExpressionDriver::getTypeOfFieldInternal(
    const fvMesh &theMesh,
    const word &name
) const
{
    IOobject f
        (
            name,
            theMesh.time().timeName(),
            theMesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
    f.headerOk();

    if(debug) {
        Pout<< "Mesh: " << theMesh.polyMesh::path()
            << " Name: " << name << " Time: " << mesh().time().timeName()
            << " Path: " << f.filePath() << " Class: "
            << f.headerClassName() << endl;
    }

    return f.headerClassName();
}

word CommonValueExpressionDriver::getTypeOfSet(const word &inName) const
{
    word name(inName);
    if(this->hasAlias(name)) {
        if(debug) {
            Pout << "CommonValueExpressionDriver::getTypeOfSet. Name: " << name
                << " is an alias for " << this->getAlias(name) << endl;
        }
        name=this->getAlias(name);
    }

    if(debug) {
        Pout << "Looking for set named " << name << endl;
    }
    IOobject f
        (
            name,
            mesh().time().timeName(),
            polyMesh::meshSubDir/"sets",
	    mesh(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );

    if(f.headerOk()) {;
        return f.headerClassName();
    } else {
        Pout << "No set " << name << " at t=" << mesh().time().timeName()
            << " falling back to 'constant'" << endl;
        f=IOobject
        (
            name,
            "constant",
            polyMesh::meshSubDir/"sets",
	    mesh(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
        f.headerOk();
        return f.headerClassName();
    }
}

bool CommonValueExpressionDriver::isCellSet(const word &name)
{
    if(getTypeOfSet(name)=="cellSet") {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isCellZone(const word &name)
{
    if(mesh().cellZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isFaceSet(const word &name)
{
    if(getTypeOfSet(name)=="faceSet") {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isFaceZone(const word &name)
{
    if(mesh().faceZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isPointSet(const word &name)
{
    if(getTypeOfSet(name)=="pointSet") {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isPointZone(const word &name)
{
    if(mesh().pointZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

void CommonValueExpressionDriver::setTrace(
    bool scanning,
    bool parsing
)
{
    trace_parsing_=parsing;
    trace_scanning_=scanning;
}

label CommonValueExpressionDriver::parse(
    const exprString &f,
    const word &start
)
{
    Dbug << "::parse() - expr: " << f << " start: " << start << endl;

    int start_token=startupSymbol(start);

    parserLastPos()=-1;

    content_ = f;
    scan_begin ();

    Dbug << "Start parsing" << endl;
    parseInternal(start_token);
    Dbug << "Parsed to " << parserLastPos() << " of " << label(f.size()) << endl;

    scan_end ();
    Dbug << "Ended at position " << parserLastPos() << endl;

    return parserLastPos();
}


void CommonValueExpressionDriver::outputResult(Ostream &o)
{
    word rType=getResultType();

    if(rType==pTraits<scalar>::typeName) {
        o << getResult<scalar>();
    } else if(rType==pTraits<vector>::typeName) {
        o << getResult<vector>();
    } else if(rType==pTraits<tensor>::typeName) {
        o << getResult<tensor>();
    } else if(rType==pTraits<symmTensor>::typeName) {
        o << getResult<symmTensor>();
    } else if(rType==pTraits<sphericalTensor>::typeName) {
        o << getResult<sphericalTensor>();
    } else {
        o << "No implementation for " << rType;
    }
}

string CommonValueExpressionDriver::outputEntry()
{
    OStringStream o;

    word rType=getResultType();
    if(rType==pTraits<scalar>::typeName) {
        result_.getResult<scalar>(true)().writeEntry("",o);
    } else if(rType==pTraits<vector>::typeName) {
        result_.getResult<vector>(true)().writeEntry("",o);
    } else if(rType==pTraits<tensor>::typeName) {
        result_.getResult<tensor>(true)().writeEntry("",o);
    } else if(rType==pTraits<symmTensor>::typeName) {
        result_.getResult<symmTensor>(true)().writeEntry("",o);
    } else if(rType==pTraits<sphericalTensor>::typeName) {
        result_.getResult<sphericalTensor>(true)().writeEntry("",o);
    } else {
        o << "No implementation for " << rType << ";";
    }

    return o.str();
}

const word CommonValueExpressionDriver::time() const
{
    return this->mesh().time().timeName();
}

const ExpressionResult &CommonValueExpressionDriver::lookupGlobal(
    const word &name
) const
{
    const ExpressionResult &result(
        GlobalVariablesRepository::getGlobalVariables(
            this->mesh()
        ).get(
            name,
            globalVariableScopes_
        )
    );

    // Pout << name << " Size: " << result.size()
    //     << " Single: " << result.isSingleValue() << endl;

    return result;
}

void CommonValueExpressionDriver::setGlobalScopes(const wordList &other)
{
    globalVariableScopes_=other;
}

void CommonValueExpressionDriver::createWriterAndRead(const word &name)
{
    if(
        hasDataToWrite()
        &&
        !writer_.valid()
    ) {
        writer_.set(
            new ExpressionDriverWriter(
                name+"_"+this->type(),
                *this
            )
        );
    }
}

void CommonValueExpressionDriver::tryWrite() const
{
    if(
        writer_.valid()
        &&
        mesh().time().outputTime()
    ) {
        writer_->write();
    }
}

bool CommonValueExpressionDriver::hasDataToWrite() const
{
    if(
        storedVariables_.size()>0
        ||
        delayedVariables_.size()>0
    ) {
        return true;
    }

    return false;
}

void CommonValueExpressionDriver::getData(const dictionary &dict)
{
    if(debug) {
        Info << "CommonValueExpressionDriver::getData(const dictionary &dict)" << endl;
    }

    if(dict.found("storedVariables")) {
        storedVariables_=List<StoredExpressionResult>(
            dict.lookup("storedVariables")
        );
    }
}

void CommonValueExpressionDriver::prepareData(dictionary &dict) const
{
    if(debug) {
        Info << "CommonValueExpressionDriver::prepareData(dictionary &dict)" << endl;
    }
    bool updated=const_cast<CommonValueExpressionDriver&>(*this).update();
    if(debug && updated) {
        Info << "Updated before write" << endl;
    }

    if(storedVariables_.size()>0) {
        const_cast<CommonValueExpressionDriver&>(
            *this
        ).updateSpecialVariables(true);

        dict.add(
            "storedVariables",
            storedVariables_
        );
    }
}

class littlerOp {
public:
    bool operator()(scalar a,scalar b) {
        return a<b;
    }
};

class biggerOp {
public:
    bool operator()(scalar a,scalar b) {
        return a>b;
    }
};


template<class Op>
class  extremeOp {
    Op &op;
public:
    extremeOp(Op &op)
        : op(op)
        {}
    Tuple2<scalar,vector> operator()(
        const Tuple2<scalar,vector> &a,
        const Tuple2<scalar,vector> &b
    ) const {
        if(op(a.first(),b.first())) {
            return a;
        } else {
            return b;
        }
    }
};

template<class Op>
vector getExtremePosition(
    Op op,
    const scalarField &vals,
    const vectorField &locs
) {
    assert(vals.size()==locs.size());

    vector pos(HUGE_VAL,HUGE_VAL,HUGE_VAL);
    scalar val=op(1,-1) ? -HUGE_VAL : HUGE_VAL;
    forAll(vals,i) {
        if(op(vals[i],val)) {
            val=vals[i];
            pos=locs[i];
        }
    }
    if(Pstream::parRun()) {
        Tuple2<scalar,vector> info(val,pos);

        reduce(info,extremeOp<Op>(op));

        pos=info.second();
    }
    return pos;
}

vector CommonValueExpressionDriver::getPositionOfMinimum(
    const scalarField &vals,
    const vectorField &locs
) const
{
    return getExtremePosition(littlerOp(),vals,locs);
}

vector CommonValueExpressionDriver::getPositionOfMaximum(
    const scalarField &vals,
    const vectorField &locs
) const
{
    return getExtremePosition(biggerOp(),vals,locs);

}

std::string CommonValueExpressionDriver::getContextString()
{
    if(contextString_=="") {
        return contextString_;
    }

    std::string result("\nContext of the error:\n\n");
    result+="\n"+contextString_;
    result+="\n  Evaluating expression \""+content()+"\"\n";

    return result;
}

void CommonValueExpressionDriver::appendToContext(const string &value)
{
    contextString_=value+"\n"+contextString_;
}

// ************************************************************************* //

const ExpressionResult &CommonValueExpressionDriver::variable(
    const word &name
) const
{
    if(delayedVariables_.found(name)) {
        return delayedVariables_[name];
    } else {
        return variables_[name];
    }
}

ExpressionResult &CommonValueExpressionDriver::variable(
    const word &name
)
{
    if(delayedVariables_.found(name)) {
        return delayedVariables_[name];
    } else {
        return variables_[name];
    }
}

bool CommonValueExpressionDriver::hasVariable(
    const word &name
) const
{
    if(debug) {
        Pout << "(var:" << delayedVariables_.found(name)
            << " " << variables_.found(name) << ")";
    }
    if(delayedVariables_.found(name)) {
        return true;
    } else {
        return variables_.found(name);
    }
}

bool CommonValueExpressionDriver::isForeignMesh(
    const word &name
) const
{
    return MeshesRepository::getRepository().hasMesh(name);
}

tmp<scalarField> CommonValueExpressionDriver::weights(
        label size,
        bool point
    ) const
{
    if(point) {
        const label pSize=this->pointSize();
        bool isCorrect=(size==pSize);
        reduce(isCorrect,andOp<bool>());
        if(!isCorrect) {
            Pout << "Expected Size: " << size << " PointSize:" << pSize << endl;
            FatalErrorInFunction
                << "Context: " << contextString() << endl
                << "At least one processor wants the wrong field size. "
                << "Check above"
                << endl
                << exit(FatalError);
        }
        // points have weight 1 per default
        tmp<scalarField> result(
            new scalarField(
                size,
                1.
            )
        );
        return result;
    } else {
        return this->weightsNonPoint(size);
    }
}

bool CommonValueExpressionDriver::hasAlias(const word &name) const
{
    if(debug) {
        Info << "CommonValueExpressionDriver::hasAlias " << name
            << " : " << aliases_.found(name) << " of "
            << aliases_.size() << endl;
    }
    return aliases_.found(name);
}

const word &CommonValueExpressionDriver::getAlias(const word &name) const
{
    if(!aliases_.found(name)){
        FatalErrorInFunction
            << "Context: " << contextString() << endl
            << "No alias of name " << name << " found." << endl
            << "Available aliases are " << aliases_.toc()
            << endl
            << exit(FatalError);
        return word::null;

    } else {
        return aliases_[name];
    }
}

const word &CommonValueExpressionDriver::resolveAlias(const word &name) const {
    if(hasAlias(name)) {
        return getAlias(name);
    }
    return name;
}

} // namespace
