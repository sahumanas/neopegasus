//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CIMReference.cpp,v $
// Revision 1.2  2001/02/26 04:33:28  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.4  2001/02/11 05:42:33  mike
// new
//
// Revision 1.3  2001/01/28 07:05:18  mike
// added instance name/reference converters
//
// Revision 1.2  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.1.1.1  2001/01/14 19:53:11  mike
// Pegasus import
//
//
//END_HISTORY

#include <cctype>
#include <cstring>
#include "CIMReference.h"
#include "Indentor.h"
#include "CIMName.h"
#include "Destroyer.h"
#include "XmlWriter.h"
#include "XmlReader.h"

PEGASUS_NAMESPACE_BEGIN

// ATTN: add a resolve method to this class to verify that the
// reference is correct (that the class name corresponds to a real
// class and that the property names are really keys and that all keys
// of the class or used. Also be sure that there is a valid conversion 
// between the string value and the value of that property.

////////////////////////////////////////////////////////////////////////////////
//
// KeyBinding
//
////////////////////////////////////////////////////////////////////////////////

KeyBinding::KeyBinding() { }

KeyBinding::KeyBinding(const KeyBinding& x)
    : _name(x._name), _value(x._value), _type(x._type) 
{ 

}

KeyBinding::KeyBinding(const String& name, const String& value, CIMType type)
    : _name(name), _value(value), _type(type) 
{ 

}

KeyBinding::~KeyBinding() 
{ 

}

KeyBinding& KeyBinding::operator=(const KeyBinding& x)
{
    _name = x._name;
    _value = x._value;
    _type = x._type;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMReference
//
////////////////////////////////////////////////////////////////////////////////

CIMReference::CIMReference()
{

}

CIMReference::CIMReference(const CIMReference& x)
    : _host(x._host), _nameSpace(x._nameSpace), 
    _className(x._className), _keyBindings(x._keyBindings)
{

}

CIMReference::CIMReference(
    const String& host,
    const String& nameSpace,
    const String& className, 
    const Array<KeyBinding>& keyBindings)
    : 
    _host(host), 
    _nameSpace(nameSpace), 
    _className(className), 
    _keyBindings(keyBindings)
{

}

CIMReference::~CIMReference()
{

}

CIMReference& CIMReference::operator=(const CIMReference& x)
{
    if (&x != this)
    {
	_host = x._host;
	_nameSpace = x._nameSpace;
	_className = x._className;
	_keyBindings = x._keyBindings;
    }
    return *this;
}

void CIMReference::clear()
{
    _host.clear();
    _nameSpace.clear();
    _className.clear();
    _keyBindings.clear();
}

void CIMReference::set(
    const String& host,
    const String& nameSpace,
    const String& className, 
    const Array<KeyBinding>& keyBindings)
{
    _host = host;
    _nameSpace = nameSpace;
    _className = className;
    _keyBindings = keyBindings;
}

void CIMReference::setNameSpace(const String& nameSpace)
{
    if (!CIMName::legal(nameSpace))
	throw IllegalName();
}

void CIMReference::setClassName(const String& className)
{
    if (!CIMName::legal(className))
	throw IllegalName();
}

void CIMReference::setKeyBindings(const Array<KeyBinding>& keyBindings) 
{
    _keyBindings = keyBindings; 
}

Boolean CIMReference::identical(const CIMReference& x) const
{
    return 
	String::equal(_host, x._host) && 
	String::equal(_nameSpace, x._nameSpace) && 
	CIMName::equal(_className, x._className) && 
	_keyBindings == x._keyBindings;
}

void CIMReference::nameSpaceToXml(Array<Sint8>& out) const
{
    if (_host.getLength())
    {
	out << "<NAMESPACEPATH>\n";
	out << "<HOST>" << _host << "</HOST>\n";
    }
    
    XmlWriter::appendLocalNameSpaceElement(out, _nameSpace);

    if (_host.getLength())
	out << "</NAMESPACEPATH>\n";
}

void CIMReference::localNameSpaceToXml(Array<Sint8>& out) const
{
    out << "<LOCALNAMESPACEPATH>\n";

    char* tmp = _nameSpace.allocateCString();

    for (char* p = strtok(tmp, "/"); p; p = strtok(NULL, "/"))
    {
	out << "<NAMESPACE NAME=\"" << p << "\"/>\n";
    }

    out << "</LOCALNAMESPACEPATH>\n";
}

void CIMReference::classNameToXml(Array<Sint8>& out) const
{
    out << "<CLASSNAME NAME=\"" << _className << "\"/>\n";
}

void CIMReference::instanceNameToXml(Array<Sint8>& out) const
{
    out << "<INSTANCENAME CLASSNAME=\"" << _className << "\">\n";

    for (Uint32 i = 0, n = _keyBindings.getSize(); i < n; i++)
    {
	out << "<KEYBINDING NAME=\"" << _keyBindings[i].getName() << "\">\n";

	out << "<KEYVALUE VALUETYPE=\"";
	out << KeyBinding::typeToString(_keyBindings[i].getType());
	out << "\">";

	out << _keyBindings[i].getValue();
	out << "</KEYVALUE>\n";

	out << "</KEYBINDING>\n";
    }

    out << "</INSTANCENAME>\n";
}

void CIMReference::toXml(Array<Sint8>& out) const
{
    out << "<VALUE.REFERENCE>\n";

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).

    if (_keyBindings.getSize())
    {
	if (_host.getLength())
	{
	    out << "<INSTANCEPATH>\n";
	    nameSpaceToXml(out);
	    instanceNameToXml(out);
	    out << "</INSTANCEPATH>";
	}
	else if (_nameSpace.getLength())
	{
	    out << "<LOCALINSTANCEPATH>\n";
	    localNameSpaceToXml(out);
	    instanceNameToXml(out);
	    out << "</LOCALINSTANCEPATH>";
	}
	else
	    instanceNameToXml(out);
    }
    else
    {
	if (_host.getLength())
	{
	    out << "<CLASSPATH>\n";
	    nameSpaceToXml(out);
	    classNameToXml(out);
	    out << "</CLASSPATH>";
	}
	else if (_nameSpace.getLength())
	{
	    out << "<LOCALCLASSPATH>\n";
	    nameSpaceToXml(out);
	    classNameToXml(out);
	    out << "</LOCALCLASSPATH>";
	}
	else
	    classNameToXml(out);
    }

    out << "</VALUE.REFERENCE>\n";
}

void CIMReference::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

const char* KeyBinding::typeToString(CIMType type)
{
    switch (type)
    {
	case KeyBinding::BOOLEAN: 
	    return "boolean"; 

	case KeyBinding::STRING:
	    return "string"; 

	case KeyBinding::NUMERIC:
	    return "numeric"; 
    }

    return "unknown";
}

void CIMReference::instanceNameToReference(
    const String& instanceName,
    CIMReference& reference)
{
    reference.clear();

    // Convert to a C String first:

    char* p = instanceName.allocateCString();

    // Extract the class name:

    char* dot = strchr(p, '.');

    if (!dot)
	throw BadInstanceName(instanceName);

    String className(p, dot - p);

    // Advance past dot:

    p = dot + 1;

    // Get the key-value pairs:

    Array<KeyBinding> keyBindings;

    for (p = strtok(p, ","); p; p = strtok(NULL, ","))
    {
	// Split about the equal sign:

	char* equal = strchr(p, '=');

	if (!equal)
	    throw BadInstanceName(instanceName);

	*equal = '\0';

	// Get key part:

	String keyString(p);

	if (!CIMName::legal(keyString))
	    throw BadInstanceName(instanceName);

	// Get the value part:

	String valueString;
	char* q = equal + 1;
	KeyBinding::CIMType type;

	if (*q == '"')
	{
	    q++;

	    type = KeyBinding::STRING;

	    while (*q && *q != '"')
	    {
		// ATTN: need to handle special characters here:

		if (*q == '\\')
		    *q++;

		valueString.append(*q++);
	    }

	    if (*q++ != '"')
		throw BadInstanceName(instanceName);

	    if (*q)
		throw BadInstanceName(instanceName);
	}
	else if (toupper(*q) == 'T' || toupper(*q) == 'F')
	{
	    type = KeyBinding::BOOLEAN;

	    char* r = q;

	    while (*r)
	    {
		*r = toupper(*r);
		r++;
	    }

	    if (strcmp(q, "TRUE") != 0 && strcmp(q, "FALSE") != 0)
		throw BadInstanceName(instanceName);

	    valueString.assign(q);
	}
	else
	{
	    type = KeyBinding::NUMERIC;

	    Sint64 x;

	    if (!XmlReader::stringToSignedInteger(q, x))
		throw BadInstanceName(instanceName);

	    valueString.assign(q);
	}

	keyBindings.append(KeyBinding(keyString, valueString, type));
    }

    reference.set(String(), String(), className, keyBindings);
}

void CIMReference::referenceToInstanceName(
    const CIMReference& reference,
    String& instanceName)
{
    instanceName.clear();

    // Get the class name:

    const String& className = reference.getClassName();
    instanceName.append(className);
    instanceName.append('.');

    // Append each key-value pair:

    const Array<KeyBinding>& keyBindings = reference.getKeyBindings();

    for (Uint32 i = 0, n = keyBindings.getSize(); i < n; i++)
    {
	instanceName.append(keyBindings[i].getName());
	instanceName.append('=');

	// ATTN: handle escaping of special characters:

	const String& value = keyBindings[i].getValue();

	KeyBinding::CIMType type = keyBindings[i].getType();
	
	if (type == KeyBinding::STRING)
	    instanceName.append('"');

	instanceName.append(value);

	if (type == KeyBinding::STRING)
	    instanceName.append('"');

	if (i + 1 != n)
	    instanceName.append(',');
    }
}

PEGASUS_NAMESPACE_END
