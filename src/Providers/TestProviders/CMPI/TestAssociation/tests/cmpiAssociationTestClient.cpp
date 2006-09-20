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

// This program makes CIMClient association method calls for CMPI (associators,
// associatorNames, references, referenceNames) to get information about the
// relationships between the CMPI_TEST_Person and CMPI_TEST_Vehicle classes.
//
// The CMPIAssociationProvider is the instance and association provider
// for the following classes:
//
//     CMPI_TEST_Person
//     CMPI_TEST_Vehicle
//     CMPI_TEST_Racing  (association class)
//     
// The executable for this CIM client application is:  cmpiAssociationTestClient.
// To display the test results (returned instances or classes), use the -v
// (verbose) option in the command line:  'cmpiAssociationTestClient -v'.
//
// Test results are verified by comparing the number of returned objects
// with the expected number of returned objects.  Any unexpected result
// is considered a failure.  If there is a failure, an error message is
// displayed and the program terminates.

#include <Pegasus/Client/CIMClient.h>
#include <string.h>

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Pegasus/Provider/CMPI/cmpi_cql.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("test/TestProvider");
const CIMName CMPI_TEST_PERSON = CIMName ("CMPI_TEST_Person");
const CIMName CMPI_TEST_VEHICLE = CIMName ("CMPI_TEST_Vehicle");
const CIMName CMPI_TEST_RACING = CIMName ("CMPI_TEST_Racing");


static Boolean verbose = false;

// exepected results - number of returned objects expected for each test

static const Uint32 resultArray_asso_P1[] = { 3, 3 };
static const Uint32 resultArray_asso_P2[] = { 3, 3 };
static const Uint32 resultArray_asso_V1[] = { 3, 3 };
static const Uint32 resultArray_asso_V2[] = { 3, 3 };
static const Uint32 resultArray_ref_P[] =   { 3, 3, 3, 3 };
static const Uint32 resultArray_ref_V[] =   { 3, 3, 3, 3 };

////////////////////////////////////////////////////////////////////////////
//  _errorExit
////////////////////////////////////////////////////////////////////////////

void _errorExit(String message)
{
    cerr << "cmpiAssociationTestClient error: " << message << endl;
    exit(1);
}

////////////////////////////////////////////////////////////////////////////
//  _verifyResult
////////////////////////////////////////////////////////////////////////////

//
//  Verify that the number of objects returned is as expected.  If the
//  number of objects returned is not expected, display an error and
//  terminate the program.
//
void _verifyResult(const Uint32 numObjects, const Uint32 numExpectedObjects)
{   
    if (numObjects != numExpectedObjects)
    {
        cout << "Error: Unexpected number of objects returned.  ";
        cout << "Expected " << numExpectedObjects << " object(s), but ";
        cout << numObjects << " object(s) were returned." << endl;
        exit(1);
    }
}

////////////////////////////////////////////////////////////////////////////
//  _displayResult
////////////////////////////////////////////////////////////////////////////

//
//  Display the result objects if verbose option is specified.
//
void _displayResult(const Array<CIMObject> & objects)
{
    if (verbose)
    {
        Uint32 numObjects = objects.size();
        for (Uint32 i = 0; i < numObjects; i++)
        {
            cout << objects[i].getPath().toString() << endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////
//  _displayResult
////////////////////////////////////////////////////////////////////////////

//
//  Display the result object names if verbose option is specified.
//
void _displayResult(const Array<CIMObjectPath> & objectPaths)
{
    if (verbose)
    {
        Uint32 numObjectPaths = objectPaths.size();
        for (Uint32 i = 0; i < numObjectPaths; i++)
        {
            cout << objectPaths[i].toString() << endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////
//  _testAssociators
////////////////////////////////////////////////////////////////////////////

void _testAssociators(CIMClient& client, CIMName assocClass, CIMObjectPath instancePath,
                      Uint32 numExpectedObjects)
{
    if (verbose)
    {
        cout << "\nAssociation Class: " << assocClass.getString() << endl;
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    try
    {
        CIMName resultClass = CIMName();
        String role = String::EMPTY;
        String resultRole = String::EMPTY;

        // Get the CIM instances that are associated with the specified source
        // instance via an instance of the AssocClass
        //

        Array<CIMObject> resultObjects =
            client.associators(NAMESPACE, instancePath, assocClass,
                               resultClass, role, resultRole);

        // verify result
        _verifyResult(resultObjects.size(), numExpectedObjects);

        // display result
        _displayResult(resultObjects);
    }
    catch (Exception& e)
    {
        _errorExit(e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////
//  _testAssociatorNames
////////////////////////////////////////////////////////////////////////////

void _testAssociatorNames(CIMClient& client, CIMName assocClass,
                          CIMObjectPath instancePath, Uint32 numExpectedObjects)
{
    if (verbose)
    {
        cout << "\nAssociation Class: " << assocClass.getString() << endl;
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    try
    {
        // Get the names of the CIM instances that are associated to the
        // specified source instance via an instance of the AssocClass.
        //
        CIMName resultClass = CIMName();
   
        String role = String::EMPTY;
        String resultRole = String::EMPTY;

        Array<CIMObjectPath> resultObjectPaths =
            client.associatorNames(NAMESPACE, instancePath,
                                   assocClass, resultClass, role, resultRole);
        // verify result
        _verifyResult(resultObjectPaths.size(), numExpectedObjects);

        // display result
        _displayResult(resultObjectPaths);
    }
    catch (Exception& e)
    {
         _errorExit(e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////
//  _testReferences
////////////////////////////////////////////////////////////////////////////

void _testReferences(CIMClient& client, CIMObjectPath instancePath, 
                     Uint32 numExpectedObjects)
{
    if (verbose)
    {
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    try
    {
        // get the association reference instances
        //
        Array<CIMObject> resultObjects;
        CIMName resultClass = CIMName();
        String role = String::EMPTY;

        resultObjects = client.references(NAMESPACE, instancePath,
                                          resultClass,role);
        // verify result
        _verifyResult(resultObjects.size(), numExpectedObjects);

        // display result
        _displayResult(resultObjects);
    }
    catch (Exception& e)
    {
         _errorExit(e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////
//  _testReferenceNames
////////////////////////////////////////////////////////////////////////////

void _testReferenceNames(CIMClient& client, CIMObjectPath instancePath,
                         Uint32 numExpectedObjects)
{
    if (verbose)
    {
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    try
    {
        // get the reference instance names
        //
        Array<CIMObjectPath> resultObjectPaths;
        CIMName resultClass = CIMName();
        String role = String::EMPTY;

        resultObjectPaths =
            client.referenceNames(NAMESPACE, instancePath, resultClass, role);

        // verify result
        _verifyResult(resultObjectPaths.size(), numExpectedObjects);

        // display result
        _displayResult(resultObjectPaths);
    }
    catch (Exception& e)
    {
        _errorExit(e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////
//  _testCMPIAssociationClassOperations
////////////////////////////////////////////////////////////////////////////

void _testCMPIAssociationClassOperations(CIMClient& client, CIMName className)
{
    Array<CIMObjectPath> resultObjectPaths;
    Array<CIMObject> resultObjects;
    CIMObjectPath op(className.getString());

    CIMName assocClass = CIMName();
    CIMName resultClass = CIMName();
    String role = String::EMPTY;
    String resultRole = String::EMPTY;

    // =======================================================================
    // associators
    //
    // Get the CIM classes that are associated with the specified CIM Class
    // =======================================================================

    if (verbose)
    {
        cout << "\n+++++ Test associators for (" << className.getString();
        cout << ")" << endl;
    }

    try
    {
        // get the association classes
        resultObjects = client.associators(NAMESPACE, op, assocClass,
            resultClass, role, resultRole);

        // display result
        _displayResult(resultObjects);
    }
    catch (Exception& e)
    {
        // Do nothing.
         _errorExit(e.getMessage());
    }

    // =======================================================================
    // associatorNames
    //
    // Get the name of the CIM classes that are associated with the specified
    // CIM class.
    // =======================================================================

    if (verbose)
    {
        cout << "\n+++++ Test associatorNames for (" << className.getString();
        cout << ")" << endl;
    }

    try
    {
        resultObjectPaths = client.associatorNames(NAMESPACE, op, assocClass,
            resultClass, role, resultRole);

        // display result
        _displayResult(resultObjectPaths);
    }
    catch (Exception& e)
    {
        // Do nothing.
         _errorExit(e.getMessage());
    }

    // =======================================================================
    // references
    //
    // Get the association classes that refer to the specified CIM class.
    // =======================================================================

    if (verbose)
    {
        cout << "\n+++++ Test references for (" << className.getString() << ")" << endl;
    }

    try
    {
        resultObjects = client.references(NAMESPACE, op, resultClass, role);

        // display result
        _displayResult(resultObjects);
    }
    catch (Exception& e)
    {
        // Do nothing.
         _errorExit(e.getMessage());
    }

    // =======================================================================
    // referenceNames
    //
    // Get the names of the association classes that refer to the specified
    // CIM class.
    // =======================================================================

    if (verbose)
    {
        cout << "\n+++++ Test referenceNames for (" << className.getString();
        cout << ")" << endl;
    }

    try
    {
        resultObjectPaths =
            client.referenceNames(NAMESPACE, op, resultClass, role);

        // display result
        _displayResult(resultObjectPaths);
    }
    catch (Exception& e)
    {
        // Do nothing.
         _errorExit(e.getMessage());
    }
}

// =========================================================================
//    Main
// =========================================================================

int main(int argc, char** argv)
{
    //
    // Check command line option
    //
    if (argc > 2)
    {
        cerr << "Usage: cmpiAssociationTestClient [-v]" << endl;
        return(1);
    }

    if (argc == 2)
    {
        const char *opt = argv[1];
        if (strcmp(opt, "-v") == 0)
        {
            verbose = true;
        }
        else
        {
            cerr << "Usage: cmpiAssociationTestClient [-v]" << endl;
            return(1);
        }
    }

    CIMClient client;

    // Connect to server
    try
    {
        client.connectLocal();
    }
    catch (Exception& e)
    {
        _errorExit(e.getMessage());
    }

    // =======================================================================
    // Test passing Instance object path to the Association Methods
    // =======================================================================

    cout << "+++++ Test CMPI Association Provider" << endl;

    // Get the instance object paths for the Person and Vehicle class
    //
    Array<CIMObjectPath> personRefs;
    Array<CIMObjectPath> vehicleRefs;
    try
    {
        personRefs =
            client.enumerateInstanceNames(NAMESPACE, CMPI_TEST_PERSON);
        vehicleRefs =
            client.enumerateInstanceNames(NAMESPACE, CMPI_TEST_VEHICLE);
    }
    catch (Exception& e)
    {
        cout << "enumerateInstanceNames() failed." << endl;
        _errorExit(e.getMessage());
    }

    /*for(Uint32 i=0;i<personRefs.size();i++)
    {
        cout<<"PersonRefs values : "<<personRefs[i].toString()<<endl;
    }
    for(Uint32 i=0;i<vehicleRefs.size();i++)
    {
        cout<<"VehicleRefs values : "<<vehicleRefs[i].toString()<<endl;
    }
    Uint32 numPersonInstances = personRefs.size();
    Uint32 numVehicleInstances = vehicleRefs.size();

    cout << "Number of PersonInstances: " << numPersonInstances << endl;
    cout << "Number of VehicleInstances: " << numVehicleInstances << endl;
    */
    Uint32 numPersonInstances = personRefs.size();
    Uint32 numVehicleInstances = vehicleRefs.size();
    

    // =======================================================================
    // Test associators
    //
    // Get the CIM instances that are associated with the specified source CIM
    // instance via an instance of a specified association class.
    // =======================================================================

    cout << "\n+++++ Test associators" << endl;
    for (Uint32 i = 0; i < numPersonInstances; i++)
    {
	_testAssociators(client, CMPI_TEST_RACING, personRefs[i],
                         resultArray_asso_P1[i]);
    }

    for (Uint32 i = 0; i < numVehicleInstances; i++)
    {
       _testAssociators(client, CMPI_TEST_RACING, vehicleRefs[i],
                        resultArray_asso_V1[i]);
    }

    // =======================================================================
    // Test associatorNames
    //
    // Get the names of the CIM instances that are associated with the specified
    // source CIM instance via an instance of a association class.
    // =======================================================================

    cout << "\n+++++ Test associatorNames" << endl;
    for (Uint32 i = 0; i < numPersonInstances; i++)
    {
        _testAssociatorNames(client, CMPI_TEST_RACING, personRefs[i],
            resultArray_asso_P1[i]);
    }
    for (Uint32 i = 0; i < numVehicleInstances; i++)
    {
        _testAssociatorNames(client, CMPI_TEST_RACING, vehicleRefs[i],
            resultArray_asso_V1[i]);
    }

    // =======================================================================
    // Test references
    //
    // Get the association instances that refer to the specified target CIM
    // instance.
    // =======================================================================

    cout << "\n+++++ Test references" << endl;
    for (Uint32 i = 0; i < numPersonInstances; i++)
    {
        _testReferences(client, personRefs[i], resultArray_ref_P[i]);
    }
    for (Uint32 i = 0; i < numVehicleInstances; i++)
    {
        _testReferences(client, vehicleRefs[i], resultArray_ref_V[i]);
    }

    // =======================================================================
    // Test referenceNames
    //
    // Get the names of the association instances that refer to the specified
    // source CIM instance.
    // =======================================================================

    cout << "\n+++++ Test referenceNames" << endl;
    for (Uint32 i = 0; i < numPersonInstances; i++)
    {
        _testReferenceNames(client, personRefs[i], resultArray_ref_P[i]);
    }
    for (Uint32 i = 0; i < numVehicleInstances; i++)
    {
        _testReferenceNames(client, vehicleRefs[i], resultArray_ref_V[i]);
    }

    // =======================================================================
    // Test passing Class object path to the Association Methods
    // =======================================================================

    cout << "\n+++++ Test association class operations" << endl;
    _testCMPIAssociationClassOperations(client, CMPI_TEST_PERSON);
    _testCMPIAssociationClassOperations(client, CMPI_TEST_VEHICLE);

    // =======================================================================
    // Association tests completed
    // =======================================================================

    cout << "\n+++++ passed all tests" << endl;
    return(0);
}