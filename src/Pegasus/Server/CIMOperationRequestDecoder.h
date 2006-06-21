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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company 
//              (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationRequestDecoder_h
#define Pegasus_CIMOperationRequestDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

/** This class decodes CIM operation requests and passes them down-stream.
 */
class CIMOperationRequestDecoder : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;
    
      CIMOperationRequestDecoder(
	 MessageQueueService* outputQueue,
	 Uint32 returnQueueId);

      ~CIMOperationRequestDecoder();

      void sendResponse(
         Uint32 queueId, 
         Buffer& message,
         Boolean closeConnect = false);

      void sendIMethodError(
         Uint32 queueId, 
         HttpMethod httpMethod,
         const String& messageId,
         const String& methodName,
         const CIMException& cimException,
         Boolean closeConnect = false);

      void sendMethodError(
         Uint32 queueId, 
         HttpMethod httpMethod,
         const String& messageId,
         const String& methodName,
         const CIMException& cimException,
         Boolean closeConnect = false);

      void sendHttpError(
         Uint32 queueId, 
         const String& status,
         const String& cimError = String::EMPTY,
         const String& messageBody = String::EMPTY,
         Boolean closeConnect = false);

      virtual void handleEnqueue(Message *);

      virtual void handleEnqueue();

      void handleHTTPMessage(HTTPMessage* httpMessage);

// l10n
      void handleMethodCall(
         Uint32 queueId,
         HttpMethod httpMethod,
         char* content,
         Uint32 contentLength,
         const String& cimProtocolVersionInHeader,
         const String& cimMethodInHeader,
         const String& cimObjectInHeader,
         String authType,
         String userName,
         const AcceptLanguageList& httpAcceptLanguages,
         const ContentLanguageList& httpContentLanguages,
         Boolean closeConnect);

      CIMCreateClassRequestMessage* decodeCreateClassRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMGetClassRequestMessage* decodeGetClassRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMModifyClassRequestMessage* decodeModifyClassRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMEnumerateClassNamesRequestMessage* decodeEnumerateClassNamesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMEnumerateClassesRequestMessage* decodeEnumerateClassesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMDeleteClassRequestMessage* decodeDeleteClassRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMCreateInstanceRequestMessage* decodeCreateInstanceRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMGetInstanceRequestMessage* decodeGetInstanceRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMModifyInstanceRequestMessage* decodeModifyInstanceRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMEnumerateInstanceNamesRequestMessage* decodeEnumerateInstanceNamesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMEnumerateInstancesRequestMessage* decodeEnumerateInstancesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMDeleteInstanceRequestMessage* decodeDeleteInstanceRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMSetQualifierRequestMessage* decodeSetQualifierRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMGetQualifierRequestMessage* decodeGetQualifierRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMEnumerateQualifiersRequestMessage* decodeEnumerateQualifiersRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMDeleteQualifierRequestMessage* decodeDeleteQualifierRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMReferenceNamesRequestMessage* decodeReferenceNamesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMReferencesRequestMessage* decodeReferencesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMAssociatorNamesRequestMessage* decodeAssociatorNamesRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMAssociatorsRequestMessage* decodeAssociatorsRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMGetPropertyRequestMessage* decodeGetPropertyRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMSetPropertyRequestMessage* decodeSetPropertyRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMExecQueryRequestMessage* decodeExecQueryRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMNamespaceName& nameSpace);

      CIMInvokeMethodRequestMessage* decodeInvokeMethodRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const CIMObjectPath& reference,
	 const String& cimMethodName);
        
      /** Sets the flag to indicate whether or not the CIMServer is 
	  shutting down.
      */
      void setServerTerminating(Boolean flag);

   private:

	//  Do not make _outputQueue an AutoPtr.
      MessageQueueService* _outputQueue; 

      // Queue where responses should be enqueued.
      Uint32 _returnQueueId;

      // Flag to indicate whether or not the CIMServer is shutting down.
      Boolean _serverTerminating;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationRequestDecoder_h */
