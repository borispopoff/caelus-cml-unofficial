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

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "surfaceMesh.hpp"
#include "fvsPatchField.hpp"
#include "pointPatchField.hpp"
#include "primitivePatchInterpolation.hpp"

#include "swakVersion.hpp"
#include "DebugOStream.hpp"

#include "processorPolyPatch.hpp"

#include <cassert>

namespace CML {

    template<class FieldType,class Subset,class T>
    tmp<Field<T>> SubsetValueExpressionDriver::getFieldInternal(
        const word &inName,
        const Subset &sub,
        bool oldTime
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "SubsetValueExpressionDriver::getFieldInternal. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        if(debug) {
            Pout << "SubsetValueExpressionDriver::getFieldInternal"
                << " searching for " << name << " Type: " << Subset::typeName
                << endl;
        }

        if(
            this->hasVariable(name)
            &&
            this->variable(name).valueType()==pTraits<T>::typeName
        ) {
            if(debug) {
                Pout << name << " is a variable" << endl;
            }
            return this->variable(name).getResult<T>(true);
        }

        tmp<Field<T>> result(new Field<T>(this->size()));
        label cnt=0;

        if(debug) {
            Pout << "searchInMemory: " << this->searchInMemory()
                << " found: " << this->mesh().foundObject<FieldType>(name)
                << endl;
        }

        if(this->searchInMemory() && this->mesh().foundObject<FieldType>(name)) {
            if(debug) {
                Pout << name << " was found in memory" << endl;
            }
            // Keep this separate to avoid copying if found in memory
            if(!oldTime) {
                const FieldType &f=this->mesh().lookupObject<FieldType>(name);

                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f[getIndexFromIterator(iter)];
                    cnt++;
                }
            } else {
                const FieldType &f=this->mesh().lookupObject<FieldType>(name).oldTime();

                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f[getIndexFromIterator(iter)];
                    cnt++;
                }
            }
        } else {
            if(debug) {
                Pout << name << " was found on disc" << endl;
            }
            // otherwise try disc and memory
            autoPtr<FieldType> f=this->getOrReadField<FieldType>(
                name,
                true, // fail if not found
                oldTime
            );

            if(!oldTime) {
                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f()[getIndexFromIterator(iter)];
                    cnt++;
                }
            } else {
                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f->oldTime()[getIndexFromIterator(iter)];
                    cnt++;
                }
            }
        }
        assert(cnt==this->size());

        return result;
    }

    template<class FieldType,class IFieldType,class Subset,class T>
    tmp<Field<T>> SubsetValueExpressionDriver::getFieldInternalAndInterpolate(
        const word &inName,
        const Subset &sub,
        bool oldTime
    )
    {
        // Yeah. I know. Code duplication without end

        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "SubsetValueExpressionDriver::getFieldInternalAndInterpolate. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }
        if(
            this->hasVariable(name)
            &&
            this->variable(name).valueType()==pTraits<T>::typeName
        ) {
            return this->variable(name).getResult<T>(true);
        }

        tmp<Field<T>> result(new Field<T>(this->size()));

        bool done=false;

        if(
            this->searchInMemory()
            &&
            this->mesh().foundObject<FieldType>(name)
        ) {
            const FieldType &f=this->mesh().lookupObject<FieldType>(name);
            label cnt=0;

            forAllConstIter(typename Subset,sub,iter) {
                const label index=getIndexFromIterator(iter);
                if(index<this->mesh().nInternalFaces()) {
                    if(!oldTime) {
                        result()[cnt]=f[index];
                    } else {
                        result()[cnt]=f.oldTime()[index];
                    }
                    cnt++;
                } else {
                    T value;
                    bool use;
                    if(!oldTime) {
                        use=getBoundaryFaceValue(f,index,value);
                    } else {
                        use=getBoundaryFaceValue(f.oldTime(),index,value);
                    }
                    if(use) {
                        result()[cnt]=value;
                        cnt++;
                    }
                }
            }

            assert(cnt==this->size());

            done=true;
        } else {
            autoPtr<FieldType> f=this->getOrReadField<FieldType>(
                name,
                false,
                oldTime
            );
            if(f.valid()) {
                label cnt=0;
                forAllConstIter(typename Subset,sub,iter) {
                    const label index=getIndexFromIterator(iter);
                    if(index<this->mesh().nInternalFaces()) {
                        if(!oldTime) {
                            result()[cnt]=f()[index];
                        } else {
                            result()[cnt]=f->oldTime()[index];
                        }
                        cnt++;
                    } else {
                        T value;
                        bool use;
                        if(!oldTime) {
                            use=getBoundaryFaceValue(f(),index,value);
                        } else {
                            use=getBoundaryFaceValue(f->oldTime(),index,value);
                        }
                        if(use) {
                            result()[cnt]=value;
                            cnt++;
                        }
                    }
                }
                done=true;

                assert(cnt==this->size());
            }
        }

        if(
            !done
            &&
            this->autoInterpolate_
        ) {
            if(
                this->warnAutoInterpolate_
            ) {
                WarningInFunction
                    << "Going to interpolate " << name
                    << endl;
            }

            const labelList &own=this->mesh().faceOwner();
            const labelList &nei=this->mesh().faceNeighbour();
            const surfaceScalarField &weights=this->mesh().weights();

            if(
                this->searchInMemory()
                &&
                this->mesh().foundObject<IFieldType>(name)
            ) {
                const IFieldType &f=this->mesh().lookupObject<IFieldType>(name);
                label cnt=0;

                forAllConstIter(typename Subset,sub,iter) {
                    label facei=getIndexFromIterator(iter);
                    if(facei<this->mesh().nInternalFaces()) {
                        if(!oldTime) {
                            result()[cnt]=
                                weights[facei]*f[own[facei]]
                                +
                                (1-weights[facei])*f[nei[facei]];
                        } else {
                            result()[cnt]=
                                weights[facei]*f.oldTime()[own[facei]]
                                +
                                (1-weights[facei])*f.oldTime()[nei[facei]];
                        }
                        cnt++;
                    } else {
                        T value;
                        bool use;
                        if(!oldTime) {
                            use=getBoundaryFaceValue(f,facei,value);
                        } else {
                            use=getBoundaryFaceValue(f.oldTime(),facei,value);
                        }
                        if(use) {
                            result()[cnt]=value;
                            cnt++;
                        }
                    }
                }

                assert(cnt==this->size());

                done=true;
            } else {
                autoPtr<IFieldType> f=this->getOrReadField<IFieldType>(
                    name,
                    false,
                    oldTime
                );

                if(f.valid()) {
                    label cnt=0;

                    forAllConstIter(typename Subset,sub,iter) {
                        label facei=getIndexFromIterator(iter);
                        if(facei<this->mesh().nInternalFaces()) {
                            if(!oldTime) {
                                result()[cnt]=
                                    weights[facei]*f()[own[facei]]
                                    +
                                    (1-weights[facei])*f()[nei[facei]];
                            } else {
                                result()[cnt]=
                                    weights[facei]*f->oldTime()[own[facei]]
                                    +
                                    (1-weights[facei])*f->oldTime()[nei[facei]];
                            }
                            cnt++;
                        } else {
                            T value;
                            bool use;
                            if(!oldTime) {
                                use=getBoundaryFaceValue(f(),facei,value);
                            } else {
                                use=getBoundaryFaceValue(f->oldTime(),facei,value);
                            }
                            if(use) {
                                result()[cnt]=value;
                                cnt++;
                            }
                        }
                    }

                    assert(cnt==this->size());

                    done=true;
                }
            }
        }

        if(!done) {
            FatalErrorInFunction
                << "Could not find a field name " << name
                << " of type " << pTraits<T>::typeName
                << " (neither " << FieldType::typeName
                << " nor " << IFieldType::typeName << ")"
                << " autoInterpolate: " << this->autoInterpolate_
                << (this->autoInterpolate_ ? "" : " (try setting 'autoInterpolate' to 'true')")
                << endl
                << exit(FatalError);
        }

        return result;
    }

    template<class Subset,class T>
    tmp<Field<T>> SubsetValueExpressionDriver::getFromFieldInternal(
        const Field<T> &original,
        const Subset &sub
    ) const
    {
        tmp<Field<T>> result(new Field<T>(this->size()));
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            result()[cnt]=original[getIndexFromIterator(iter)];
            cnt++;
        }

        assert(cnt==this->size());

        return result;
    }

    template<class Subset,class T>
    tmp<Field<T>> SubsetValueExpressionDriver::getFromSurfaceFieldInternal(
        const GeometricField<T,fvsPatchField,surfaceMesh> &original,
        const Subset &sub
    ) const
    {
        tmp<Field<T>> result(new Field<T>(this->size()));
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            label facei=getIndexFromIterator(iter);
            if(facei<this->mesh().nInternalFaces()) {
                result()[cnt]=original[facei];
                cnt++;
            } else {
                T value;
                bool use;
                use=getBoundaryFaceValue(original,facei,value);
                if(use) {
                    result()[cnt]=value;
                    cnt++;
                }
            }
        }

        assert(cnt==this->size());

        return result;
    }

    bool SubsetValueExpressionDriver::useFaceValue(
        const label facei
    ) const
    {
        if(facei<this->mesh().nInternalFaces()) {
            return true;
        } else {
            label patchI=this->mesh().boundaryMesh().whichPatch(facei);
            if(patchI<0) {
                FatalErrorInFunction
                    << "Can't find the correct patch for face " << facei
                    << endl
                    << exit(FatalError);
            }
            const polyPatch &patch=this->mesh().boundaryMesh()[patchI];
            if(!patch.coupled()) {
                return true;
            } else {
                if(isA<processorPolyPatch>(patch)) {
                    const processorPolyPatch &pp=dynamicCast<const processorPolyPatch&>(
                        patch
                    );
                    return pp.owner();
                } else {
                    return false;
                }
            }
        }
    }

    template<class FieldType,class T>
    bool SubsetValueExpressionDriver::getBoundaryFaceValue(
        const FieldType &field,
        const label index,
        T &value
    ) const
    {
        value=pTraits<T>::zero;

        if(index<this->mesh().nInternalFaces()) {
            FatalErrorInFunction
                << "Face with index " << index << " is an internal face (<"
                << this->mesh().nInternalFaces() << ")"
                << endl
                << exit(FatalError);
        }
        label patchI=this->mesh().boundaryMesh().whichPatch(index);

        if(useFaceValue(index)) {
            const polyPatch &patch=this->mesh().boundaryMesh()[patchI];
            const typename FieldType::PatchFieldType &bf=
                field.boundaryField()[patchI];
            value=bf[patch.whichFace(index)];
            return true;
        } else {
            return false;
        }
    }

    template<class Subset>
    label SubsetValueExpressionDriver::nrValidFaces(const Subset &sub) const
    {
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            const label facei=getIndexFromIterator(iter);
            if(useFaceValue(facei)) {
                cnt++;
            }
        }

        return cnt;
    }
}


// ************************************************************************* //
