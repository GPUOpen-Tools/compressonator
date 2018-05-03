//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=====================================================================

#include <QApplication>
#include "cpMainComponents.h"
#include "PluginManager.h"
#include "mips.h"

#define MSG_HANDLER

// Our Static Plugin Interfaces
#pragma comment(lib,"ASTC.lib")
#pragma comment(lib,"BoxFilter.lib")
#pragma comment(lib,"DDS.lib")
#pragma comment(lib,"EXR.lib")
#pragma comment(lib,"KTX.lib")
#pragma comment(lib,"TGA.lib")
#pragma comment(lib,"IMGAnalysis.lib")

#ifdef USE_CRN
#pragma comment(lib,"CRN.lib")
#endif


extern void *make_Plugin_ASTC();
extern void *make_Plugin_BoxFilter();
extern void *make_Plugin_DDS();
extern void *make_Plugin_EXR();
extern void *make_Plugin_KTX();
extern void *make_Plugin_TGA();
extern void *make_Plugin_CAnalysis();

#ifdef USE_CRN
extern void *make_Plugin_CRN();
#endif

#define SEPERATOR_STYLE "QMainWindow::separator { background-color: #d7d6d5; width: 3px; height: 3px; border:none; }"
#define PERCENTAGE_OF_MONITOR_WIDTH_FOR_SCREEN  0.65
#define PERCENTAGE_OF_MONITOR_HEIGHT_FOR_SCREEN 0.8

PluginManager   g_pluginManager;
bool            g_bAbortCompression;
CMIPS*          g_CMIPS;                                // Global MIPS functions shared between app and all IMAGE plugins 
CMIPS*          g_GUI_CMIPS;                            // Global MIPS functions shared by 3DModels


void GetSupportedFileFormats(QList<QByteArray> &g_supportedFormats)
{

    // Assemble list of supported Image Formats from our plugin
    int numPlugins = g_pluginManager.getNumPlugins();
    for (int i = 0; i< numPlugins; i++)
    {
        if (strcmp(g_pluginManager.getPluginType(i), "IMAGE") == 0)
        {
            QByteArray bArray = g_pluginManager.getPluginName(i);
            QByteArray fformat = bArray.toUpper();
            if (fformat == "ANALYSIS") continue;
            if (!g_supportedFormats.contains(fformat))
                g_supportedFormats.append(fformat);
        }
        else
        if (strcmp(g_pluginManager.getPluginType(i), "3DMODEL_LOADER") == 0)
        {
            QByteArray bArray = g_pluginManager.getPluginName(i);
            QByteArray fformat = bArray.toUpper();
            if (!g_supportedFormats.contains(fformat))
                g_supportedFormats.append(fformat);
        }
    }

    // Get a list of all Supported file formats from Qt Plugins
    QList<QByteArray> QtFormats = QImageReader::supportedImageFormats();

    // Upppercase List
    QList<QByteArray>::Iterator i;
    for (i = QtFormats.begin(); i != QtFormats.end(); ++i)
    {
        QByteArray fformat = (*i);
        fformat = fformat.toUpper();
        if (!g_supportedFormats.contains(fformat))
            g_supportedFormats.append(fformat);
    }
    
    // Sort the list to alphabetical order
    std::sort(g_supportedFormats.begin(), g_supportedFormats.end());

}

int main(int argc, char **argv)
{

    try
    {
        QApplication app(argc, argv);
        QString dirPath = QApplication::applicationDirPath();
        QApplication::addLibraryPath(dirPath + "./plugins/platforms/");
        QApplication::addLibraryPath(dirPath + "./plugins/");

        app.setWindowIcon(QIcon(":/CompressonatorGUI/Images/acompress-256.png"));

        // ==========================
        // Mip Settings Class
        // ==========================
        g_GUI_CMIPS =  new CMIPS;

        const QIcon iconPixMap(":/CompressonatorGUI/Images/compress.png");
        const QString ProductName = "Compressonator";
        //----------------------------------
        // Load plugin List for processing
        //----------------------------------
    
        g_pluginManager.registerStaticPlugin("IMAGE",  "ASTC",      make_Plugin_ASTC);
        g_pluginManager.registerStaticPlugin("IMAGE",  "DDS",       make_Plugin_DDS);
        g_pluginManager.registerStaticPlugin("IMAGE",  "EXR",       make_Plugin_EXR);
        g_pluginManager.registerStaticPlugin("IMAGE",  "KTX",       make_Plugin_KTX);

#ifdef USE_CRN
        g_pluginManager.registerStaticPlugin("IMAGE", "CRN", make_Plugin_CRN);
#endif

        // TGA is supported by Qt to some extent if it fails we will try to load it using our custom code
        g_pluginManager.registerStaticPlugin("IMAGE",  "TGA",       make_Plugin_TGA);

        g_pluginManager.registerStaticPlugin("FILTER", "BOXFILTER", make_Plugin_BoxFilter);
        g_pluginManager.registerStaticPlugin("IMAGE", "ANALYSIS",   make_Plugin_CAnalysis);
    
        g_pluginManager.getPluginList("/plugins");
        g_bAbortCompression = false;
    
        cpMainComponents mainComponents(NULL,NULL);

#ifdef MSG_HANDLER
        PrintStatusLine = &mainComponents.PrintStatus;
        qInstallMsgHandler(mainComponents.msgHandler);
#endif
        QDesktopWidget *desktop = new QDesktopWidget();
        mainComponents.resize(desktop->screenGeometry().width()*PERCENTAGE_OF_MONITOR_WIDTH_FOR_SCREEN, desktop->screenGeometry().height()*PERCENTAGE_OF_MONITOR_HEIGHT_FOR_SCREEN);
        mainComponents.show();

        app.setStyleSheet(SEPERATOR_STYLE);


        int ret = app.exec();

        delete g_GUI_CMIPS;

        return ret;

    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }
    catch (...)
    {
        qDebug() << "Unknown Error";
    }
    return(-1);
}
