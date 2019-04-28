// MaxSimpleAsset.h: interface for the MaxSimpleAsset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAXSIMPLEASSET_H__6901B7B7_729E_4159_9C2B_C41E5A5967B1__INCLUDED_)
#define AFX_MAXSIMPLEASSET_H__6901B7B7_729E_4159_9C2B_C41E5A5967B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwCommAsset.h"
#include "RwCommExportOptions.h"

class MaxSimpleAsset  : public RwCommAsset  
{
public:
	MaxSimpleAsset();
	virtual ~MaxSimpleAsset();

    virtual RwCommError SetAssetScope(const AssetScope scope) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError GetAssetScope(AssetScope &scope) const;

    virtual RwCommError SetName(const string &name);
    virtual RwCommError GetName(string &name) const;

    virtual RwCommError SetEnable(const bool bEnable) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError GetEnable(bool &bEnable) const;

    virtual RwCommError SetAssetTemplateName(const string &templateName);
    virtual RwCommError GetAssetTemplateName(string &templateName) const;

    virtual RwCommError SetExportPath(const string &exportPath) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError GetExportPath(string &exportPath) const { return RWCOMM_NOT_IMPLEMENTED; }

    virtual RwCommError SetGUID(const string &guid) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError GetGUID(string &guid) const { return RWCOMM_NOT_IMPLEMENTED; }

    virtual RwCommError SetSelection(const RwCommScene *pScene, const vector<RwCommTransform *> &vSelection) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError GetSelection(RwCommScene *pScene, vector<RwCommTransform *> &vSelection) const { return RWCOMM_SUCCESS; }

    virtual RwCommError GetSelectionNames(vector<pair<string, int> > &sNames) const { return RWCOMM_NOT_IMPLEMENTED; }

    virtual RwCommError SetOptions(const RwCommExportOptions &options) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError GetOptions(RwCommExportOptions &options) const { return RWCOMM_SUCCESS; }

    virtual RwCommError SaveSelection() { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError LoadSelection() const { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError LoadSelection(const vector<int> &SelectedAssest) const { return RWCOMM_NOT_IMPLEMENTED; }


    virtual RwCommError RemoveAsset(const vector<int> &Indicies) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual RwCommError RemoveAsset(int Index) { return RWCOMM_NOT_IMPLEMENTED; }

    virtual RwCommError SetParent(RwCommAsset *pParent) { return RWCOMM_NOT_IMPLEMENTED; }
	virtual RwCommAsset *GetParent() const { return NULL; }

   	virtual RwCommError AddChild(RwCommAsset *pChild) { return RWCOMM_NOT_IMPLEMENTED; }
   	virtual RwCommError RemoveChild(int i) { return RWCOMM_NOT_IMPLEMENTED; }
    virtual int         GetNumChildren() const { return 0; }
	virtual RwCommAsset *GetChild(int i) const { return NULL; }

    virtual RwCommError AddSelection(const RwCommAsset &other) { return RWCOMM_NOT_IMPLEMENTED; }

  

protected:

    string                      m_name;
    string                      m_templateName;
};

#endif // !defined(AFX_MAXSIMPLEASSET_H__6901B7B7_729E_4159_9C2B_C41E5A5967B1__INCLUDED_)
