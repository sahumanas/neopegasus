//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";
const String CLASSNAME = "PG_ProviderModule";
const String CLASSNAME2 = "PG_Provider";
const String CLASSNAME3 = "PG_ProviderCapabilities";

void TestModifyInstances(ProviderRegistrationManager prmanager)
{
    //
    // create Provider module instances
    //
    CIMObjectPath returnRef;

    CIMClass cimClass(CLASSNAME);

    CIMInstance cimInstance(CLASSNAME);

    cimInstance.addProperty(CIMProperty("Name", String("providersModule1")));
    cimInstance.addProperty(CIMProperty("Vendor", String("HP")));
    cimInstance.addProperty(CIMProperty("Version", String("2.0")));
    cimInstance.addProperty(CIMProperty("InterfaceType", String("PG_DefaultC++")));
    cimInstance.addProperty(CIMProperty("InterfaceVersion", String("2.0")));
    cimInstance.addProperty(CIMProperty("Location", String("/tmp/module1")));

    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);

    instanceName.setNameSpace(NAMESPACE);
    instanceName.setClassName(CLASSNAME);

    try
    {
        returnRef = prmanager.createInstance(instanceName, cimInstance);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    // create PG_Provider instances

    CIMObjectPath returnRef2;

    CIMClass cimClass2(CLASSNAME2);

    CIMInstance cimInstance2(CLASSNAME2);

    cimInstance2.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance2.addProperty(CIMProperty("Name", String("PG_ProviderInstance1")));

    CIMObjectPath instanceName2 = cimInstance2.buildPath(cimClass2);

    instanceName2.setNameSpace(NAMESPACE);
    instanceName2.setClassName(CLASSNAME2);

    try
    {
	returnRef2 = prmanager.createInstance(instanceName2, cimInstance2);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    //
    // create provider capability instances
    //

    Array <String> namespaces;
    Array <Uint16> providerType;
    Array <String> supportedMethods;
    Array <String> supportedProperties;

    namespaces.append("root/cimv2");
    namespaces.append("root/cimv3");
    
    providerType.append(2);
    providerType.append(5);

    supportedMethods.append("test_method1");
    supportedMethods.append("test_method2");

    supportedProperties.append("PkgStatus");
    supportedProperties.append("PkgIndex");

    CIMObjectPath returnRef3;

    CIMClass cimClass3(CLASSNAME3);

    CIMInstance cimInstance3(CLASSNAME3);

    cimInstance3.addProperty(CIMProperty("ProviderModuleName", String("providersModule1")));
    cimInstance3.addProperty(CIMProperty("ProviderName", String("PG_ProviderInstance1")));
    cimInstance3.addProperty(CIMProperty("CapabilityID", String("capability1")));
    cimInstance3.addProperty(CIMProperty("ClassName", String("TestSoftwarePkg")));
    cimInstance3.addProperty(CIMProperty("Namespaces", namespaces));
    cimInstance3.addProperty(CIMProperty("ProviderType", providerType));
    cimInstance3.addProperty(CIMProperty("SupportedMethods", supportedMethods));
    cimInstance3.addProperty(CIMProperty("SupportedProperties", supportedProperties));

    CIMObjectPath instanceName3 = cimInstance3.buildPath(cimClass3);

    instanceName3.setNameSpace(NAMESPACE);
    instanceName3.setClassName(CLASSNAME3);

    try
    {
        returnRef3 = prmanager.createInstance(instanceName3, cimInstance3);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    // create CIMObjectPath
    Array<KeyBinding> keys;
    KeyBinding kb1("ProviderModuleName", "providersModule1", KeyBinding::STRING);
    KeyBinding kb2("ProviderName", "PG_ProviderInstance1", KeyBinding::STRING);
    KeyBinding kb3("CapabilityID", "capability1", KeyBinding::STRING);
 
    keys.append(kb1);
    keys.append(kb2);
    keys.append(kb3);
 
    instanceName3.setKeyBindings(keys);

    Array <String> supportedMethods2;
    Array <String> namespaces2;
    Array<CIMName> propertyList;

    // create new instance which will replace cimInstance3 
    CIMInstance cimInstance4(CLASSNAME3);
    namespaces2.append("root/cimv4");
    namespaces2.append("root/cimv5");

    supportedMethods2.append("test_method2");
    supportedMethods2.append("test_method3");

    cimInstance4.addProperty(CIMProperty("Namespaces", namespaces2));
    cimInstance4.addProperty(CIMProperty("SupportedMethods", supportedMethods2));

    Uint32 flags = 0x00000010;

    propertyList.append("Namespaces");
    propertyList.append("SupportedMethods");
   
    try
    {
        prmanager.modifyInstance(instanceName3, cimInstance4, flags, propertyList);
    }
    catch(CIMException& e)
    {
        throw (e);
    }
}

int main(int argc, char** argv)
{
    const char* tmpDir = getenv ("PEGASUS_TMP");
    String repositoryRoot;
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot.append("/repository");
    CIMRepository r (repositoryRoot) ;

    ProviderRegistrationManager prmanager(&r);

    try
    {
	TestModifyInstances(prmanager);
    }

    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	PEGASUS_STD (cout) << "+++++ modify instances failed"
                           << PEGASUS_STD (endl);
	exit(-1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    
    exit (0);
}
