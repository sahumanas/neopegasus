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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>


#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        tmpDir = ".";
    }
    String path;
    assert(FileSystem::getCurrentDirectory(path));
    // Need to add test to confirm that the directory
    // is indeed FileSystem. 
    assert(FileSystem::exists("FileSystem.cpp"));
    assert(!FileSystem::exists("NoSuchFile.dat"));
    // assert(!FileSystem::canExecute("FileSytem.cpp"));
    assert(!FileSystem::canRead("NoSuchFile.dat"));
    assert(!FileSystem::canWrite("NoSuchFile.dat"));
    // assert(!FileSystem::canRead("NoCanRead.dat"));
    assert(FileSystem::isDirectory(".."));
    assert(FileSystem::isDirectory("."));
    assert(!FileSystem::isDirectory("FileSystem.cpp"));
    assert(FileSystem::isDirectory("./testdir"));

    Array<String> paths;
    assert( FileSystem::getDirectoryContents("./testdir", paths) );

    for (Uint32 i = 0; i < paths.size(); i++)
    {
	if (String::equal(paths[i], "CVS"))
	{
	    paths.remove(i);
	    break;
	}
    }

    String realName;
    assert(FileSystem::existsNoCase("filesystem.cpp", realName));
    assert(String::equal(realName, "FileSystem.cpp"));

    assert(FileSystem::existsNoCase(
	"../FileSystem/filesystem.cpp", realName));
    assert(String::equal(realName, "../FileSystem/FileSystem.cpp"));

    BubbleSort(paths);
    assert(paths.size() == 3);
    assert(String::equal(paths[0], "a"));
    assert(String::equal(paths[1], "b"));
    assert(String::equal(paths[2], "c"));

    // Test for getCurrentDirectory
    // Go to testdir, test for file "a"
    // Then return and test for file
    {
	String saveDir;
	assert(FileSystem::getCurrentDirectory(saveDir));
	assert(FileSystem::changeDirectory("testdir"));
	assert(FileSystem::exists("a"));
	FileSystem::changeDirectory(saveDir);
	String newSaveDir;
	assert(FileSystem::getCurrentDirectory(newSaveDir));
	assert(saveDir == newSaveDir);
	assert(FileSystem::exists("FileSystem.cpp"));
    }
    // Test the Create and delete functions
    // Creates directories and files and deletes them.
    {
        String t (tmpDir);
        t += "/TestDirectory";
        String t1 (tmpDir);
        t1 += "/TestDirectory2";
        String tf (tmpDir);
        tf += "/TestFile.txt";
        ArrayDestroyer <char> tfd (tf.allocateCString ());
        const char* f = tfd.getPointer ();
        String tf1 (tmpDir);
        tf1 += "/TestFile1.txt";
        ArrayDestroyer <char> tf1d (tf1.allocateCString ());
        const char* f1 = tf1d.getPointer ();

	FileSystem::makeDirectory(t);
	assert(FileSystem::isDirectory(t));
	FileSystem::removeDirectory(t);
	assert(!FileSystem::isDirectory(t));

	// Tests for remove hiearchy command
	// ATTN: Removed following until next test ks
	// because remove hiearchy does not work yet.
	FileSystem::makeDirectory(t);

	String save_cwd;
	FileSystem::getCurrentDirectory(save_cwd);

	// create some files in new directory
	if (!FileSystem::changeDirectory(t))
	    // ATTN: what is valid error return?
	    return -1;

  	ofstream of1(f);
	of1 << "test" << endl;
	of1.close();
	assert(FileSystem::exists(f));

  	ofstream of2(f1);
	of2 << "test" << endl;
	of2.close();
	assert(FileSystem::exists(f1));

	// Create a second level directory
	FileSystem::makeDirectory(t1);

	// Create files in this dir
	if (!FileSystem::changeDirectory(t1))
	    return -1;

	ofstream of3("testfile3.txt");
	of3 << "test" << endl;
	of3.close();

    	ofstream of4("testfile4.txt");
	of4 << "test" << endl;
	of4.close();

	// Go back to top level directory

	FileSystem::changeDirectory(save_cwd);
        assert(FileSystem::isDirectory(t));
	FileSystem::removeDirectoryHier(t);
	// be sure directory is removed
	assert(!FileSystem::isDirectory(t));
        
    }
    // Test renameFile:
    {
        String rf1 (tmpDir);
        rf1 += "/file1.txt";
        ArrayDestroyer <char> rf1d (rf1.allocateCString ());
        const char* FILE1 = rf1d.getPointer ();
        String rf2 (tmpDir);
        rf2 += "/file2.txt";
        ArrayDestroyer <char> rf2d (rf2.allocateCString ());
        const char* FILE2 = rf2d.getPointer ();

  	ofstream of1(FILE1);
	of1 << "test" << endl;
	of1.close();
	assert(FileSystem::exists(FILE1));

	assert(FileSystem::exists(FILE1));
	assert(!FileSystem::exists(FILE2));
	assert(FileSystem::renameFile(FILE1, FILE2));

	assert(!FileSystem::exists(FILE1));

	assert(FileSystem::exists(FILE2));
	assert(FileSystem::renameFile(FILE2, FILE1));
	assert(FileSystem::exists(FILE1));
	assert(!FileSystem::exists(FILE2));
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
