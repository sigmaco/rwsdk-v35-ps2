// simple command line tool for processing rf3 files

#include "stdafx.h"

using namespace std;

#include "util\util.h"

#include "RwCommMessage.h"

#include "includes\rw_includes.h"

#include "RwExpAnmExporter.h"
#include "RwExpBspExporter.h"
#include "RwExpDffExporter.h"
#include "RwExpSplExporter.h"

#include "RwRf3Builder.h"
#include "RwRf3Exporter.h"

#include "RwExpMgr.h"
#include "RwExpMgrExportManager.h"


// a simple command line text RwCommMessage implementation
class RwCmdLineMessage : public RwCommMessage
{
public:
    RwCmdLineMessage() {}
    ~RwCmdLineMessage() {};

    RwCommError AddMessage(const string &msg, const string &nodeName = "", const int level = 0);    
};

RwCommError
RwCmdLineMessage::AddMessage(const string &msg, const string &nodeName, const int level)
{
    if( level == 0 )
    {
        printf( "Error: " );
    }    

    if( !nodeName.empty() )
    {
        printf( "%s: ", nodeName.c_str() );
    }

    printf( "%s\r\n", msg.c_str() );

    return RWCOMM_SUCCESS;
}

// check if string is a valid boolean flag
bool
CheckFlag( const string sFlag, bool& result )
{
    if( stricmp(sFlag.c_str(), "true") == 0)
    {
        result = true;
    }
    else if( stricmp(sFlag.c_str(), "false") == 0)
    {
        result = false;
    }    
    else
    {
        return false;
    }

    return true;
}

// nice and simple
int
main(int argc, char* argv[])
{
    char buffer[256];
    RwCommScene* pScene = NULL;
    RwCommExportOptions outputOptions;
    RwCommError eResult;
    string sProjectTemplate = "Generic.rwt";
    string sProjectName = "untitled";
    string sProjectPath = "";
    bool processFlags = true;
    bool nameSet = false;
    vector<string> vFiles;
    int argIndex = 1;
    bool badusage = false;
    RwCmdLineMessage pCmdLineMessage;

    // Set the message output for the command line
    RwCommMessage::RegisterInstance(&pCmdLineMessage);
    
    // get the current path as the default project path
    if( GetCurrentDirectory(256, buffer) != 0 )
    {
        sProjectPath =  buffer;
    }

    printf("Current Directory: %s\r\n", buffer);

    // cleat the output options
    outputOptions.ClearAllOptions();
       
    if( argc > 1 )
    {
        // process the command line flags
        while( (processFlags) && (argIndex < argc) )
        {                  
            string sFlag = argv[argIndex];

            if( sFlag == "-t" ) // project template file
            {
                argIndex++;
                sProjectTemplate = argv[argIndex];
                argIndex++;
            }
            else if( sFlag == "-n" ) // project name
            {
                argIndex++;
                sProjectName = argv[argIndex];
                argIndex++;
                nameSet = true;
            }
            else if( sFlag == "-p" ) // export path
            {
                argIndex++;
                sProjectPath = argv[argIndex];
                argIndex++;
            }
            else if( sFlag == "-expold" )
            {              
                argIndex++;
                string sOldFlag = argv[argIndex];
                argIndex++;
                bool expold;

                if( CheckFlag( sOldFlag, expold ) )
                {  
                    outputOptions.SetOption(RwCommOption(EXPORTTOLEGACYFILES, expold));
                }
                else
                {
                    badusage = true;
                    printf( "BAD USAGE: the \"-expold\" flag requires a boolean value. use either\r\n" );
                    printf( "           \"true\" or \"false\"\r\n\r\n" );
                }
            }
            else if( sFlag == "-exprws" )
            {   
                argIndex++;
                string sRWSFlag = argv[argIndex];
                argIndex++;
                bool expRWS;

                if( CheckFlag( sRWSFlag, expRWS ) )
                {  
                    outputOptions.SetOption(RwCommOption(EXPORTTORWS, expRWS));
                }
                else
                {
                    badusage = true;
                    printf( "BAD USAGE: the \"-exprws\" flag requires a boolean value. use either\r\n" );
                    printf( "           \"true\" or \"false\"\r\n\r\n" );                    
                }
            }          
/*            else if( sFlag == "-exprf3" )
            {   
                argIndex++;
                string sRF3Flag = argv[argIndex];
                argIndex++;
                bool expRF3;

                if( CheckFlag( sRF3Flag, expRF3 ) )
                {
                    outputOptions.SetOption(RwCommOption(EXPORTTORF3, expRF3));
                }
                else
                {
                    badusage = true;
                    printf( "BAD USAGE: the \"-exprf3\" flag requires a boolean value. use either\r\n" );
                    printf( "           \"true\" or \"false\"\r\n\r\n" );
                }
            }*/
            else
            {
                processFlags = false;
            }
        }

        if( !badusage )
        {
            // set the project name to the first rf3 file's name by default
            if( !nameSet )
            {
                sProjectName = argv[argIndex];
                StripToFileName( sProjectName );
                StripOffExtension( sProjectName );
            }
           
            // create a list of the rf3 files
            for( int i = argIndex; i < argc; i++ )
            {
                string cleanFilePath;
                string sRf3File = argv[i];
                StripOffExtension( sRf3File );
                sRf3File += ".rf3";

                cleanFilePath = sRf3File;

                // convert to absolute path
                if( PathIsRelative( sRf3File.c_str() ) )
                {
                    // create absolute
                    cleanFilePath = string( buffer ) + "\\" + sRf3File;

                    // check if path exists
                    if( !PathFileExists( cleanFilePath.c_str() ) )
                    {
                        // no... use project instead
                        cleanFilePath = sProjectPath + "\\" + sRf3File;
                
                        // check if path exists
                        if( !PathFileExists( cleanFilePath.c_str() ) )
                        {
                            // no.. leave it as it was
                            cleanFilePath = sRf3File;
                        }
                    }
                }

                if( PathFileExists( cleanFilePath.c_str() ) )
                {
                    vFiles.push_back( cleanFilePath );
                }
                else
                {
                    printf( "Error: RF3 file does not exist! %s\r\n", sRf3File.c_str() );
                }
            }

            if( !vFiles.empty() )
            {
                // Set the rf3 builder
                RwRf3Builder* pBuilder = new RwRf3Builder;
                RwExpMgrExportManager::TheInstance()->SetBuilder( pBuilder );

                // export the rf3 files
                eResult = RwExpMgrExportManager::TheInstance()->Export(vFiles,
                    sProjectTemplate, sProjectName, sProjectPath, &outputOptions);

                if( eResult == RWCOMM_FAIL )
                {
                    printf( "\r\nError occured while building rf3!\r\n" );
                }
            }
        }
    }
    else
    {
        badusage = true;
    }

    // print help if we have a bad usage of the tool
    if( badusage )
    {
        string appName = argv[0];
        StripToFileName( appName );
        StripOffExtension( appName );

        printf( "%s : RenderWare RF3 file commandline compiler\r\n", appName.c_str() );
        printf( "        (c) 2003 Criterion Software Ltd.\r\n\r\n" );
        printf( "%s usage :\r\n", appName.c_str() );
        printf( "%s <options>... <rf3 files>...\r\n", appName.c_str() );
        printf( "options :\r\n" );
        printf( "-t <template>        The project template file\r\n" );
        printf( "-n <name>            The project name (default rws filename)\r\n" );
        printf( "-p <path>            The export path\r\n" );
        printf( "-expold <true/false> This flag overrides the Export Legacy files\r\n" );
        printf( "                     flag in the project template\r\n" );
        //printf( "-exprf3 <true/false> This flag overrides the Export RF3 file\r\n" );
        //printf( "                     flag in the project template\r\n" );
        printf( "-exprws <true/false> This flag overrides the Export RWS file\r\n" );
        printf( "                     flag in the project template\r\n\r\n" );
        printf( "examples :\r\n" );
        printf( "   %s cube.rf3\r\n", appName.c_str() );
        printf( "       export cube.rf3 assets to the same directory\r\n" );
        printf( "   %s -t CubeScene.rwt -n CubeOnTable -p c:\\ cube.rf3 table.rf3\r\n", appName.c_str() );
        printf( "       export both cube.rf3 and table.rf3 assets to c:\\CubeOnTable.rws using\r\n" );
        printf( "       the CubeScene.rwt template\r\n\r\n" );
        printf( "For more information on %s please refer to the rf3cc user guide.\r\n", appName.c_str() );

        return 1;
    }

	return 0;
}