//%//-*-c++-*-//////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationContext_h
#define Pegasus_OperationContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

static const Uint32 CONTEXT_EMPTY =                     0;
static const Uint32 CONTEXT_IDENTITY =                  1;
static const Uint32 CONTEXT_AUTHENICATION =             2;
static const Uint32 CONTEXT_AUTHORIZATION =             3;
static const Uint32 CONTEXT_OTHER_SECURITY =            4;
static const Uint32 CONTEXT_LOCALE =                    5;
static const Uint32 CONTEXT_OPTIONS =                   6;
static const Uint32 CONTEXT_VENDOR =                    7;
static const Uint32 CONTEXT_UID_PRESENT =               8;
static const Uint32 CONTEXT_UINT32_PRESENT =            9;
static const Uint32 CONTEXT_OTHER =                     10;
static const Uint32 CONTEXT_COPY_MEMORY =               11;
static const Uint32 CONTEXT_DELETE_MEMORY =             12;
static const Uint32 CONTEXT_POINTER =                   13;
static const Uint32 CONTEXT_PROVIDERID =                14;

static const Uint32 OPERATION_NONE =                    0x00000000;
static const Uint32 OPERATION_LOCAL_ONLY =              0x00000001;
static const Uint32 OPERATION_INCLUDE_QUALIFIERS =      0x00000002;
static const Uint32 OPERATION_INCLUDE_CLASS_ORIGIN =    0x00000004;
static const Uint32 OPERATION_DEEP_INHERITANCE =        0x00000008;
static const Uint32 OPERATION_PARTIAL_INSTANCE =        0x00000010;
static const Uint32 OPERATION_REMOTE_ONLY =             0x00000020;
static const Uint32 OPERATION_DELIVER =                 0x00000040;
static const Uint32 OPERATION_RESERVE =                 0x00000080;
static const Uint32 OPERATION_PROCESSING =              0x00000100;
static const Uint32 OPERATION_COMPLETE =                0x00000200;

class OperationContextRep;

/**
    This class represents a set of arbitrary information (stored in containers)
    associated with a CIM operation request. As an example, each operation may
    have an associated locale that can be represented as a container in the
    context object.
*/
class PEGASUS_COMMON_LINKAGE OperationContext
{
public:
    class PEGASUS_COMMON_LINKAGE Container
    {
    public:
        ///
        Container(const Uint32 key = CONTEXT_EMPTY);

        virtual ~Container(void);

        ///
        const Uint32 & getKey(void) const;

        // Caller is responsible for deleting dynamically allocated memory.
        virtual Container * clone(void) const;

    protected:
        Uint32 _key;

    };

public:
    ///
    OperationContext(void);

    ///
    OperationContext(const OperationContext & context);

    ///
    virtual ~OperationContext(void);

    OperationContext & operator=(const OperationContext & context);

    /**	
        clear - Removes all containers in the current object.
	
        @param none
	    @return none
	    @exception none
    */
    void clear(void);

    ///
    const Container & get(const Uint32 key) const;

    ///
    void set(const Container & container);

    ///
    void insert(const Container & container);

    ///
    void remove(const Uint32 key);

protected:
    OperationContextRep* _rep;

};


class PEGASUS_COMMON_LINKAGE IdentityContainer : public OperationContext::Container
{
public:
    IdentityContainer(const OperationContext::Container & container);
    IdentityContainer(const String & userName);

    virtual OperationContext::Container * clone(void) const;

    String getUserName(void) const;

protected:
    String _userName;

};

class PEGASUS_COMMON_LINKAGE LocaleContainer : public OperationContext::Container
{
public:
    LocaleContainer(const OperationContext::Container & container);
    LocaleContainer(const String & languageId);

    virtual OperationContext::Container * clone(void) const;

    String getLanguageId(void) const;

protected:
    String _languageId;

};

class PEGASUS_COMMON_LINKAGE ProviderIdContainer : public OperationContext::Container
{
public:
    ProviderIdContainer(const OperationContext::Container & container);
    ProviderIdContainer(const CIMInstance & module, const CIMInstance & provider);

    virtual OperationContext::Container * clone(void) const;

    CIMInstance getModule(void) const;
    CIMInstance getProvider(void) const;

protected:
    CIMInstance _module;
    CIMInstance _provider;

};

PEGASUS_NAMESPACE_END

#endif
