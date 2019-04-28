// RwRf3VertexList.h: interface for the RwRf3VertexList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RwRf3VERTEXLIST_H__C228FE20_1DB9_441E_8B34_302222B272AB__INCLUDED_)
#define AFX_RwRf3VERTEXLIST_H__C228FE20_1DB9_441E_8B34_302222B272AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwXMLDOM.h"

#include "RwCommVertex.h"
#include "RwRf3Format.h"

// Vertex lists
#define RWRF3_VXLIST_TYPE     "vertexlist"
#define RWRF3_VXLIST_VERTEX   "v"
#define RWRF3_VXLIST_COUNT    "count"

//Vertex format
#define RWRF3_FVF         "vertexformat"
#define RWRF3_FVF_POS     "position"
#define RWRF3_FVF_NORMAL  "normal"
#define RWRF3_FVF_COLOR   "color"
#define RWRF3_FVF_UV      "uv"
#define RWRF3_FVF_ILLUM   "illumination"

/**
*   \ingroup rf3helper 
*
*   This class is an rf3 vertex list node. It contains helper methods for setting and getting the vertex data.
*   Since it derives from the RwXMLNode class, it can be attached to the XML tree like any XML node.
*/
class RF3_API RwRf3VertexList  : public RwXMLNode
{
public:
	RwRf3VertexList();
    RwRf3VertexList( RwXMLNode* parentNode );
    RwRf3VertexList( const string& name, RwXMLNode* parentNode = NULL );
	virtual ~RwRf3VertexList();

    RwCommError SetData(const vector<RwCommVertex> &vVertexList, unsigned int vertexData, int nUvs);
    RwCommError GetData(vector<RwCommVertex> &vVertexList);

    virtual RwCommError AddUserData( RwCommUserDataCollection& userDataColl );
    virtual RwCommError GetUserData( RwCommUserDataCollection& userDataColl );

protected:

    RwRf3Format    *CreateVertexFormatNode(unsigned int vertexData, int nUvs);
};

#endif // !defined(AFX_RwRf3VERTEXLIST_H__C228FE20_1DB9_441E_8B34_302222B272AB__INCLUDED_)
