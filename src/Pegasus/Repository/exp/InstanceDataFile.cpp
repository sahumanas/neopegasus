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
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By: Michael E. Brasher (mbrasher@bmc.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <Pegasus/Common/Config.h>
#include "InstanceDataFile.h"
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Boolean InstanceDataFile::_openFile(
    PEGASUS_STD(fstream)& fs,
    const String& path,
    int mode)
{
    if (FileSystem::openNoCase(fs, path, mode))
	return true;

    //
    // File does not exist so create it:
    //

    ArrayDestroyer<char> p(path.allocateCString());
    fs.open(p.getPointer(), mode);

    return !!fs;
}

Boolean InstanceDataFile::loadInstance(
    const String& path, 
    Uint32 index,
    Uint32 size,  
    Array<Sint8>& data)
{
    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(fs, path, ios::binary | ios::in))
	return false;

    //
    // Postion file pointer:
    //

    fs.seekg(index);

    if (!fs)
	return false;

    //
    // Read the instance:
    //

    data.grow(size, '\0');
    fs.read((char*)data.getData(), size);

    if (!fs)
	return false;

    //
    // Close the file:
    //

    fs.close();

    return true;
}

Boolean InstanceDataFile::loadAllInstances(
    const String& path, 
    Array<Sint8>& data)
{
    //
    // Get size of the data file:
    //

    Uint32 fileSize;

    if (!FileSystem::getFileSizeNoCase(path, fileSize))
	return false;

    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(fs, path, ios::binary | ios::in))
	return false;

    //
    // Suck the entire contents of the file into the data array parameter:
    //

    data.grow(fileSize, '\0');
    fs.read((char*)data.getData(), fileSize);

    if (!fs)
	return false;

    //
    // Close the file:
    //

    fs.close();

    return true;
}

Boolean InstanceDataFile::appendInstance(
    const String& path, 
    const Array<Sint8>& data)
{
    //
    // Open the file for append:
    //

    fstream fs;

    if (!_openFile(fs, path, ios::binary | ios::app))
	return false;

    //
    // Write the instance:
    //

    fs.write((char*)data.getData(), data.size());

    if (!fs)
	return false;

    //
    // Close the file:
    //

    fs.close();

    return true;
}

Boolean InstanceDataFile::beginTransacation(const String& path)
{
    //
    // Get the size of the rollback file:
    //

    Uint32 fileSize;

    if (!FileSystem::getFileSizeNoCase(path, fileSize))
	return false;

    //
    // Open the rollback file:
    //

    String rollbackFilePath = Cat(path, ".rollback");

    fstream fs;

    if (!_openFile(fs, rollbackFilePath, ios::binary | ios::out))
	return false;

    //
    // Save the size of the data file in the rollback file.
    //

    char buffer[9];
    sprintf(buffer, "%08x", fileSize);
    fs.write(buffer, strlen(buffer));

    //
    // Close the file.
    //

    fs.close();

    return true;
}

Boolean InstanceDataFile::rollbackTransaction(const String& path)
{
    //
    // Open the rollback file:
    //

    String rollbackFilePath = Cat(path, ".rollback");

    fstream rollbackFs;

    if (!_openFile(rollbackFs, rollbackFilePath, ios::binary | ios::out))
	return false;

    //
    // Retrieve the file size from the rollback file:
    //

    char buffer[9];
    rollbackFs.read(buffer, 8);

    if (!rollbackFs)
	return false;

    buffer[8] = '\0';

    char* end = 0;
    long fileSize = strtol(buffer, &end, 16);

    if (!end || !*end | tmp < 0)
	return false;

    rollbackFs.close();

    //
    // Now truncate the data file to that size:
    //

    fstream fs;

    if (!_openFile(fs, path, ios::binary | ios::out))
	return false;

    fs.seekg(fileSize);
    fs.close();
}

Boolean InstanceDataFile::commitTransaction(const String& path)
{
    //
    // To commit the transaction, we simply remove the rollback file:
    //

    return FileSystem::removeFileNoCase(Cat(path, ".rollback"));
}

Boolean InstanceDataFile::compact(
    const String& path,
    const Array<Uint32>& freeFlags,
    const Array<Uint32>& indices,
    const Array<Uint32>& sizes)
{
    //
    // Open the input file (the data file):
    //

    fstream fs;

    if (!_openFile(fs, path, ios::binary | ios::in))
	return false;

    //
    // Open the output file (temporary data file):
    //

    fstream tmpFs;

    if (!_openFile(tmpFs, path, ios::binary | ios::out))
	return false;

    Array<Sint8> data;

    //
    // Copy over instances which have not been freed:
    //

    for (Uint32 i = 0, n = freeFlags.size(); i < n; i++)
    {
	//
	// If this entry is not free, then copy it over to the
	// temporary file. Otherwise, pay retail for it.
	//

	if (!freeFlags[i])
	{
	    //
	    // Read the next instance:
	    //

	    if (!fs.seekg(indices[i]))
		return false;

	    data.grow(sizes[i], '\0');

	    fs.read((char*)data.getData(), sizes[i]);

	    if (!fs)
		return false;

	    //
	    //  Write out the next instance:
	    //

	    tmpFs.write((char*)data.getData(), sizes[i]);
	}
    }

    //
    // Close the files:
    //
    
    fs.close();
    tmpFs.close();

    return true;
}

PEGASUS_NAMESPACE_END
