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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Config/ConfigManager.h>
#include "CIMExportRequestDecoder.h"
#include <Pegasus/Common/CommonUTF.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestDecoder::CIMExportRequestDecoder(
   MessageQueueService* outputQueue,
   Uint32 returnQueueId)
   :   
   Base(PEGASUS_QUEUENAME_EXPORTREQDECODER),
   _outputQueue(outputQueue),
   _returnQueueId(returnQueueId),
   _serverTerminating(false)
{

}

CIMExportRequestDecoder::~CIMExportRequestDecoder()
{

}

void CIMExportRequestDecoder::sendResponse(
   Uint32 queueId, 
   Array<Sint8>& message)
{
   MessageQueue* queue = MessageQueue::lookup(queueId);

   if (queue)
   {
      HTTPMessage* httpMessage = new HTTPMessage(message);
      queue->enqueue(httpMessage);
   }
}

void CIMExportRequestDecoder::sendEMethodError(
   Uint32 queueId, 
   HttpMethod httpMethod,
   const String& messageId,
   const String& eMethodName,
   const CIMException& cimException) 
{
    Array<Sint8> message;
    message = XmlWriter::formatSimpleEMethodErrorRspMessage(
        eMethodName,
        messageId,
        httpMethod,
        cimException);

    sendResponse(queueId, message);
}

void CIMExportRequestDecoder::sendHttpError(
   Uint32 queueId,
   const String& status,
   const String& cimError,
   const String& messageBody)
{
    Array<Sint8> message;
    message = XmlWriter::formatHttpErrorRspMessage(
        status,
        cimError,
        messageBody);

    sendResponse(queueId, message);
}

void CIMExportRequestDecoder::handleEnqueue(Message *message)
{
   if (!message)
      return;

   switch (message->getType())
   {
      case HTTP_MESSAGE:
	 handleHTTPMessage((HTTPMessage*)message);
	 break;
   }

   delete message;
}


void CIMExportRequestDecoder::handleEnqueue()
{
   Message* message = dequeue();
   if(message)
      handleEnqueue(message);
}

//------------------------------------------------------------------------------
//
// From the HTTP/1.1 Specification (RFC 2626):
//
// Both types of message consist of a start-line, zero or more header fields 
// (also known as "headers"), an empty line (i.e., a line with nothing 
// preceding the CRLF) indicating the end of the header fields, and possibly 
// a message-body.
//
// Example CIM request:
//
//     M-POST /cimom HTTP/1.1 
//     HOST: www.erewhon.com 
//     Content-Type: application/xml; charset="utf-8" 
//     Content-Length: xxxx 
//     Man: http://www.dmtf.org/cim/operation ; ns=73 
//     73-CIMExport: MethodRequest
//     73-CIMExportMethod: ExportIndication
//     73-CIMObject: root/cimv2 
// 
//------------------------------------------------------------------------------

void CIMExportRequestDecoder::handleHTTPMessage(HTTPMessage* httpMessage)
{
   // Save queueId:

   Uint32 queueId = httpMessage->queueId;

   // Save userName:

   String userName;

   // Bug #351:
   if ( httpMessage->message.size() == 0 ) 
   {
	// The message is empty; just drop it. The connection has
	// probably closed.
	return;
   }
   // </bug>
   if ( httpMessage->authInfo->isAuthenticated() )
   {
      userName = httpMessage->authInfo->getAuthenticatedUser();
   }

   // Parse the HTTP message:

   String startLine;
   Array<HTTPHeader> headers;
   Sint8* content;
   Uint32 contentLength;

   httpMessage->parse(startLine, headers, contentLength);

   // Parse the request line:

   String methodName;
   String requestUri;
   String httpVersion;
   HttpMethod httpMethod = HTTP_METHOD__POST;

   // ATTN-RK-P3-20020404: The requestUri may need to be pruned of the host
   // name.  All we care about at this point is the path.
   HTTPMessage::parseRequestLine(
      startLine, methodName, requestUri, httpVersion);

   //
   //  Set HTTP method for the request
   //
   if (methodName == "M-POST")
   {
       httpMethod = HTTP_METHOD_M_POST;
   }

   // Unsupported methods are caught in the HTTPAuthenticatorDelegator
   //<Bug #351>
   //PEGASUS_ASSERT(methodName == "M-POST" || methodName == "POST");
   if( methodName != "M-POST" && methodName != "POST" )
    {
       sendHttpError(
          queueId,
          HTTP_STATUS_NOTIMPLEMENTED,
          "Only POST and M-POST are implemented" );
       return;
   }
   //</bug>
   //
   // Not true: "Mismatch of method and version is caught in HTTPAuthenticatorDelegator", bug #351 fixes this:
   //
   //PEGASUS_ASSERT (!((httpMethod == HTTP_METHOD_M_POST) &&
   //                  (httpVersion == "HTTP/1.0")));
   if( (httpMethod == HTTP_METHOD_M_POST) &&
        (httpVersion == "HTTP/1.0") )
   {
       sendHttpError(queueId,
		HTTP_STATUS_BADREQUEST,
		"M-POST method is not valid with version 1.0" );
       return;
   }
   //</bug>
   // Process M-POST and POST messages:
   String cimContentType;
   String cimExport;
   String cimExportBatch;
   Boolean cimExportBatchFlag;
   String cimProtocolVersion;
   String cimExportMethod;

   // Validate the "Content-Type" header:

   // 4.2.2. Accept-Charset
   //  If a CIM client includes an Accept-Charset header in a request,
   //  it MUST specify a value which allows the CIM Server or CIM Listener
   //  to return an entity body using the character set "utf-8".
   //  A CIM server or CIM Listener MUST accept any value for this header
   //  which implies that "utf-8" is an acceptable character set for an
   //  response entity.  A CIM Server or CIM Listener SHOULD return
   //  "406 Not Acceptable" if the Accept-Charset header indicates that
   //  this character set is not acceptable. 
   //
   //  If a CIM Server or CIM Listener decides to accept a request to return
   //  an entity using a character set other than "utf-8", the nature
   //  of the response is outside of the domain of this specification. 


   Boolean contentTypeHeaderFound = HTTPMessage::lookupHeader(headers,
							      "Content-Type",
							      cimContentType,
							      true);

   Uint32 validateSize= httpMessage->message.size();
   Uint8  *validateContent = (Uint8*) httpMessage->message.getData();
   Uint32 count;

   // If the Content-Type header is missing we will assume a charset of ISO-8859-1
   if(!contentTypeHeaderFound)
   {
       // We will verify that the characters fall in the 7-bit ASCII range
       for(count = 0;count < validateSize; ++count)
       {
	   if(validateContent[count] > 0x7F)
	   {
	       sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "unsupported-Content-Type",
			     String("8-bit characters detected, Content-Type value is required."));
	       return;
	   } 
       }
   }
   // Validating the charset is utf-8
   else if(!String::equalNoCase(cimContentType, "application/xml; charset=\"utf-8\""))
   {
       sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "unsupported-Content-Type",
		     String("Content-Type value \"") + cimContentType +
		     "\" is not supported.");
       return; 
   }
   // Validating content falls within UTF8
   else
   {
       char currentChar;
       count = 0;
       while(count<validateSize)
       {
	   UTF8_NEXT(validateContent,count,currentChar);
	   if (!(String::isUTF8(&validateContent[count])))
	   {
	       sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "unsupported-Content-Type",
			     String("Invalid UTF-8 character detected."));
	       return; 
	   }
       }
   }


   // Validate the "CIMExport" header:

   Boolean exportHeaderFound = HTTPMessage::lookupHeader(
      headers, "CIMExport", cimExport, true);
   // If the CIMExport header was missing, the HTTPAuthenticatorDelegator
   // would not have passed the message to us.

   // <bug #351>
   // PEGASUS_ASSERT(exportHeaderFound);
   if (!exportHeaderFound) 
   {
	sendHttpError(queueId,
                    HTTP_STATUS_BADREQUEST,
                    "Export header not found");
   }
   // </bug>
   if (!String::equalNoCase(cimExport, "MethodRequest"))
   {
      // The Specification for CIM Operations over HTTP reads:
      //     3.3.5. CIMExport
      //     If a CIM Listener receives CIM Export request with this
      //     header, but with a missing value or a value that is not
      //     "MethodRequest", then it MUST fail the request with
      //     status "400 Bad Request". The CIM Server MUST include a
      //     CIMError header in the response with a value of
      //     unsupported-operation.
      sendHttpError(queueId,
                    HTTP_STATUS_BADREQUEST,
                    "unsupported-operation");
      return;
   }

   // Validate the "CIMExportBatch" header:

   cimExportBatchFlag = HTTPMessage::lookupHeader(
          headers, "CIMExportBatch", cimExportBatch, true);
   if (cimExportBatchFlag)
   {
      // The Specification for CIM Operations over HTTP reads:
      //     3.3.10. CIMExportBatch
      //     If a CIM Listener receives CIM Export Request for which the
      //     CIMExportBatch header is present, but the Listener does not
      //     support Multiple Exports, then it MUST fail the request and
      //     return a status of "501 Not Implemented".
      sendHttpError(queueId,
                    HTTP_STATUS_NOTIMPLEMENTED,
                    "multiple-requests-unsupported");
      return;
   }

   // Save these headers for later checking

   if (!HTTPMessage::lookupHeader(
          headers, "CIMProtocolVersion", cimProtocolVersion, true))
   {
      // Mandated by the Specification for CIM Operations over HTTP
      cimProtocolVersion.assign("1.0");
   }

   if (HTTPMessage::lookupHeader(
          headers, "CIMExportMethod", cimExportMethod, true))
   {
      if (cimExportMethod == String::EMPTY)
      {
         // This is not a valid value, and we use EMPTY to mean "absent"
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch");
         return;
      }
   }

// l10n start
   AcceptLanguages acceptLanguages = AcceptLanguages::EMPTY;;
   ContentLanguages contentLanguages = ContentLanguages::EMPTY;
   try 
   { 
		// Get and validate the Accept-Language header, if set 	   	  
		String acceptLanguageHeader;		
		if (HTTPMessage::lookupHeader(
		      headers, 
	    	  "Accept-Language", 
		      acceptLanguageHeader,
	    	  false) == true)
	    {
			acceptLanguages = AcceptLanguages(acceptLanguageHeader);
	    }

		// Get and validate the Content-Language header, if set 	
		String contentLanguageHeader;
		if (HTTPMessage::lookupHeader(
		      headers, 
	    	  "Content-Language", 
		      contentLanguageHeader,
	    	  false) == true)
	    {						
			contentLanguages = ContentLanguages(contentLanguageHeader);      
	    }
   }			
   catch (Exception &e)
   {
		sendHttpError(queueId, HTTP_STATUS_BADREQUEST, 
					"request-not-valid",
                    e.getMessage());		         	
       	return;
   }        
// l10n end   

   // Zero-terminate the message:

   httpMessage->message.append('\0');

   // Calculate the beginning of the content from the message size and
   // the content length.  Subtract 1 to take into account the null
   // character we just added to the end of the message.

   content = (Sint8*) httpMessage->message.getData() +
      httpMessage->message.size() - contentLength - 1;

   // If it is a method call, then dispatch it to be handled:

// l10n
   handleMethodRequest(queueId, httpMethod, content, requestUri, 
                       cimProtocolVersion, cimExportMethod, userName,
                       acceptLanguages, contentLanguages);
}


void CIMExportRequestDecoder::handleMethodRequest(
   Uint32 queueId,
   HttpMethod httpMethod,
   Sint8* content,
   const String& requestUri,
   const String& cimProtocolVersionInHeader,
   const String& cimExportMethodInHeader,
   const String& userName,
	const AcceptLanguages& httpAcceptLanguages, // l10n
	const ContentLanguages& httpContentLanguages)	 
{
// l10n
	// Set the Accept-Language into the thread for this service.
	// This will allow all code in this thread to get
	// the languages for the messages returned to the client.
	Thread::setLanguages(new AcceptLanguages(httpAcceptLanguages));	

   //
   // If CIM Listener is shutting down, return error response
   //
   if (_serverTerminating)
   {
      sendHttpError(queueId, HTTP_STATUS_SERVICEUNAVAILABLE,
                    String::EMPTY,
                    "CIM Listener is shutting down.");
      return;
   }

   // Create a parser:

   XmlParser parser(content);
   XmlEntry entry;
   String messageId;
   const char* cimExportMethodName = "";
   Message* request;

   try
   {
      //
      // Process <?xml ... >
      //

      // These values are currently unused
      const char* xmlVersion = 0;
      const char* xmlEncoding = 0;

      XmlReader::getXmlDeclaration(parser, xmlVersion, xmlEncoding);

      // Expect <CIM ...>

      const char* cimVersion = 0;
      const char* dtdVersion = 0;

      XmlReader::getCimStartTag(parser, cimVersion, dtdVersion);

      if (strcmp(cimVersion, "2.0") != 0)
      {
         sendHttpError(queueId,
                       HTTP_STATUS_NOTIMPLEMENTED,
                       "unsupported-cim-version");
         return;
      }

      if (strcmp(dtdVersion, "2.0") != 0)
      {
         sendHttpError(queueId,
                       HTTP_STATUS_NOTIMPLEMENTED,
                       "unsupported-dtd-version");
         return;
      }

      // Expect <MESSAGE ...>

      String protocolVersion;

      if (!XmlReader::getMessageStartTag(
	     parser, messageId, protocolVersion))
      {
	 throw XmlValidationError(
	    parser.getLine(), "expected MESSAGE element");
      }

      // Validate that the protocol version in the header matches the XML

      if (!String::equalNoCase(protocolVersion, cimProtocolVersionInHeader))
      {
         sendHttpError(queueId,
                       HTTP_STATUS_BADREQUEST,
                       "header-mismatch");
         return;
      }

      // We only support protocol version 1.0

      if (!String::equalNoCase(protocolVersion, "1.0"))
      {
         // See Specification for CIM Operations over HTTP section 4.3
         sendHttpError(queueId,
                       HTTP_STATUS_NOTIMPLEMENTED,
                       "unsupported-protocol-version");
         return;
      }

      if (XmlReader::testStartTag(parser, entry, "MULTIEXPREQ"))
      {
         // We wouldn't have gotten here if CIMExportBatch header was
         // specified, so this must be indicative of a header mismatch
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch");
         return;
         // Future: When MULTIEXPREQ is supported, must ensure CIMExportMethod
         // header is absent, and CIMExportBatch header is present.
      }

      // Expect <SIMPLEEXPREQ ...>

      XmlReader::expectStartTag(parser, entry, "SIMPLEEXPREQ");
	
      // Expect <EXPMETHODCALL ...>

      if (!XmlReader::getEMethodCallStartTag(parser, cimExportMethodName))
      {
	 throw XmlValidationError(parser.getLine(), 
				  "expected EXPMETHODCALL element");
      }

      // The Specification for CIM Operations over HTTP reads:
      //     3.3.9. CIMExportMethod
      //
      //     This header MUST be present in any CIM Export Request
      //     message that contains a Simple Export Request. 
      //
      //     It MUST NOT be present in any CIM Export Response message,
      //     nor in any CIM Export Request message that is not a
      //     Simple Export Request. It MUST NOT be present in any CIM 
      //     Operation Request or Response message. 
      //
      //     The name of the CIM export method within a Simple Export
      //     Request is defined to be the value of the NAME attribute
      //     of the <EXPMETHODCALL> element. 
      //
      //     If a CIM Listener receives a CIM Export Request for which
      //     either:
      //
      //     - The CIMExportMethod header is present but has an invalid
      //       value, or; 
      //     - The CIMExportMethod header is not present but the Export
      //       Request Message is a Simple Export Request, or; 
      //     - The CIMExportMethod header is present but the Export
      //       Request Message is not a Simple Export Request, or; 
      //     - The CIMExportMethod header is present, the Export Request
      //       Message is a Simple Export Request, but the CIMIdentifier
      //       value (when unencoded) does not match the unique method
      //       name within the Simple Export Request, 
      //
      //     then it MUST fail the request and return a status of
      //     "400 Bad Request" (and MUST include a CIMError header in the
      //     response with a value of header-mismatch), subject to the 
      //     considerations specified in Errors. 
      if (!String::equalNoCase(cimExportMethodName, cimExportMethodInHeader))
      {
         // ATTN-RK-P3-20020404: How to decode cimExportMethodInHeader?
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch");
         return;
      }
	
      // This try block only catches CIMExceptions, because they must be
      // responded to with a proper EMETHODRESPONSE.  Other exceptions are
      // caught in the outer try block.
      try
      {
         // Delegate to appropriate method to handle:

         if (System::strcasecmp(cimExportMethodName, "ExportIndication") == 0)
         {
            request = decodeExportIndicationRequest(queueId, parser, messageId, requestUri);
         }
         else
         {
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
               String("Unrecognized export method: ") + cimExportMethodName);
         }
      }
      catch (CIMException& e)
      {
         sendEMethodError(
            queueId,
            httpMethod,
            messageId,
            cimExportMethodName,
            e);

         return;
      }

      // Expect </EXPMETHODCALL>

      XmlReader::expectEndTag(parser, "EXPMETHODCALL");

      // Expect </SIMPLEEXPREQ>

      XmlReader::expectEndTag(parser, "SIMPLEEXPREQ");

      // Expect </MESSAGE>

      XmlReader::expectEndTag(parser, "MESSAGE");

      // Expect </CIM>

      XmlReader::expectEndTag(parser, "CIM");
   }
   catch (XmlValidationError& e)
   {
       Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMExportRequestDecoder::handleMethodRequest - XmlValidationError exception has occurred. Message: $0",e.getMessage());

      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "request-not-valid",
                    e.getMessage());
      return;
   }
   catch (XmlSemanticError& e)
   {
       Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMExportRequestDecoder::handleMethodRequest - XmlSemanticError exception has occurred. Message: $0",e.getMessage());
      // ATTN-RK-P2-20020404: Is this the correct response for these errors?
      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "request-not-valid",
                    e.getMessage());
      return;
   }
   catch (XmlException& e)
   {
       Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMExportRequestDecoder::handleMethodRequest - XmlException has occurred. Message: $0",e.getMessage());

      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "request-not-well-formed",
                    e.getMessage());
      return;
   }
   catch (Exception& e)
   {
      // Don't know why I got this exception.  Seems like a bad thing.
      // Any exceptions we're expecting should be caught separately and
      // dealt with appropriately.  This is a last resort.
      sendHttpError(queueId, HTTP_STATUS_INTERNALSERVERERROR, String::EMPTY,
                    e.getMessage());
      return;
   }
   catch (...)
   {
      // Don't know why I got whatever this is.  Seems like a bad thing.
      // Any exceptions we're expecting should be caught separately and
      // dealt with appropriately.  This is a last resort.
      sendHttpError(queueId, HTTP_STATUS_INTERNALSERVERERROR);
      return;
   }

//l10n start
// l10n TODO - might want to move A-L and C-L to Message
// to make this more maintainable
	// Add the language headers to the request.
	// Note: Since the text of an export error response will be ignored
	// by the export client, ignore Accept-Language in the export request.
	// This will cause any export error response message to be sent in the
	// default language.
	CIMMessage * cimmsg = dynamic_cast<CIMMessage *>(request);
	if (cimmsg != NULL)
	{
		cimmsg->contentLanguages = httpContentLanguages;			
	}
	else
	{
		;	// l10n TODO - error back to client here	
	}
// l10n end	

   _outputQueue->enqueue(request);
}

CIMExportIndicationRequestMessage* CIMExportRequestDecoder::decodeExportIndicationRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& requestUri)
{
   CIMInstance instanceName;

   String destStr = requestUri.subString(requestUri.find ("/CIMListener") + 12, PEG_NOT_FOUND);

   for (const char* name; XmlReader::getEParamValueTag(parser, name);)
   {
      if (System::strcasecmp(name, "NewIndication") == 0)
	 XmlReader::getInstanceElement(parser, instanceName);

      XmlReader::expectEndTag(parser, "EXPPARAMVALUE");
   }
    
   CIMExportIndicationRequestMessage* request = new CIMExportIndicationRequestMessage(
      messageId,  
      destStr,
      instanceName,
      QueueIdStack(queueId, _returnQueueId));
    
   return(request);
}

void CIMExportRequestDecoder::setServerTerminating(Boolean flag)
{
   _serverTerminating = flag;
}

PEGASUS_NAMESPACE_END
