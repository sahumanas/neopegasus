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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMParamValue.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    CIMParamValue pv;
    assert(!pv);

    CIMParameter p1("message", CIMType::STRING);
    p1.addQualifier(CIMQualifier("in", true));
    CIMValue v1("argument_Test");
    CIMParamValue a1(p1, v1);

    CIMParameter p2("message2", CIMType::STRING);
    p2.addQualifier(CIMQualifier("in", true));
    CIMValue v2("argument_Test2");
    CIMParamValue a2(p2, v2);

    CIMParameter p3("message3", CIMType::STRING);
    p3.addQualifier(CIMQualifier("in", true));
    CIMValue v3("argument_Test3");
    CIMParamValue a3(p3, v3);

    CIMParameter p4("message4", CIMType::STRING);
    p4.addQualifier(CIMQualifier("in", true));
    CIMValue v4("argument_Test4");
    CIMParamValue a4(p4, v4);
    CIMParamValue a5 = a4;

    CIMParameter p6("message6", CIMType::STRING);
    p6.addQualifier(CIMQualifier("in", true));
    CIMValue v6("argument_Test6");
    CIMParamValue a6(p6, v6, false, 0, "myClass");

    Array<CIMParamValue> aa;
    aa.append(a1);
    aa.append(a2);

    aa.append(CIMParamValue(CIMParameter("message3", CIMType::UINT16), 
		CIMValue(200000)));

    aa.append(CIMParamValue(CIMParameter("message4", CIMType::STRING), 
		CIMValue("test4")));
    
    //
    // clone
    //
    CIMParamValue a4clone = a4.clone();
    aa.append(a4clone);

    for (int i=0; i< aa.size(); i++)
    {
        aa[i].print(cout);
    }

    //
    // toMof
    //
    Array<Sint8> mofOut;
    a4clone.toMof(mofOut);

    //
    // toXml
    //
    Array<Sint8> xmlOut;
    a4clone.toXml(xmlOut);

    //
    // identical
    //
    Boolean same;
    same  = a4clone.identical(a4);
    assert(same);
 
    //
    // not identical
    //
    same = a4clone.identical(a3);
    assert(!same);
 
    //
    // add qualifiers
    //
    a1.addQualifier(CIMQualifier("q1", true));
    a1.addQualifier(CIMQualifier("q2", true));

    //
    // get qualifier count
    //
    assert(a1.getQualifierCount() == 2);

    //
    // get qualifiers
    //
    CIMQualifier q1 = a1.getQualifier(0);
    CIMQualifier q2 = a1.getQualifier(1);
    assert(q1);
    assert(q2);

    CIMConstQualifier q1const = a1.getQualifier(0);
    CIMConstQualifier q2const = a1.getQualifier(1);
    assert(q1const);
    assert(q2const);

    //
    // isArray
    //
    assert(a1.isArray() == false);

    //
    // getAraySize
    //
    assert(a1.getAraySize() == 0);

    //
    // test CIMConstParamValue methods
    //
    CIMConstParamValue ca1 = a1;
    CIMConstParamValue ca2 = a2;
    CIMConstParamValue ca3 = a3;
    CIMConstParamValue ca4(p4, v4);
    CIMConstParamValue ca5 = ca4;
    CIMConstParamValue ca6 = a4;

    CIMConstParamValue ca3clone = ca3.clone();

    ca1.print(cout);

    ca1.toXml(xmlOut);

    assert(ca3clone.identical(ca3) == true);
 
    assert(ca1.getQualifierCount() == 2);

    const CIMConstQualifier cq1 = ca1.getQualifier(0);
    assert(cq1);
    CIMConstQualifier cq2 = ca1.getQualifier(0);
    assert(cq2);

    assert(ca1.isArray() == false);

    assert(ca1.getAraySize() == 0);

}


int main()
{
    try
    {
        test01();
    }
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
