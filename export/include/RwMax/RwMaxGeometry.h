
#ifndef RWMAXGEOMETRY_H
#define RWMAXGEOMETRY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "RwMax.h"
#include "RwCommObjCreator.h"
#include "RwMaxCustomAttributes.h"
#include "RwMaxTraverseModifiers.h"
#include "RwMaxTraverseControllers.h"

class RwMaxScene;

// =============================================================================
// ---------------------------  RwMaxGeometry CLASS  ---------------------------
// =============================================================================

/**
*   \ingroup maxhelper 
*
*   This is a intermediate base class for all Max geometries. The class includes several
*   general geometry related utilities, that deriving classes can use. 
*
*/
class RWMAX_API RwMaxGeometry : public RwMaxCustomAttributes
{
public:
    RwMaxGeometry();
    virtual ~RwMaxGeometry();

    static Matrix3      GetObjOffsetMatrix(INode *iNode);
    RwCommVec3          GetWireColor(INode *iNode) const;
    RwCommBoundingBox   RwMaxBox3ToRwExpBoundingBox(const Box3& b) const;
    virtual RwCommError GetDeformBBoxInWorldSpace(INode *iNode, RwCommReal nFrame,
                                  RwCommScene *pScene, RwCommBoundingBox &bbox) const;

    template <class T> RwCommError GetAttribute(INode *iNode, string name, T &value,
                                                        RwCommReal nFrame, int index);

    virtual RwCommError GetCustomAttributeNames(INode *iNode, vector<string> &vAttributeNames, 
        vector<RwCommAnimatable::AttributeType> &vAttributeTypes);

protected:

    void CalcMatrices(INode* pINode, RwCommReal nFrame);

protected:

    Matrix3         m_offsetMat;
    Matrix3         m_worldMat;
    Matrix3         m_normalMat;
    bool            m_bOffsetIsMirrored;
    
};

// =============================================================================
// -----------------------  RwMaxGeometryCreator CLASS  ------------------------
// =============================================================================

/**
*  \ingroup maxbuilder 
*
*  This is an intermediate base class for creating RwCommGeometry objects.
*
*   \see RwCommGeometry
*/
class RWMAX_API RwMaxGeometryCreator : public RwCommObjCreator,
                                       public RwMaxTraverseModifiers,
                                       public RwMaxTraverseControllers 
{
public:
    RwMaxGeometryCreator();
    virtual ~RwMaxGeometryCreator();

    virtual void Init(RwCommObject* pObject,
                      const RwCommCreationData* pCreationData,
                      const RwCommExportOptions* pExportOptions) const;

	virtual void Attach(RwCommObject* pObject, RwCommObject* pParent, RwCommScene* pScene,
                        const RwCommCreationData* pCreationData) const;

};

#endif // RWMAXGEOMETRY_H