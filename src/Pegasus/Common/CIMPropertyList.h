//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMPropertyList_h
#define Pegasus_CIMPropertyList_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN


/** This class is for representing property lists in the CIM interface.

    This class comprises an array of propertyNames and a flag indicating whether
    the list is null. There are three possibilties which must be represented;
    the property list is:

    <ul>
	<li>Non-empty (and non-null)</li>
	<li>Empty (and non-null)</li>
	<li>Null</li>
    </ul>

    The isNull member is used to indicate whether the parameter list is
    null (or not).

    To create a null property list use the default constructor. Otherwise 
    use the constructor which takes a property array (pass an empty property
    array to get an empty property list object).

    Members are provided for accessing elements of the the internal property
    list. There are none for modifying elements (the entire array must be
    formed and passed to the constructor or replaced by calling set()).
*/
class PEGASUS_COMMON_LINKAGE CIMPropertyList
{
public:

    /** Default constructor (sets isNull to true).
    */
    CIMPropertyList();

    /** Copy constructor.
    */
    CIMPropertyList(const CIMPropertyList& x);

    /** Constructor. Initializes propertyNames (sets isNull to false).
    */
    CIMPropertyList(const Array<String>& propertyNames);

    /** Modifier for propertyNames (sets isNull to false).
    */
    void set(const Array<String>& propertyNames);

    /** Assignment operator.
    */
    CIMPropertyList& operator=(const CIMPropertyList& x);

    /** Clears the propertyNames array (sets isNull to true).
    */
    void clear();

    /** Returns true if the property list is null.
    */
    Boolean isNull() const { return _isNull; }

    /** Get the number of propertyNames in the list.
    */
    Uint32 getNumProperties() const { return _propertyNames.size(); }

    /** Get the property at the given position.
    */
    const String& getPropertyName(Uint32 pos) const
    {
	return _propertyNames[pos];
    }

    /** Get an array of the property names
    */
    const Array<String>& getPropertyNameArray() const { return _propertyNames; }

private:

    Array<String> _propertyNames;
    Boolean _isNull;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMPropertyList_h */
