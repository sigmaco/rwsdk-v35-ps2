// RwMIAsset.h: interface for the RwMIAsset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWMIASSET_H__4986B501_415E_4DEC_9E34_F03667159F83__INCLUDED_)
#define AFX_RWMIASSET_H__4986B501_415E_4DEC_9E34_F03667159F83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwMI.h"
#include "RwCommAsset.h"
#include "RwCommExportOptions.h"

/**
*   \ingroup miscenegraph
*
*   This is the MI implementation of RwCommAsset.
*
*   \see RwCommAsset
*/
class RWMI_API RwMIAsset : public RwCommAsset  
{
public:
	RwMIAsset();
	virtual ~RwMIAsset();

    virtual RwCommError SetAssetScope(const AssetScope aScope);
    virtual RwCommError GetAssetScope(AssetScope &aScope) const;

    virtual RwCommError SetName(const string &name);
    virtual RwCommError GetName(string &name) const;

    virtual RwCommError SetEnable(const bool bEnable);
    virtual RwCommError GetEnable(bool &bEnable) const;

    virtual RwCommError SetAssetTemplateName(const string &templateName);
    virtual RwCommError GetAssetTemplateName(string &templateName) const;

    virtual RwCommError SetExportPath(const string &exportPath);
    virtual RwCommError GetExportPath(string &exportPath) const;

    virtual RwCommError SetGUID(const string &guid);
    virtual RwCommError GetGUID(string &guid) const;

    virtual RwCommError SetSelection(const RwCommScene *pScene, const vector<RwCommTransform *> &vSelection);
    virtual RwCommError GetSelection(RwCommScene *pScene, vector<RwCommTransform *> &vSelection) const;

    virtual RwCommError GetSelectionNames(vector<pair<string, int> > &sNames) const;

    virtual RwCommError SetOptions(const RwCommExportOptions &options);
    virtual RwCommError GetOptions(RwCommExportOptions &options) const;

    virtual RwCommError SaveSelection() { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError LoadSelection() const { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError LoadSelection(const vector<int> &SelectedAssest) const { return RWCOMM_NOT_IMPLEMENTED; }

    virtual RwCommError RemoveAsset( int Index) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError RemoveAsset( const vector<int> &Indices) { return RWCOMM_NOT_IMPLEMENTED; }

    virtual RwCommError SetParent(RwCommAsset *pParent);
	virtual RwCommAsset *GetParent() const;

   	virtual RwCommError AddChild(RwCommAsset *pChild);
   	virtual RwCommError RemoveChild(int i);
    virtual int         GetNumChildren() const;
	virtual RwCommAsset *GetChild(int i) const;

    virtual RwCommError AddSelection(const RwCommAsset &other);


  

protected:

    AssetScope                  m_scope;
    bool                        m_bEnable;
    string                      m_assetName;
    string                      m_templateName;
    string                      m_exportPath;
    string                      m_guid;
    vector<RwCommTransform *>   m_vSelection;
    RwCommExportOptions         m_exportOptions;
    RwCommAsset                 *m_pParent;
    vector<RwCommAsset*>        m_vChildren;
};

#endif // !defined(AFX_RWMIASSET_H__4986B501_415E_4DEC_9E34_F03667159F83__INCLUDED_)
