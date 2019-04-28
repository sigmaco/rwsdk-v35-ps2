#if !defined(RW_MAYA_CREATION_DATA_H)
#define RW_MAYA_CREATION_DATA_H

#include "stdafx.h"

/**
*   \ingroup mayabuilder 
*
*   This class is the generic creation data base class used to pass information about Maya scene nodes
*   to RwMayaObjCreator classes. The GetObject function will always return a valid MObject. The
*   GetDagPath function will only return a valid dag path for dependency nodes that are also members
*   of the DAG.
*/
class RWMAYA_API RwMayaCreationData : public RwCommCreationData
{
public:
	RwMayaCreationData(MObject object, MDagPath dagPath);
	virtual ~RwMayaCreationData();

    /**
    *   \return The Maya MObject of the object
    */
	MObject     GetObject() const { return m_MObject; }
	
    /**
    *   \return The Maya MDagPath of the object
    */
    MDagPath    GetDagPath() const { return m_DagPath; }

protected:

	MObject     m_MObject;
    MDagPath    m_DagPath;
};


#endif // !defined(RW_MAYA_CREATION_DATA_H)
