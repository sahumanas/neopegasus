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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Mike Day (mdday@us.ibm.com)
//              Yi Zhou (yi_zhou@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//		Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusDispatcher_Dispatcher_h
#define PegasusDispatcher_Dispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/OperationContext.h>

#include <Pegasus/Server/CIMServer.h>

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN


/* Class to manage the aggregation of data required by post processors. This
    class is private to the dispatcher. An instance is created by the operation
    dispatcher to aggregate request and response information and used by the
    post processor to aggregate responses together.
*/
class PEGASUS_SERVER_LINKAGE OperationAggregate
{
public:

    OperationAggregate(CIMRequestMessage* request,
                                Uint32 msgRequestType,
                                String messageId)
    : _messageId(messageId), _msgRequestType(msgRequestType),
      _request(request), _totalIssued(0), _magicNumber(12345)
    {}

    ~OperationAggregate()
    {
        _magicNumber = 0;
        delete _request;
    }

    void setTotalIssued(Uint32 i)
    {
	_totalIssued = i;
    }

    Boolean valid() {return(_magicNumber == 12345)? true: false; }

    Uint32 totalIssued() { return _totalIssued; }

    // Append a new entry to the response list
    void appendResponse(CIMResponseMessage* response)
    {
        _appendResponseMutex.lock(pegasus_thread_self());
	_responseList.append(response);
        _appendResponseMutex.unlock();
    }

    Uint32 numberResponses() { return _responseList.size(); }

    CIMResponseMessage* getResponse(const Uint32& pos)
    {
	return _responseList[pos];
    }

    void deleteResponse(const Uint32&pos)
    {
        delete _responseList[pos];
        _responseList.remove(pos);
    }

    Uint32 getRequestType(){ return _msgRequestType;}
    
    String _messageId;
    Uint32 _msgRequestType;
    Uint32 dest;
    Array<String> classes;
    Array<String> serviceNames;
    Array<String> controlProviderNames;
    Array<String> propertyList;
private:
    /** Hidden (unimplemented) copy constructor */
    OperationAggregate(const OperationAggregate& x){ }

    Array<CIMResponseMessage*> _responseList;
    Mutex _appendResponseMutex;
    CIMRequestMessage* _request;
    Uint32 _totalIssued;
    Uint32 _magicNumber;
};
class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher : public MessageQueueService
{
public:

      typedef MessageQueueService Base;

      CIMOperationRequestDispatcher(
    	 CIMRepository* repository,
         ProviderRegistrationManager* providerRegistrationManager);

      virtual ~CIMOperationRequestDispatcher();

      virtual void handleEnqueue(Message *);

      virtual void handleEnqueue();

      void handleGetClassRequest(
    	 CIMGetClassRequestMessage* request);

      void handleGetInstanceRequest(
    	 CIMGetInstanceRequestMessage* request);

      void handleDeleteClassRequest(
    	 CIMDeleteClassRequestMessage* request);

      void handleDeleteInstanceRequest(
          CIMDeleteInstanceRequestMessage* request);

      void handleCreateClassRequest(
          CIMCreateClassRequestMessage* request);

      void handleCreateInstanceRequest(
          CIMCreateInstanceRequestMessage* request);

      void handleModifyClassRequest(
    	 CIMModifyClassRequestMessage* request);

      void handleModifyInstanceRequest(
    	 CIMModifyInstanceRequestMessage* request);

      void handleEnumerateClassesRequest(
    	 CIMEnumerateClassesRequestMessage* request);

      void handleEnumerateClassNamesRequest(
    	 CIMEnumerateClassNamesRequestMessage* request);

      void handleEnumerateInstancesRequest(
    	 CIMEnumerateInstancesRequestMessage* request);

      void handleEnumerateInstanceNamesRequest(
    	 CIMEnumerateInstanceNamesRequestMessage* request);

      void handleAssociatorsRequest(
    	 CIMAssociatorsRequestMessage* request);

      void handleAssociatorNamesRequest(
    	 CIMAssociatorNamesRequestMessage* request);

      void handleReferencesRequest(
    	 CIMReferencesRequestMessage* request);

      void handleReferenceNamesRequest(
          CIMReferenceNamesRequestMessage* request);

      void handleGetPropertyRequest(
    	 CIMGetPropertyRequestMessage* request);

      void handleSetPropertyRequest(
    	 CIMSetPropertyRequestMessage* request);

      void handleGetQualifierRequest(
    	 CIMGetQualifierRequestMessage* request);

      void handleSetQualifierRequest(
    	 CIMSetQualifierRequestMessage* request);

      void handleDeleteQualifierRequest(
    	 CIMDeleteQualifierRequestMessage* request);

      void handleEnumerateQualifiersRequest(
    	 CIMEnumerateQualifiersRequestMessage* request);

      void handleExecQueryRequest(
    	 CIMExecQueryRequestMessage* request);

      void handleInvokeMethodRequest(
    	 CIMInvokeMethodRequestMessage* request);

      static void _forwardForAggregationCallback(AsyncOpNode *, 
					   MessageQueue *, 
					   void *);
      static void _forwardRequestCallback(AsyncOpNode *, 
					   MessageQueue *, 
					   void *);
      
      // Response Handler functions
      void handleOperationResponseAggregation(
			OperationAggregate* poA);
      
      static void handleEnumerateInstancesResponse(OperationAggregate* poA);

      static void handleEnumerateInstanceNamesResponse(OperationAggregate* poA);


   protected:

	/** _getSubClassNames - Gets the names of all subclasses of the defined
	    class (including the class) and returns it in an array of strings. Uses 
	    a similar function in the repository class to get the names.
	    @param namespace
	    @param className
	    @return Array of strings with class names.  Note that there should be 
	    at least one classname in the array (the input name)
	    Note that there is a special exception to this function, the __namespace
	    class which does not have any representation in the class repository.
	    @exception CIMException(CIM_ERR_INVALID_CLASS)
	*/
       Array<String> _getSubClassNames(
	    String& nameSpace,
	    String& className) throw(CIMException);
       
       Boolean _lookupInternalProvider(
        const String& nameSpace,
        const String& className,
        String& service,
        String& provider);

      String _lookupInstanceProvider(
    	const String& nameSpace, const String& className);

      Boolean _lookupNewInstanceProvider(
    	const String& nameSpace, 
    	const String& className,
    	String& serviceName,
    	String& controlProviderName);

      Array<String> _lookupAssociationProvider(
    	const String& nameSpace, const String& className,
        const String& assocClassName = String::EMPTY,
        const String& resultClassName = String::EMPTY);

      String _lookupMethodProvider(const String& nameSpace,
    	const String& className, const String& methodName);

      void _forwardRequestToService(
        const String& serviceName,
        CIMRequestMessage* request,
        CIMResponseMessage*& response);

      void _forwardRequestToControlProvider(
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        CIMResponseMessage*& response);

      void _forwardRequestForAggregation(
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        OperationAggregate* poA);

      void _forwardRequest(
        const String& className,
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request);

      void _enqueueResponse(
          CIMRequestMessage* request, CIMResponseMessage* response);
      
      CIMValue _convertValueType(const CIMValue& value, CIMType type);

      void _fixInvokeMethodParameterTypes(CIMInvokeMethodRequestMessage* request);

      void _fixSetPropertyValueType(CIMSetPropertyRequestMessage* request);

      CIMRepository * _repository;

      ProviderRegistrationManager* _providerRegistrationManager;

      AtomicInt _dying;

      Boolean _enableAssociationTraversal;

      // << Tue Feb 12 08:48:09 2002 mdd >> meta dispatcher integration
      virtual void _handle_async_request(AsyncRequest *req);
   private:
      static void _handle_enqueue_callback(AsyncOpNode *, MessageQueue *, void *);

};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
