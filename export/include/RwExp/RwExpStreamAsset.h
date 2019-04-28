// RwExpStreamAsset.h: interface for the RwExpStreamAsset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPSTREAMASSET_H__7823C909_44AD_4C82_88AA_7DF328D63532__INCLUDED_)
#define AFX_RWEXPSTREAMASSET_H__7823C909_44AD_4C82_88AA_7DF328D63532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwExp.h"

// =============================================================================
// ----------------------------  RwExpAsset CLASS  -----------------------------
// =============================================================================

/**
*  \ingroup structures
*   This class represents a standard stream asset in the StreamAssetList. The assets that are currently supported are:
*   \li rwID_WORLD
*   \li rwID_CLUMP
*   \li rwID_HANIMANIMATION
*   \li rwID_DMORPHANIMATION
*   \li rwID_SPLINE
*   \li rwID_MTEFFECTDICT
*   \li rwID_PITEXDICTIONARY
*
*   Users can create their own custom stream assets by deriving from the RwExpStreamAsset class and implementing the
*   StreamWrite() and StreamToFile() virtual methods.
*
*   \see RwExpStreamAssetList
*/
class RWEXP_API RwExpStreamAsset
{
public:
    RwExpStreamAsset(RwCorePluginID newType, void *newData, string &newFileName, string guid = "");
    ~RwExpStreamAsset();

    /** Stream out the asset to a RenderWare stream. Users implementing this method should use the standard RenderWare
    streaming utilities for doing so.*/
    virtual void    StreamWrite(RwStream *stream);

    /** Stream out the asset to a file. Use the m_fileName member to obtain the file name.*/
    virtual void    StreamToFile();

    /** Get the plugin ID **/
    RwCorePluginID GetPluginID() const { return m_type; }

    /** Get a pointer to the asset's data **/
    void*   GetData() const { return m_pData; }

    /** Get the asset file name **/
    const string& GetFileName() const { return m_fileName; }

    /** Get the asset GUID **/
    const string& GetGuid() const { return m_guid; }

private:

    RwCorePluginID  m_type;
    void            *m_pData;
    string          m_fileName;
    string          m_guid;
};

typedef list<RwExpStreamAsset*>::iterator rwExpAssetListIt;

// =============================================================================
// --------------------------  RwExpAssetList CLASS  ---------------------------
// =============================================================================

/**
*  \ingroup structures
*   The exporters build the appropriate RenderWare Graphics structures and add them to the stream asset list.
*   The stream asset list is a collection of RenderWare Graphics primitives, such as RpClumps, RpWorlds, 
*   RpHAnimHierarchy etc. These primitives are created in the exporters, appended to the asset list and streamed out
*   to a file (usually an .RWS file) at the end of the export process.
*
*   \see RwExpStreamAsset
*/
class RWEXP_API RwExpStreamAssetList
{
public:
    RwExpStreamAssetList();
    ~RwExpStreamAssetList();

    /** Remove all stream assets from the list.*/
    void    ClearAssets();

    /** Add an asset to the list.*/
    void    AddEntry(RwExpStreamAsset *asset);

    /** Get the list of assets .*/
    const list<RwExpStreamAsset*> &GetAssets() const { return m_assets; }


    // Streaming methods
    void    StreamToc(RwStream *stream);
    void    StreamWrite(RwStream *stream, bool bToc);
    void    StreamToFile(string fileName, bool bToc);
    void    StreamToFiles();

    // Texture methods
    int     GetNumTextures() const { return m_textures.size(); }
    void    AddTexture(const string& name);
    void    AddUnresolvedTexture(const string& name);

    const vector<string>    &GetTextures() { return m_textures; }
    const vector<string>    &GetUnresolvedTextures() { return m_unresolvedTextures; }

    // Effect files
    vector<string>          &GetEffectFiles() { return m_effectFiles; }
    void                    AddEffectFile(const string& file);
    
    // Tetxure dictionary
    RwTexDictionary         *GetTextureDictionary() { return m_pTextureDictionary; };
    void                    SetTextureDictionary(RwTexDictionary *pTextureDictionary)
                                { m_pTextureDictionary = pTextureDictionary; };

protected:

private:
    list<RwExpStreamAsset*>       m_assets;
    RwTexDictionary         *m_pTextureDictionary;

    vector<string>          m_textures;
    vector<string>          m_unresolvedTextures;
    vector<string>          m_effectFiles;
};

#endif // !defined(AFX_RWEXPSTREAMASSET_H__7823C909_44AD_4C82_88AA_7DF328D63532__INCLUDED_)
