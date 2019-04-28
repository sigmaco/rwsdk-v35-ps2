// RwExpRwCamera.h: interface for the RwExpRwCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPRWCAMERA_H__FC9221F9_C8D2_4A8C_B2F2_BDD9BB1EF8BF__INCLUDED_)
#define AFX_RWEXPRWCAMERA_H__FC9221F9_C8D2_4A8C_B2F2_BDD9BB1EF8BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwExp.h"
#include "RwCommExportObject.h"

class RwCommTransform;

/**
*  \ingroup exportobject
*   This is the class responsible for exporting RwCamera objects. It is mapped to the RwCommCamera object in the
*   Dff exporter.
*   In its main Export() method the class creates an RwCamera, sets its near and far planes, sets its projection
*   mode and adjusts its view window. It then, later on, attaches the camera to its appropriate frame.
*   
*   Users wishing to customize this step can override the Attach() method. 
*
*   At the end of the Export() method the camera is added to the clump.
*
*/
class RWEXP_API RwExpRwCamera  : public RwCommExportObject
{
public:
	RwExpRwCamera();
	virtual ~RwExpRwCamera();

    virtual RwCommError     Export( RwCommObject* pObj,
                                RwCommObject* pParent,
                                RwCommScene* pScene,
                                const RwCommExportOptions* pExportOptions );

    virtual RwCommError     AddUserData(RwCommUserDataCollection &userDataColl);

    /** Get the generated RwCamera object. */
    RwCamera                *GetRwCamera() const { return m_pRwCamera; }

protected:

    // Attach camera to frame
    virtual RwCommError Attach(RwCamera* pCamera, RwCommTransform* pTransform);

protected:

    RwCamera*   m_pRwCamera;

};

#endif // !defined(AFX_RWEXPRWCAMERA_H__FC9221F9_C8D2_4A8C_B2F2_BDD9BB1EF8BF__INCLUDED_)
