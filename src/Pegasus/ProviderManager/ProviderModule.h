//%/////////////////////////////////////////////////////////////////////////////
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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderModule_h
#define Pegasus_ProviderModule_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>

#include <Pegasus/Provider/CIMBaseProvider.h>
#include <Pegasus/ProviderManager/ProviderAdapter.h>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

// The ProviderModule class represents the physical module, as defined by the
// operating, that contains a provider. This class effectively encapsulates the
// "physical" portion of a provider.
class PEGASUS_SERVER_LINKAGE ProviderModule
{
public:
    ProviderModule(const String & fileName, const String & providerName);
    ProviderModule(const String & fileName, const String & providerName,
                   const String & interfaceName);
    ProviderModule(const ProviderModule & pm);
    virtual ~ProviderModule(void);

    const String & getFileName(void) const;
    const String & getProviderName(void) const;

    const String & getInterfaceName(void) const;
    ProviderAdapter * getAdapter(void) const;

    void load(void);
    void unload(void);
    void unloadModule(void);

    virtual CIMBaseProvider * getProvider(void) const;

protected:
    String _fileName;
    String _providerName;

    String _interfaceName;
    String _interfaceFilename; // for later use with interface registration
    ProviderAdapter * _adapter;

    DynamicLibraryHandle _library;
    CIMBaseProvider * _provider;

};

inline const String & ProviderModule::getInterfaceName(void) const
{
    return _interfaceName;
}

inline ProviderAdapter * ProviderModule::getAdapter(void) const
{
    return _adapter;
}

inline const String & ProviderModule::getFileName(void) const
{
    return(_fileName);
}

inline const String & ProviderModule::getProviderName(void) const
{
    return(_providerName);
}

inline CIMBaseProvider * ProviderModule::getProvider(void) const
{
    return(_provider);
}

PEGASUS_NAMESPACE_END

#endif
