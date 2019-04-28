// RwCommObject.h: interface for the RwCommObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWCOMMOBJECT_H__82316C62_7428_4337_B4CE_4971EE7B2D47__INCLUDED_)
#define AFX_RWCOMMOBJECT_H__82316C62_7428_4337_B4CE_4971EE7B2D47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwComm.h"
#include "RwCommError.h"
#include "RwCommPrimitives.h"

/**
*  \ingroup commlowlevelobjects
*   This is a type associated with all objects in the scene graph. For each class that derives
*   from RwCommObject there is a constant defining its type. For example for RwCommCamera object,
*   there is a global constant:
*   
*   \code const RwCommType RwCommCameraType("RwCommCamera"); \endcode
*
*   You can use these types to get all objects of the same type from the scene graph using:
*   RwCommScene::GetAllObjectsOfType(). Type's hierarchy is a vector types of all objects
*   current object derives from. To get object's type or object's type hierarchy, use
*   RwCommObject::GetType() and RwCommObject::GetTypeHierarchy().
*/
typedef string RwCommType;

class RwCommExportObject;
class RwCommScene;
class RwCommUserData;
class RwCommUserDataCollection;

const RwCommType RwCommObjectType("RwCommObject");

/**
*  \ingroup commscenegraph  
*
*   This is the base class for most of the RwComm objects. It exposes a set of generic high level methods.
*   The class uses a reference count mechanism for managing the destruction of objects. All RwCommObject
*   instances, including objects that were allocated in user plugins, are automatically deallocated using
*   the Release() method. Users should never explicitly delete their derived RwCommObject classes.
*/
class RWCOMM_API RwCommObject  
{
    friend class RwCommScene; 

public:
	RwCommObject();
	virtual ~RwCommObject();

     /**
     *  Increments an object's reference count.
     *  \returns how many objects are referencing this object.
     *
     */
	int		            AddRef();

    /**
     *  Decrements an object's reference count and deletes the object if the ref count is zero
     *  \returns how many objects are referencing this object after the call.
     *
     */
	int		            Release();

    /**
     *  Sets the object's name.
     *  \param name string to be set.
     *
     */
	void	            SetName(const string& name) { m_name = name; }

    /**
     *  \returns the object's name.
     *
     */
	const string&	    GetName() const { return m_name; }

    /**
     *  \returns the object's type.
     *
     */
	virtual RwCommType	GetType() const { return RwCommObjectType; }

    /**
     *  Extracts a list of the class's type hierarchy. This list contains the object's type
     *  followed by its super class's type, and so on, recursively until the base class type.
     *  You can use this method when performing certain tasks on a family of scene graph
     *  elements which share the same super classes.    
     *
     *  \param vTypeVec is filled with the type hierarchy.
     */
    virtual void        GetTypeHierarchy(vector<RwCommType>& vTypeVec) const {}

    /**
     *  Extracts user data on this object.
     * 
     *  \param userDataColl to be filled in with the user data.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     *
     *  \note If this is a geometry, this will only return user data attached to the object as
     *   a whole. To extract vertex and primitive user data collections use the
     *   GetVertexUserData() and GetPrimitiveUserData() methods of the RwCommGeometry class. 
     *
     *  \see RwCommUserData class
     *  \see RwCommUserDataCollection class.
     */
    virtual RwCommError GetUserData(RwCommUserDataCollection &userDataColl) { return RWCOMM_NOT_IMPLEMENTED; }

    /**
     *  \param pScene to be set to the object.
     *
     *  \note You shouldn't need to call this method explicitly. It will be called by RwCommScene::AddObject.
     *
     *  \see RwCommScene class.
     */
    void                SetScene(RwCommScene* pScene) { m_pScene = pScene; }

    /**
     *  \returns the scene object this object is attached to.
     *
     */
    RwCommScene         *GetScene() const { return m_pScene; }

    // object ordering
    unsigned int        Hash() const { return m_hash; }
    

protected:
    
    /** Deletes the object. Called from Release().*/
    virtual void        Destroy();

private:

    void            SetHash(unsigned int hash) { m_hash = hash; }

	int	            m_refCount;
	string      	m_name;
    RwCommScene*    m_pScene;
    unsigned int    m_hash;
};

#endif // !defined(AFX_RWCOMMOBJECT_H__82316C62_7428_4337_B4CE_4971EE7B2D47__INCLUDED_)
