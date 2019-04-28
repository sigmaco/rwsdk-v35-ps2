// RwCommAssetAutoCreator.h: interface for the RwCommAssetAutoCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWCOMMASSETAUTOCREATOR_H__C713DBD7_BACF_4A42_A255_656C6A4D3D8D__INCLUDED_)
#define AFX_RWCOMMASSETAUTOCREATOR_H__C713DBD7_BACF_4A42_A255_656C6A4D3D8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwCommAsset.h"

/**
*  \ingroup commscenegraph  
*
*   This is a singleton class responsible for auto-creating assets. It will
*   analyze the scene and create an asset for each logical entity in the scene.
*
*   \see RwCommAsset
*/
class RWCOMM_API RwCommAssetAutoCreator  
{
public:
	RwCommAssetAutoCreator();
	virtual ~RwCommAssetAutoCreator();
    
    /**
    *   \param pInstance of the derived class to registe. RwCommAssetAutoCreator
    *          is a singleton.
    */
    static void RegisterInstance(RwCommAssetAutoCreator *pInstance);
    /**
    *   \return static instance of the class. RwCommAssetAutoCreator is a
    *           singleton.
    */
    static RwCommAssetAutoCreator *TheInstance();

    /**
    *   Analyze the scene and call AutoCreateAsset for each logical entity in the
    *   scene. If bSelectionOnly is set to true then only the current selection will
    *   be analyzed.
    *
    *   \param bSelectionOnly Boolean indicating whether the whole scene or just the current selection should be analyzed
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError AutoCreateAssets(bool bSelectionOnly = false) = 0;
    /**
    *   \return Analyze current selection in the scene, create an asset for selected
    *           objects and set-up appropriate options
    */
    virtual RwCommAsset *AutoCreateAsset() = 0;

private:
    static RwCommAssetAutoCreator *m_pTheInstance;
};

#endif // !defined(AFX_RWCOMMASSETAUTOCREATOR_H__C713DBD7_BACF_4A42_A255_656C6A4D3D8D__INCLUDED_)
