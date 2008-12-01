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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WsmValue_h
#define Pegasus_WsmValue_h

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/WsmServer/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class WsmEndpointReference;
class WsmInstance;

enum WsmType
{
    WSMTYPE_REFERENCE,
    WSMTYPE_OBJECT,
    WSMTYPE_INSTANCE,
    WSMTYPE_OTHER
};


class PEGASUS_WSMSERVER_LINKAGE WsmValue
{
public:

    WsmValue();
    WsmValue(const WsmEndpointReference& ref);
    WsmValue(const Array<WsmEndpointReference>& ref);
    WsmValue(const WsmInstance& inst);
    WsmValue(const Array<WsmInstance>& inst);
    WsmValue(const String& str);
    WsmValue(const Array<String>& str);
    WsmValue(const WsmValue& val);

    ~WsmValue()
    {
        _release();
    }

    WsmValue& operator=(const WsmValue& val);

    Boolean isArray() const { return _isArray; }
    Boolean isNull() const { return _isNull; }
    void setNull();
    WsmType getType() const { return _type; }
    Uint32 getArraySize();

    void toArray();
    void add(const WsmValue& val);

    void get(WsmEndpointReference& ref) const;
    void get(Array<WsmEndpointReference>& ref) const;
    void get(WsmInstance& inst) const;
    void get(Array<WsmInstance>& inst) const;
    void get(String& str) const;
    void get(Array<String>& str) const;

    void set(const WsmEndpointReference& ref);
    void set(const Array<WsmEndpointReference>& ref);
    void set(const WsmInstance& inst);
    void set(const Array<WsmInstance>& inst);
    void set(const String& str);
    void set(const Array<String>& str);

private:

    void _release();

    WsmType _type;
    Boolean _isArray;
    Boolean _isNull;

    union WsmValueRep
    {
        WsmEndpointReference* ref;
        Array<WsmEndpointReference>* refa;
        WsmInstance* inst;
        Array<WsmInstance>* insta;
        Array<String>* stra;
        String* str;
    } _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmValue_h */
