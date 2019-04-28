// RwMIMesh.h: interface for the RwMIMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RwMIMesh_H__8FADC0A8_CC61_4722_B64B_6D39CC9B7D2A__INCLUDED_)
#define AFX_RwMIMesh_H__8FADC0A8_CC61_4722_B64B_6D39CC9B7D2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwMI.h"
#include "RwCommMesh.h"
#include "RwMIGeometry.h"
#include "RwMIAttributes.h"

/**
*   \ingroup miscenegraph
*
*   This is the MI implementation of RwCommMesh.. The class caches the mesh's triangulation data.
*
*/
class RWMI_API RwMIMesh : public RwCommMesh, public RwMIAttributes, public RwMIGeometry 
{
public:
	RwMIMesh();
	virtual ~RwMIMesh();

    // Comparison operator used for instancing
    virtual bool operator==(const RwCommGeometry& geom) const;

    virtual RwCommError GetUserData(RwCommUserDataCollection &userDataColl);
    virtual RwCommError GetVertexUserData(RwCommUserDataCollection &userDataColl,
                             const vector<RwCommVertexId>& vertIds);
    virtual RwCommError GetPrimitiveUserData(RwCommUserDataCollection &userDataColl);

    // Get attribute methods.
    virtual RwCommError GetAttribute(const string& name, int &value, int index = 0);
    virtual RwCommError GetAttribute(const string& name, float &value, int index = 0);
    virtual RwCommError GetAttribute(const string& name, string &value, int index = 0);
    virtual RwCommError GetAttributeNames(vector<string>& vAttributeNames, vector<AttributeType> &vAttributeTypes);

    // Shape methods
    virtual RwCommError GetVisibility(bool& bIsVisible) const;

   // Bounding box
    virtual RwCommError GetWorldSpaceBoundingBox(RwCommBoundingBox&) const;

    // Get vertex by id
    virtual RwCommError GetVertex(const RwCommVertexId& id, RwCommVertex& vertex, bool bWorldSpace = false);

    // Primitive methods
    virtual RwCommError GetNumPrimitives(int& nPrims) const;
    virtual RwCommError ResetPrimIterator();
    virtual RwCommError AdvancePrimIterator();
    virtual RwCommError GetPrimVertexCount(int& nVertices);
    virtual RwCommError GetPrimVertex(int index, RwCommVertex& vertex, RwCommVertexId& id, bool bWorldSpace = false);
    virtual RwCommError GetPrimIndex(int& nIndex);
    virtual RwCommError GetPrimShader(RwCommShader*& pShader);
    virtual RwCommError GetPrimNormal(RwCommVec3& normal, bool bWorldSpace = false);
    virtual RwCommError GetPrimTriangleCount(int& nTriangles);
    virtual RwCommError GetPrimTriangulation(vector< int >& triangleIndices);
    virtual RwCommError GetPrimEdges(vector<RwCommEdge>& edges);
    virtual RwCommError GetDefaultColor(RwCommVec3 &color) const;
    virtual RwCommError GetToonInks(vector<RwCommToonInk>& toonInks);

    // RwMIMesh methods

    RwCommError     AddTriangulation(const vector<int>& vTriangulation);
    RwCommError     AddPrimEdges(const vector<RwCommEdge> &vEdges);
    RwCommError     SetToonInks(const vector<RwCommToonInk>& vToonInks);

protected:

    RwCommError ApplyModifiers(RwCommVertex& vertex, int index) const;
    virtual RwCommError     OnSetFrame(RwCommReal frame);

protected:

    vector< vector<int> >           m_vTriangulation;
    vector< vector<RwCommEdge> >    m_vEdges;
    vector<RwCommToonInk>           m_vToonInks;
};

#endif // !defined(AFX_RwMIMesh_H__8FADC0A8_CC61_4722_B64B_6D39CC9B7D2A__INCLUDED_)
