//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Yi Zhou (yi_zhou@hp.com)
//
// Modified By: Chip Vincent (cvincent@us.ibm.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderRegistrationProvider.h"

#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Constants.h>

PEGASUS_NAMESPACE_BEGIN

/**
   The name of the CapabilityID property for provider capabilities class
*/
static const CIMName _PROPERTY_CAPABILITYID  = CIMName ("CapabilityID");

/**
   stopping provider method
*/
static const CIMName _STOP_PROVIDER     = CIMName ("Stop");

/**
   starting provider method
*/
static const CIMName _START_PROVIDER   = CIMName ("Start");

ProviderRegistrationProvider::ProviderRegistrationProvider(
    ProviderRegistrationManager * providerRegistrationManager)	
    :_id(peg_credential_types::PROVIDER)
{
    _providerRegistrationManager = providerRegistrationManager;

    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
    if(_client_handle == NULL)
        throw UninitializedObjectException();
}

ProviderRegistrationProvider::~ProviderRegistrationProvider(void)	
{
    if (_providerRegistrationManager)
    {
	delete _providerRegistrationManager;
    }

    if (_client_handle)
    {
	delete _client_handle;
    }

}

void ProviderRegistrationProvider::initialize(CIMOMHandle & cimom)
{
    // This method should not be called because this is a control provider
    // and is not dynamically loaded through the provider manager
}

void ProviderRegistrationProvider::terminate(void)
{
}

// get registered provider
void ProviderRegistrationProvider::getInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{

    if(!instanceReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
            instanceReference.getNameSpace().getString());
    }

    // ensure the class existing in the specified namespace
    CIMName className = instanceReference.getClassName();

    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    CIMInstance instance;

    try
    {
	instance = _providerRegistrationManager->getInstance(instanceReference,
							     includeQualifiers,
							     includeClassOrigin,
							     propertyList);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    handler.deliver(instance);

    // complete processing the request
    handler.complete();
}

// get all registered providers
void ProviderRegistrationProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
    if(!classReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            classReference.getNameSpace().getString());
    }

    // ensure the class existing in the specified namespace
    CIMName className = classReference.getClassName();

    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    Array<CIMInstance> enumInstances;

    try
    {
        enumInstances = 
	    _providerRegistrationManager->enumerateInstances(classReference, 
							     includeQualifiers,
			 				     includeClassOrigin, 
							     propertyList);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    handler.deliver(enumInstances);

    // complete processing the request
    handler.complete();
}

// get all registered provider names
void ProviderRegistrationProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ObjectPathResponseHandler & handler)
{
    if(!classReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            classReference.getNameSpace().getString());
    }

    // ensure the class existing in the specified namespace
    CIMName className = classReference.getClassName();

    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    Array<CIMObjectPath> enumInstanceNames;

    // get all instance names from repository
    try
    {
    	enumInstanceNames =
	    _providerRegistrationManager->enumerateInstanceNames(classReference);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    handler.deliver(enumInstanceNames);

    // complete processing the request
    handler.complete();
}

// change properties for the registered provider
// only support to change property of Namespaces, property of
// SupportedProperties, and property of SupportedMethods
void ProviderRegistrationProvider::modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        ResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to modify the registration."); 
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,MessageLoaderParms(
	    		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_MODIFY_REGISTRATION",
	    		"You must have superuser privilege to modify the registration.")); 	
    }

    if(!instanceReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            instanceReference.getNameSpace().getString());
    }

    //
    // only support to modify the instance of PG_ProviderCapabilities
    //
    if (!instanceReference.getClassName().equal 
        (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            instanceReference.getClassName().getString());
    }

    //
    // only can modify the property of Namespaces, property of
    // SupportedProperties, and property of SupportedMethods
    //
    if (propertyList.isNull())
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
	    //"Only can modify Namespaces, SupportedProperties, and SupportedMethods.");
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
	    		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.CAN_ONLY_MODIFY_ERR",
	    		"Only can modify Namespaces, SupportedProperties, and SupportedMethods."));
    }

    Array<CIMName> propertyArray = propertyList.getPropertyNameArray();
    for (Uint32 i=0; i<propertyArray.size(); i++)
    {
	if (!propertyArray[i].equal (_PROPERTY_NAMESPACES) &&
	    !propertyArray[i].equal (_PROPERTY_SUPPORTEDPROPERTIES) &&
	    !propertyArray[i].equal (_PROPERTY_SUPPORTEDMETHODS))
	{
	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
                propertyArray[i].getString());
	}
    }

    // begin processing the request
    handler.processing();

    try
    {
        _providerRegistrationManager->modifyInstance(instanceReference, 
						     instanceObject, 
						     includeQualifiers, 
	    					     propertyArray);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    // complete processing the request
    handler.complete();
}

// register a provider
void ProviderRegistrationProvider::createInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    ObjectPathResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to register providers."); 
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
	    			"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_REGISTER_PROVIDERS",
	    			"You must have superuser privilege to register providers.")); 	
    }

    CIMName className = instanceReference.getClassName();
    CIMNamespaceName nameSpace = instanceReference.getNameSpace();

    CIMObjectPath returnReference;

    CIMInstance instance = instanceObject;

    if(!nameSpace.equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            nameSpace.getString());
    }

    // ensure the class existing in the specified namespace
    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    //
    // Check all required properties are set
    //
    if (className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	//
	// Name, Version, InterfaceType, InterfaceVersion, and Location
	// properties must be set
	// OperationalStatus property needs to be set. If not, set to default
	//
	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULE_NAME) ==
            PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Name which is required property in PG_ProviderModule class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_NAME_IN_PG_PROVIDERMODULE",
		"Missing Name which is required property in PG_ProviderModule class."));
	}

	if (instanceObject.findProperty(_PROPERTY_VENDOR) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Vendor which is required property in PG_ProviderModule class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_VENDOR_IN_PG_PROVIDERMODULE",
		"Missing Vendor which is required property in PG_ProviderModule class."));
	}

	if (instanceObject.findProperty(_PROPERTY_VERSION) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Version which is required property in PG_ProviderModule class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_VERSION_IN_PG_PROVIDERMODULE",
		"Missing Version which is required property in PG_ProviderModule class."));
	}

        Uint32 ifcTypeIndex =
            instanceObject.findProperty(_PROPERTY_INTERFACETYPE);
	if (ifcTypeIndex == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing InterfaceType which is required property in PG_ProviderModule class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_INTERFACETYPE_IN_PG_PROVIDERMODULE",
		"Missing InterfaceType which is required property in PG_ProviderModule class."));
	}
        String ifcTypeString;
        instanceObject.getProperty(ifcTypeIndex).getValue().
            get(ifcTypeString);

        if (ifcTypeString != "C++Default"
#ifdef ENABLE_CMPI_PROVIDER_MANAGER
            && ifcTypeString != "CMPI"
#endif
	        )
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"Unsupported InterfaceType value: \"" + ifcTypeString + "\"");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.UNSUPPORTED_INTERFACETYPE_VALUE",
		"Unsupported InterfaceType value: \"$0\"",ifcTypeString));
	}

        Uint32 ifcVersionIndex =
            instanceObject.findProperty(_PROPERTY_INTERFACEVERSION);
	if (ifcVersionIndex == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing InterfaceVersion which is required property in PG_ProviderModule class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_INTERFACEVERSION_IN_PG_PROVIDERMODULE",
		"Missing InterfaceVersion which is required property in PG_ProviderModule class."));
	}
        String ifcVersionString;
        instanceObject.getProperty(ifcVersionIndex).getValue().
            get(ifcVersionString);
        if (
#ifdef ENABLE_CMPI_PROVIDER_MANAGER
           (ifcTypeString == "CMPI" &&
            ifcVersionString != "2.0.0") ||
#endif
           (ifcTypeString == "C++Default" &&
            ifcVersionString != "2.1.0" &&
            ifcVersionString != "2.2.0" &&
            ifcVersionString != "2.3.0"))
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"Unsupported InterfaceVersion value: \"" + ifcVersionString +
                   // "\"");
        String unsupported = "InterfaceVersion";
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.UNSUPPORTED_INTERFACEVERSION_VALUE",
		"Unsupported InterfaceVersion value: \"$0\"",ifcVersionString));
	}

	if (instanceObject.findProperty(_PROPERTY_LOCATION) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Location which is required property in PG_ProviderModule class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_LOCATION_IN_PG_PROVIDERMODULE",
		"Missing Location which is required property in PG_ProviderModule class."));
	}

	if (instanceObject.findProperty(_PROPERTY_OPERATIONALSTATUS) == 
            PEG_NOT_FOUND)
	{
	    Array<Uint16> _operationalStatus;
	    _operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);
	    instance.addProperty (CIMProperty
		(_PROPERTY_OPERATIONALSTATUS, _operationalStatus));
	}
    }
    else if (className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
    {
	//
	// ProviderModuleName, ProviderName, InstanceID, ClassName,
 	// Namespaces, and ProviderType properties must be set
	//

	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULENAME) ==
            PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderModuleName which is required property in PG_ProviderCapabilities class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDERMODULENAME_IN_PG_PROVIDERCAPABILITIES",
		"Missing ProviderModuleName which is required property in PG_ProviderCapabilities class."));
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERNAME) == 
            PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderName which is required property in PG_ProviderCapabilities class.");
		String missing = "ProviderName";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing ProviderName which is required property in PG_ProviderCapabilities class."));
	}

	if (instanceObject.findProperty(_PROPERTY_CAPABILITYID) == 
            PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing CapabilityID which is required property in PG_ProviderCapabilities class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSINGCAPABILITYID_IN_PG_PROVIDERCAPABILITIES",
		"Missing CapabilityID which is required property in PG_ProviderCapabilities class."));
	}

	if (instanceObject.findProperty(_PROPERTY_CLASSNAME) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ClassName which is required property in PG_ProviderCapabilities class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_CLASSNAME_IN_PG_PROVIDERCAPABILITIES",
		"Missing ClassName which is required property in PG_ProviderCapabilities class."));
	}

	if (instanceObject.findProperty(_PROPERTY_NAMESPACES) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Namespaces which is required property in PG_ProviderCapabilities class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_NAMESPACES_IN_PG_PROVIDERCAPABILITIES",
		"Missing Namespaces which is required property in PG_ProviderCapabilities class."));
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERTYPE) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderType which is required property in PG_ProviderCapabilities class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDERTYPE_IN_PG_PROVIDERCAPABILITIES",
		"Missing ProviderType which is required property in PG_ProviderCapabilities class."));
	}
    }
    else if (className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES))
    {
	//
	// ProviderModuleName, ProviderName, CapabilityID, ProviderType,
 	// and Destinations properties must be set
	//

	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULENAME) ==
            PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderModuleName which is required property in PG_ConsumerCapabilities class.");
	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDER_MODULE_NAME_WHICH_IS_REQUIRED",
		"Missing ProviderModuleName which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERNAME) == 
            PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderName which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDER_NAME_WHICH_IS_REQUIRED",
		"Missing ProviderName which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_CAPABILITYID) == 
            PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing CapabilityID which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_CAPABILITY_ID_WHICH_IS_REQUIRED",
		"Missing CapabilityID which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERTYPE) == PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderType which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDER_TYPE_WHICH_IS_REQUIRED",
		"Missing ProviderType which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_INDICATIONDESTINATIONS) == 
	    PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Destinations which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_DESTINATIONS_TYPE_WHICH_IS_REQUIRED",
		"Missing Destinations which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}
    }
    else // PEGASUS_CLASSNAME_PROVIDER
    {
	//
	// Name and ProviderModuleName properties must be set
	//
	if (instanceObject.findProperty(_PROPERTY_PROVIDER_NAME) == PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Name which is required property in PG_Provider class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing Name which is required property in PG_Provider class."));
	}
	
	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULENAME) == 
            PEG_NOT_FOUND)
	{
		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderModuleName which is required property in PG_Provider class.");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDERMODULENAME_IN_PG_PROVIDER",
		"Missing ProviderModuleName which is required property in PG_Provider class."));
	}
    }

    // begin processing the request
    handler.processing();
	
    try
    {
    	returnReference =
	    _providerRegistrationManager->createInstance(instanceReference, instance);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    handler.deliver(returnReference);

    // complete processing request
    handler.complete();
}

// Unregister a provider
void ProviderRegistrationProvider::deleteInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    ResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to unregister providers.");
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_UNREGISTER_PROVIDERS",
	    "You must have superuser privilege to unregister providers.")); 	
    }

    if(!instanceReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            instanceReference.getNameSpace().getString());
    }


// l10n
    // Get the client's list of preferred languages for the response
    AcceptLanguages al = AcceptLanguages::EMPTY;
    try 
    {
        AcceptLanguageListContainer al_container = 
		(AcceptLanguageListContainer)context.get(AcceptLanguageListContainer::NAME);
        al = al_container.getLanguages();
    }
    catch (...)
    {
        ;   // Leave AcceptLanguages empty
    }

    CIMName className = instanceReference.getClassName();

    // ensure the class existing in the specified namespace
    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    String moduleName;
    Boolean moduleFound = false;
    Array<CIMKeyBinding> keys = instanceReference.getKeyBindings();

    //
    // disable provider before delete provider 
    // registration if the class is PG_Provider
    //
    if (className.equal (PEGASUS_CLASSNAME_PROVIDER))
    {
    	// get module name from reference

    	for(Uint32 i=0; i<keys.size() ; i++)
    	{
	    if(keys[i].getName().equal (_PROPERTY_PROVIDERMODULENAME))
	    {
	        moduleName = keys[i].getValue();
	        moduleFound = true;
	    }
	}

    	// if _PROPERTY_PROVIDERMODULENAME key not found
    	if( !moduleFound)
    	{
    		//l10n 485
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"key ProviderModuleName was not found");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.PROVIDERMODULENAME_KEY_NOT_FOUND",
		"key ProviderModuleName was not found"));
    	}

	// 
	// disable the provider 
	//
	try
	{
	     Sint16 ret_value = _disableModule(instanceReference, moduleName, true, al);

             //
             // if the provider disable failed
             //
// l10n
             if (ret_value == -1)
             {
             	//l10n
                 //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     //"disable the provider failed.");
                 throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
					"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.DISABLE_PROVIDER_FAILED",
                    "disable the provider failed."));
             }
	     //
             // The provider disable failed since there are pending requests
             //
             if (ret_value == -2)
             {
//L10N TODO
                // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     //"disable the provider failed: Provider is busy.");
                 throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
					"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.DISABLE_PROVIDER_FAILED_PROVIDER_BUSY",
                    "disable the provider failed: Provider is busy."));
             }
	}
    	catch(CIMException&)
    	{
	    throw;
    	}
    }

    //
    // disable provider module before remove provider registration
    // if the class is PG_ProviderModule 
    //

    if (className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
    	// get module name from reference

    	for(Uint32 i=0; i<keys.size() ; i++)
    	{
	    if(keys[i].getName().equal (_PROPERTY_PROVIDERMODULE_NAME))
	    {
	        moduleName = keys[i].getValue();
	        moduleFound = true;
	    }
	}

    	// if _PROPERTY_PROVIDERMODULE_NAME key not found
    	if( !moduleFound)
    	{
    		//l10n 485
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"key Name was not found");
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.NAME_KEY_NOT_FOUND",
		"key Name was not found"));
    	}

	// 
	// disable the provider module
	//
	try
	{
	    Sint16 ret_value = _disableModule(instanceReference, moduleName, false, al);

            //
            // if the provider module disable failed
            //
// l10n
            if (ret_value == -1)
            {
            	//l10n
                 //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     //"disable the provider module failed.");
                     throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
						"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.DISABLE_PROVIDER_MODULE_FAILED",
                     	"disable the provider module failed."));
            }
	    
	    //
            // The provider module disable failed since there are pending requests
            //
            if (ret_value == -2)
            {
//L10N TODO
                 //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                    // "disable the provider module failed: Provider is busy.");
                 throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
						"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.DISABLE_PROVIDER_MODULE_FAILED_PROVIDER_BUSY",
                     	"disable the provider module failed: Provider is busy."));
            }
	}
    	catch(CIMException& e)
    	{
	    throw (e);
    	}
    }

    try
    {
    	_providerRegistrationManager->deleteInstance(instanceReference);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    // complete processing the request
    handler.complete();
}

// Block a provider, unblock a provider, and stop a provider
void ProviderRegistrationProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to disable or enable providers."); 
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_DISABLE_ENABLE_PROVIDERS",
	    "You must have superuser privilege to disable or enable providers.")); 	
    }

    if(!objectReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            objectReference.getNameSpace().getString());
    }


// l10n
    // Get the client's list of preferred languages for the response
    AcceptLanguages al = AcceptLanguages::EMPTY;
    try 
    {
        AcceptLanguageListContainer al_container = 
		(AcceptLanguageListContainer)context.get(AcceptLanguageListContainer::NAME);
        al = al_container.getLanguages();
    }
    catch (...)
    {
        ;   // Leave AcceptLanguages empty
    }

    String moduleName;
    Boolean moduleFound = false;

    // get module name from reference
    Array<CIMKeyBinding> keys = objectReference.getKeyBindings();

    for(Uint32 i=0; i<keys.size() ; i++)
    {
	if(keys[i].getName().equal (_PROPERTY_PROVIDERMODULE_NAME))
	{
	    moduleName = keys[i].getValue();
	    moduleFound = true;
	}
    }

    // if _PROPERTY_PROVIDERMODULE_NAME key not found
    if( !moduleFound)
    {
    	//l10n 485
	throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.NAME_KEY_NOT_FOUND",
		"key Name was not found"));
    }

    handler.processing();

    Sint16 ret_value;

    try
    {
        if(methodName.equal(_STOP_PROVIDER))
    	{
	    // disable module
    	     ret_value =  _disableModule(objectReference, moduleName, false, al);
	}
    	else if(methodName.equal(_START_PROVIDER))
    	{
	    // enable module
    	     ret_value =  _enableModule(objectReference, moduleName, al);
	}
        else
    	{
	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_METHOD_NOT_AVAILABLE, String::EMPTY);
    	}
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    CIMValue retValue(ret_value);
    handler.deliver(retValue);
    handler.complete();
    return;
}

// get provider manager service
MessageQueueService * ProviderRegistrationProvider::_getProviderManagerService()
{
    MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
    MessageQueueService * _service = dynamic_cast<MessageQueueService *>(queue);

    return(_service);
}

ProviderRegistrationProvider & ProviderRegistrationProvider::operator=(const ProviderRegistrationProvider & handle)
{
    if(this == &handle)
    {
        return(*this);
    }

    return(*this);
}

Array<Uint16> ProviderRegistrationProvider::_sendDisableMessageToProviderManager(
        CIMDisableModuleRequestMessage * disable_req)
{
    MessageQueueService * _service = _getProviderManagerService();
    Uint32 _queueId = _service->getQueueId();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
            _service->get_next_xid(),
            NULL,
            _queueId,
            disable_req,
            _queueId);

    AsyncReply * asyncReply = _controller->ClientSendWait(*_client_handle,
							  _queueId,
							  asyncRequest);
    CIMDisableModuleResponseMessage * response =
	reinterpret_cast<CIMDisableModuleResponseMessage *>(
             (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
	CIMException e = response->cimException;
        delete asyncRequest;
        delete asyncReply;
        delete response;
	throw (e);
    }

    Array<Uint16> operationalStatus = response->operationalStatus;

    delete asyncRequest;
    delete asyncReply;
    delete response;

    return(operationalStatus);
}

Array<Uint16> ProviderRegistrationProvider::_sendEnableMessageToProviderManager(
        CIMEnableModuleRequestMessage * enable_req)
{
    MessageQueueService * _service = _getProviderManagerService();
    Uint32 _queueId = _service->getQueueId();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
            _service->get_next_xid(),
            NULL,
            _queueId,
            enable_req,
            _queueId);

    AsyncReply * asyncReply = _controller->ClientSendWait(*_client_handle,
							  _queueId,
							  asyncRequest);
    CIMEnableModuleResponseMessage * response =
	reinterpret_cast<CIMEnableModuleResponseMessage *>(
             (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
	CIMException e = response->cimException;
        delete asyncRequest;
        delete asyncReply;
        delete response;
	throw (e);
    }

    Array<Uint16> operationalStatus = response->operationalStatus;

    delete asyncRequest;
    delete asyncReply;
    delete response;

    return(operationalStatus);
}

// send termination message to subscription service
void ProviderRegistrationProvider::_sendTerminationMessageToSubscription(
    const CIMObjectPath & ref, const String & moduleName,
    const Boolean disableProviderOnly,
    const AcceptLanguages & al)
{
    CIMInstance instance;
    String _moduleName;
    Array<CIMInstance> instances;

    if (!disableProviderOnly)
    {
        CIMObjectPath reference("", PEGASUS_NAMESPACENAME_INTEROP,
	    PEGASUS_CLASSNAME_PROVIDER, ref.getKeyBindings());

        Array<CIMObjectPath> instanceNames =
	    _providerRegistrationManager->enumerateInstanceNames(reference);

        // find all the instances which have same module name as moduleName
        for (Uint32 i = 0, n=instanceNames.size(); i < n; i++)
        {
	    //
            // get provider module name from reference
            //

            Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

            for(Uint32 j=0; j < keys.size(); j++)
            {
                if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
                {
                    _moduleName = keys[j].getValue();
                }
            }

	    if (String::equalNoCase(moduleName, _moduleName))
	    {
	        reference.setKeyBindings(keys);
	        instance = _providerRegistrationManager->getInstance(reference);
		//
		// if the provider is indication provider
		//
		if (_isIndicationProvider(moduleName, instance, reference))
		{
	            instances.append(instance);
		}
	    }
        }
    }
    else
    {
	instance = _providerRegistrationManager->getInstance(ref);

	//
	// if the provider is indication provider
	//
	if (_isIndicationProvider(moduleName, instance, ref))
	{
	    instances.append(instance);
	}
    }

    //
    // get indication server queueId
    //
    MessageQueueService * _service = _getIndicationService();

    if (_service != NULL)
    {
        Uint32 _queueId = _service->getQueueId();

        CIMNotifyProviderTerminationRequestMessage * termination_req =
	    new CIMNotifyProviderTerminationRequestMessage(
	        XmlWriter::getNextMessageId (),
	        instances,
	        QueueIdStack(_service->getQueueId()));

// l10n
        termination_req->operationContext.set(AcceptLanguageListContainer(al));

        // create request envelope
        AsyncLegacyOperationStart * asyncRequest =
            new AsyncLegacyOperationStart (
                _service->get_next_xid(),
                NULL,
                _queueId,
                termination_req,
                _queueId);

        if( false  == _controller->ClientSendForget(
                           *_client_handle,
                           _queueId,
                           asyncRequest))
        {
            delete asyncRequest;
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
        }
    }
}

// get indication service
MessageQueueService * ProviderRegistrationProvider::_getIndicationService()
{
    MessageQueue * queue = MessageQueue::lookup(
	PEGASUS_QUEUENAME_INDICATIONSERVICE);

    MessageQueueService * _service =
	dynamic_cast<MessageQueueService *>(queue);
    return(_service);
}

// disable provider module, return 0 if module is disabled successfully,
// return 1 if module is already disabled, return -2 if module can not be
// disabled since there are pending requests, otherwise, return -1
Sint16 ProviderRegistrationProvider::_disableModule(
    const CIMObjectPath & objectReference, 
    const String & moduleName,
    Boolean disableProviderOnly,
    const AcceptLanguages & al)         // l10n
{
    	//
    	// get module status
    	//
    	Array<Uint16> _OperationalStatus =
	    _providerRegistrationManager->getProviderModuleStatus( moduleName);

	for (Uint32 i = 0; i<_OperationalStatus.size(); i++)
	{
	    // retValue equals 1 if module is already disabled
	    if (_OperationalStatus[i] == CIM_MSE_OPSTATUS_VALUE_STOPPED ||
		_OperationalStatus[i] == CIM_MSE_OPSTATUS_VALUE_STOPPING)
	    {
		return (1);
	    }
	}

	CIMInstance instance;
	Array<CIMInstance> instances;
        CIMInstance mInstance;
	String _moduleName;
	Uint16 providers;
	CIMObjectPath providerRef;
	Boolean indProvider = false;
        Array<Boolean> indicationProviders;

	// disable a provider module or delete a provider module
	if (!disableProviderOnly)
	{
	    providerRef = CIMObjectPath(objectReference.getHost(),
				 objectReference.getNameSpace(),
				 PEGASUS_CLASSNAME_PROVIDER,
				 objectReference.getKeyBindings());

            // get module instance
            mInstance = 
	        _providerRegistrationManager->getInstance(objectReference);

	}
	else // disable a provider
	{
            // get module instance
	    Array <CIMKeyBinding> moduleKeyBindings;
	    moduleKeyBindings.append (CIMKeyBinding
		(_PROPERTY_PROVIDERMODULE_NAME, moduleName, 
		 CIMKeyBinding::STRING)); 

	    CIMObjectPath moduleRef(objectReference.getHost(),
				    objectReference.getNameSpace(),
				    PEGASUS_CLASSNAME_PROVIDERMODULE,
				    moduleKeyBindings);
				    
            mInstance = 
	        _providerRegistrationManager->getInstance(moduleRef);
	}

        if (!disableProviderOnly)
        {
	    // get all provider instances which have same module name as 
	    // moduleName
 	    Array<CIMObjectPath> instanceNames = 
	        _providerRegistrationManager->enumerateInstanceNames(providerRef);

	    for(Uint32 i = 0, n=instanceNames.size(); i < n; i++)
	    {
	        //
                // get provider module name from reference
                //

                Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

                for(Uint32 j=0; j < keys.size(); j++)
                {
                    if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
                    {
                        _moduleName = keys[j].getValue();
                    }
                }

	        if (String::equalNoCase(_moduleName, moduleName))
	        {
		    providerRef.setKeyBindings(keys);
		    instance = _providerRegistrationManager->getInstance
			(providerRef);
		    if (_isIndicationProvider(moduleName, instance, providerRef))
                    {
                        indProvider = true;
                        indicationProviders.append(true);
                    }
                    else
                    {
                        indicationProviders.append(false);
                    }
		    instances.append(instance);
	        }

	    }
        }
        else
        {
	    instance = _providerRegistrationManager->getInstance(objectReference);
            if (_isIndicationProvider(moduleName, instance, objectReference))
            {
                indProvider = true;
                indicationProviders.append(true);
            }
            else
            {
                indicationProviders.append(false);
            }

            instances.append(instance);
        }

        //
        // get provider manager service
        //
        MessageQueueService * _service = _getProviderManagerService();

	if (_service != NULL)
	{
	    // create CIMDisableModuleRequestMessage
	    CIMDisableModuleRequestMessage * disable_req =
	        new CIMDisableModuleRequestMessage(
		    XmlWriter::getNextMessageId (),
		    mInstance,
		    instances,
		    disableProviderOnly,
		    indicationProviders,
		    QueueIdStack(_service->getQueueId()));
// l10n
		disable_req->operationContext.set(AcceptLanguageListContainer(al));;

  	    Array<Uint16> _opStatus =
	        _sendDisableMessageToProviderManager(disable_req);

	    if (!disableProviderOnly) // disable provider module
	    {
	        for (Uint32 i = 0; i<_opStatus.size(); i++)
	        {
		    // module was disabled successfully
	            if (_opStatus[i] == CIM_MSE_OPSTATUS_VALUE_STOPPED)
	            {
			if (indProvider)
			{
	 	            // send termination message to subscription service
		            _sendTerminationMessageToSubscription(objectReference,
				  moduleName, false, al);
			}
		        return (0);
	            }

		    // module is not disabled since there are pending
                    // requests for the providers in the module
                    if (_opStatus[i] == CIM_MSE_OPSTATUS_VALUE_OK)
                    {
                        return (-2);
                    }
	        }
	    }
	    else // disable provider
	    {
		if (indProvider)
		{
	            _sendTerminationMessageToSubscription(objectReference,
			moduleName, true, al);
		}
	        return (0);
	    }
  	}

        // disable failed
	return (-1);
}

// enable provider module 
// return 0 if module is enabled successfully,
// return 1 if module is already enabled,
// return 2 if module can not be enabled since module is stopping,
// otherwise, return -1 
Sint16 ProviderRegistrationProvider::_enableModule(
    const CIMObjectPath & moduleRef, 
    const String & moduleName,
    const AcceptLanguages & al)
{
    	//
    	// get module status
    	//
    	Array<Uint16> _OperationalStatus =
	    _providerRegistrationManager->getProviderModuleStatus( moduleName);

	for (Uint32 i = 0; i<_OperationalStatus.size(); i++)
	{
	    // retValue equals 1 if module is already enabled
	    if (_OperationalStatus[i] == CIM_MSE_OPSTATUS_VALUE_OK)
	    {
		return (1);
	    }

	    // retValue equals 2 if module is stopping
	    // at this stage, module can not be started
	    if (_OperationalStatus[i] == CIM_MSE_OPSTATUS_VALUE_STOPPING)
	    {
                return (2);
	    }
	}

        // get module instance
        CIMInstance mInstance = 
	    _providerRegistrationManager->getInstance(moduleRef);

	//
        // get provider manager service
        //
        MessageQueueService * _service = _getProviderManagerService();
	Boolean enabled = false;

        if (_service != NULL)
	{
	    // create CIMEnableModuleRequestMessage
	    CIMEnableModuleRequestMessage * enable_req =
	        new CIMEnableModuleRequestMessage(
		    XmlWriter::getNextMessageId (),
		    mInstance,
		    QueueIdStack(_service->getQueueId()));
// l10n
        enable_req->operationContext.set(AcceptLanguageListContainer(al));;    
		
  	    Array<Uint16> _opStatus;
            _opStatus = _sendEnableMessageToProviderManager(enable_req);

	    for (Uint32 i = 0; i<_opStatus.size(); i++)
	    {
		// module is enabled successfully
	        if (_opStatus[i] == CIM_MSE_OPSTATUS_VALUE_OK)
	        {
		    enabled = true;
	        }
	    }
	}

	if (enabled)
	{
	    //
	    // Since module is enabled, need get updated module instance
	    //
            CIMInstance updatedModuleInstance = 
	        _providerRegistrationManager->getInstance(moduleRef);

	    // module is enabled, initialize providers as necessary
	    _providerRegistrationManager->initializeProviders(
		updatedModuleInstance);

	    //
	    // The module is enabled, need to send enable message to 
	    // subscription service if the provider is an indication provider
	    //

	    CIMObjectPath providerRef = CIMObjectPath(String::EMPTY,
				    moduleRef.getNameSpace(),
				    PEGASUS_CLASSNAME_PROVIDER,
				    Array<CIMKeyBinding>());

	    //
	    // get all provider instances which have same module name as
	    // moduleName
            //
	    Array<CIMObjectPath> instanceNames =
	        _providerRegistrationManager->enumerateInstanceNames(providerRef);
	    CIMInstance pInstance;
	    String _moduleName;
	    String _providerName;
	    Array<CIMInstance> capInstances;

	    for(Uint32 i = 0, n=instanceNames.size(); i < n; i++)
	    {

                Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

                for(Uint32 j=0; j < keys.size(); j++)
                {
	            //
                    // get provider module name from reference
                    //
                    if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
                    {
                        _moduleName = keys[j].getValue();
                    }

	            //
                    // get provider name from reference
                    //
                    if(keys[j].getName().equal (_PROPERTY_PROVIDER_NAME))
                    {
                        _providerName = keys[j].getValue();
                    }
                }

	        if (String::equalNoCase(_moduleName, moduleName))
	        {
	 	    providerRef.setKeyBindings(keys);
	            pInstance = _providerRegistrationManager->getInstance
			(providerRef);
		    // 
		    // get all the indication capability instances which belongs 
		    // to this provider
		    //
		    capInstances = _getIndicationCapInstances(
				   moduleName, pInstance, providerRef);

		    //
		    // if there are indication capability instances
		    //
		    if (capInstances.size() != 0)
		    {
		        _sendEnableMessageToSubscription(updatedModuleInstance,
		  					 pInstance,
						     	 capInstances,
							 al);
		    }
	        }    
	    }
	    return (0);
	}


        // enable failed
	return (-1);
}

// send enable message to indication service
void ProviderRegistrationProvider::_sendEnableMessageToSubscription(
    const CIMInstance & mInstance,
    const CIMInstance & pInstance,
    const Array<CIMInstance> & capInstances,
    const AcceptLanguages & al)
{
    //
    // get indication server queueId
    //
    MessageQueueService * _service = _getIndicationService();

    if (_service != NULL)
    {
    	Uint32 _queueId = _service->getQueueId();
	    
    	CIMNotifyProviderEnableRequestMessage * enable_req = 
		new CIMNotifyProviderEnableRequestMessage (
		    XmlWriter::getNextMessageId (),
		    capInstances,
		    QueueIdStack(_service->getQueueId()));

	enable_req->operationContext.set(AcceptLanguageListContainer(al));
    enable_req->operationContext.insert(ProviderIdContainer(mInstance,pInstance));
	
	// create request envelope
        AsyncLegacyOperationStart * asyncRequest =
            new AsyncLegacyOperationStart (
                _service->get_next_xid(),
                NULL,
                _queueId,
                enable_req,
                _queueId);
	    
	AsyncReply * asyncReply = 
	    _controller->ClientSendWait(*_client_handle,
                      			_queueId,
                      			asyncRequest);

        CIMNotifyProviderEnableResponseMessage * response =
	    reinterpret_cast<CIMNotifyProviderEnableResponseMessage *>(
		(static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

        if (response->cimException.getCode() != CIM_ERR_SUCCESS)
	{
	    CIMException e = response->cimException;
	    delete asyncRequest;
	    delete asyncReply;
	    delete response;
	    throw (e);
	}

        delete asyncRequest;
        delete asyncReply;
        delete response;
    }
}

// If the provider is indication provider, return true,
// otherwise, return false
Boolean ProviderRegistrationProvider::_isIndicationProvider(
    const String & moduleName,
    const CIMInstance & instance,
    const CIMObjectPath & providerRef)
{
    // get provider name
    String providerName;
    Uint32 pos = instance.findProperty(CIMName (_PROPERTY_PROVIDER_NAME));
    if (pos != PEG_NOT_FOUND)
    {
	instance.getProperty(pos).getValue().get(providerName);	
    }

    CIMObjectPath capabilityRef;

    capabilityRef = CIMObjectPath(providerRef.getHost(),
				  providerRef.getNameSpace(),
		       		  PEGASUS_CLASSNAME_CAPABILITIESREGISTRATION,
		       		  providerRef.getKeyBindings());

    // get all Capabilities instances
    Array<CIMObjectPath> instanceNames =
	_providerRegistrationManager->enumerateInstanceNames(capabilityRef);
			
    //
    // get provider module name and provider name from capability reference
    //
    String _moduleName, _providerName;
    CIMInstance capInstance;
    Array<Uint16> providerTypes;
    for(Uint32 i = 0, n=instanceNames.size(); i < n; i++)
    {
	Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

	for(Uint32 j=0; j < keys.size(); j++)
        {
             if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
             {
                  _moduleName = keys[j].getValue();
             }

             if(keys[j].getName().equal (_PROPERTY_PROVIDERNAME))
             {
                  _providerName = keys[j].getValue();
             }

	     //
	     // if capability instance has same module name as moduleName
	     // and same provider name as providerName, get provider type
	     //
	     if(String::equal(_moduleName, moduleName) &&
		String::equal(_providerName, providerName))
	     {
		  capInstance = _providerRegistrationManager->getInstance
				(instanceNames[i]);

		  Uint32 pos = capInstance.findProperty(CIMName (_PROPERTY_PROVIDERTYPE));
    		  if (pos != PEG_NOT_FOUND)
		  {
		       capInstance.getProperty(pos).getValue().get(providerTypes); 

    			for (Uint32 k=0; k < providerTypes.size(); k++)
    			{
        		    if (providerTypes[k] == _INDICATION_PROVIDER)
        		    {
            			return (true);
        		    }
    			}
		  }
  	     }
        }
    }

    return (false);
}

//
// get all the capability instances whose provider type is indication 
// 
Array<CIMInstance> ProviderRegistrationProvider::_getIndicationCapInstances(
    const String & moduleName,
    const CIMInstance & instance,
    const CIMObjectPath & providerRef)
{
    // get provider name
    String providerName;
    Uint32 pos = instance.findProperty(CIMName (_PROPERTY_PROVIDER_NAME));
    if (pos != PEG_NOT_FOUND)
    {
	instance.getProperty(pos).getValue().get(providerName);	
    }

    CIMObjectPath capabilityRef;

    capabilityRef = CIMObjectPath(providerRef.getHost(),
				  providerRef.getNameSpace(),
		       		  PEGASUS_CLASSNAME_CAPABILITIESREGISTRATION,
		       		  providerRef.getKeyBindings());

    // get all Capabilities instances
    Array<CIMObjectPath> instanceNames =
	_providerRegistrationManager->enumerateInstanceNames(capabilityRef);
			
    String _moduleName, _providerName;
    CIMInstance capInstance;
    Array<Uint16> providerTypes;
    Array<CIMInstance> indCapInstances = 0;
    for(Uint32 i = 0, n=instanceNames.size(); i < n; i++)
    {
	Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

	for(Uint32 j=0; j < keys.size(); j++)
        {
             if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
             {
                  _moduleName = keys[j].getValue();
             }

             if(keys[j].getName().equal (_PROPERTY_PROVIDERNAME))
             {
                  _providerName = keys[j].getValue();
             }
	}

       //
       // if capability instance has same module name as moduleName
       // and same provider name as providerName, get provider type
       //
       if(String::equal(_moduleName, moduleName) &&
          String::equal(_providerName, providerName))
       {
	    capInstance = _providerRegistrationManager->getInstance
				(instanceNames[i]);

	    Uint32 pos = capInstance.findProperty(CIMName (_PROPERTY_PROVIDERTYPE));
    	    if (pos != PEG_NOT_FOUND)
	    {
	        capInstance.getProperty(pos).getValue().get(providerTypes); 

    		for (Uint32 k=0; k < providerTypes.size(); k++)
    		{
		    // 
		    // if provider type of the instance is indication,
		    // append the instance 
		    //
        	    if (providerTypes[k] == _INDICATION_PROVIDER)
        	    {
            	        indCapInstances.append(capInstance);
        	    }
    		}
	    }
	}
    }

    return (indCapInstances);
}

PEGASUS_NAMESPACE_END
