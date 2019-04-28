// RwMITexture.h: interface for the RwMITexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWMITEXTURE_H__867D3DBA_A3CF_4260_B7F4_8F4920A4C11A__INCLUDED_)
#define AFX_RWMITEXTURE_H__867D3DBA_A3CF_4260_B7F4_8F4920A4C11A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwMI.h"
#include "RwCommTexture.h"
#include "RwCommImage.h"
#include "RwMIAttributes.h"
#include "RwMIAnimProp.h"

/**
*   \ingroup miscenegraph
*
*   This is the MI implementation of RwCommTexture.
*
*   \see RwCommTexture
*/
class RWMI_API RwMITexture : public RwCommTexture, public RwMIAttributes
{
public:
	RwMITexture();
	virtual ~RwMITexture();

    virtual RwCommError GetUserData(RwCommUserDataCollection &userDataColl);

    // Get attribute methods.
    virtual RwCommError GetAttribute(const string& name, int &value, int index = 0);
    virtual RwCommError GetAttribute(const string& name, float &value, int index = 0);
    virtual RwCommError GetAttribute(const string& name, string &value, int index = 0);
    virtual RwCommError GetAttributeNames(vector<string>& vAttributeNames, vector<AttributeType> &vAttributeTypes);

	// Texture methods
	virtual RwCommError GetFileName(string&) const;
	virtual RwCommError GetTiling(int& tile) const;
	virtual RwCommError GetCroppingValues(	RwCommReal& u, RwCommReal& v,
					    					RwCommReal& w, RwCommReal& h ) const;
	virtual RwCommError GetUVTransform(RwCommMatrix&) const;
    virtual RwCommError GetFilter(FilterType&) const;

    // Image methods
    virtual RwCommError GetImage(RwCommImage& image, RwCommTexture *pAlphaTexture= NULL);
    virtual RwCommError GetImage(RwCommImage& image, int width, int height, RwCommTexture *pAlphaTexture= NULL);
    virtual RwCommError GetImageInfo(RwCommImageInfo& info);

    // RwMITexture methods
    RwCommError SetFileName(RwCommReal frame, const string& filename);
    RwCommError SetFileName(vector< RwMIAnimPropKey<string> > &vKeys);

    RwCommError SetTiling(int tile);

    RwCommError SetCroppingValues(RwCommReal frame, RwCommReal u, RwCommReal v, RwCommReal w, RwCommReal h);
    RwCommError SetCropU(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);
    RwCommError SetCropV(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);
    RwCommError SetCropW(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);
    RwCommError SetCropH(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);
    
    RwCommError SetUVTransform(RwCommReal frame, const RwCommMatrix &M);
    RwCommError SetUVTransform(vector< RwMIAnimPropKey<RwCommMatrix> > &vKeys);

    RwCommError SetFilter(FilterType filter);

    static void        InvertAlphaChannel(RwCommImage& image);

protected:
    int                         m_tileMode;

    RwMIAnimProp<string>        m_fileName;
    RwMIAnimProp<RwCommReal>    m_cropU;
    RwMIAnimProp<RwCommReal>    m_cropV;
    RwMIAnimProp<RwCommReal>    m_cropW;
    RwMIAnimProp<RwCommReal>    m_cropH;

    RwMIAnimProp<RwCommMatrix>  m_uvTransform;

    FilterType      m_filter;

    RwCommImageInfo             m_imgInfoCache;
    string                      m_imgInfoCacheName;

};

#endif // !defined(AFX_RWMITEXTURE_H__867D3DBA_A3CF_4260_B7F4_8F4920A4C11A__INCLUDED_)
