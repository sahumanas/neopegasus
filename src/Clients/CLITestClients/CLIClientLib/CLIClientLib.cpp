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
// Author:  Karl Schopmeyer (k.schopmeyer@opengroup.org) 
//          Mary Hinton (m.hinton@verizon.net)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/Tracer.h>
#include "CLIClientLib.h"
#include <Pegasus/Common/Stopwatch.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

const String DEFAULT_NAMESPACE = "root/cimv2";

// Character sequences used in help/usage output.


String printPropertyList (CIMPropertyList)
{
    return "TBD add code here to print property list";
}

static const char * version = "2.0";
static const char * usage = "This command executes single WBEM Operations.";


// Note that the following is one long string.
static const char * usageDetails = "Using CLI examples:n \
CLI enumerateinstancenames pg_computersystem  -- enumerateinstances of class\n \
    or \n \
CLI ei pg_computersystem    -- Same as above \n\n \
CLI enumerateclassnames -- Enumerate classnames from root/cimv2.\n \
CLI ec /n root -- Enumerate classnames from namespace root. \n \
CLI ec -o xml   -- Enumerate classes with XML output starting at root\n \
CLI enumerateclasses CIM_Computersystem -o xml\n    -- Enumerate classes in MOF starting with \
CIM_Computersystem\n \
CLI getclass CIM_door -a -u guest =p guest\n    -- Get class with authentication set and \
user = guest, password = guest.\n \
CLI rn TST_Person.name=@MIKE@ -n root/sampleprovider -rc TST_Lineage. \n \
CLI ec -o XML -- enumerate classes and output XML rather than MOF. \n \
CLI getqualifiers -- Get the qualifiers in mof output format\n";

void _displaySummary(Uint32 count, String& description, String item, Options& opts)
{
        cout << count << " " << description 
            << " " << item << " returned. ";
        if (opts.repeat > 0)
            cout << opts.repeat;
        if(opts.time && opts.repeat)
            cout << " " << opts.saveElapsedTime;
        cout << endl;
        if ((opts.count) != 97832 && (opts.count != count))
        {
            cout << "Failed count test. Expected= " << opts.count << " Actual rcvd' " << count << endl;
        }

}

/**** NOT TESTED
String _nextToken(String& input, Uint32 start, Char16 separator)
{
    String rtn;
    Uint32 end;
    end = input.find(input.find(start, separator));
    if (end == PEG_NOT_FOUND)
    {
        start = PEG_NOT_FOUND
        rtn = input.subString(start,(input.size() - start);
    }
    else
    {
        rtn = input.subString(start,end);
        start = end + 1;
    }
    return(rtn);
}
*/
/** tokenize an input string into an array of Strings,
 * separating the tokens at the separator character
 * provided
 * @param input String
 * @param separator character
 * @returns Array of separated strings
 * */
Array<String> _tokenize(String& input, Char16 separator)
{
    Array<String> tokens;
    if (input.size() != 0)
    {
        Uint32 start = 0;
        Uint32 length = 0;
        Uint32 end = 0;
        while ((end = input.find(start, separator)) != PEG_NOT_FOUND)
        {
            length = end - start;
            tokens.append(input.subString(start, length));
            start += (length + 1);
        }
        if(start < input.size())
        {
            tokens.append(input.subString(start));
        }
    }
    return tokens;
}

CIMParamValue _createMethodParamValue(String& input)
{
    Array<String> pair = _tokenize(input, '=');
    CIMParamValue pv;
    if (pair.size() != 2)
    {
        cout << "Input Parameter error " << input;
        exit(1);
    }
    else
    {
        // We need to type the inputs somehow.
        cout << "Name = " << pair[0] << "Value= " << pair[1] << endl;
        pv.setParameterName(pair[0]);
        // ATTN: KS 20030423 P2This is incomplete since it only allows us to do string input.
        // ATTN: KS 20030424 P2. We don't have any documentation on the isTyped function.
        CIMValue v(pair[1]);
        pv.setValue(v);
    }
    return pv;
}


int OutputFormatInstance(OutputType format, CIMInstance& instance)
{
    if (format == OUTPUT_XML)
        XmlWriter::printInstanceElement(instance, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendInstanceElement(x, instance);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    return 0;
}

int OutputFormatClass(OutputType format, CIMClass& myClass)
{
    if (format == OUTPUT_XML)
        XmlWriter::printClassElement(myClass, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendClassElement(x, myClass);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    else
        cout << "Error, Format Definition Error" << endl;
    return 0;
}

int OutputFormatObject(OutputType format, CIMObject& myObject)
{
       
    if (myObject.isClass())
    {
        CIMClass c(myObject);
        OutputFormatClass(format, c);
    }
    else if (myObject.isInstance())
    {
        CIMInstance i(myObject);
        OutputFormatInstance(format, i);
    }
    else
        cout << "Error, Object is neither class or instance" << endl;
    return 0;
}

int OutputFormatQualifierDecl(OutputType format, CIMQualifierDecl& myQualifierDecl)
{
    if (format == OUTPUT_XML)
        XmlWriter::printQualifierDeclElement(myQualifierDecl, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendQualifierDeclElement(x, myQualifierDecl);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    else
        cout << "Format type error" << endl;
    return 0;
}

int OutputFormatCIMValue(OutputType format, CIMValue& myValue)
{
    if (format == OUTPUT_XML)
        XmlWriter::printValueElement(myValue, cout);
    else if (format == OUTPUT_MOF)
    {
        Array<Sint8> x;
        MofWriter::appendValueElement(x, myValue);

        x.append('\0');

        mofFormat(cout, x.getData(), 4);
    }   
    else
        cout << " Format type error" << endl;
    return 0;
}

///////////////////////////////////////////////////////////////////
//                                                               //
//     The following section  defines each action function       //
//     ex. getInstance. Parameters are defined in the            //
//     opts structure.  There are not exception catches.         //
//     exception handling is in the main path                    //
///////////////////////////////////////////////////////////////////

/* This command searches the entire namespace and displays names of all instances.
    It is in effect enumerate classes followed by enumerate instances.
   The user may either provide a starting class or not, in which case
   it searches the complete namespace, not simply the defined class.
*/
int enumerateAllInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    CIMName myClassName = CIMName();
    /****if (opts.className != "")
    {
        myClassName = opts.className;
    }*/
    
    Array<CIMName> classNames; 
    if (opts.time) opts.elapsedTime.reset();
    
    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance);
    
    for (Uint32 iClass = 0; iClass < classNames.size(); iClass++)
    {
        if (opts.verboseTest)
        {
            cout << "EnumerateInstanceNames "
                << "Namespace = " << opts.nameSpace
                << ", Class = " << opts.className
                << endl;
        }
        Array<CIMObjectPath> instanceNames = 
        client.enumerateInstanceNames(opts.nameSpace, classNames[iClass]);
        if (opts.summary)
        {
	  String s = "instance names of class";   
	  _displaySummary(instanceNames.size(), s, opts.className.getString(),opts);    
        }
        else
        {
            //simply output the list one per line for the moment.
            for (Uint32 i = 0; i < instanceNames.size(); i++)
                        cout << instanceNames[i].toString() << endl;
        }
    }
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    return 0;
}        

int enumerateInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstanceNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    
    Array<CIMObjectPath> instanceNames = 
        client.enumerateInstanceNames(opts.nameSpace, opts.className);

    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}

    if (opts.summary)
    {
      String s = "instances names of class";  
      _displaySummary(instanceNames.size(), s, opts.className.getString(),opts);    
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < instanceNames.size(); i++)
                    cout << instanceNames[i].toString() << endl;
    }
    return 0;
}        



int enumerateInstances(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstances "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << ", localOnly = " << (opts.localOnly? "true" : "false")
            << endl;
    }
    
    Array<CIMInstance> instances; 
    if (opts.time) opts.elapsedTime.reset();
    instances = client.enumerateInstances(opts.nameSpace,
                                                   opts.className,
                                                   opts.deepInheritance,
                                                   opts.localOnly,
                                                   opts.includeQualifiers,
                                                   opts.includeClassOrigin);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    
    if (opts.summary)
    {
      String s = "instances of class";
        _displaySummary(instances.size(), s, opts.className.getString(),opts);    
        /*
        cout << instances.size() << " instances of class " << opts.className << " returned. ";
            if (opts.repeat > 0)
                cout << opts.repeat;
            if(opts.time && opts.repeat)
                cout << " " << opts.saveElapsedTime;
        cout << endl;*/
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            CIMInstance instance = instances[i];
            // Check Output Format to print results
            OutputFormatInstance(opts.outputType, instance);
        }
    }
    return 0;
}

int deleteInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteInstance "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    client.deleteInstance(opts.nameSpace, opts.objectName);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    return 0;  
}

int getInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getInstance "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.objectName
            << endl;
    }
    
    if (opts.time) opts.elapsedTime.reset();
    CIMInstance cimInstance = client.getInstance(opts.nameSpace,
                                                 opts.objectName);

    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    // Check Output Format to print results
    OutputFormatInstance(opts.outputType, cimInstance);
    return 0;
}

int enumerateClassNames(CIMClient& client, Options& opts)
{
    
    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << endl;
    }
    Array<CIMName> classNames; 
    
    if (opts.time) opts.elapsedTime.reset();
    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    if (opts.summary)
    {
      String s = "class names";
        _displaySummary(classNames.size(), s,
             opts.className.getString(),opts);    
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < classNames.size(); i++)
                cout << classNames[i] << endl;
    }
    return 0;
}

int enumerateClasses(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasses "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << ", localOnly = " << (opts.localOnly? "true" : "false")
            << ", includeQualifiers = " << (opts.includeQualifiers? "true" : "false")
            << ", includeClassOrigin = " << (opts.includeClassOrigin? "true" : "false")
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    /*
    CIMName myClassName = CIMName();
    if (opts.className != "")
    {
        myClassName = opts.className;
    }
    */
    Array<CIMClass> classes; 
    if (opts.time) opts.elapsedTime.reset();
    
    classes = client.enumerateClasses(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    if (opts.summary)
    {
      String s = "classes";
        _displaySummary(classes.size(), s, opts.className.getString(),opts);    
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < classes.size(); i++)
        {
            CIMClass myClass = classes[i];
            OutputFormatClass(opts.outputType, myClass);
        }
    }
    return 0;
}

int deleteClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteClasses "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    
    client.deleteClass(opts.nameSpace, opts.className);

    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    return 0;  
}

int getClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getClass "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << ", deepInheritance = " << (opts.deepInheritance? "true" : "false")
            << ", localOnly = " << (opts.localOnly? "true" : "false")
            << ", includeQualifiers = " << (opts.includeQualifiers? "true" : "false")
            << ", includeClassOrigin = " << (opts.includeClassOrigin? "true" : "false")
            << " PropertyList = " << " ADD property list print HERE KSTEST"
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();

    CIMClass cimClass = client.getClass(opts.nameSpace,
                                        opts.className,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin,
                                        opts.propertyList);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}

    OutputFormatClass(opts.outputType, cimClass);
    return 0;
}

int getProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getProperty "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.instanceName.toString()
            << ", propertyName = " << opts.propertyName
            << endl;
    }
    
    CIMValue cimValue;
    if (opts.time) opts.elapsedTime.reset();
    
    cimValue = client.getProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
                                   
    // ATTN: TODO: display returned property
    return 0;
}


int setProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setProperty "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.instanceName.toString()
            << ", propertyName = " << opts.propertyName
            // KS The new value goes here
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    
    client.setProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName,
                                   opts.newValue);
                                   
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}

 // ATTN: TODO:
    return 0;
}

int getQualifier(CIMClient& client, Options& opts)
{
    cout << "entering getQualifier " << endl;
    //if (opts.verboseTest)
    {
        cout << "getQualifier "
            << "Namespace = " << opts.nameSpace
            << ", Qualifier = " << opts.qualifierName
            << endl;
    }
    
    
    CIMQualifierDecl cimQualifierDecl;
    if (opts.time) opts.elapsedTime.reset();
    
    cimQualifierDecl = client.getQualifier( opts.nameSpace,
                                   opts.qualifierName);
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
                                   
    // display new qualifier
    
    OutputFormatQualifierDecl(opts.outputType, cimQualifierDecl);
    
    return 0;
}
int setQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setQualifiers "
            << "Namespace = " << opts.nameSpace
            // KS add the qualifier decl here.
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    client.setQualifier( opts.nameSpace,
                         opts.qualifierDeclaration);
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    return 0;
}
int deleteQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteQualifiers "
            << "Namespace = " << opts.nameSpace
            << " Qualifier " << opts.qualifierName
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    client.deleteQualifier( opts.nameSpace,
                            opts.qualifierName);
                                   
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    return 0;
}
int enumerateQualifiers(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "enumerateQualifiers "
            << "Namespace = " << opts.nameSpace
            << endl;
    }
  
    Array<CIMQualifierDecl> qualifierDecls;
    if (opts.time) opts.elapsedTime.reset();
    qualifierDecls = client.enumerateQualifiers( opts.nameSpace);
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    
    if (opts.summary)
    {
        cout << qualifierDecls.size() << " returned." << endl;
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < qualifierDecls.size(); i++)
        {
            CIMQualifierDecl myQualifierDecl = qualifierDecls[i];
        
            OutputFormatQualifierDecl(opts.outputType, myQualifierDecl);
        }
    }
                                  
    return 0;
}

/*
    Array<CIMObjectPath> referenceNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY

*/
int referenceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "ReferenceNames "
            << "Namespace = " << opts.nameSpace
            << ", ObjectPath = " << opts.objectName
            << ", resultClass = " << opts.resultClass
            << ", resulRole = " << opts.role
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();

    Array<CIMObjectPath> referenceNames = 
    client.referenceNames( opts.nameSpace, opts.objectName, opts.resultClass, opts.role);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    
    /*
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY */
    
    if (opts.summary)
    {
      String s = "referenceNames";
        _displaySummary(referenceNames.size(),s,
             opts.objectName,opts);    
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < referenceNames.size(); i++)
                    cout << referenceNames[i].toString() << endl;
    }
    return 0;
}        


/****
     Array<CIMObject> references(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );
    */
int references(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "References "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << ", includeQualifiers = " << opts.includeQualifiers
            << ", includeClassOrigin = " << opts.includeClassOrigin
            << ", CIMPropertyList = "  << printPropertyList(opts.propertyList)
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    
    Array<CIMObject> objects =  
    client.references(  opts.nameSpace, 
                        opts.objectName,
                        opts.resultClass,
                        opts.role,
                        opts.includeQualifiers,
                        opts.includeClassOrigin,
                        opts.propertyList);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}

    if (opts.summary)
    {
      String s = "references";
        _displaySummary(objects.size(), s,
             opts.objectName,opts);    
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            OutputFormatObject(opts.outputType, objects[i]);
        }
    }
    return 0;
}        

/*
    Array<CIMObjectPath> associatorNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY
    );

*/
int associatorNames(CIMClient& client, Options& opts)
{
    
    if (opts.verboseTest)
    {
        cout << "associatorNames "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << ", assocClass = " << opts.assocClass
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << ", resultRole = " << opts.resultRole
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    
    Array<CIMObjectPath> associatorNames = 
    client.associatorNames( opts.nameSpace, 
                            opts.objectName,
                            opts.assocClass,
                            opts.resultClass,
                            opts.role,
                            opts.resultRole);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
    /*
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY */
    
    if (opts.summary)
    {
      String s = "associator names";
        _displaySummary(associatorNames.size(), s,
             opts.objectName,opts);    
    }
    else
    {
        // Output the list one per line for the moment.
        for (Uint32 i = 0; i < associatorNames.size(); i++)
                    cout << associatorNames[i].toString() << endl;
    }
    return 0;
}        


/****
    Array<CIMObject> associators(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );
    */
int associators(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "Associators "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << ", assocClass = " << opts.assocClass
            << ", resultClass = " << opts.resultClass
            << ", role = " << opts.role
            << ", resultRole = " << opts.resultRole
            << ", includeQualifiers = " << ((opts.includeQualifiers)? "true" : "false")
            << ", includeClassOrigin = " << ((opts.includeClassOrigin)? "true" : "false")
            << ", propertyList = " << printPropertyList(opts.propertyList)
            << endl;
    }
    if (opts.time) opts.elapsedTime.reset();
    
    Array<CIMObject> objects =  
    client.associators( opts.nameSpace, 
                        opts.objectName,
                        opts.assocClass,
                        opts.resultClass,
                        opts.role,
                        opts.resultRole,
                        opts.includeQualifiers,
                        opts.includeClassOrigin,
                        opts.propertyList);
    
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}

    if (opts.summary)
    {
      String s = "associators";
        _displaySummary(objects.size(), s, opts.objectName,opts);    
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            OutputFormatObject(opts.outputType, objects[i]);
        }
    }
    return 0;
} 

                /*
                    CIMValue invokeMethod(
                    	const CIMNamespaceName& nameSpace,
                    	const CIMObjectPath& instanceName,
                    	const CIMName& methodName,
                    	const Array<CIMParamValue>& inParameters,
	                    Array<CIMParamValue>& outParameters
                 */

 int invokeMethod(CIMClient& client, Options& opts)
 {
     if (opts.verboseTest)
     {
         cout << "Associators"
             << " Namespace = " << opts.nameSpace
             << ", Object = " << opts.objectName
             << ", methodName = " << opts.methodName
             << endl;
         CIMValue retValue;
         Array<CIMParamValue> outParams;

         if (opts.time) opts.elapsedTime.reset();

         retValue = client.invokeMethod(opts.nameSpace, opts.objectName,
             opts.methodName, opts.inParams, outParams);

         if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}
         // Now display the CIMValue

         cout << "Return Value= ";
         XmlWriter::printValueElement(retValue, cout);
         for (Uint32 i = 0; i < outParams.size() ; i++)
        {
             XmlWriter::printParamValueElement(outParams[i], cout);
             /*if (opts.outputType == OUTPUT_XML)
                 XmlWriter::printParamValueElement(instance, cout);
             else (format == OUTPUT_MOF)
             {
                 Array<Sint8> x;
                 MofWriter::appendParamValueElement(x, instance);

                 x.append('\0');

                 mofFormat(cout, x.getData(), 4);
             }*/   
        }

     }
     return 0;
 }
 

 /* Enumerate the Namespaces.  This function is based on using the __Namespace class
    and either returns all namespaces or simply the ones starting at the namespace input
    as the namespace variable.
    It assumes that the input classname is __Namespace.
 */
int enumerateNamespaces_Namespace(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateNamespaces "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className
            << endl;
    }
    Array<CIMNamespaceName> namespaceNames;
    
    // Build the namespaces incrementally starting at the root
    // ATTN: 20030319 KS today we start with the "root" directory but this is wrong. We should be
    // starting with null (no directoyr) but today we get an xml error return in Pegasus
    // returned for this call. Note that the specification requires that the root namespace be used
    // when __namespace is defined but does not require that it be the root for allnamespaces. That
    // is a hole is the spec, not in our code.
    namespaceNames.append(opts.nameSpace);
    Uint32 start = 0;
    Uint32 end = namespaceNames.size();
    if (opts.time) opts.elapsedTime.reset();

    do
    {
        // for all new elements in the output array
        for (Uint32 range = start; range < end; range ++)
        {
            // Get the next increment in naming for all a name element in the array
            Array<CIMInstance> instances = client.enumerateInstances(namespaceNames[range],opts.className);
            for (Uint32 i = 0 ; i < instances.size(); i++)
            {
                Uint32 pos;
                // if we find the property and it is a string, use it.
                if ((pos = instances[i].findProperty("name")) != PEG_NOT_FOUND)
                {
                    CIMValue value;
                    String namespaceComponent;
                    value = instances[i].getProperty(pos).getValue();
                    if (value.getType() == CIMTYPE_STRING)
                    {
                        value.get(namespaceComponent);
                        String ns = namespaceNames[range].getString();
                        ns.append("/");
                        ns.append(namespaceComponent);
                        namespaceNames.append(ns);
                    }
                }
            }
            start = end;
            end = namespaceNames.size();
        }
    }
    while (start != end);
    // Validate that all of the returned entities are really namespaces. It is legal for us to
    // have an name component that is really not a namespace (ex. root/fred/john is a namespace
    // but root/fred is not.
    // There is no clearly defined test for this so we will simply try to get something, in this
    // case a wellknown assoication
    Array<CIMNamespaceName> returnNamespaces;

    for (Uint32 i = 0 ; i < namespaceNames.size() ; i++)
    {
        try
        {
            CIMQualifierDecl cimQualifierDecl;
            cimQualifierDecl = client.getQualifier(namespaceNames[i],
                                           "Association");

            returnNamespaces.append(namespaceNames[i]);
        }
        catch(CIMException& e)
        {
            if (e.getCode() != CIM_ERR_INVALID_NAMESPACE)
                returnNamespaces.append(namespaceNames[i]);
        }
    }
    if (opts.time) {opts.saveElapsedTime = opts.elapsedTime.getElapsed();}

    if (opts.summary)
    {
        cout << returnNamespaces.size() << " namespaces " << " returned." << endl;
    }
    else
    {
        for( Uint32 cnt = 0 ; cnt < returnNamespaces.size(); cnt++ ) 
        {
            cout << returnNamespaces[cnt] << endl;;
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////
// The following section manages the input options.  It includes   //
// both the options processor (based on common/options             //
// and the options table.                                          //
/////////////////////////////////////////////////////////////////////

void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& testHome)
{
    static const char* outputFormats[] = { "xml", "mof", "txt"};
    static const Uint32 NUM_OUTPUTFORMATS = sizeof(outputFormats) /
                                            sizeof(outputFormats[0]);

    static OptionRow optionsTable[] =
        //optionname defaultvalue rqd  type domain domainsize clname hlpmsg
    {
        /*{"authenticate", "false" , false, Option::BOOLEAN, 0, 0, "a",
                                        "Defines whether user authentication is used" },*/
        {"User", "", false, Option::STRING, 0, 0, "u",
                                        "Defines User Name for authentication" },
        
        {"Password", "", false, Option::STRING, 0, 0, "p",
                                        "Defines password for authentication" },
        
        {"location", "localhost:5988", false, Option::STRING, 0, 0, "l",
                            "specifies system and port (HostName:port). Port is optional" },
        
        {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
                            "Specifies namespace to use for operation" },
        
        {"deepInheritance", "false", false, Option::BOOLEAN, 0, 0, "di",
                            "If set deepInheritance parameter set true "},
        
        {"localOnly", "false", false, Option::BOOLEAN, 0, 0, "lo",
                            "If set localonly parameter set true "},
        
        {"includeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "iq",
                            "If set sets includeQualifiers option true "},
        
        {"includeClassOrigin", "false", false, Option::BOOLEAN, 0, 0, "ic",
                            "If set includeClassOriginOption set toTrue"},

        {"propertyList", "", false, Option::STRING, 0, 0, "pl",
                            "Defines a propertyNameList. Format is p1,p2,p3 (without spaces) "},
        
        {"assocClass", "", false, Option::STRING, 0, 0, "ac",
                            "Defines a assocClass string for Associator calls"},
        
        {"AssocRole", "", false, Option::STRING, 0, 0, "ar",
                            "Defines a role string for Associatiors AssocRole parameter"},

        
        {"resultClass", "", false, Option::STRING, 0, 0, "rc",
                            "Defines a resultClass string for References and Associatiors "},
        
        {"resultRole", "", false, Option::STRING, 0, 0, "rr",
                            "Defines a role string for References, etc. "},

        {"inputParameters", "", false, Option::STRING, 0, 0, "ip",
                            "Defines an invokeMethod input parameter list. Format is p1=v1,p2=v2,..,pn=vn (without spaces) "},
        
        {"outputParameters", "", false, Option::STRING, 0, 0, "op",
                            "Defines an invokeMethod output parameter list. Format is p1=v1,p2=v2,..,pn=vn (without spaces) "},
        
        {"filter", "", false, Option::STRING, 0, 0, "f",
                            "defines a filter to use for query. One String input "},

        {"substitute", "", false, Option::STRING, 0, 0, "-s",
                            "Defines a conditional substition of input parameters. ) "},
        
        // KS change the output formats to use the enum options function
        // Deprecate this function.
        {"outputformats", "mof", false, Option::STRING, 0,NUM_OUTPUTFORMATS, "o",
                            "Output in xml, mof, txt"},
        
        {"xmlOutput", "false", false, Option::BOOLEAN, 0,0, "x",
                            "Output objects in xml instead of mof format"},

        {"version", "false", false, Option::BOOLEAN, 0, 0, "-v",
                            "Displays software Version "},
        
        {"verbose", "false", false, Option::BOOLEAN, 0, 0, "v",
                            "Verbose Display. Includes Detailed Param Input display "},
        
        
        {"summary", "false", false, Option::BOOLEAN, 0, 0, "s",
                            "Displays only summary count for enumerations, associators, etc. "},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                            "Prints help message with command line options "},


        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
                            "Not Implemented "},
        
        {"trace", "0", false, Option::WHOLE_NUMBER, 0, 0, "trace", 
                            "Set Pegasus Common Components Trace. Sets the Trace level. 0 is off"},

        {"delay", "0", false, Option::WHOLE_NUMBER, 0, 0, "delay", 
                            "Delay between connection and request "},

        {"count", "97832", false, Option::NATURAL_NUMBER, 0, 0, "count", 
                            "Expected count of objects returned if  "},

        {"repeat", "0", false, Option::WHOLE_NUMBER, 0, 0, "-r", 
                            "Number of times to repeat the function. Zero means one time "},

        {"time", "false", false, Option::BOOLEAN, 0, 0, "-t", 
                            "Measure time for the operation and present results. "},


    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = "CLI.conf";

    //cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
             om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}
/* showCommands - Display the list of operation commands.
*/
void showCommands()
{
    for( Uint32 i = 0; i < NUM_COMMANDS; i++ ) 
    {
        printf("%-5s %-21s%s\n",CommandTable[i].ShortCut, CommandTable[i].CommandName, CommandTable[i].UsageText);
    }
}
/* PrintHelpMsg - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void printHelpMsg(const char* pgmName, const char* usage, const char* extraHelp, 
                OptionManager& om)
{
    String header = "Usage: cli <CIMOperationName> <CIMOperationObject> <Options>";
    String header2 = "  Execute the <CimOperationName> on the <CIMOperationObject> with <Options>"; 
    
    String optionsTrailer = "Options vary by command consistent with CIM Operations";
    cout << endl << pgmName << " V"<< version << " " << header << endl <<header2 << endl;
    cout << endl;
    cout << "The options for this command are:" << endl;
    om.printOptionsHelpTxt(usage, optionsTrailer);
    //om.printHelp(const char* pgmName, OptionManager om);
    
    showCommands();
    
    cout << endl;
    
    cout << extraHelp << endl;

}

int CheckCommonOptionValues(OptionManager& om, char** argv, Options& opts) 
{
    // Check to see if user asked for help (-h otpion):
    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;

    if (verboseTest)
        opts.verboseTest = verboseTest;

    // Base code for parameter substition.  Dispabled until complete
    /*{
        String target;
        if (om.lookupValue("substitution", target))
        {
            // Get directory for file
            Array<String> fileList;
            String fileName = "/conditions.txt";
            String temp = ".";
            temp.append(fileName);
            fileList.append(temp);
            const char* tmpDir = getenv ("PEGASUS_HOME");
            if (tmpDir != NULL)
            {
                temp = tmpDir;
                temp.append(fileName);
                fileList.append(temp);
            }
    }*/
    String temprole;
    if(om.lookupValue("role", temprole))
    {
        // we need to deliver String::EMPTY when no param.
        if (temprole != "")
            opts.role = temprole;
        if (verboseTest)
           cout << "role = " << opts.role << endl;
    }

    String tempResultRole;
    if(om.lookupValue("resultRole", tempResultRole))
    {
        // we need to deliver String::EMPTY when no param.
        if (tempResultRole != "")
            opts.role = tempResultRole;
        if (verboseTest)
           cout << "resultrole = " << opts.resultRole << endl;
    }

    om.lookupValue("location", opts.location);

    if(om.lookupValue("resultClass", opts.resultClassName))
    {
       
       if (opts.resultClassName != "")
       {
           //Covers fact that assigning to CIMName can cause exception.
           try
           {
               opts.resultClass = opts.resultClassName;
           }
           catch(Exception& e)
           {
               cout << "Error in Result Class. Exception " << e.getMessage() << endl;
               exit(1);
           }
       }
       if (verboseTest)
           cout << "resultClassName = " << opts.resultClassName << endl;
    }


    if(!om.lookupIntegerValue("delay", opts.delay))
    {
            opts.delay = 0;
    }
    if (verboseTest && opts.delay != 0)
    {
        cout << "delay set to " << opts.delay << " Seconds" << endl;
    }


    if(om.lookupValue("assocClass", opts.assocClassName))
    {
       if (verboseTest)
           cout << "assocClassName = " << opts.assocClassName << endl;
       if (opts.assocClassName != "")
       {
           try
           {
               opts.assocClass = opts.assocClassName;
           }
           catch(Exception& e)
           {
               cout << "Error in assoc Class. Exception " << e.getMessage() << endl;
               exit(1);
           }
       }
    }

    opts.deepInheritance = om.isTrue("deepInheritance");
    if (om.isTrue("deepInheritance")  & verboseTest)
        cout << "deepInteritance set" << endl;
    
    opts.localOnly = om.isTrue("localOnly");
    if (om.isTrue("localOnly")  & verboseTest)
        cout << "localOnly set" << endl;
    
    opts.includeQualifiers = om.isTrue("includeQualifiers");
    if (om.isTrue("includeQualifiers")  & verboseTest)
        cout << "includeQualifiers set" << endl;
    
    opts.includeClassOrigin = om.isTrue("includeClassOrigin");
    if (om.isTrue("includeClassOrigin")  & verboseTest)
        cout << "includeClassOrigin set" << endl;

    opts.time = om.isTrue("time");
    if (om.isTrue("time")  & verboseTest)
        cout << "time set" << endl;

    if(!om.lookupIntegerValue("trace", opts.trace))
    {
            opts.trace = 0;
    }
    else
    {
        Uint32 traceLevel = 0;
        switch (opts.trace)
        {
            case 0:             // This covers the default.
                break;
            case 1 :
                traceLevel = Tracer::LEVEL1;
                break;
            case 2 :
                traceLevel = Tracer::LEVEL2;
                break;
            case 3 :
                traceLevel = Tracer::LEVEL3;
                break;
            case 4 :
                traceLevel = Tracer::LEVEL4;
                break;
            default:
                cout << "Illegal value for Trace. Max = 4" << endl;
        }
        opts.trace = traceLevel;
    }
    if (verboseTest && opts.trace != 0)
    {
        cout << "Pegasus Trace set to  Level  " << opts.trace << endl;
    }


    opts.summary = om.isTrue("summary");
    
    // get User name and password if set.
     if(om.lookupValue("User", opts.user))
     {
         if (opts.user.size() == 0)
             opts.user = String::EMPTY;
         if (verboseTest)
             cout << "User = " << opts.user << endl;
     }
     
     if(om.lookupValue("Password", opts.password))
     {
         if (opts.password.size() == 0)
             opts.password = String::EMPTY;
         if (verboseTest)
             cout << "Password = " << opts.password << endl;
     }
    
    // Create a variable with the format output and a correponding type.
    // Suggest we might change this whole thing to the option type that
    // mike used in the example of colors so that  you could do -red -blue
    // or in our case -mof -xml, etc.
    
     opts.isXmlOutput = om.isTrue("xmlOutput");
     if (opts.isXmlOutput  & verboseTest)
         cout << "xmlOutput set" << endl;

    if(om.lookupValue("outputformats", opts.outputFormat))
     {
        if (verboseTest)
            cout << "Output Format = " << opts.outputFormat << endl;
     }

    // Get the output format parameter and save it
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    for( ; cnt < NUM_OUTPUTS; cnt++ ) 
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    // Note that this makes no notice of a not found
    if (cnt != NUM_OUTPUTS)
    {
        opts.outputFormatType = cnt;
        opts.outputType = OutputTable[cnt].OutputType;
    }

    if(!om.lookupIntegerValue("repeat", opts.repeat))
    {
            opts.repeat = 0;
            
    }
    if (opts.verboseTest)
    {
        cout << "Repeat Count " << opts.repeat << endl;
    }

    if(!om.lookupIntegerValue("count", opts.count))
    {
            opts.count = 97832;
    }
    if (opts.verboseTest)
    {
        if (opts.count != 97832)
        {
        cout << "Comparison Count " << opts.repeat << endl;
        }
    }

    /*  Property List parameter.
        Separate an input stream into an array of Strings
    */
    {
        String properties;
        Array<CIMName> paramList;
        if(om.lookupValue("propertyList", properties))
        {
            
            Array<String> pList =  _tokenize(properties, ',');

            if (verboseTest)
            {
                cout << "Property List =";
                for (Uint32 i = 0 ; i < pList.size() ; i++)
                    cout << " " << pList[i] << endl;
            }
            for (Uint32 i = 0 ; pList.size(); i++)
                paramList.append(CIMName(pList[i]));

            opts.propertyList.set(paramList);
        }
    }

    /* Method input parameters processing.  Process as one
       string containing multiple parameters in the form
       name=value
       ATTN: KS 20030426 Note that we have not accounted for
       the typing on parameters. We must do that at some point
    */
    String inputParameters;
    if(om.lookupValue("inputParameters", inputParameters))
    {

        Array<String> pList =  _tokenize(inputParameters, ',');
        for (Uint32 i = 0 ; i< pList.size() ; i++)
        {
            _createMethodParamValue(pList[i]);
        }
    }

    return 0;
}


//------------------------------------------------------------------------------
//
// mofFormat
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentSize)
{
    Uint32 n = level * indentSize;
    if (n > 50)
    {
    cout << "Jumped Ship " << level << " size " << indentSize << endl;
    exit(1);
    }

    for (Uint32 i = 0; i < n; i++)
        os << ' ';
}

/* Format the output stream for indented MOF format
*/
void mofFormat(
    PEGASUS_STD(ostream)& os, 
    const char* text, 
    Uint32 indentSize)
{
    char* tmp = strcpy(new char[strlen(text) + 1], text);
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    Boolean qualifierState = false;
    char c;
    char prevchar;
    while (c = *tmp++)
    {
        count++;
        // This is too simplistic and must move to a token based mini parser
        // but will do for now. One problem is tokens longer than 12 characters that
        // overrun the max line length.
        switch (c)
        {
            case '\n':
                os << Sint8(c);
                prevchar = c;
                count = 0 + (indent * indentSize);
                _indent(os, indent, indentSize);   
                break;

            case '\"':   // quote 
                os << Sint8(c);
                prevchar = c;
                quoteState = !quoteState;
                break;

            case ' ':
                os << Sint8(c);
                prevchar = c;
                if (count > 66)
                {
                    if (quoteState)
                    {   
                        os << "\"\n";
                        _indent(os, indent + 1, indentSize);   
                        os <<"\"";
                    }
                    else
                    {
                        os <<"\n";
                        _indent(os, indent + 1,  indentSize);   
                    }
                    count = 0 + ((indent + 1) * indentSize);
                }
                break;
            case '[':
                if (prevchar == '\n')
                {
                    indent++;
                    _indent(os, indent,  indentSize);
                    qualifierState = true;
                }
                os << Sint8(c);
                prevchar = c;
                break;

            case ']':
                if (qualifierState)
                {
                    if (indent > 0)
                        indent--;
                    qualifierState = false;
                }
                os << Sint8(c);
                prevchar = c;
                break;

            default:
                os << Sint8(c);
                prevchar = c;
        }

    }
}

PEGASUS_NAMESPACE_END
// END_OF_FILE
