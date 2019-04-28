// RwCommAsset.h: interface for the RwCommAsset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWCOMMASSET_H__012453FC_45C4_422C_8681_B9A40F0B6EF8__INCLUDED_)
#define AFX_RWCOMMASSET_H__012453FC_45C4_422C_8681_B9A40F0B6EF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwCommObject.h"

class RwCommTransform;
class RwCommScene;
class RwCommExportOptions;

const RwCommType RwCommAssetType("RwCommAsset");

/**
*  \ingroup commscenegraph  
*
*   This is the class representing an asset. The RwComm scene graph can
*   be split into different export entities: for example into static world, several animated
*   hierarchies etc. An asset is a node in the scene representing a sub set of the scene.
*   Each asset holds a list of scene graph objects that specify that sub set. That list is called the
*   asset's selection, which can be set and queried using the SetSelection(), GetSelection() methods.
*   
*   In addition, each asset stores all the export options describing that entity. These options
*   do not include all the options stored in the asset templates. The options can
*   be set or queried using the SetOptions()/GetOptions() methods. To query and set the asset template name
*   that the asset is assigned to, use the SetAssetTemplateName()/GetAssetTemplateName() methods.
*   
*   Assets can be linked to other assets (i.e. animations to animated hierarchies assets), creating
*   an hierarchy of assets. This hierarchy can be queried and set, using the available methods.
*   
*   Each asset also has a GUID, that identifies that asset. That GUID is streamed out in the TOC of every
*   .rws file. 
*/
class RWCOMM_API RwCommAsset : public RwCommObject  
{
public:
    
    /** Asset scope */
    enum AssetScope
    {
        /** Node refers to a scene asset */
        ASSET = 0,
        /** Node refers to the scene */
        SCENE,
    };

    RwCommAsset();
	virtual ~RwCommAsset();

    // Type methods
	virtual RwCommType	GetType() const { return RwCommAssetType; }
	virtual void        GetTypeHierarchy(vector<RwCommType>& vTypeVec) const;
    
    /**
    *   \param aScope of the asset to set 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetAssetScope(const AssetScope aScope) = 0;
    /**
    *   \param aScope returns the asset scope
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetAssetScope(AssetScope &aScope) const = 0;

    /**
    *   \param name of the asset to set 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetName(const string &name) = 0;
    /**
    *   \param name returns the asset name
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetName(string &name) const = 0;

    /**
    *   Enables/disables the asset. Only enabled assets are exported. 
    *   \param bEnable indicates if asset should be enabled or disabled
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetEnable(const bool bEnable) = 0;
    /**
    *   \param bEnable returns true if asset is enabled, false otherwise
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetEnable(bool &bEnable) const = 0;

    /**
    *   Several asset can share options in an external file called asset template.
    *   
    *   \param templateName of the asset to set 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetAssetTemplateName(const string &templateName) = 0;
    /**
    *   Several asset can share options in an external file called asset template.
    *   
    *   \param templateName returns the asset template name
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetAssetTemplateName(string &templateName) const = 0;

    /**
    *   \param exportPath of the asset to set 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetExportPath(const string &exportPath) = 0;
    /**
    *   \param exportPath returns the asset export path
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetExportPath(string &exportPath) const = 0;

    /**
    *   GUID serves as an unique indentifier for the asset.
    *   
    *   \param guid of the asset to set 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetGUID(const string &guid) = 0;
    /**
    *   GUID serves as an unique indentifier for the asset.
    *   
    *   \param guid returns the asset export path
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetGUID(string &guid) const = 0;

    /**
    *   Sets the asset's selction.
    *   \param pScene pointer to the comm scene
    *   \param vSelection vector of transform nodes to set as an asset selection
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetSelection(const RwCommScene *pScene,
                            const vector<RwCommTransform *> &vSelection) = 0;
    /**
    *   Gets the asset's selection.
    *   \param pScene pointer to the comm scene
    *   \param vSelection returns selection for this asset
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetSelection(RwCommScene *pScene,
                            vector<RwCommTransform *> &vSelection) const = 0;

    /**
    *   \param sNames returns all the names for the nodes this asset refers to
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetSelectionNames(vector<pair<string, int> > &sNames) const = 0;

    /**
    *   Sets the asset's export options. These do not include the asset template options.
    *   \param options of the asset to set 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetOptions(const RwCommExportOptions &options) = 0;
    /**
    *   \param options returns the asset options
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetOptions(RwCommExportOptions &options) const = 0;

    /** Save currently selected objects in the scene as a selection for this asset */
    virtual RwCommError SaveSelection() = 0;
    /** Select in the scene all the objects this asset refers to */
    virtual RwCommError LoadSelection() const = 0;
    /**
    *   Select in the scene just the object in the selection, specified by
    *   an integer vector
    */
    virtual RwCommError LoadSelection(const vector<int> &SelectedAssest) const = 0;

    /**
    *   \param Indices in the selction of objects to remove.
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError RemoveAsset(const vector<int> &Indices) = 0;
    /**
    *   \param Index in the selction of an object to remove.
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError RemoveAsset(int Index) = 0;

    /**
    *   Sets the asset's parent.
    *
    *   \param pParent asset node to set as a parent 
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetParent(RwCommAsset *pParent) = 0;
    /**
    *   \return asset's parent.
    *
    *   \see SetParent().
    */
	virtual RwCommAsset *GetParent() const = 0;

    /**
    *   Adds a child asset.
    *   \param pChild to add to the asset
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    *
    *   \see SetParent().
    */
   	virtual RwCommError AddChild(RwCommAsset *pChild) = 0;
    /**
    *   Removes a child asset.
    *   \param i index of an child to remove
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    *
    *   \see SetParent().
    */
   	virtual RwCommError RemoveChild(int i) = 0;
    /**
    *   \return number of children
    *
    *   \see SetParent().
    */
    virtual int         GetNumChildren() const = 0;
    /**
    *   \param i child index to retrieve
    *
    *   \return specified child asset
    *
    *   \see SetParent().
    */
	virtual RwCommAsset *GetChild(int i) const = 0;

    /**
    *   \param other an asset node to add the selection from.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError AddSelection(const RwCommAsset &other) = 0;


protected:
};

#endif // !defined(AFX_RWCOMMASSET_H__012453FC_45C4_422C_8681_B9A40F0B6EF8__INCLUDED_)
