//%LICENSE////////////////////////////////////////////////////////////////
// 
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
//////////////////////////////////////////////////////////////////////////
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WsmReader_h
#define Pegasus_WsmReader_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/WsmServer/WsmInstance.h>
#include <Pegasus/WsmServer/WsmSelectorSet.h>
#include <Pegasus/WsmServer/WsmEndpointReference.h>
#include <Pegasus/WsmServer/WsmUtils.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_WSMSERVER_LINKAGE WsmReader
{
public:

    WsmReader(char* text);
    ~WsmReader();

    Boolean getXmlDeclaration(
        const char*& xmlVersion, const char*& xmlEncoding);

    Boolean testStartTag(
        XmlEntry& entry,
        int nsType,
        const char* tagName = 0);
    Boolean testStartOrEmptyTag(
        XmlEntry& entry,
        int nsType,
        const char* tagName = 0);
    Boolean testEndTag(int nsType, const char* tagName = 0);

    void expectStartTag(
        XmlEntry& entry,
        int nsType,
        const char* tagName);
    void expectStartOrEmptyTag(
        XmlEntry& entry,
        int nsType,
        const char* tagName);
    void expectEndTag(
        int nsType,
        const char* tagName);
    void expectContentOrCData(XmlEntry& entry);

    Boolean getAttributeValue(
        Uint32 lineNumber,
        XmlEntry& entry,
        const char* attributeName,
        String& attributeValue,
        Boolean required = true);

    Boolean mustUnderstand(XmlEntry& entry);

    Uint64 getEnumerationContext(XmlEntry& entry);
    Uint32 getUint32ElementContent(XmlEntry& entry, const char* name);
    const char* getElementContent(XmlEntry& entry);
    Boolean getElementStringValue(
        int nsType,
        const char* tagName,
        String& stringValue,
        Boolean required = false);

    Boolean getSelectorElement(WsmSelector& selector);
    Boolean getSelectorSetElement(WsmSelectorSet& selectorSet);
    void getEPRElement(WsmEndpointReference& endpointReference);
    Boolean getSelectorEPRElement(WsmEndpointReference& endpointReference);
    Boolean getInstanceEPRElement(WsmEndpointReference& endpointReference);

    void skipElement(XmlEntry& entry);
    Boolean next(XmlEntry& entry);

    void decodeRequestSoapHeaders(
        String& wsaMessageId,
        String& wsaTo,
        String& wsaAction,
        String& wsaFrom,
        String& wsaReplyTo,
        String& wsaFaultTo,
        String& wsmResourceUri,
        WsmSelectorSet& wsmSelectorSet,
        Uint32& wsmMaxEnvelopeSize,
        AcceptLanguageList& wsmLocale,
        Boolean& wsmRequestEpr,
        Boolean& wsmRequestItemCount);

    void decodeEnumerateBody(
        String& expiration, 
        WsmbPolymorphismMode& polymorphismMode, 
        WsenEnumerationMode& enumerationMode, 
        Boolean& optimized, 
        Uint32& maxElements);

    void decodePullBody(
        Uint64& enumerationContext, 
        String& maxTime, 
        Uint32& maxElements,
        Uint32& maxCharacters);

    void decodeReleaseBody(Uint64& enumerationContext);

    void getInstanceElement(WsmInstance& instance);
    Boolean getPropertyElement(
        int nsType,
        String& propName,
        WsmValue& propValue);
    void getValueElement(WsmValue& value, int nsType, const char* propNameTag);

private:

    XmlParser _parser;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmReader_h */
