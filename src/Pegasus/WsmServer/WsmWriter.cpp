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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cstdio>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Buffer.h>
#include <Pegasus/Common/StrLit.h>
#include <Pegasus/Common/StringConversion.h>
#include "WsmConstants.h"
#include "WsmUtils.h"
#include "WsmWriter.h"
#include "WsmSelectorSet.h"

PEGASUS_NAMESPACE_BEGIN

inline void _writeNewlineForReadability(Buffer& out)
{
    // Uncomment this statement for XML response readability.
    //out << '\n';
}

//-----------------------------------------------------------------------------
//
// WsmWriter::formatHttpErrorRspMessage()
//
//-----------------------------------------------------------------------------

Buffer WsmWriter::formatHttpErrorRspMessage(
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    return XmlWriter::formatHttpErrorRspMessage(status, cimError, errorDetail);
}

Buffer WsmWriter::formatSoapFault(
    const SoapNotUnderstoodFault& fault,
    const String& messageId,
    const String& relatesTo,
    HttpMethod httpMethod,
    Uint32& httpHeaderSize)
{
    Buffer out(WSM_MIN_MAXENVELOPESIZE_VALUE);
    String action = String(WsmNamespaces::supportedNamespaces[
        WsmNamespaces::WS_ADDRESSING].extendedName) + String("/fault");
    ContentLanguageList msgLang = fault.getMessageLanguage();

    _appendHTTPResponseHeader(out, action, httpMethod, msgLang, true);
    httpHeaderSize = out.size();

    _appendSoapEnvelopeStart(out);
    _appendSoapHeaderStart(out);

    // Append the 'NotUnderstood' tag
    out << STRLIT("<");
    out << WsmNamespaces::supportedNamespaces[
        WsmNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":NotUnderstood qname=\"ns:");
    out << fault.getHeaderName();
    out << STRLIT("\" xmlns:ns=\"");
    out << fault.getNamespace();
    out << STRLIT("\"/>");
    _writeNewlineForReadability(out);

    _appendSoapHeader(out, action, messageId, relatesTo);
    _appendSoapHeaderEnd(out);
    _appendSoapBodyStart(out);

    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Fault"));
    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Code"));
    String codeValue = String(WsmNamespaces::supportedNamespaces[
        WsmNamespaces::SOAP_ENVELOPE].localName) + String(":MustUnderstand");
    appendTagValue(
        out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Value"), codeValue);
    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Code"));

    String msg = fault.getMessage();
    if (msg.size() != 0)
    {
        String lang;
        if (msgLang.size() == 0)
            lang = "en-US";
        else
            lang = msgLang.getLanguageTag(0).toString();

        appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Reason"));
        appendTagValue(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Text"),
            msg, "xml:lang", lang);
        appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Reason"));
    }

    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Fault"));
    _appendSoapBodyEnd(out);
    _appendSoapEnvelopeEnd(out);
    return out;
}

Buffer WsmWriter::formatWsmFault(
    const WsmFault& fault,
    const String& messageId,
    const String& relatesTo,
    HttpMethod httpMethod,
    Uint32& httpHeaderSize)
{
    Buffer out(WSM_MIN_MAXENVELOPESIZE_VALUE);
    String action(fault.getAction());
    ContentLanguageList reasonLang = fault.getReasonLanguage();

    _appendHTTPResponseHeader(out, action, httpMethod, reasonLang, true);
    httpHeaderSize = out.size();

    _appendSoapEnvelopeStart(out);
    _appendSoapHeaderStart(out);
    _appendSoapHeader(out, action, messageId, relatesTo);
    _appendSoapHeaderEnd(out);
    _appendSoapBodyStart(out);

    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Fault"));
    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Code"));
    appendTagValue(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Value"),
        fault.getCode());
    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Subcode"));
    appendTagValue(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Value"),
        fault.getSubcode());
    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Subcode"));
    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Code"));

    String reason = fault.getReason();
    if (reason.size() != 0)
    {
        String lang;
        if (reasonLang.size() == 0)
            lang = "en-US";
        else
            lang = reasonLang.getLanguageTag(0).toString();

        appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Reason"));
        appendTagValue(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Text"),
                       reason, "xml:lang", lang);
        appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Reason"));
    }

    String detail = fault.getFaultDetail();
    if (detail.size() != 0)
    {
        appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Detail"));
        appendTagValue(
            out, WsmNamespaces::WS_MAN, STRLIT("FaultDetail"), detail);
        appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Detail"));
    }

    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Fault"));
    _appendSoapBodyEnd(out);
    _appendSoapEnvelopeEnd(out);
    return out;
}

Buffer WsmWriter::formatWsmRspMessage(
    const String& action,
    const String& messageId,
    const String& relatesTo,
    HttpMethod httpMethod,
    const ContentLanguageList& contentLanguages,
    const Buffer& body,
    const Buffer& headers,
    Uint32& httpHeaderSize)
{
    Buffer out(WSM_MIN_MAXENVELOPESIZE_VALUE);

    _appendHTTPResponseHeader(
        out, action, httpMethod, contentLanguages, false);
    httpHeaderSize = out.size();

    _appendSoapEnvelopeStart(out, contentLanguages);
    _appendSoapHeaderStart(out);
    _appendSoapHeader(out, action, messageId, relatesTo);
    out << headers;
    _appendSoapHeaderEnd(out);
    _appendSoapBodyStart(out);
    out << body;
    _appendSoapBodyEnd(out);
    _appendSoapEnvelopeEnd(out);

    return out;
}

void WsmWriter::appendInstanceElement(
    Buffer& out,
    WsmInstance& instance,
    Boolean isEmbedded)
{
    // Class opening element:
    out << STRLIT("<class:") << instance.getClassName();
    out << STRLIT(" xmlns:class=\"");
    out << WsmNamespaces::supportedNamespaces[WsmNamespaces::WS_CIM_SCHEMA].
        extendedName;
    out << STRLIT("/") << instance.getClassName();
    out << STRLIT("\"");

    // DSP0230, section 7.2.5.2. The property element MUST contain an 
    // xsi:type attribute with the XSD type of the class of the instance 
    // (see section 7.3.1).
    if (isEmbedded)
    {
        out << STRLIT(" ");
        out << WsmNamespaces::supportedNamespaces[
            WsmNamespaces::XML_SCHEMA_INSTANCE].localName;
        out << STRLIT(":type=\"");
        out << instance.getClassName();
        out << STRLIT("_Type\"");
    }
    out << STRLIT(">");
    _writeNewlineForReadability(out);

    // Sort properties before writing them out
    instance.sortProperties();

    // Properties:
    for (Uint32 i = 0, n = instance.getPropertyCount(); i < n; i++)
        appendPropertyElement(out, instance.getProperty(i));

    // Class closing element:
    out << STRLIT("</class:") << instance.getClassName() << STRLIT(">");
    _writeNewlineForReadability(out);
}

void WsmWriter::appendPropertyElement(
    Buffer& out,
    WsmProperty& property)
{
    WsmValue val = property.getValue();
    String propName = property.getName();

    if (val.isNull())
    {
        out << STRLIT("<class:") << propName;
        out << " " << WsmNamespaces::supportedNamespaces[
            WsmNamespaces::XML_SCHEMA_INSTANCE].localName;
        out << STRLIT(":nil=\"true\"/>");
        return;
    }

    if (val.isArray())
    {
        switch (val.getType())
        {
            case WSMTYPE_REFERENCE:
            {
                Array<WsmEndpointReference> eprs;
                val.get(eprs);
                for (Uint32 i = 0, n = eprs.size(); i < n; i++)
                {
                    out << STRLIT("<class:") << propName << STRLIT(">");
                    _writeNewlineForReadability(out);
                    appendEPRElement(out, eprs[i]);
                    out << STRLIT("</class:") << propName << STRLIT(">");
                    _writeNewlineForReadability(out);
                }
                break;
            }
            case WSMTYPE_INSTANCE:
            {
                Array<WsmInstance> instances;
                val.get(instances);
                for (Uint32 i = 0, n = instances.size(); i < n; i++)
                {
                    out << STRLIT("<class:") << propName << STRLIT(">");
                    _writeNewlineForReadability(out);
                    appendInstanceElement(out, instances[i], true);
                    out << STRLIT("</class:") << propName << STRLIT(">");
                    _writeNewlineForReadability(out);
                }
                break;
            }
            case WSMTYPE_OTHER:
            {
                Array<String> strs;
                val.get(strs);
                for (Uint32 i = 0, n = strs.size(); i < n; i++)
                {
                    out << STRLIT("<class:") << propName << STRLIT(">");
                    _writeNewlineForReadability(out);
                    appendStringElement(out, strs[i]);
                    out << STRLIT("</class:") << propName << STRLIT(">");
                    _writeNewlineForReadability(out);
                }
                break;
            }
            default:
            {
                PEGASUS_ASSERT(0);
            }
        }
    }
    else
    {
        switch (val.getType())
        {
            case WSMTYPE_REFERENCE:
            {
                WsmEndpointReference epr;
                val.get(epr);
                out << STRLIT("<class:") << propName << STRLIT(">");
                _writeNewlineForReadability(out);
                appendEPRElement(out, epr);
                out << STRLIT("</class:") << propName << STRLIT(">");
                _writeNewlineForReadability(out);
                break;
            }
            case WSMTYPE_INSTANCE:
            {
                WsmInstance instance;
                val.get(instance);
                out << STRLIT("<class:") << propName << STRLIT(">");
                _writeNewlineForReadability(out);
                appendInstanceElement(out, instance, true);
                out << STRLIT("</class:") << propName << STRLIT(">");
                _writeNewlineForReadability(out);
                break;
            }
            case WSMTYPE_OTHER:
            {
                String str;
                val.get(str);
                out << STRLIT("<class:") << propName << STRLIT(">");
                _writeNewlineForReadability(out);
                appendStringElement(out, str);
                out << STRLIT("</class:") << propName << STRLIT(">");
                _writeNewlineForReadability(out);
                break;
            }
            default:
            {
                PEGASUS_ASSERT(0);
            }
        }
    }
}

void WsmWriter::appendEPRElement(
    Buffer& out,
    const WsmEndpointReference& epr)
{
    appendTagValue(
        out, WsmNamespaces::WS_ADDRESSING, STRLIT("Address"), epr.address);
    appendStartTag(
        out, WsmNamespaces::WS_ADDRESSING, STRLIT("ReferenceParameters"));
    appendTagValue(
        out, WsmNamespaces::WS_MAN, STRLIT("ResourceURI"), epr.resourceUri);

    if (epr.selectorSet && epr.selectorSet->selectors.size() > 0)
    {
        appendStartTag(out, WsmNamespaces::WS_MAN, STRLIT("SelectorSet"));
        for (Uint32 i = 0, n = epr.selectorSet->selectors.size(); i < n; i++)
        {
            WsmSelector selector = epr.selectorSet->selectors[i];
            appendStartTag(out, WsmNamespaces::WS_MAN, STRLIT("Selector"),
                "Name", selector.name);
            if (selector.type == WsmSelector::EPR)
            {
                appendStartTag(out,
                    WsmNamespaces::WS_ADDRESSING,
                    STRLIT("EndpointReference"));
                appendEPRElement(out, selector.epr);
                appendEndTag(out,
                    WsmNamespaces::WS_ADDRESSING,
                    STRLIT("EndpointReference"));
            }
            else
            {
                out << selector.value;
                _writeNewlineForReadability(out);
            }
            appendEndTag(out, WsmNamespaces::WS_MAN, STRLIT("Selector"));
        }
        appendEndTag(out, WsmNamespaces::WS_MAN, STRLIT("SelectorSet"));
    }
    appendEndTag(
        out, WsmNamespaces::WS_ADDRESSING, STRLIT("ReferenceParameters"));
}

void WsmWriter::appendStringElement(
    Buffer& out,
    const String& str)
{
    out << str;
    _writeNewlineForReadability(out);
}

void WsmWriter::appendStartTag(
    Buffer& out,
    WsmNamespaces::Type nsType,
    const StrLit& tagName,
    const char* attrName,
    const String& attrValue)
{
    out << STRLIT("<");
    out << WsmNamespaces::supportedNamespaces[nsType].localName;
    out << STRLIT(":") << tagName;
    if (attrName)
    {
        out << STRLIT(" ") << attrName << STRLIT("=");
        out << STRLIT("\"") << attrValue << STRLIT("\"");
    }
    out << STRLIT(">");
    _writeNewlineForReadability(out);
}

void WsmWriter::appendEndTag(
    Buffer& out,
    WsmNamespaces::Type nsType,
    const StrLit& tagName)
{
    out << STRLIT("</");
    out << WsmNamespaces::supportedNamespaces[nsType].localName;
    out << STRLIT(":") << tagName << STRLIT(">");
    _writeNewlineForReadability(out);
}

void WsmWriter::appendTagValue(
    Buffer& out,
    WsmNamespaces::Type nsType,
    const StrLit& tagName,
    const String& value,
    const char* attrName,
    const String& attrValue)
{
    appendStartTag(out, nsType, tagName, attrName, attrValue);
    out << value;
    _writeNewlineForReadability(out);
    appendEndTag(out, nsType, tagName);
}

void WsmWriter::_appendHTTPResponseHeader(
    Buffer& out,
    const String& action,
    HttpMethod httpMethod,
    const ContentLanguageList& contentLanguages,
    Boolean isFault,
    Uint32 contentLength)
{
    if (isFault)
    {
        out << STRLIT("HTTP/1.1 " HTTP_STATUS_INTERNALSERVERERROR "\r\n");
    }
    else
    {
        out << STRLIT("HTTP/1.1 " HTTP_STATUS_OK "\r\n");
    }

    out << STRLIT("Content-Type: application/soap+xml;charset=UTF-8\r\n");
    OUTPUT_CONTENTLENGTH(out, contentLength);

    if (contentLanguages.size() > 0)
    {
        out << STRLIT("Content-Language: ") << contentLanguages
            << STRLIT("\r\n");
    }
    if (httpMethod == HTTP_METHOD_M_POST)
    {
        char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

        out << STRLIT("Ext:\r\n");
        out << STRLIT("Cache-Control: no-cache\r\n");
        out << STRLIT("Man: urn:schemas-xmlsoap-org:soap.v1; ns=") << nn
            << STRLIT("\r\n");
        out << nn << STRLIT("-SOAPAction: ") << action << STRLIT("\r\n");
    }
    else
    {
        out << STRLIT("SOAPAction: ");
        out << action << STRLIT("\r\n");
    }
    out << STRLIT("\r\n");

    out << STRLIT("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
    _writeNewlineForReadability(out);
}

void WsmWriter::_appendSoapEnvelopeStart(
    Buffer& out,
    const ContentLanguageList& contentLanguages)
{
    out << STRLIT("<");
    out << WsmNamespaces::supportedNamespaces[
        WsmNamespaces::SOAP_ENVELOPE].localName;
    out << STRLIT(":Envelope");

    // DSP0226 R6.3-4: In any response, event, or singleton message, the
    // service should include the xml:lang attribute in the s:Envelope (or
    // other elements) to signal to the receiver that localized content
    // appears in the body of the message. This attribute may be omitted
    // if no descriptive content appears in the body. Including the
    // xml:lang attribute is not an error, even if no descriptive content
    // occurs.

    if (contentLanguages.size() > 0)
    {
        out << STRLIT(" xml:lang=\"");
        out << contentLanguages.getLanguageTag(0).toString() << STRLIT("\"");
    }

    for (Uint32 i = 0; i < WsmNamespaces::LAST; i++)
    {
        _writeNewlineForReadability(out);
        out << STRLIT(" xmlns:");
        out << WsmNamespaces::supportedNamespaces[i].localName;
        out << STRLIT("=\"");
        out << WsmNamespaces::supportedNamespaces[i].extendedName;
        out << STRLIT("\"");
    }
    out << STRLIT(">");
    _writeNewlineForReadability(out);
}

void WsmWriter::_appendSoapEnvelopeEnd(Buffer& out)
{
    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Envelope"));
}

void WsmWriter::_appendSoapHeaderStart(Buffer& out)
{
    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Header"));
}

void WsmWriter::_appendSoapHeaderEnd(Buffer& out)
{
    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Header"));
}

void WsmWriter::_appendSoapBodyStart(Buffer& out)
{
    appendStartTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Body"));
}

void WsmWriter::_appendSoapBodyEnd(Buffer& out)
{
    appendEndTag(out, WsmNamespaces::SOAP_ENVELOPE, STRLIT("Body"));
}

void WsmWriter::_appendSoapHeader(
    Buffer& out,
    const String& action,
    const String& messageId,
    const String& relatesTo)
{
    // Add <wsa:To> entry
    appendStartTag(out, WsmNamespaces::WS_ADDRESSING, STRLIT("To"));
    // Note: The reply is always written on the requestor's connection.
    // The WsmRequestDecoder checks that the wsa:ReplyTo header is anonymous.
    out << STRLIT(WSM_ADDRESS_ANONYMOUS);
    _writeNewlineForReadability(out);
    appendEndTag(out, WsmNamespaces::WS_ADDRESSING, STRLIT("To"));

    // Add <wsa:Action> entry
    appendTagValue(out, WsmNamespaces::WS_ADDRESSING, STRLIT("Action"), action);

    // Add <wsa:MessageID> entry
    appendTagValue(
        out, WsmNamespaces::WS_ADDRESSING, STRLIT("MessageID"), messageId);

    // Add <wsa:RelatesTo> entry
    appendTagValue(
        out, WsmNamespaces::WS_ADDRESSING, STRLIT("RelatesTo"), relatesTo);
}

PEGASUS_NAMESPACE_END