// RfExpLight.h: interface for the RfExpLight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RwRf3EXPLIGHT_H__00B31588_05F6_4628_B9E2_08574E190634__INCLUDED_)
#define AFX_RwRf3EXPLIGHT_H__00B31588_05F6_4628_B9E2_08574E190634__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwRf3.h"
#include "RwRf3Object.h"

#include "RwRf3ObjCreator.h"

#define RF3_LIGHT_TAG               "light"
#define RF3_LIGHT_TYPE              "type"
#define RF3_LIGHT_TYPEUNKNOWN       "UNKNOWN"
#define RF3_LIGHT_TYPEPOINT         "POINT"
#define RF3_LIGHT_TYPEDIRECTIONAL   "DIRECTIONAL"
#define RF3_LIGHT_TYPEAMBIENT       "AMBIENT"
#define RF3_LIGHT_TYPESPOT          "SPOT"
#define RF3_LIGHT_CONEANGLE         "cone angle"
#define RF3_LIGHT_HOTSPOTANGLE      "hotspot angle"
#define RF3_LIGHT_COLOR             "color"
#define RF3_LIGHT_INTENSITY         "intensity"
#define RF3_LIGHT_ATTENUATIONSTART  "attenuation start"
#define RF3_LIGHT_ATTENUATIONEND    "attenuation end"

/**
*   \ingroup rf3write
*
*   This class is responsible for exporting RwCommLight objects. In its Export() method the class
*   exports all the light properties as RwRf3Param objects and attaches them to the local root m_pNode object. At
*   the end of the method the m_pNode is attached to its XML parent transform node.
*
*   \see RwCommLight
*   \see RwRf3Param
*/
class RF3_API RwRf3ExpLight : public RwRf3ExpObject
{
public:
	RwRf3ExpLight();
	virtual ~RwRf3ExpLight();

    virtual RwCommError     Export( RwCommObject* pObj,
                                RwCommObject* pParent,
                                RwCommScene* pScene,
                                const RwCommExportOptions* pExportOptions );
};


/**
*   \ingroup rf3read
*
*   This class builds an RwMILight from an XML node. 
*
*   \see RwMICamera
*/
class RF3_API RwRf3LightCreator : public RwRf3ObjCreator
{
public:
    RwRf3LightCreator() {}
    virtual ~RwRf3LightCreator() {}

	virtual RwCommObject* Create(const RwCommCreationData* pCreationData,
								const RwCommExportOptions* pExportOptions) const;

	virtual void Attach(RwCommObject* pObject, RwCommObject* pParent, RwCommScene* pScene,
                        const RwCommCreationData* pCreationData) const;

    virtual void Init(	RwCommObject* pObject,
						const RwCommCreationData* pCreationData,
						const RwCommExportOptions* pExportOptions) const;
};

#endif // !defined(AFX_RwRf3EXPLIGHT_H__00B31588_05F6_4628_B9E2_08574E190634__INCLUDED_)