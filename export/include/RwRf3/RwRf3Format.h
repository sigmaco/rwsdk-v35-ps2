// RwRf3Format.h: interface for the RwRf3Format class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWRF3FORMAT_H__38C8BB74_E499_4DE5_BBE0_349FE6DB2932__INCLUDED_)
#define AFX_RWRF3FORMAT_H__38C8BB74_E499_4DE5_BBE0_349FE6DB2932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwXMLDOM.h"
#include "RwRf3Param.h"


class RwRf3Format  
{
public:
    RwRf3Format( );
	RwRf3Format( RwXMLNode* pNode );
    
	virtual ~RwRf3Format();

    RwXMLNode* GetNode( void );

    RwRf3Param* GetParam( const string& name );
    RwCommError AddParam( const string& name, RwRf3ParamDataType type, int dimX = 1, int dimY = 1 );

    RwCommError GenerateFormatString( string& buffer );
    RwCommError ParseFormatString( const string& buffer );

    void ClearParamContents( void );

private:

    RwXMLNode* mFormatNode;
};

#endif // !defined(AFX_RWRF3FORMAT_H__38C8BB74_E499_4DE5_BBE0_349FE6DB2932__INCLUDED_)
