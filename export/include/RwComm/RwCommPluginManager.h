// RwCommPluginManager.h: interface for the RwCommPluginManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWCOMMPLUGINMANAGER_H__ABB64C10_D29E_4FEE_99D7_8784FEDE1C12__INCLUDED_)
#define AFX_RWCOMMPLUGINMANAGER_H__ABB64C10_D29E_4FEE_99D7_8784FEDE1C12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;

#pragma warning(disable : 4251)

#include "RwComm.h"
#include "RwCommError.h"

#if (!defined(DOXYGEN))

class RWCOMM_API RwCommPluginManager  
{
public:

    struct ModuleInfo
    {
        string Name;
        string Version;
        string FullPath;
    };

	virtual ~RwCommPluginManager();

    static RwCommPluginManager& TheInstance();

    const vector<HMODULE>& GetAllModules() const { return m_vModules; }

    const vector<ModuleInfo>& GetAllUnloadedModules() const { return m_vUnloadedModules; }

    RwCommError     CheckLibVersion(const string& module, int v1, int v2, int v3, int v4,
                                bool bCheckMinorUpdateVersion = true);

    static void     SetPluginFolderPath(const string& path);

    static string   GetDllFolderPath();
    static void     SetDllFolderPath(const string& path);
    
private:
    RwCommPluginManager();
    void            LoadModules(const string& filePath);
    void            AddInvalidModule(const string &name, unsigned int version, const string &path);

private:
    vector<HMODULE>    m_vModules;
    vector<ModuleInfo> m_vUnloadedModules;
    static string      m_pluginFolderPath;
    static string      m_dllFolderPath;
};

#endif // (!defined(DOXYGEN))

#endif // !defined(AFX_RWCOMMPLUGINMANAGER_H__ABB64C10_D29E_4FEE_99D7_8784FEDE1C12__INCLUDED_)
