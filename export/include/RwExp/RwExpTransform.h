// RwExpTransform.h: interface for the RwExpTransform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPTRANSFORM_H__1E180456_6F69_4DE0_B28D_5491889D1D7D__INCLUDED_)
#define AFX_RWEXPTRANSFORM_H__1E180456_6F69_4DE0_B28D_5491889D1D7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwExp.h"
#include "RwCommExportObject.h"

class RwCommTransform;
class RwExpGlobalData;

/**
*  \ingroup exportobject
*
*   This class is a base class for RwCommTransform exporters. It is responsible for calculating the object space
*   transformation matrix from the RwCommTransform. Part of these calculations include removing the scaling from
*   the matrix and flipping matrices for cameras and lights.
*
*
*   \see RwCommTransform
*   \see RwFrame
*/
class RWEXP_API RwExpTransform  : public RwCommExportObject
{
public:
	RwExpTransform();
	virtual ~RwExpTransform();

    virtual RwCommError GetNodeIndex(int &nodeIndex);
    virtual RwCommError SetNodeIndex(int nodeIndex);

    /** Get the propogated scale value of the matrix hierarchy. Since scaling is removed from all the matrices
        the removed scale value is propogated down the matrix hierarchy.*/
    virtual RwV3d       GetPropogatedScale() const { return m_vScale; }

    /** Get the calculated RwMatrix.*/
    virtual RwMatrix    *GetMatrix() const { return m_pMatrix; }

    virtual bool        GetFlipAxis() const { return m_bFlipAxis; }
    virtual void        FlipMatrixAxis(RwCommTransform *pTransform, RwCommTransform *pParent,
                            RwMatrix *matrix, bool bReFlip);
    virtual RwV3d       CalcTransformMatrix(RwCommTransform *pTransform, RwMatrix *matrix,
                            RwCommReal scale, bool bSetRootIdent);
    virtual void        ApplyAxisSystem(RwMatrix *pMatrix, RwCommMatrix *pAxisMatrix);
    virtual bool        CalcHierarchyTransformMatrix(RwCommTransform *pTransform,
                            RwCommTransform *pParent, RwMatrix *pMatrix, RwCommReal scale,
                            bool bSetRootIdent = false, bool bApplyFlips = true);

protected:

    RwMatrix*   m_pMatrix;
    int         m_nodeIndex;
    RwV3d       m_vScale;
    bool        m_bFlipAxis;
};

#endif // !defined(AFX_RWEXPTRANSFORM_H__1E180456_6F69_4DE0_B28D_5491889D1D7D__INCLUDED_)
