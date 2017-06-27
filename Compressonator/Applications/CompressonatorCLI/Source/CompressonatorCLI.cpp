//
// AMDCompressCLI.cpp
//
// 
// Contact Info: Navin Patel @ AMD.com
//
// Initial version - Nov 2014
// Updated code to share with GUI - Sep 19 2015
//
// Code is based on the following Libs
// Standard Library             Ref:http://www.cplusplus.com/reference/ 
// Standard Template Library Ref:http://en.cppreference.com/w/cpp 
// 

#ifdef _DEBUG
//#include <vld.h>   Enable to check for code leaks
#endif

// #define USE_AMD_BMP
// #define USE_AMD_PNG

#include <windows.h>
#include "cmdline.h"
#include "PluginManager.h"
#include "TextureIO.h"

// Our Static Plugin Interfaces
#pragma comment(lib,"ASTC.lib")
#pragma comment(lib,"BoxFilter.lib")
#pragma comment(lib,"DDS.lib")
#pragma comment(lib,"EXR.lib")
#pragma comment(lib,"KTX.lib")
#pragma comment(lib,"TGA.lib")
#pragma comment(lib,"IMGAnalysis.lib")

extern void *make_Plugin_ASTC();
extern void *make_Plugin_BoxFilter();
extern void *make_Plugin_DDS();
extern void *make_Plugin_EXR();
extern void *make_Plugin_KTX();
extern void *make_Plugin_TGA();
extern void *make_Plugin_CAnalysis();

#ifdef USE_AMD_PNG
#pragma comment(lib,"PNG.lib")
extern void *make_Plugin_PNG();
#endif

#ifdef USE_AMD_BMP
#pragma comment(lib,"BMP.lib")
extern void *make_Plugin_BMP();
#endif

extern int          RunInfo();
extern bool         CompressionCallback(float fProgress, DWORD_PTR pUser1, DWORD_PTR pUser2);
extern void         LocalPrintF(char *buff);

PluginManager       g_pluginManager;
bool                g_bAbortCompression = false;
CMIPS*              g_CMIPS;                                // Global MIPS functions shared between app and all IMAGE plugins

#ifdef USE_QT_IMAGELOAD
#include <QCoreApplication>
#include <qdebug.h>
#endif


bool ProgressCallback(float fProgress, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
   return CompressionCallback(fProgress, pUser1, pUser2);
}

int main(int argc,  char* argv[])
{
#ifdef USE_QT_IMAGELOAD
    QCoreApplication app(argc, argv);
#endif

    g_pluginManager.registerStaticPlugin("IMAGE","ASTC", make_Plugin_ASTC);
    g_pluginManager.registerStaticPlugin("IMAGE","DDS", make_Plugin_DDS);
    g_pluginManager.registerStaticPlugin("IMAGE","EXR", make_Plugin_EXR);
    g_pluginManager.registerStaticPlugin("IMAGE","KTX", make_Plugin_KTX);
    g_pluginManager.registerStaticPlugin("IMAGE","TGA", make_Plugin_TGA);  // Use for load only, Qt will be used for Save
    g_pluginManager.registerStaticPlugin("IMAGE", "ANALYSIS", make_Plugin_CAnalysis);
    
#ifdef USE_AMD_PNG
    g_pluginManager.registerStaticPlugin("IMAGE", "PNG", make_Plugin_PNG);
#endif

#ifdef USE_AMD_BMP
    g_pluginManager.registerStaticPlugin("IMAGE", "BMP", make_Plugin_BMP);
#endif

    g_pluginManager.registerStaticPlugin("FILTERS","BOXFILTER", make_Plugin_BoxFilter);
    g_pluginManager.getPluginList("\\Plugins");

#ifdef USE_QT_IMAGELOAD
    QString dirPath = QCoreApplication::applicationDirPath();
    QCoreApplication::addLibraryPath(dirPath + "./plugins/imageformats");
#endif

    // Check if print status line has been assigned
    // if not get it a default to printf
    if (PrintStatusLine == NULL)
        PrintStatusLine = &LocalPrintF;

    //----------------------------------
    // Process user command line parameters 
    //----------------------------------
    if (argc > 1)
    {
        if (
            (!ParseParams(argc, argv)) ||
            (g_CmdPrams.SourceFile.length() == 0) ||
            (g_CmdPrams.DestFile.length() == 0)   
            )
        {
            return (-1);
        }
        
        return ProcessCMDLine(&CompressionCallback,NULL);

    }
    else  
        return(RunInfo());
}


