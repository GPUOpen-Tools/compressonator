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
//
/// \file PluginManager.h
/// \version 2.20
//
//=====================================================================

#ifndef  _PLUGINMANAGER_H
#define  _PLUGINMANAGER_H

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <direct.h>
//#include "PluginInterface.h"
#include "PluginBase.h"

using namespace std;

#define MAX_PLUGIN_FILENAME_STR   512
#define MAX_PLUGIN_NAME_STR       256
#define MAX_PLUGIN_UUID_STR       37
#define MAX_PLUGIN_TYPE_STR       64
#define MAX_PLUGIN_CATEGORY_STR   64

class PluginDetails
{
    public:
        PluginDetails() { clearMembers(); }
        
        ~PluginDetails();
        
        void *makeNewInstance();

        void setFileName(char * nm);
        char *getName()         { return pluginName; }
        char *getUUID()         { return pluginUUID; }
        char *getType()         { return pluginType; }
        char *getCategory()    { return pluginCategory; }

        void setName(char * nm);  
        void setUUID(char * nm);
        void setType(char * nm);
        void setCategory(char * nm);

        bool                isStatic;
        PLUGIN_FACTORYFUNC  funcHandle;
        
    private:

        void clearMembers()
        {
            dllHandle  = NULL;
            isStatic   = false;
            filename[0]   = 0;
            pluginType[0] = 0;
            pluginName[0] = 0;
            pluginUUID[0] = 0;
            pluginCategory[0] = 0;
        }

        char filename   [MAX_PLUGIN_FILENAME_STR];
        char pluginType [MAX_PLUGIN_TYPE_STR];
        char pluginName [MAX_PLUGIN_NAME_STR];
        char pluginUUID [MAX_PLUGIN_UUID_STR];
        char pluginCategory[MAX_PLUGIN_CATEGORY_STR];

        HINSTANCE           dllHandle;

};


class PluginManager
{
    public:
        PluginManager(){}
        ~PluginManager() { clearPluginList(); }

        void getPluginList(char * dirPath);

        void registerStaticPlugin(char *pluginType, char *pluginName, void *  makePlugin);
        void registerStaticPlugin(char *pluginType, char *pluginName, char *uuid, void *  makePlugin);

        void * makeNewPluginInstance(int index)
        {
            return pluginRegister.at(index)->makeNewInstance();
        }
        
        int getNumPlugins()
        {
            return int(pluginRegister.size());
        }
        
        char * getPluginName(int index)
        {
            return pluginRegister.at(index)->getName();
        }


        char * getPluginUUID(int index)
        {
            return pluginRegister.at(index)->getUUID();
        }

        char * getPluginCategory(int index)
        {
            return pluginRegister.at(index)->getCategory();
        }
        
        char * getPluginType(int index)
        {
            return pluginRegister.at(index)->getType();
        }

        void *GetPlugin(char *type, char *name)
        {
            int numPlugins = getNumPlugins();
            for (int i=0; i< numPlugins; i++)
            {
                if ( (strcmp(getPluginType(i),type) == 0) && 
                     (strcmp(getPluginName(i),name)   == 0))
                {
                        return ( (void *)makeNewPluginInstance(i) );
                }
            }
            return (NULL);
        }

        void *GetPlugin(char *uuid)
        {
            int numPlugins = getNumPlugins();
            for (int i = 0; i< numPlugins; i++)
            {
                if (strcmp(getPluginUUID(i), uuid) == 0)
                {
                    return ((void *)makeNewPluginInstance(i));
                }
            }
            return (NULL);
        }


        bool PluginSupported(char *type, char *name)
        {
            int numPlugins = getNumPlugins();
            for (int i = 0; i< numPlugins; i++)
            {
                //PrintInfo("Type : %s  Name : %s\n",pluginManager.getPluginType(i),pluginManager.getPluginName(i));
                if ((strcmp(getPluginType(i), type) == 0) &&
                    (strcmp(getPluginName(i), name) == 0))
                {
                    return (true);
                }
            }
            return (false);
        }

    private:

        void clearPluginList()
        {
            for (unsigned int i = 0; i < pluginRegister.size(); i++)
            {
                delete pluginRegister.at(i);
                pluginRegister.at(i) = NULL;
            }    
            pluginRegister.clear();
        }
        vector<PluginDetails*> pluginRegister;
};

#endif
