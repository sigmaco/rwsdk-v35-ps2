// RwMICamera.h: interface for the RwMICamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWMICAMERA_H__6C59EB85_4CAE_46AB_9902_60390E36A2F0__INCLUDED_)
#define AFX_RWMICAMERA_H__6C59EB85_4CAE_46AB_9902_60390E36A2F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwMI.h"
#include "RwCommCamera.h"
#include "RwMIAttributes.h"
#include "RwMIAnimProp.h"

/**
*   \ingroup miscenegraph
*
*   This is the MI implementation of RwCommCamera.
*
*   \see RwCommCamera
*/
class RWMI_API RwMICamera : public RwCommCamera, public RwMIAttributes 
{
public:
	RwMICamera();
	virtual ~RwMICamera();

    // User Data
    virtual RwCommError     GetUserData(RwCommUserDataCollection &userDataColl);

    // Get attribute methods.
    virtual RwCommError GetAttribute(const string& name, int &value, int index = 0);
    virtual RwCommError GetAttribute(const string& name, float &value, int index = 0);
    virtual RwCommError GetAttribute(const string& name, string &value, int index = 0);
    virtual RwCommError GetAttributeNames(vector<string>& vAttributeNames, vector<AttributeType> &vAttributeTypes);

    // RwCommShape methods
    virtual RwCommError     GetVisibility(bool& bIsVisible) const;

    // RwCommCamera methods
    virtual RwCommError GetProjectionType(ProjectionType &type) const;
    virtual RwCommError GetNear(RwCommReal &fNear) const;
    virtual RwCommError GetFar(RwCommReal &fFar) const;
    virtual RwCommError GetFovX(RwCommReal &fov) const;
    virtual RwCommError GetFovY(RwCommReal &fov) const;
    virtual RwCommError GetOrthoWidth(RwCommReal &width) const;
    virtual RwCommError GetOrthoHeight(RwCommReal &height) const;
    virtual RwCommError GetTargetDistance(RwCommReal &distance) const;
    virtual RwCommError GetFogDistance(RwCommReal &distance) const;

    // RwMICamera methods

    RwCommError SetProjectionType(ProjectionType projection);

    RwCommError SetVisibility(RwCommReal frame, bool visibility);
    RwCommError SetVisibility(vector< RwMIAnimPropKey<bool> > &vKeys);

    RwCommError SetNear(RwCommReal frame, RwCommReal fNear);
    RwCommError SetNear(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);

    RwCommError SetFar(RwCommReal frame, RwCommReal fFar);
    RwCommError SetFar(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);

    RwCommError SetFovX(RwCommReal frame, RwCommReal fov);
    RwCommError SetFovX(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);

    RwCommError SetFovY(RwCommReal frame, RwCommReal fov);
    RwCommError SetFovY(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);

    RwCommError SetOrthoWidth(RwCommReal frame, RwCommReal width);
    RwCommError SetOrthoWidth(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);
    
    RwCommError SetOrthoHeight(RwCommReal frame, RwCommReal height);
    RwCommError SetOrthoHeight(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);

    RwCommError SetTargetDistance(RwCommReal frame, RwCommReal distance);
    RwCommError SetTargetDistance(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);

    RwCommError SetFogDistance(RwCommReal frame, RwCommReal distance);
    RwCommError SetFogDistance(vector< RwMIAnimPropKey<RwCommReal> > &vKeys);


protected:

    ProjectionType  m_projection;

    RwMIAnimProp<bool>          m_visibility;
    RwMIAnimProp<RwCommReal>    m_far;
    RwMIAnimProp<RwCommReal>    m_near;
    RwMIAnimProp<RwCommReal>    m_fovX;
    RwMIAnimProp<RwCommReal>    m_fovY;
    RwMIAnimProp<RwCommReal>    m_width;
    RwMIAnimProp<RwCommReal>    m_height;
    RwMIAnimProp<RwCommReal>    m_targetDistance;
    RwMIAnimProp<RwCommReal>    m_fogDistance;
};

#endif // !defined(AFX_RWMICAMERA_H__6C59EB85_4CAE_46AB_9902_60390E36A2F0__INCLUDED_)
