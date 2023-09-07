//=====================================================================
// Copyright 2016-2018 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file PluginManager.cpp
/// \version 3.1
/// \brief Declares the interface to the Compressonator & ArchitectMF SDK
//=====================================================================

#include "pluginmanager.h"

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#define USE_NewLoader
#endif

#include <string>

#ifdef USE_NewLoader
#pragma warning(disable : 4091)  //'fopen': This function or variable may be unsafe.
#include "imagehlp.h"
#pragma comment(lib, "imagehlp.lib")

bool GetDLLFileExports(LPCSTR szFileName, std::vector<std::string>& names)
{
    //printf("%s\n",__FUNCTION__);
    _LOADED_IMAGE LoadedImage;

    if (!MapAndLoad(szFileName, NULL, &LoadedImage, TRUE, TRUE))
        return false;

    _IMAGE_EXPORT_DIRECTORY* ImageExportDirectory;
    ULONG                    DataSize;

    ImageExportDirectory = (_IMAGE_EXPORT_DIRECTORY*)ImageDirectoryEntryToData(LoadedImage.MappedAddress, false, IMAGE_DIRECTORY_ENTRY_EXPORT, &DataSize);
    if (ImageExportDirectory != NULL)
    {
        DWORD* dExportNameAddress(0);
        char*  FileExportName;

        dExportNameAddress = (DWORD*)ImageRvaToVa(LoadedImage.FileHeader, LoadedImage.MappedAddress, ImageExportDirectory->AddressOfNames, NULL);
        if (!dExportNameAddress)
        {
            UnMapAndLoad(&LoadedImage);
            return false;
        }

        for (size_t i = 0; i < ImageExportDirectory->NumberOfNames; i++)
        {
            FileExportName = (char*)ImageRvaToVa(LoadedImage.FileHeader, LoadedImage.MappedAddress, dExportNameAddress[i], NULL);
            if (FileExportName)
                names.push_back(FileExportName);
        }
    }

    UnMapAndLoad(&LoadedImage);

    //    This code that does not use MapAndLoad() and is much lower level
    //    unsigned int nNoOfExports;
    //
    //    HANDLE hFile;
    //    HANDLE hFileMapping;
    //    LPVOID lpFileBase;
    //    PIMAGE_DOS_HEADER pImg_DOS_Header;
    //    PIMAGE_NT_HEADERS pImg_NT_Header;
    //    PIMAGE_EXPORT_DIRECTORY pImg_Export_Dir;
    //
    //    hFile = CreateFileA(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //    if (hFile == INVALID_HANDLE_VALUE)
    //        return false;
    //
    //    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    //    if (hFileMapping == 0)
    //    {
    //        CloseHandle(hFile);
    //        return false;
    //    }
    //
    //    lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    //    if (lpFileBase == 0)
    //    {
    //        CloseHandle(hFileMapping);
    //        CloseHandle(hFile);
    //        return false;
    //    }
    //
    //    pImg_DOS_Header = (PIMAGE_DOS_HEADER)lpFileBase;
    //
    //    pImg_NT_Header = (PIMAGE_NT_HEADERS)((BYTE*)pImg_DOS_Header + pImg_DOS_Header->e_lfanew);
    //
    //    if (IsBadReadPtr(pImg_NT_Header, sizeof(IMAGE_NT_HEADERS)) || pImg_NT_Header->Signature != IMAGE_NT_SIGNATURE)
    //    {
    //        UnmapViewOfFile(lpFileBase);
    //        CloseHandle(hFileMapping);
    //        CloseHandle(hFile);
    //        return false;
    //    }
    //
    //    pImg_Export_Dir = (PIMAGE_EXPORT_DIRECTORY)pImg_NT_Header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    //    if (!pImg_Export_Dir)
    //    {
    //        UnmapViewOfFile(lpFileBase);
    //        CloseHandle(hFileMapping);
    //        CloseHandle(hFile);
    //        return false;
    //    }
    //
    //    pImg_Export_Dir = (PIMAGE_EXPORT_DIRECTORY)ImageRvaToVa(pImg_NT_Header, pImg_DOS_Header, (DWORD)pImg_Export_Dir, 0);
    //
    //    DWORD **ppdwNames = (DWORD **)pImg_Export_Dir->AddressOfNames;
    //
    //    ppdwNames = (PDWORD*)ImageRvaToVa(pImg_NT_Header, pImg_DOS_Header, (DWORD)ppdwNames, 0);
    //    if (!ppdwNames)
    //    {
    //        UnmapViewOfFile(lpFileBase);
    //        CloseHandle(hFileMapping);
    //        CloseHandle(hFile);
    //        return false;
    //    }
    //
    //    nNoOfExports = pImg_Export_Dir->NumberOfNames;
    //
    //    Bug here skips alternate names : error is in incr address of ppdwNames
    //    for (unsigned i = 0; i < nNoOfExports; i++)
    //    {
    //        char *szFunc = (PSTR)ImageRvaToVa(pImg_NT_Header, pImg_DOS_Header, (DWORD)*ppdwNames, 0);
    //        if (szFunc)
    //            names.push_back(szFunc);
    //        ppdwNames++;
    //    }
    //
    //    UnmapViewOfFile(lpFileBase);
    //    CloseHandle(hFileMapping);
    //    CloseHandle(hFile);
    return true;
};
#endif

PluginManager::PluginManager()
{
    //printf("%s\n", __FUNCTION__);

    m_pluginlistset = false;
}

PluginManager::~PluginManager()
{
    //printf("%s\n", __FUNCTION__);

    clearPluginList();
}

void PluginManager::registerStaticPlugin(char* pluginType, char* pluginName, void* makePlugin)
{
    //printf("%s\n", __FUNCTION__);

    PluginDetails* curPlugin = new PluginDetails();
    curPlugin->funcHandle    = reinterpret_cast<PLUGIN_FACTORYFUNC>(makePlugin);
    curPlugin->isStatic      = true;
    curPlugin->setType(pluginType);
    curPlugin->setName(pluginName);

    pluginRegister.push_back(curPlugin);
}

void PluginManager::registerStaticPlugin(char* pluginType, char* pluginName, char* uuid, void* makePlugin)
{
    PluginDetails* curPlugin = new PluginDetails();
    curPlugin->funcHandle    = reinterpret_cast<PLUGIN_FACTORYFUNC>(makePlugin);
    curPlugin->isStatic      = true;
    curPlugin->setType(pluginType);
    curPlugin->setName(pluginName);
    curPlugin->setUUID(uuid);

    pluginRegister.push_back(curPlugin);
}

void PluginManager::getPluginDetails(PluginDetails* curPlugin)
{
    //printf("%s\n", __FUNCTION__);

#ifdef _WIN32
    HINSTANCE dllHandle;

    dllHandle = LoadLibraryA(curPlugin->getFileName());

    if (dllHandle != NULL)
    {
        PLUGIN_TEXTFUNC textFunc;
        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginType"));
        if (textFunc)
            curPlugin->setType(textFunc());

        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginName"));
        if (textFunc)
            curPlugin->setName(textFunc());

        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginUUID"));
        if (textFunc)
            curPlugin->setUUID(textFunc());

        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginCategory"));
        if (textFunc)
            curPlugin->setCategory(textFunc());

		PLUGIN_ULONGFUNC ulongFunc;
        ulongFunc = reinterpret_cast<PLUGIN_ULONGFUNC>(GetProcAddress(dllHandle, "getPluginOptions"));
        if (ulongFunc)
            curPlugin->setOptions(ulongFunc());
        else
            curPlugin->setOptions(0);


        curPlugin->isRegistered = true;

        FreeLibrary(dllHandle);
    }
#endif
}

void PluginManager::clearPluginList()
{
    //printf("%s\n", __FUNCTION__);

    for (unsigned int i = 0; i < pluginRegister.size(); i++)
    {
        delete pluginRegister.at(i);
        pluginRegister.at(i) = NULL;
    }
    pluginRegister.clear();
}

bool PluginManager::fileExists(const std::string& abs_filename)
{
    bool ret = false;
    FILE* fp;
#ifdef _WIN32
    errno_t err = fopen_s(&fp, abs_filename.c_str(), "rb");
    if (err != 0)
        return false;
#else
    fp = fopen(abs_filename.c_str(), "rb");
#endif
    if (fp)
    {
        ret = true;
        fclose(fp);
    }
    return ret;
}


void PluginManager::getPluginList(char* SubFolderName, bool append)
{
    //printf("%s\n", __FUNCTION__);

    // Check for prior setting, if set clear for new one
    if (m_pluginlistset)
    {
        if (!append)
            clearPluginList();
        else
            return;
    }
    else
        m_pluginlistset = true;

#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char             fname[MAX_PATH];

    //----------------------------------
    // Load plugin List for processing
    // Use an Enviornment var, search through systems path
    // or use default app running folder.
    //----------------------------------
    char dirPath[MAX_PATH];

    // v2.1 change - to check if path exists in PATH or AMDCOMPRESS_PLUGINS is set
    // else use current exe directory
    char*  pPath;
    size_t len;

#ifdef _WIN32
    _dupenv_s(&pPath, &len, "AMDCOMPRESS_PLUGINS");
#else
    pPath = getenv("AMDCOMPRESS_PLUGINS") + '\0';
    len   = strlen(pPath);
#endif

    if (len > 0)
    {
        snprintf(dirPath, 260, "%s", pPath + '\0');
    }
    else
    {
        bool pathFound = false;

        //Get the exe directory
        DWORD   pathsize;
        HMODULE hModule = GetModuleHandleA(NULL);
        pathsize        = GetModuleFileNameA(hModule, dirPath, MAX_PATH);
        if (pathsize > 0)
        {
            char* appName    = (strrchr(dirPath, '\\') + 1);
            int   pathLen    = (int)strlen(dirPath);
            int   appNameLen = (int)strlen(appName);

            // Null terminate the dirPath so that FileName is removed
            pathLen          = pathLen - appNameLen;
            dirPath[pathLen] = 0;
            if (dirPath[pathLen - 1] == '/' || dirPath[pathLen - 1] == '\\')
            {
                pathLen--;
                dirPath[pathLen] = 0;
            }

            strcat_s(dirPath, SubFolderName);

            snprintf(fname, 260, "%s\\*.dll", dirPath);

            HANDLE hFind = FindFirstFileA(fname, &fd);

            if (hFind != INVALID_HANDLE_VALUE)
            {
                pathFound = true;
            }

            FindClose(hFind);
        }

        if (!pathFound)
        {
#ifdef _WIN32
            _dupenv_s(&pPath, &len, "PATH");
#else
            pPath = getenv("PATH");
            len   = strlen(pPath);
#endif
            if (len > 0)
            {
                std::string s         = pPath;
                char        delimiter = ';';
                size_t      pos       = 0;
                std::string token;
                while ((pos = s.find(delimiter)) != std::string::npos)
                {
                    token = s.substr(0, pos);
                    snprintf(dirPath, 260, "%s\\compressonatorCLI.exe", token.c_str());
                    if (fileExists(dirPath))
                    {
                        snprintf(dirPath, 260, "%s", token.c_str());
                        strcat_s(dirPath, SubFolderName);
                        pathFound = true;
                        break;
                    }
                    s.erase(0, pos + sizeof(delimiter));
                }
            }
        }
    }

#ifdef _WIN32
    strcpy_s(fname, dirPath);
#else
    strcpy(fname, dirPath);
#endif
    len = strlen(fname);
    if (fname[len - 1] == '/' || fname[len - 1] == '\\')
        strcat_s(fname, "*.dll");
    else
        strcat_s(fname, "\\*.dll");
    HANDLE hFind = FindFirstFileA(fname, &fd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return;
    }

    do
    {
        HINSTANCE                dllHandle = NULL;
        std::vector<std::string> names;

        try
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                snprintf(fname, MAX_PATH, "%s\\%s", dirPath, fd.cFileName);

#ifdef USE_NewLoader

                //printf("GetDLL File exports %s\n", fd.cFileName);
                // Valid only for Windows need one for Linux
                names.clear();
                GetDLLFileExports(fname, names);

                if ((names.size() >= 3) && (names.size() <= 50))
                {
                    bool bmakePlugin    = false;
                    bool bgetPluginType = false;
                    bool bgetPluginName = false;

                    for (std::vector<std::string>::const_iterator str = names.begin(); str != names.end(); ++str)
                    {
                        if (*str == "makePlugin")
                            bmakePlugin = true;
                        else if (*str == "getPluginType")
                            bgetPluginType = true;
                        else if (*str == "getPluginName")
                            bgetPluginName = true;
                    }

                    if (bmakePlugin && bgetPluginType && bgetPluginName)
                    {
                        // we have a vaild plugin to register to use when needed save it!
                        PluginDetails* curPlugin = new PluginDetails();
                        curPlugin->setFileName(fname);
                        pluginRegister.push_back(curPlugin);
                    }
                }
#else
                dllHandle = LoadLibraryA(fname);
                if (dllHandle != NULL)
                {
                    // Is this DLL a plugin for us if so keep its type and name details
                    PLUGIN_FACTORYFUNC funcHandle;
                    funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC>(GetProcAddress(dllHandle, "makePlugin"));
                    if (funcHandle != NULL)
                    {
                        PluginDetails* curPlugin = new PluginDetails();
                        //printf("new: %s\n", fname);
                        curPlugin->setFileName(fname);

                        PLUGIN_TEXTFUNC textFunc;
                        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginType"));
                        if (textFunc)
                            curPlugin->setType(textFunc());

                        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginName"));
                        if (textFunc)
                            curPlugin->setName(textFunc());

                        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginUUID"));
                        if (textFunc)
                            curPlugin->setUUID(textFunc());

                        textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginCategory"));
                        if (textFunc)
                            curPlugin->setCategory(textFunc());

                        curPlugin->isRegistered = true;

                        pluginRegister.push_back(curPlugin);
                    }
                    FreeLibrary(dllHandle);
                }
#endif
            }
        }
        catch (...)
        {
            if (dllHandle != NULL)
                FreeLibrary(dllHandle);
        }
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);
#endif
}

void* PluginManager::makeNewPluginInstance(int index)
{
    if (!pluginRegister.at(index)->isRegistered)
        getPluginDetails(pluginRegister.at(index));

    return pluginRegister.at(index)->makeNewInstance();
}

int PluginManager::getNumPlugins()
{
    return static_cast<int>(pluginRegister.size());
}

char* PluginManager::getPluginName(int index)
{
    if (!pluginRegister.at(index)->isRegistered)
        getPluginDetails(pluginRegister.at(index));
    return pluginRegister.at(index)->getName();
}

char* PluginManager::getPluginUUID(int index)
{
    if (!pluginRegister.at(index)->isRegistered)
        getPluginDetails(pluginRegister.at(index));
    return pluginRegister.at(index)->getUUID();
}

char* PluginManager::getPluginCategory(int index)
{
    if (!pluginRegister.at(index)->isRegistered)
        getPluginDetails(pluginRegister.at(index));
    return pluginRegister.at(index)->getCategory();
}

char* PluginManager::getPluginType(int index)
{
    if (!pluginRegister.at(index)->isRegistered)
        getPluginDetails(pluginRegister.at(index));
    return pluginRegister.at(index)->getType();
}

unsigned long PluginManager::getPluginOption(int index)
{
    if (!pluginRegister.at(index)->isRegistered)
        getPluginDetails(pluginRegister.at(index));
    return pluginRegister.at(index)->getOptions();
}

void* PluginManager::GetPlugin(char* type, const char* name)
{
    if (!m_pluginlistset)
    {
        getPluginList(DEFAULT_PLUGINLIST_DIR);
    }

    unsigned int numPlugins = getNumPlugins();
    for (unsigned int i = 0; i < numPlugins; i++)
    {
        PluginDetails* pPlugin = pluginRegister.at(i);
        if (!pPlugin->isRegistered)
            getPluginDetails(pPlugin);
        //printf("%2d getPlugin(Type %s name %s) == [%s,%s] \n", i, getPluginType(i), getPluginName(i), type, name);
        if ((strcmp(getPluginType(i), type) == 0) && (strcmp(getPluginName(i), name) == 0))
        {
            return ((void*)makeNewPluginInstance(i));
        }
    }
    return (NULL);
}

bool PluginManager::RemovePlugin(char* type, char* name)
{
    if (!m_pluginlistset)
    {
        getPluginList(DEFAULT_PLUGINLIST_DIR);
    }

    int numPlugins = getNumPlugins();
    for (int i = 0; i < numPlugins; i++)
    {
        if (!pluginRegister.at(i)->isRegistered)
            getPluginDetails(pluginRegister.at(i));

        if ((strcmp(getPluginType(i), type) == 0) && (strcmp(getPluginName(i), name) == 0))
        {
            delete pluginRegister.at(i);
            return true;
        }
    }
    return (false);
}

void* PluginManager::GetPlugin(char* uuid)
{
    if (!m_pluginlistset)
    {
        getPluginList(DEFAULT_PLUGINLIST_DIR);
    }

    int numPlugins = getNumPlugins();
    for (int i = 0; i < numPlugins; i++)
    {
        if (!pluginRegister.at(i)->isRegistered)
            getPluginDetails(pluginRegister.at(i));

        if (strcmp(getPluginUUID(i), uuid) == 0)
        {
            return ((void*)makeNewPluginInstance(i));
        }
    }
    return (NULL);
}

bool PluginManager::PluginSupported(char* type, char* name)
{
    if (!type)
        return false;
    if (!name)
        return false;
    if (!m_pluginlistset)
    {
        getPluginList(DEFAULT_PLUGINLIST_DIR);
    }

    int numPlugins = getNumPlugins();
    for (int i = 0; i < numPlugins; i++)
    {
        if (!pluginRegister.at(i)->isRegistered)
            getPluginDetails(pluginRegister.at(i));

        //PrintInfo("Type : %s  Name : %s\n",pluginManager.getPluginType(i),pluginManager.getPluginName(i));
        if ((strcmp(getPluginType(i), type) == 0) && (strcmp(getPluginName(i), name) == 0))
        {
            return (true);
        }
    }
    return (false);
}

void PluginManager::getPluginListTypeNames(char* pluginType, std::vector<std::string> &TypeNames)
{
    TypeNames.clear();
    PluginDetails* plugin;
    char*          pName;
    char*          pType;
    for (unsigned int i = 0; i < pluginRegister.size(); i++)
    {
        plugin = pluginRegister.at(i);

        pType = plugin->getType();
        if (strlen(pType) == 0)
        { 
            getPluginDetails(plugin);
            pName = plugin->getName();
            pType = plugin->getType();
        }
        else
            pName = plugin->getName();

        if (strlen(pType) > 0)
        {
            if (strcmp(pluginType, pType) == 0)
            {
                TypeNames.push_back(pName);
            }
        }
    }
}

void PluginManager::getPluginListOptionNames(char* pluginType, unsigned long options, std::vector<std::string>& TypeNames)
{
    PluginDetails* plugin;
    char*          pName;
    char*          pType;
    unsigned long  uOptions;

    for (unsigned int i = 0; i < pluginRegister.size(); i++)
    {
        plugin = pluginRegister.at(i);

        pType = plugin->getType();
        if (strlen(pType) == 0)
        { 
            getPluginDetails(plugin);
            pName = plugin->getName();
            pType = plugin->getType();
            uOptions = plugin->getOptions();
        }
        else
        {
            pName = plugin->getName();
            uOptions = plugin->getOptions();
        }

        if (strlen(pType) > 0)
        {
            unsigned long selectedoption = uOptions & options;
            if ((strcmp(pluginType, pType) == 0) && (selectedoption > 0))
            {
                TypeNames.push_back(pName);
            }
        }
    }
}

//----------------------------------------------

PluginDetails::~PluginDetails()
{
#ifdef _WIN32
    if (dllHandle)
        FreeLibrary(dllHandle);
#endif
    clearMembers();
}

void PluginDetails::setFileName(char* nm)
{
#ifdef _WIN32
    strcpy_s(filename, MAX_PLUGIN_FILENAME_STR, nm);
#else
    strcpy(filename, nm);
#endif
}

void PluginDetails::setName(char* nm)
{
#ifdef _WIN32
    strcpy_s(pluginName, MAX_PLUGIN_NAME_STR, nm);
#else
    strcpy(pluginName, nm);
#endif
}

void PluginDetails::setUUID(char* nm)
{
#ifdef _WIN32
    strcpy_s(pluginUUID, MAX_PLUGIN_UUID_STR, nm);
#else
    strcpy(pluginUUID, nm);
#endif
}

void PluginDetails::setOptions(unsigned long  uoptions)
{
    pluginOptions = uoptions;
}

void PluginDetails::setType(char* nm)
{
#ifdef _WIN32
    strcpy_s(pluginType, MAX_PLUGIN_TYPE_STR, nm);
#else
    strcpy(pluginType, nm);
#endif
} 

void PluginDetails::setCategory(char* nm)
{
#ifdef _WIN32
    strcpy_s(pluginCategory, MAX_PLUGIN_CATEGORY_STR, nm);
#else
    strcpy(pluginCategory, nm);
#endif
}

void* PluginDetails::makeNewInstance()
{
    if (isStatic)
    {
        return funcHandle();
    }
    else
    {
#ifdef _WIN32
        if (!dllHandle)
            dllHandle = LoadLibraryA(filename);

        if (dllHandle != NULL)
        {
            funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC>(GetProcAddress(dllHandle, "makePlugin"));
            if (funcHandle != NULL)
            {
                return funcHandle();
            }
        }
#endif
    }
    return NULL;
}
