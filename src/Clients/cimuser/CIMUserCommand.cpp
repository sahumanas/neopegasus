//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Sushma Fernandes, Hewlett Packard Company (sushma_fernandes@hp.com)
//
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>

#include <iostream>

#include <Clients/cliutils/Command.h>
#include <Clients/cliutils/CommandException.h>

#include <Pegasus/getoopt/getoopt.h>

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/PegasusVersion.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//l10n
/**
 * The CLI message resource name
 */

static const char MSG_PATH [] 				= "pegasus/pegasusCLI";

/**
    The command name.
*/
static const char COMMAND_NAME []              = "cimuser";

/**
    This constant represents the name of the User name property in the schema
*/
static const CIMName PROPERTY_NAME_USER_NAME            = CIMName ("Username");

/**
    This constant represents the name of the Password property in the schema
*/
static const CIMName PROPERTY_NAME_PASSWORD             = CIMName ("Password");

/**
    The usage string for this command.  This string is displayed
    when an error occurs in parsing or validating the command line.
*/
static const char USAGE []                     = "usage: ";

/**
    This constant represents the getoopt argument designator
*/
static const char GETOPT_ARGUMENT_DESIGNATOR   = ':';

/*
    These constants represent the operation modes supported by the CLI.
    Any new operation should be added here.
*/

/**
    This constant signifies that an operation option has not been recorded
*/
static const Uint32 OPERATION_TYPE_UNINITIALIZED  = 0;

/**
    This constant represents a add user operation
*/
static const Uint32 OPERATION_TYPE_ADD            = 1;

/**
    This constant represents a add user operation
*/
static const Uint32 OPERATION_TYPE_MODIFY         = 2;

/**
    This constant represents a remove user operation
*/
static const Uint32 OPERATION_TYPE_REMOVE         = 3;

/**
    This constant represents a list operation
*/
static const Uint32 OPERATION_TYPE_LIST           = 4;


/**
    The constants representing the messages.
*/

//l10n

static const char NOT_PRIVILEGED_USER []         = 
             "You must have superuser privilege to run this command.";

static const char NOT_PRIVILEGED_USER_KEY []         = "Clients.cimuser.CIMUserCommand.NOT_PRIVILEGED_USER";

static const char CIMOM_NOT_RUNNING []         = 
                        "CIM Server may not be running.";

static const char CIMOM_NOT_RUNNING_KEY []         = "Clients.cimuser.CIMUserCommand.CIMOM_NOT_RUNNING";

static const char ADD_USER_FAILURE []    = 
                        "Failed to add user.";

static const char ADD_USER_FAILURE_KEY []    = "Clients.cimuser.CIMUserCommand.ADD_USER_FAILURE";

static const char REMOVE_USER_FAILURE []    = 
                        "Failed to remove user.";

static const char REMOVE_USER_FAILURE_KEY []    = "Clients.cimuser.CIMUserCommand.REMOVE_USER_FAILURE";

static const char CHANGE_PASSWORD_FAILURE []  = 
                        "Failed to change password.";

static const char CHANGE_PASSWORD_FAILURE_KEY []  = "Clients.cimuser.CIMUserCommand.CHANGE_PASSWORD_FAILURE";

static const char LIST_USERS_FAILURE [] = 
                        "Failed to list the users. ";

static const char LIST_USERS_FAILURE_KEY [] = "Clients.cimuser.CIMUserCommand.LIST_USERS_FAILURE";

static const char ADD_USER_SUCCESS []    = 
                        "User added successfully.";

static const char ADD_USER_SUCCESS_KEY []    = "Clients.cimuser.CIMUserCommand.ADD_USER_SUCCESS";

static const char REMOVE_USER_SUCCESS[]  = 
                        "User removed successfully.";

static const char REMOVE_USER_SUCCESS_KEY[]  = "Clients.cimuser.CIMUserCommand.REMOVE_USER_SUCCESS";

static const char CHANGE_PASSWORD_SUCCESS []  = 
                        "Password changed successfully.";

static const char CHANGE_PASSWORD_SUCCESS_KEY []  = "Clients.cimuser.CIMUserCommand.CHANGE_PASSWORD_SUCCESS";

static const char PASSWORD_BLANK []  = 
                     "Password cannot be blank. Please re-enter your password.";

static const char PASSWORD_BLANK_KEY []  = "Clients.cimuser.CIMUserCommand.PASSWORD_BLANK";

static const char NO_USERS_FOUND[] =
			"No users found for listing.";

static const char NO_USERS_FOUND_KEY[] = "Clients.cimuser.CIMUserCommand.NO_USERS_FOUND";

static const char AUTH_SCHEMA_NOT_LOADED []  =
    "Please restore the internal repository on the CIM Server.";

static const char AUTH_SCHEMA_NOT_LOADED_KEY []  = "Clients.cimuser.CIMUserCommand.AUTH_SCHEMA_NOT_LOADED";

static const char REQUIRED_ARGS_MISSING []        =
                        "Required arguments missing.";

static const char REQUIRED_ARGS_MISSING_KEY []        = "Clients.cimuser.CIMUserCommand.REQUIRED_ARGS_MISSING";

static const char INVALID_ARGS []        =
                        "Invalid arguments.";

static const char INVALID_ARGS_KEY []        = "Clients.cimuser.CIMUserCommand.INVALID_ARGS";

static const char USER_ALREADY_EXISTS []        =
                        "Specified user name already exist.";

static const char USER_ALREADY_EXISTS_KEY []        = "Clients.cimuser.CIMUserCommand.USER_ALREADY_EXISTS";

static const char USER_NOT_FOUND []        =
                        "Specified user name was not found.";

static const char USER_NOT_FOUND_KEY []        = "Clients.cimuser.CIMUserCommand.USER_NOT_FOUND";

static const char USERNAME_REQUIRED []        =
                        "User name is required.";

static const char USERNAME_REQUIRED_KEY []        = "Clients.cimuser.CIMUserCommand.USERNAME_REQUIRED";

/**
    The option character used to specify add user.
*/
static const char   OPTION_ADD                 = 'a';

/**
    The option character used to specify modify user.
*/
static const char   OPTION_MODIFY              = 'm';

/**
    The option character used to specify user name.
*/
static const char   OPTION_USER_NAME           = 'u';

/**
    The option character used to specify password.
*/
static const char   OPTION_PASSWORD            = 'w';

/**
    The option character used to specify new password.
*/
static const char   OPTION_NEW_PASSWORD        = 'n';

/**
    The option character used to specify remove user.
*/
static const char   OPTION_REMOVE              = 'r';

/**
    The option character used to specify listing of users.
*/
static const char   OPTION_LIST                = 'l';

/**
    The name of the Method that implements modify password
*/
static const CIMName   MODIFY_METHOD            = CIMName ("modifyPassword");

/**
    The input parameter name for old password 
*/
static const char   OLD_PASS_PARAM[]             = "OldPassword";

/**
    The input parameter name for new password 
*/
static const char   NEW_PASS_PARAM[]             = "NewPassword";


static const char   PASSWORD_PROMPT []  =
                        "Please enter your password: ";

static const char   PASSWORD_PROMPT_KEY [] = "Clients.cimuser.CIMUserCommand.PASSWORD_PROMPT";

static const char   OLD_PASSWORD_PROMPT []  =
                        "Please enter your old password: ";

static const char   OLD_PASSWORD_PROMPT_KEY []  ="Clients.cimuser.CIMUserCommand.OLD_PASSWORD_PROMPT";

static const char   RE_ENTER_PROMPT []  =
                        "Please re-enter your password: ";

static const char   RE_ENTER_PROMPT_KEY []  ="Clients.cimuser.CIMUserCommand.RE_ENTER_PROMPT";

static const char   NEW_PASSWORD_PROMPT []  =
                        "Please enter your new password: ";

static const char   NEW_PASSWORD_PROMPT_KEY []  ="Clients.cimuser.CIMUserCommand.NEW_PASSWORD_PROMPT";

static const char   PASSWORD_DOES_NOT_MATCH []  =
                        "Passwords do not match. Please Re-enter.";

static const char   PASSWORD_DOES_NOT_MATCH_KEY []  = "Clients.cimuser.CIMUserCommand.PASSWORD_DOES_NOT_MATCH";

static const char   PASSWORD_SAME_ERROR []  =
                        "Error, new and old passwords cannot be same.";
                        
static const char   PASSWORD_SAME_ERROR_KEY []  = "Clients.cimuser.CIMUserCommand.PASSWORD_SAME_ERROR";
/**
This is a CLI used to manage users of the CIM Server.  This command supports 
operations to add, modify, list and remove users.  

@author Sushma Fernandes, Hewlett-Packard Company
*/

class CIMUserCommand : public Command 
{

public:

    /**    
        Constructs a CIMUserCommand and initializes instance variables.
    */
    CIMUserCommand ();

    //
    // Overrides the virtual function setCommand from Command class
    // This is defined as an empty function. 
    //
    void setCommand (
		      Uint32                   argc, 
		      char*                    argv [])
		      throw (CommandFormatException)
    {
        // Empty function 
    }

    /**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments. This implementation of
    setCommand includes the parameters for output and error stream. 

    @param  ostream    The stream to which command output is written.
    @param  ostream    The stream to which command errors are written.
    @param  args       The string array containing the command line arguments
    @param  argc       The int containing the arguments count

    @throws  CommandFormatException  if an error is encountered in parsing
                                     the command line
    */
    void setCommand (
                      ostream&                outPrintWriter, 
                      ostream&                errPrintWriter,
		      Uint32                  argc, 
		      char*                   argv []);

    /**
    Executes the command and writes the results to the output streams.

    @param ostream    The stream to which command output is written.
    @param ostream    The stream to which command errors are written.
    @return  0        if the command is successful
             1        if an error occurs in executing the command
    */
    Uint32 execute ( 
		      ostream&                outPrintWriter,
                      ostream&                errPrintWriter);


private:

    //
    // Add a new user to the CIM Server
    //
    // @param ostream        The stream to which command output is written.
    // @param ostream        The stream to which command errors are written.
    // 
    // @exception Exception  if failed to add user
    //
    void _AddUser
        (
        ostream&    		outPrintWriter, 
        ostream&    		errPrintWriter
        ); 

    //
    // Modify an existing user's password.
    //
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    // 
    // @exception Exception  if failed to modify password
    //
    void _ModifyUser
        (
        ostream&                 outPrintWriter,
        ostream&                 errPrintWriter
        );

    //
    // Remove an existing user from the CIM Server 
    //  
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    //
    // @exception Exception  if failed to remove user
    //
    void _RemoveUser
        (
        ostream&		outPrintWriter, 
        ostream&		errPrintWriter
        ); 

    //
    // List all users.       
    // 
    // @param ostream        The stream to which command output is written.
    // @param ostream        The stream to which command errors are written.
    //
    void _ListUsers
    (
	ostream&                outPrintWriter,
	ostream&                errPrintWriter
    );

    //
    // The CIM Client reference
    //
    CIMClient*    _client;

    //
    // The host name. 
    //
    String        _hostName;

    //
    // The name of the user.
    //
    String        _userName;

    //
    // The password of the user. 
    //
    String        _password;

    //
    // The new password of the user. 
    //
    String        _newpassword;

    //
    // The type of operation specified on the command line. 
    //
    Uint32        _operationType;

    //
    // Flags for command options
    //
    Boolean       _userNameSet;
    Boolean       _passwordSet;
    Boolean       _newpasswordSet;

};

/**
    Constructs a CIMUserCommand and initializes instance variables.
*/
CIMUserCommand::CIMUserCommand ()
{
    /**
        Initialize the instance variables.
    */
    _operationType       = OPERATION_TYPE_UNINITIALIZED;
    _userName            = String::EMPTY;
    _password            = String::EMPTY;
    _newpassword         = String::EMPTY;
    _hostName            = String::EMPTY;
    _passwordSet         = false;
    _newpasswordSet      = false;
    _userNameSet         = false;

    /**
        Build the usage string for the config command.  
    */
    String usage;
    usage.reserveCapacity(200);
    usage.append(USAGE);
    usage.append(COMMAND_NAME);

    usage.append(" -").append(OPTION_ADD);
    usage.append(" -").append(OPTION_USER_NAME).append(" username");
    usage.append(" [ -").append(OPTION_PASSWORD).append(" password")
         .append(" ] \n");

    usage.append("               -").append(OPTION_MODIFY);
    usage.append(" -").append(OPTION_USER_NAME).append(" username");
    usage.append(" [ -").append(OPTION_PASSWORD).append(" old password")
         .append(" ]");
    usage.append(" [ -").append(OPTION_NEW_PASSWORD).append(" new password")
         .append(" ] \n");

    usage.append("               -").append(OPTION_REMOVE);
    usage.append(" -").append(OPTION_USER_NAME).append(" username \n");

    usage.append("               -").append(OPTION_LIST).append(" \n");

    setUsage (usage);
}


/**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments.
*/
void CIMUserCommand::setCommand (
                        ostream& outPrintWriter, 
                        ostream& errPrintWriter,
			Uint32   argc, 
			char*    argv []) 
{
    Uint32            i                = 0;
    Uint32            c                = 0;
    String            badOptionString  = String ();
    String            optString        = String ();

    //
    //  Construct optString
    //
    optString.append(OPTION_ADD);
    optString.append(OPTION_USER_NAME);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_PASSWORD);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_MODIFY);
    optString.append(OPTION_USER_NAME);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_PASSWORD);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_NEW_PASSWORD);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_REMOVE);
    optString.append(OPTION_USER_NAME);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_LIST);

    //
    //  Initialize and parse options
    //
    getoopt options ("");
    options.addFlagspec(optString);

    options.parse (argc, argv);

    if (options.hasErrors ())
    {
        CommandFormatException e (options.getErrorStrings () [0]);
        throw e;
    }
    _operationType = OPERATION_TYPE_UNINITIALIZED;

    //
    //  Get options and arguments from the command line
    //
    for (i =  options.first (); i <  options.last (); i++)
    {
        if (options [i].getType () == Optarg::LONGFLAG)
        {
            //
            //  This path should not be hit
            //  The cimuser command has no LONGFLAG options
            //
            c = options [i].getopt () [0];

            UnexpectedOptionException e (c);
            throw e;
        } 
        else if (options [i].getType () == Optarg::REGULAR)
        {
            //
            //  The cimuser command has no non-option argument options
            //
            UnexpectedArgumentException e (options [i].Value ()); 
            throw e;
        } 
        else /* if (options [i].getType () == Optarg::FLAG) */
        {

            c = options [i].getopt () [0];

            switch (c) 
            {
                case OPTION_ADD: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_ADD);
                        throw e;
                    }

                    if (options.isSet (OPTION_ADD) > 1)
                    {
                        //
                        // More than one add user option was found
                        //
                        DuplicateOptionException e (OPTION_ADD); 
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_ADD;

                    break;
                }
                case OPTION_MODIFY:
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_MODIFY);
                        throw e;
                    }

                    if (options.isSet (OPTION_MODIFY) > 1)
                    {
                        //
                        // More than one modify user option was found
                        //
                        DuplicateOptionException e (OPTION_MODIFY);
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_MODIFY;

                    break;
                }
                case OPTION_USER_NAME:
                {
                    if (options.isSet (OPTION_USER_NAME) > 1)
                    {
                        //
                        // More than one username option was found
                        //
                        DuplicateOptionException e (OPTION_USER_NAME);
                        throw e;
                    }

                    _userName = options [i].Value ();

                    _userNameSet = true; 

                    break;
                }
                case OPTION_PASSWORD:
                {
                    if (options.isSet (OPTION_PASSWORD) > 1)
                    {
                        //
                        // More than one password option was found
                        //
                        DuplicateOptionException e (OPTION_PASSWORD);
                        throw e;
                    }

                    String password = options [i].Value ();
                    _password = password.subString(0,8);

                    _passwordSet = true; 

                    break;
                }
                case OPTION_NEW_PASSWORD:
                {
                    if (options.isSet (OPTION_NEW_PASSWORD) > 1)
                    {
                        //
                        // More than one new password option was found
                        //
                        DuplicateOptionException e (OPTION_NEW_PASSWORD);
                        throw e;
                    }

                    String newpassword = options [i].Value ();
                    _newpassword = newpassword.subString(0,8);

                    _newpasswordSet = true; 

                    break;
                }
                case OPTION_REMOVE: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_REMOVE);
                        throw e;
                    }

                    if (options.isSet (OPTION_REMOVE) > 1)
                    {
                        //
                        // More than one remove user option was found
                        //
                        DuplicateOptionException e (OPTION_REMOVE); 
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_REMOVE;

                    break;
                }

                case OPTION_LIST: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_LIST);
                        throw e;
                    }

                    if (options.isSet (OPTION_LIST) > 1)
                    {
                        //
                        // More than one list option was found
                        //
                        DuplicateOptionException e (OPTION_LIST); 
                        throw e;
                    }
                    _operationType = OPERATION_TYPE_LIST;
                    break;
                }

                default:
		{ 
		    // 
		    // Should never get here
		    //
		    break;
                }
            }
        }
    }

    // 
    // Some more validations
    //
    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // No operation type was specified 
        // Show the usage 
        //
		  //l10n
		  //CommandFormatException e (REQUIRED_ARGS_MISSING);
        CommandFormatException e (localizeMessage(MSG_PATH,REQUIRED_ARGS_MISSING_KEY, REQUIRED_ARGS_MISSING));
        throw e;
    }
	
    if ( _operationType == OPERATION_TYPE_LIST  && 
	 ( _userNameSet || _passwordSet || _newpasswordSet ) )
    {
    	//l10n
    	MessageLoaderParms parms("Clients.cimuser.CIMUserCommand.UNEXPECTED_OPTION",
    							 "Unexpected Option.");
    	CommandFormatException e(parms);						 
        //CommandFormatException e("Unexpected Option.");	
        throw e;
    }
	
    if (_operationType == OPERATION_TYPE_ADD)
    {
	if ( _newpasswordSet )
	{
            //
            // An invalid option was encountered
            //
            UnexpectedOptionException e( OPTION_NEW_PASSWORD );
            throw e;
        }
        if ( !_userNameSet )
        {
            //
            // An invalid option was encountered
            //
            MissingOptionException e (OPTION_USER_NAME);
            throw e;
        }
        if ( !_passwordSet )
        {
            //
            // Password is not set, prompt for the password
            //
	    String pw = String::EMPTY;
            do
            {
                pw = System::getPassword( PASSWORD_PROMPT );

		if ( pw == String::EMPTY || pw == "" )
		{
			//l10n
		    //errPrintWriter << PASSWORD_BLANK << endl;
			 errPrintWriter << localizeMessage(MSG_PATH,PASSWORD_BLANK_KEY,PASSWORD_BLANK) << endl;
	            pw = String::EMPTY;
		    continue;
                }
                if ( pw != System::getPassword( RE_ENTER_PROMPT ))
                {
					 //l10n
	            //errPrintWriter << PASSWORD_DOES_NOT_MATCH << endl;
					errPrintWriter << localizeMessage(MSG_PATH,PASSWORD_DOES_NOT_MATCH_KEY,PASSWORD_DOES_NOT_MATCH_KEY) << endl;
	            pw = String::EMPTY;
                }
            }
            while ( pw == String::EMPTY );

            _password = pw ;
        }
    }

    if (_operationType == OPERATION_TYPE_MODIFY)
    {
        if ( !_userNameSet )
        {
            //
            // An invalid option was encountered
            //
            MissingOptionException e (OPTION_USER_NAME);
            throw e;
        }
	if ( _passwordSet && _newpasswordSet )
	{
	    if ( _newpassword == _password )
	    {
		 //l10n
		//cerr << PASSWORD_SAME_ERROR << endl;
		cerr << localizeMessage(MSG_PATH,PASSWORD_SAME_ERROR_KEY,PASSWORD_SAME_ERROR) << endl; 
		exit (1);
            }
        }
        if ( !_passwordSet )
        {
            //
            // Password is not set, prompt for the old password once
            //
	    String pw = String::EMPTY;
	    do
	    {
                pw = System::getPassword( OLD_PASSWORD_PROMPT );
		if ( pw == String::EMPTY || pw == "" )
		{
		//l10n
		    //errPrintWriter << PASSWORD_BLANK << endl;
			 errPrintWriter << localizeMessage(MSG_PATH,PASSWORD_BLANK_KEY,PASSWORD_BLANK) << endl;
	            pw = String::EMPTY;
		    continue;
                }
            }
	    while ( pw == String::EMPTY );
            _password = pw ;
        }
        if ( !_newpasswordSet )
        {
            //
            // Password is not set, prompt for the new password twice
            //
	    String newPw = String::EMPTY;
            do
            {
                newPw = System::getPassword( NEW_PASSWORD_PROMPT );
		if ( newPw == String::EMPTY || newPw == "" )
		{
		//l10n
		    //errPrintWriter << PASSWORD_BLANK << endl;
			 errPrintWriter << localizeMessage(MSG_PATH,PASSWORD_BLANK_KEY,PASSWORD_BLANK) << endl;
	            newPw = String::EMPTY;
		    continue;
                }

                if ( newPw != System::getPassword( RE_ENTER_PROMPT ))
              {
					 //l10n
	            //errPrintWriter << PASSWORD_DOES_NOT_MATCH << endl;
					errPrintWriter << localizeMessage(MSG_PATH,PASSWORD_DOES_NOT_MATCH_KEY,PASSWORD_DOES_NOT_MATCH) << endl;
	            newPw = String::EMPTY;
                }
            }
            while ( newPw == String::EMPTY );
            _newpassword = newPw ;
	    if ( _newpassword == _password )
	    {
		 //l10n
		//cerr << PASSWORD_SAME_ERROR << endl;
		cerr << localizeMessage(MSG_PATH,PASSWORD_SAME_ERROR_KEY, PASSWORD_SAME_ERROR) << endl;
                exit (-1);
            }
        }
    }

    if (_operationType == OPERATION_TYPE_REMOVE)
    {
        if ( !_userNameSet )
        {
            //
            // An invalid option was encountered
            //
            MissingOptionException e (OPTION_USER_NAME);
            throw e;
        }
        if ( _passwordSet )
        {
            //
            // An invalid option was encountered
            //
            UnexpectedOptionException e ( OPTION_PASSWORD );
            throw e;
        }
        if ( _newpasswordSet )
        {
            //
            // An invalid option was encountered
            //
            UnexpectedOptionException e ( OPTION_NEW_PASSWORD );
            throw e;
        }
    }
}

/**
    Executes the command and writes the results to the PrintWriters.
*/
Uint32 CIMUserCommand::execute (
    ostream& outPrintWriter, 
    ostream& errPrintWriter)
{
    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // The command was not initialized
        //
        return 1;
    }

    // 
    // Get local host name
    //
    _hostName.assign(System::getHostName());

    try
    {
        //
        // Open connection with CIMSever
        //
        _client = new CIMClient;
		_client->setRequestDefaultLanguages();
        _client->connectLocal();
    }
    catch(Exception& e)
    {
	 //l10n
        //outPrintWriter << CIMOM_NOT_RUNNING << endl;
		  outPrintWriter << localizeMessage(MSG_PATH,CIMOM_NOT_RUNNING_KEY,CIMOM_NOT_RUNNING) << endl;
	return 1;
    }

    //
    // Perform the requested operation
    //
    switch ( _operationType )
    {
        case OPERATION_TYPE_ADD:
            try
            {
                _AddUser( outPrintWriter, errPrintWriter );
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_FAILED)
                {
					 //l10n
		    //outPrintWriter << ADD_USER_FAILURE << endl;
			 		outPrintWriter << localizeMessage(MSG_PATH,ADD_USER_FAILURE_KEY,ADD_USER_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
		else if (code == CIM_ERR_NOT_SUPPORTED)
		{
		//l10n
		    //outPrintWriter << ADD_USER_FAILURE << endl;
			 outPrintWriter << localizeMessage(MSG_PATH,ADD_USER_FAILURE_KEY,ADD_USER_FAILURE) << endl;
		    errPrintWriter << e.getMessage()  << endl;
                }
		else if (code == CIM_ERR_ALREADY_EXISTS)
		{
		//l10n
		    //outPrintWriter << ADD_USER_FAILURE << endl;
			 outPrintWriter << localizeMessage(MSG_PATH,ADD_USER_FAILURE_KEY,ADD_USER_FAILURE) << endl;
		    //outPrintWriter << USER_ALREADY_EXISTS << endl;
			 outPrintWriter << localizeMessage(MSG_PATH,USER_ALREADY_EXISTS_KEY,USER_ALREADY_EXISTS) << endl;
		    errPrintWriter << e.getMessage()  << endl;
		}    
                else if (code == CIM_ERR_INVALID_CLASS)
                {
					 //l10n
		    //outPrintWriter << ADD_USER_FAILURE << endl;
			 outPrintWriter << localizeMessage(MSG_PATH,ADD_USER_FAILURE_KEY,ADD_USER_FAILURE) << endl;
		    //outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
			 outPrintWriter << localizeMessage(MSG_PATH,AUTH_SCHEMA_NOT_LOADED_KEY,AUTH_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
		    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
				//l10n
                //outPrintWriter << ADD_USER_FAILURE << endl <<
                    //e.getMessage() << endl;
					 outPrintWriter << localizeMessage(MSG_PATH,ADD_USER_FAILURE_KEY,ADD_USER_FAILURE) << 
					 					endl << e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break; 

            case OPERATION_TYPE_MODIFY:
            try
            {
                _ModifyUser( outPrintWriter, errPrintWriter );
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
					 //l10n
                    //outPrintWriter << CHANGE_PASSWORD_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,CHANGE_PASSWORD_FAILURE_KEY,CHANGE_PASSWORD_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
					 //l10n
                    //outPrintWriter << CHANGE_PASSWORD_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,CHANGE_PASSWORD_FAILURE_KEY,CHANGE_PASSWORD_FAILURE) << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
					 //l10n
                    //outPrintWriter << CHANGE_PASSWORD_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,CHANGE_PASSWORD_FAILURE_KEY,CHANGE_PASSWORD_FAILURE) << endl;
		    				//outPrintWriter << USER_NOT_FOUND          << endl;
			 			outPrintWriter << localizeMessage(MSG_PATH,USER_NOT_FOUND_KEY,USER_NOT_FOUND) << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
					 //l10n
                    //outPrintWriter << CHANGE_PASSWORD_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,CHANGE_PASSWORD_FAILURE_KEY,CHANGE_PASSWORD_FAILURE) << endl;
                    //outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,AUTH_SCHEMA_NOT_LOADED_KEY,AUTH_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
				//l10n
                //outPrintWriter << CHANGE_PASSWORD_FAILURE << endl <<
                    //e.getMessage() << endl;
					outPrintWriter << localizeMessage(MSG_PATH,CHANGE_PASSWORD_FAILURE_KEY,CHANGE_PASSWORD_FAILURE) << 
										  endl << e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_REMOVE:
            try
            {
                _RemoveUser( outPrintWriter, errPrintWriter );
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
					 //l10n
                    //outPrintWriter << REMOVE_USER_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,REMOVE_USER_FAILURE_KEY,REMOVE_USER_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
					 //l10n
                    //outPrintWriter << REMOVE_USER_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,REMOVE_USER_FAILURE_KEY,REMOVE_USER_FAILURE) << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
					 //l10n
                    //outPrintWriter << REMOVE_USER_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,REMOVE_USER_FAILURE_KEY,REMOVE_USER_FAILURE) << endl;
		   			  //outPrintWriter << USER_NOT_FOUND          << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,USER_NOT_FOUND_KEY,USER_NOT_FOUND) << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
					 //l10n
                    //outPrintWriter << REMOVE_USER_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,REMOVE_USER_FAILURE_KEY,REMOVE_USER_FAILURE) << endl;
                    //outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,AUTH_SCHEMA_NOT_LOADED_KEY,AUTH_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
				//l10n
                //outPrintWriter << REMOVE_USER_FAILURE << endl <<
                    //e.getMessage() << endl;
					 outPrintWriter << localizeMessage(MSG_PATH,REMOVE_USER_FAILURE_KEY,REMOVE_USER_FAILURE) << 
					 				   endl << e.getMessage() << endl; 
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_LIST:
            try
            {
                _ListUsers(outPrintWriter, errPrintWriter);
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
					 //l10n
                    //outPrintWriter << LIST_USERS_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,LIST_USERS_FAILURE_KEY,LIST_USERS_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
					 //l10n
                    //outPrintWriter << LIST_USERS_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,LIST_USERS_FAILURE_KEY,LIST_USERS_FAILURE) << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
					 //l10n
                    //outPrintWriter << LIST_USERS_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,LIST_USERS_FAILURE_KEY,LIST_USERS_FAILURE) << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
					 //l10n
                    //outPrintWriter << LIST_USERS_FAILURE << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,LIST_USERS_FAILURE_KEY,LIST_USERS_FAILURE) << endl;
                    //outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,AUTH_SCHEMA_NOT_LOADED_KEY,AUTH_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
					//l10n
                //outPrintWriter << LIST_USERS_FAILURE << endl <<
                    //e.getMessage() << endl;
						  outPrintWriter << localizeMessage(MSG_PATH,LIST_USERS_FAILURE_KEY,LIST_USERS_FAILURE) << 
						  					endl << e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break;

        default:
            //
            // Should never get here
            //
            break;
    }

    return (RC_SUCCESS);
}

/**
    Add the user to the CIM Server.
*/
void CIMUserCommand::_AddUser
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    CIMProperty prop;
    
    try
    {
        CIMInstance newInstance( PEGASUS_CLASSNAME_USER );
	newInstance.addProperty ( 
			  CIMProperty( PROPERTY_NAME_USER_NAME, _userName ) );
	newInstance.addProperty ( 
			  CIMProperty( PROPERTY_NAME_PASSWORD , _password ) );

	_client->createInstance( PEGASUS_NAMESPACENAME_USER, newInstance );
	//l10n
	//outPrintWriter << ADD_USER_SUCCESS << endl;
	outPrintWriter << localizeMessage(MSG_PATH,ADD_USER_SUCCESS_KEY,ADD_USER_SUCCESS) << endl;

    }
    catch (Exception& e)
    {
        throw e;
    }
}

//
// Modify the password for a user
//
void CIMUserCommand::_ModifyUser
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    try
    {
        Array<CIMKeyBinding>      kbArray;
        CIMKeyBinding             kb;
	Array<CIMParamValue>   inParams;
	Array<CIMParamValue>   outParams;

	//
	// Build the input params
	//
	inParams.append ( CIMParamValue ( OLD_PASS_PARAM,
                                          CIMValue ( _password )));
	inParams.append ( CIMParamValue ( NEW_PASS_PARAM, 
                                          CIMValue ( _newpassword )));

        kb.setName(CIMName ("Username"));
        kb.setValue(_userName);
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_USER,
            PEGASUS_CLASSNAME_USER, kbArray);

	//
	// Call the invokeMethod with the input parameters
	// 

	//
	// Not checking for return code as all error conditions will
	// throw exceptions and will be handled by the catch block. If new 
	// return codes are added in future, they need to be handled here.  
	//
	CIMValue retValue = _client->invokeMethod (
		                       PEGASUS_NAMESPACENAME_USER,
		                       reference,
		                       MODIFY_METHOD,
		                       inParams,
		                       outParams );
			//l10n
        //outPrintWriter << CHANGE_PASSWORD_SUCCESS << endl;
		  outPrintWriter << localizeMessage(MSG_PATH,CHANGE_PASSWORD_SUCCESS_KEY,CHANGE_PASSWORD_SUCCESS) << endl;
    }
    catch (Exception& e)
    {
        throw e;
    }
}

//
// Remove a user    
//
void CIMUserCommand::_RemoveUser
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    try
    {
        Array<CIMKeyBinding> kbArray;
        CIMKeyBinding        kb;

        kb.setName(PROPERTY_NAME_USER_NAME);
        kb.setValue(_userName);
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_USER,
            PEGASUS_CLASSNAME_USER, kbArray);

        _client->deleteInstance(PEGASUS_NAMESPACENAME_USER, reference);
		  //l10n
	//outPrintWriter << REMOVE_USER_SUCCESS << endl;
			outPrintWriter << localizeMessage(MSG_PATH,REMOVE_USER_SUCCESS_KEY,REMOVE_USER_SUCCESS) << endl;   	
    }
    catch (Exception& e)
    {
	throw e;
    }
}


/**
    get a list of all user names from the CIM Server.
 */
void CIMUserCommand::_ListUsers
    ( 
    ostream&    outPrintWriter,
    ostream&    errPrintWriter
    )
{
    try
    {
        //
        // get all the instances of class PG_User
        //
        Array<CIMObjectPath> instanceNames =
            _client->enumerateInstanceNames(
	        PEGASUS_NAMESPACENAME_USER, 
	        PEGASUS_CLASSNAME_USER);

        if ( instanceNames.size() == 0 )
        {
		  	//l10n
             //outPrintWriter << NO_USERS_FOUND << endl;
				 outPrintWriter << localizeMessage(MSG_PATH,NO_USERS_FOUND_KEY,NO_USERS_FOUND) << endl; 
        }
        else
	{
            //
            // List all the users.
            //
            for (Uint32 i = 0; i < instanceNames.size(); i++)
            {
                Array<CIMKeyBinding> kbArray = instanceNames[i].getKeyBindings();

                if (kbArray.size() > 0)
                {
	            outPrintWriter << kbArray[0].getValue() << endl;
                }
            }
        }
    }
    catch (Exception& e)
    {
        throw e;
    }
}

PEGASUS_NAMESPACE_END

//
// exclude main from the Pegasus Namespace
//
PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

///////////////////////////////////////////////////////////////////////////////
/**
    Parses the command line, and execute the command.

    @param   args  the string array containing the command line arguments
*/
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv []) 
{
    CIMUserCommand*      command;
    Uint32               retCode;

	MessageLoader::_useProcessLocale = true; //l10n set message loading to process locale

    //
    // Check if root is issuing the command
    //
    if ( !System::isPrivilegedUser(System::getEffectiveUserName()) )
    {
	cerr << NOT_PRIVILEGED_USER << endl;
	return 1;
    }
	 
    command  = new CIMUserCommand ();

    try 
    {
        command->setCommand ( cout, cerr, argc, argv);
    } 
    catch (CommandFormatException& cfe) 
    {
        if (!String::equal(cfe.getMessage (), ""))
        {
            cerr << COMMAND_NAME << ": " << cfe.getMessage () << endl;
        }
        cerr << command->getUsage () << endl;
        return 1;
    }

    retCode = command->execute (cout, cerr);

    return (retCode);
}

