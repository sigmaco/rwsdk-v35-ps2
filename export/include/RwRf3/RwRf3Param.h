// RwRf3Param.h: interface for the RwRf3Param class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RwRf3PARAM_H__77812B03_F864_4ECF_8034_70FE3708F871__INCLUDED_)
#define AFX_RwRf3PARAM_H__77812B03_F864_4ECF_8034_70FE3708F871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwXMLDOM.h"

#include "RwCommObject.h"
#include "RwRf3Stream.h"

#include "RwMIAnimProp.h"

#define RWRF3_PARAM_TAG         "param"
#define RWRF3_PARAM_NAME        "name"
#define RWRF3_PARAM_TYPE        "type"
#define RWRF3_PARAM_DATA        "data"

// Data types
#define RWRF3_TYPE_REAL         "real"
#define RWRF3_TYPE_BOOL         "bool"
#define RWRF3_TYPE_INT          "int"
#define RWRF3_TYPE_UINT         "uint"
#define RWRF3_TYPE_STRING       "string"

#define RWRF3_DATA_REAL         "real"
#define RWRF3_DATA_INT          "int"
#define RWRF3_DATA_UINT         "uint"
#define RWRF3_DATA_BOOL         "bool"
#define RWRF3_DATA_STRING       "string"

#define RWRF3_ANIMATION_TYPE    "animation"
#define RWRF3_KEY_TYPE          "key"
#define RWRF3_ANIMATION_CHANNEL "channels"

enum RwRf3ParamDataType
{
    RwRf3NONTYPE = 0,
    RwRf3REAL32,            // 32 bit floating point real
    RwRf3UINT32,            // 32 bit unsigned integer
    RwRf3INT32,             // 32 bit signed integer
    RwRf3STRING,            // a string
    RwRf3BOOL               // a booean value
};

typedef RwMIAnimPropKey<RwCommReal> RwRf3RealKey;
typedef RwMIAnimPropKey<RwCommVec2> RwRf3Vec2Key;
typedef RwMIAnimPropKey<RwCommVec3> RwRf3Vec3Key;
typedef RwMIAnimPropKey<RwCommVec4> RwRf3Vec4Key;
typedef RwMIAnimPropKey< vector<RwCommReal> > RwRf3VecKey;
typedef RwMIAnimPropKey<RwCommMatrix> RwRf3MatrixKey;
typedef RwMIAnimPropKey<string> RwRf3StringKey;

/**
*   \ingroup rf3helper 
*
*   This class is an rf3 param node. It contains helper methods for querying the param's name, type and value.
*   Since it derives from the RwXMLNode class, it can be attached to the XML tree like any XML node.
*   Users can set the param's value using the SetData() methods. In addition, the value can be animated using
*   the SetKeys() methods. All data can be extracted using the GetData() and GetKeys() methods.
*/
class RF3_API RwRf3Param : public RwXMLNode
{
public:
	RwRf3Param();
    RwRf3Param( RwXMLNode* parentNode );
    RwRf3Param( const string& name, RwXMLNode* parentNode = NULL );
	virtual ~RwRf3Param();

    RwCommError SetDataType( RwRf3ParamDataType type, int dimX = 1, int dimY = 1 );
    RwCommError GetDataType( RwRf3ParamDataType& type, int& dimX, int& dimY );

    /** Returns the X dimension of the array*/
    int GetDimX( void ) { return mDimX; }

    /** Returns the Y dimension of the array*/
    int GetDimY( void ) { return mDimY; }

    void SetParamName( const string& name );
    RwCommError GetParamName( string& name );

    RwCommError SetData( vector<RwCommVec4>& vValues1, vector<RwCommVec4>& vValues2 );
    RwCommError SetData( vector<RwCommVec3>& vValues1, vector<RwCommVec3>& vValues2 );
    RwCommError SetData( vector<RwCommVec2>& vValues1, vector<RwCommVec2>& vValues2 );
    RwCommError SetData( vector<RwCommVec4>& vValues );
    RwCommError SetData( vector<RwCommVec3>& vValues );
    RwCommError SetData( vector<RwCommVec2>& vValues );
    RwCommError SetData( vector<RwCommReal>& vValues );
    RwCommError SetData( vector<string>& vValues );
    RwCommError SetData( vector<int>& vValues );
    RwCommError SetData( vector<unsigned int>& vValues );
    RwCommError SetData( vector<bool>& vValues );
    RwCommError SetData( const RwCommMatrix& M );
    RwCommError SetData( const RwCommVec2& v );
    RwCommError SetData( const RwCommVec3& v );
    RwCommError SetData( const RwCommVec4& v );
    RwCommError SetData( const RwCommReal& v );
    RwCommError SetData( const string& s );
    RwCommError SetData( const bool v );
    RwCommError SetData( const unsigned int v );
    RwCommError SetData( const int v );

    RwCommError GetData( vector<RwCommVec4>& vValues1, vector<RwCommVec4>& vValues2 );
    RwCommError GetData( vector<RwCommVec3>& vValues1, vector<RwCommVec3>& vValues2 );
    RwCommError GetData( vector<RwCommVec2>& vValues1, vector<RwCommVec2>& vValues2 );
    RwCommError GetData( vector<RwCommVec4>& vValues );
    RwCommError GetData( vector<RwCommVec3>& vValues );
    RwCommError GetData( vector<RwCommVec2>& vValues );
    RwCommError GetData( vector<RwCommReal>& vValues );
    RwCommError GetData( vector<string>& vValues );
    RwCommError GetData( vector<int>& vValues );
    RwCommError GetData( vector<bool>& vValues );
    RwCommError GetData( RwCommMatrix& M );
    RwCommError GetData( RwCommVec2& v );
    RwCommError GetData( RwCommVec3& v );
    RwCommError GetData( RwCommVec4& v );
    RwCommError GetData( RwCommReal& v );
    RwCommError GetData( string& s );
    RwCommError GetData( int& i );
    RwCommError GetData( bool& b );

    RwCommError GetKeys( vector<RwRf3MatrixKey>& vKeys );
    RwCommError GetKeys( vector<RwRf3RealKey>& vKeys );
    RwCommError GetKeys( vector<RwRf3Vec2Key>& vKeys );
    RwCommError GetKeys( vector<RwRf3Vec3Key>& vKeys );
    RwCommError GetKeys( vector<RwRf3Vec4Key>& vKeys );
    RwCommError GetKeys( vector<RwRf3VecKey>& vKeys );
    RwCommError GetKeys( vector<RwRf3StringKey>& vKeys );

    RwCommError SetKeys( vector<RwRf3MatrixKey>& vKeys );
    RwCommError SetKeys( vector<RwRf3RealKey>& vKeys );
    RwCommError SetKeys( vector<RwRf3Vec2Key>& vKeys );
    RwCommError SetKeys( vector<RwRf3Vec3Key>& vKeys );
    RwCommError SetKeys( vector<RwRf3Vec4Key>& vKeys );
    RwCommError SetKeys( vector<RwRf3VecKey>& vKeys );
    RwCommError SetKeys( vector<RwRf3StringKey>& vKeys );

    RwCommError Parse( RwRf3IStream& stream );

    bool IsAnimated( void );
    void RemoveAnimation( void );
    RwCommError GetAnimatedChannels( string& sChannels );
    RwCommError SetAnimatedChannels( const string& sChannels );

    RwCommError ParseDataType( void );

protected:
     
    string GenerateDataType( const string& type, int dimX = 1, int dimY = 1);
    RwCommError CheckDirty( void );

private:

    int mDimX, mDimY;
    RwRf3ParamDataType mDataType;
    // do we need to generate type info
    bool mDirtyData;
};



class RF3_API RwRf3ParamList
{
public:
    RwRf3ParamList( RwXMLNode* pNode );
    virtual ~RwRf3ParamList();

    RwRf3Param* GetParam( const string& name );

private:
    RwXMLNodeList* paramList;
};

#endif // !defined(AFX_RwRf3PARAM_H__77812B03_F864_4ECF_8034_70FE3708F871__INCLUDED_)
