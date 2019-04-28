// RwExpFrame.h: interface for the RwExpFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPFRAME_H__919C836C_A390_4353_B5DE_0C230E430C62__INCLUDED_)
#define AFX_RWEXPFRAME_H__919C836C_A390_4353_B5DE_0C230E430C62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwExp.h"
#include "RwCommExportObject.h"

#include "RwExpTransform.h"

class RwCommTransform;

/**
*  \ingroup exportobject
*
*   This class is responsible for exporting an RwFrame object from an RwCommTransform object.
*   In its main Export() method the class extracts the RwCommTransform's object space matrix, removes
*   its scaling and builds an RwFrame object from the matrix.
*
*   \see RwCommTransform
*   \see RwFrame
*/
class RWEXP_API RwExpFrame  : public RwExpTransform
{
public:
	RwExpFrame();
	virtual ~RwExpFrame();

    virtual RwCommError Export( RwCommObject* pObj,
                            RwCommObject* pParent,
                            RwCommScene* pScene,
                            const RwCommExportOptions* pExportOptions );

    virtual RwCommError     AddUserData(RwCommUserDataCollection &userDataColl);
    static RwCommError      AddUserDataToRwFrame(RwFrame* pFrame,
                                RwCommUserDataCollection &userDataColl);

    /**
     *  Get the generated RwFrame object.
     *  \return the frame generated
     */
    RwFrame*    GetFrame() { return m_pFrame; }

protected:

    virtual RwCommError Attach(RwFrame* pParentFrame, RwFrame* pFrame);

protected:

    RwFrame*    m_pFrame;
};

#endif // !defined(AFX_RWEXPFRAME_H__919C836C_A390_4353_B5DE_0C230E430C62__INCLUDED_)
