//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMBinMsgSerializer_h
#define Pegasus_CIMBinMsgSerializer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMBuffer.h>

PEGASUS_NAMESPACE_BEGIN

/** This class converts CIMMessage objects into binary messages. Please see
    comments for The CIMBuffer class for more information about goals of the
    the serialization/deserialization scheme.
*/
class PEGASUS_COMMON_LINKAGE CIMBinMsgSerializer
{
public:

    static void serialize(CIMBuffer& out, CIMMessage* cimMessage);

private:

    PEGASUS_HIDDEN_LINKAGE
    static void _putRequestMessage(
        CIMBuffer& out,
        CIMRequestMessage* cimMessage);

    PEGASUS_HIDDEN_LINKAGE
    static void _putResponseMessage(
        CIMBuffer& out,
        CIMResponseMessage* cimMessage);

    PEGASUS_HIDDEN_LINKAGE
    static void _serializeUserInfo(
        CIMBuffer& out,
        const String& authType,
        const String& userName);

    PEGASUS_HIDDEN_LINKAGE
    static void _serializeQueueIdStack(
        CIMBuffer& out,
        const QueueIdStack& queueIdStack);

    PEGASUS_HIDDEN_LINKAGE
    static void _serializeOperationContext(
        CIMBuffer& out,
        const OperationContext& operationContext);

    PEGASUS_HIDDEN_LINKAGE
    static void _serializeContentLanguageList(
        CIMBuffer& out,
        const ContentLanguageList& contentLanguages);

    PEGASUS_HIDDEN_LINKAGE
    static void _serializeAcceptLanguageList(
        CIMBuffer& out,
        const AcceptLanguageList& acceptLanguages);

    PEGASUS_HIDDEN_LINKAGE
    static void _putException(
        CIMBuffer& out,
        const CIMException& cimException);

    PEGASUS_HIDDEN_LINKAGE
    static void _putPropertyList(
        CIMBuffer& out,
        const CIMPropertyList& cimPropertyList);

    PEGASUS_HIDDEN_LINKAGE
    static void _putObjectPath(
        CIMBuffer& out,
        const CIMObjectPath& cimObjectPath);

    PEGASUS_HIDDEN_LINKAGE
    static void _putInstance(
        CIMBuffer& out,
        const CIMInstance& cimInstance);

    PEGASUS_HIDDEN_LINKAGE
    static void _putNamespaceName(
        CIMBuffer& out,
        const CIMNamespaceName& cimNamespaceName);

    PEGASUS_HIDDEN_LINKAGE
    static void _putName(
        CIMBuffer& out,
        const CIMName& cimName);

    PEGASUS_HIDDEN_LINKAGE
    static void _putObject(
        CIMBuffer& out,
        const CIMObject& object);

    PEGASUS_HIDDEN_LINKAGE
    static void _putParamValue(
        CIMBuffer& out,
        const CIMParamValue& paramValue);

    PEGASUS_HIDDEN_LINKAGE
    static void _putGetInstanceRequestMessage(
        CIMBuffer& out,
        CIMGetInstanceRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putDeleteInstanceRequestMessage(
        CIMBuffer& out,
        CIMDeleteInstanceRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putCreateInstanceRequestMessage(
        CIMBuffer& out,
        CIMCreateInstanceRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putModifyInstanceRequestMessage(
        CIMBuffer& out,
        CIMModifyInstanceRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putEnumerateInstancesRequestMessage(
        CIMBuffer& out,
        CIMEnumerateInstancesRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putEnumerateInstanceNamesRequestMessage(
        CIMBuffer& out,
        CIMEnumerateInstanceNamesRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putExecQueryRequestMessage(
        CIMBuffer& out,
        CIMExecQueryRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putAssociatorsRequestMessage(
        CIMBuffer& out,
        CIMAssociatorsRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putAssociatorNamesRequestMessage(
        CIMBuffer& out,
        CIMAssociatorNamesRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putReferencesRequestMessage(
        CIMBuffer& out,
        CIMReferencesRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putReferenceNamesRequestMessage(
        CIMBuffer& out,
        CIMReferenceNamesRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putGetPropertyRequestMessage(
        CIMBuffer& out,
        CIMGetPropertyRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putSetPropertyRequestMessage(
        CIMBuffer& out,
        CIMSetPropertyRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putInvokeMethodRequestMessage(
        CIMBuffer& out,
        CIMInvokeMethodRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putCreateSubscriptionRequestMessage(
        CIMBuffer& out,
        CIMCreateSubscriptionRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putModifySubscriptionRequestMessage(
        CIMBuffer& out,
        CIMModifySubscriptionRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putDeleteSubscriptionRequestMessage(
        CIMBuffer& out,
        CIMDeleteSubscriptionRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putExportIndicationRequestMessage(
        CIMBuffer& out,
        CIMExportIndicationRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putProcessIndicationRequestMessage(
        CIMBuffer& out,
        CIMProcessIndicationRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putDisableModuleRequestMessage(
        CIMBuffer& out,
        CIMDisableModuleRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putEnableModuleRequestMessage(
        CIMBuffer& out,
        CIMEnableModuleRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putStopAllProvidersRequestMessage(
        CIMBuffer& out,
        CIMStopAllProvidersRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putInitializeProviderAgentRequestMessage(
        CIMBuffer& out,
        CIMInitializeProviderAgentRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putNotifyConfigChangeRequestMessage(
        CIMBuffer& out,
        CIMNotifyConfigChangeRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putSubscriptionInitCompleteRequestMessage(
        CIMBuffer& out,
        CIMSubscriptionInitCompleteRequestMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putGetInstanceResponseMessage(
        CIMBuffer& out,
        CIMGetInstanceResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putDeleteInstanceResponseMessage(
        CIMBuffer& out,
        CIMDeleteInstanceResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putCreateInstanceResponseMessage(
        CIMBuffer& out,
        CIMCreateInstanceResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putModifyInstanceResponseMessage(
        CIMBuffer& out,
        CIMModifyInstanceResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putEnumerateInstancesResponseMessage(
        CIMBuffer& out,
        CIMEnumerateInstancesResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putEnumerateInstanceNamesResponseMessage(
        CIMBuffer& out,
        CIMEnumerateInstanceNamesResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putExecQueryResponseMessage(
        CIMBuffer& out,
        CIMExecQueryResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putAssociatorsResponseMessage(
        CIMBuffer& out,
        CIMAssociatorsResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putAssociatorNamesResponseMessage(
        CIMBuffer& out,
        CIMAssociatorNamesResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putReferencesResponseMessage(
        CIMBuffer& out,
        CIMReferencesResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putReferenceNamesResponseMessage(
        CIMBuffer& out,
        CIMReferenceNamesResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putGetPropertyResponseMessage(
        CIMBuffer& out,
        CIMGetPropertyResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putSetPropertyResponseMessage(
        CIMBuffer& out,
        CIMSetPropertyResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putInvokeMethodResponseMessage(
        CIMBuffer& out,
        CIMInvokeMethodResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putCreateSubscriptionResponseMessage(
        CIMBuffer& out,
        CIMCreateSubscriptionResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putModifySubscriptionResponseMessage(
        CIMBuffer& out,
        CIMModifySubscriptionResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putDeleteSubscriptionResponseMessage(
        CIMBuffer& out,
        CIMDeleteSubscriptionResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putExportIndicationResponseMessage(
        CIMBuffer& out,
        CIMExportIndicationResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putProcessIndicationResponseMessage(
        CIMBuffer& out,
        CIMProcessIndicationResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putDisableModuleResponseMessage(
        CIMBuffer& out,
        CIMDisableModuleResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putEnableModuleResponseMessage(
        CIMBuffer& out,
        CIMEnableModuleResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putStopAllProvidersResponseMessage(
        CIMBuffer& out,
        CIMStopAllProvidersResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putInitializeProviderAgentResponseMessage(
        CIMBuffer& out,
        CIMInitializeProviderAgentResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putNotifyConfigChangeResponseMessage(
        CIMBuffer& out,
        CIMNotifyConfigChangeResponseMessage* msg);

    PEGASUS_HIDDEN_LINKAGE
    static void _putSubscriptionInitCompleteResponseMessage(
        CIMBuffer& out,
        CIMSubscriptionInitCompleteResponseMessage* msg);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMBinMsgSerializer_h */