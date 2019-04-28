// RwExpExporter.h: interface for the RwExpExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPEXPORTER_H__98CCDE62_5B87_4ED9_B355_D1A92DCBA97A__INCLUDED_)
#define AFX_RWEXPEXPORTER_H__98CCDE62_5B87_4ED9_B355_D1A92DCBA97A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786)
#pragma warning(disable : 4251)
#include <map>

#include "RwCommExportObject.h"
#include "RwCommExportObjCreator.h"

class RwCommTransform;

/**
*  \ingroup commexporter  
*
*   This is an abstract base class responsible for exporting the scene graph. Since
*   traversing the scene graph can be performed in many ways, the RwCommExporter
*   class leaves this task to its specific classes derived from it. The main entry point for
*   the export process is the Export() method. It is responsible for traversing 
*   and exporting the various elements. To assist with this task, the RwCommExporter
*   object holds a mapping table, containing a map from a RwCommType to an 
*   RwCommExportObjCreator instance. For each RwComm object the traverse process
*   encounters, it creates an instance of a RwCommExportObject. This object
*   is an export handler object of that specific RwCommObjectType, and is
*   responsible for executing the actual export action on that specific RwComm
*   object. The ExportObject() method is the one that creates an RwCommExportObject
*   instance, and executes its RwCommExportObject::Export() operation.
*
*   Classes, deriving from RwCommExporter, perform their own specific traverse and
*   call this method for each node they encounter. After the RwCommExportObject has
*   been created, you can always re-extract it from the exporter using the
*   GetExportObject() method. Classes, deriving from RwCommExporter, are usually
*   constructed with their own default set of RwCommExportObjects mapped to specific
*   RwComm objects. You can always override this behavior, by registering your own
*   set of custom RwCommExportObjects. For this, in your RwComm plugin you can
*   include the following function:
* 
*   \code void RegisterCustomExporterObjects(RwCommExporter* pExporter); \endcode
* 
*   Within this function you can register your own custom exporter objects.It
*   will be picked up by AttachPlugins() method every time an RwCommExporter object
*   is instanced.
*
*   To register your own exporter object use the RegisterExportObject() macro,
*   supplying the RwCommObjectType and the name of your RwCommExportObject class.
*/
class RWCOMM_API RwCommExporter : public RwCommObject
{
public:
    RwCommExporter();
    virtual ~RwCommExporter();

    /**
    *   Export the scene. This is the main entry point for the exporter. The method
    *   should traverse the scene graph, calling ExportObject() on each RwCommObject
    *   that it encounters.
    *   
    */
    virtual RwCommError         Export(RwCommScene* pScene,
                                    const vector<RwCommTransform*>& vSelection,
                                    string sPath, string sGUID) = 0;
    /**
    *   The ExportObject() method creates an RwCommExportObject instance, for a specific
    *   RwComm object and executes its RwCommExportObject::Export() operation. When given
    *   an RwCommObject, this method creates the appropriate RwCommExportObject by
    *   searching the table, executes its RwCommExportObject::Export() method and returns
    *   the instance of the RwCommExportObject it created.
    *   
    *   \param pObject being exported
    *   \param pParent of the object being exported in the scene graph.
    *   \param pScene RwCommScene object to be passed to the
    *                 RwCommExportObject::Export() method.
    *
    *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError         ExportObject(RwCommObject* pObject, RwCommObject* pParent,
                                                                     RwCommScene* pScene);

    /**
    *   Retrieve the export object for specified RwCommObject.
    *
    *   \param pObject for which to find an export object
    *
    *   \return RwCommExportObject* or NULL if not found. 
    */
    RwCommExportObject          *GetExportObject(const RwCommObject* pObject) const;

    /**
    *   \return An RwCommExportOptions used in the export process.
    */
    RwCommExportOptions         *GetExportOptions() const {return m_pExportOptions;}
    /** \param  pExportOptions RwCommExportOptions object to be set for the exporter. */
    void                        SetExportOptions(RwCommExportOptions* pExportOptions)
                                                   {m_pExportOptions = pExportOptions;}

    /**
    *   Register an RwCommExportObjCreator for a specified object type. Users can use
    *   the RegisterExportObject() macro instead.
    */
    void                        Register(const RwCommType sType,
                                    RwCommExportObjCreator* pExportObject);

    /** Check if there is an export object registered for the specified type. */
    bool                        IsRegistered(const RwCommType sType) const;

    /** Clear the list of all export object the exporter is holding. */
    void                        ClearAllExportObjects();

protected:
    /** Iterate Open Export plugins and call:
    *   
    *   \code void RegisterCustomExporterObjects(RwCommExporter* pExporter); \endcode
    *   
    *   if present.
    */
    void                        AttachPlugins();

    /** Called before export of each object The return value should be true for export
    *   process to proceed.
    *
    *   \param pObject that is being exported.
    *
    *   \return true if export process should proceed. Default is true.
    */
    virtual bool OnExportObject(RwCommObject* pObject) { return true; }

protected:

    RwCommExportOptions* m_pExportOptions;

private:
    map <RwCommType, RwCommExportObjCreator* >      m_mExporters;
    map <const RwCommObject*, RwCommExportObject*>  m_mExpObjects;
};

/**
*  \ingroup commexporter  
*
*   This macro is used to register an RwCommExportObjCreator for a specified object type.
*   For example, for registering your own Mesh exporter:
*   \code
*   pDffExporter->RegisterExportObject(RwCommMesh, MyMeshExporter);
*   \endcode
*/
#define RegisterExportObject(TYPE, CREATOR) Register(TYPE, new RwCommExportObjCreatorImpl<CREATOR>) 

#endif // !defined(AFX_RWEXPEXPORTER_H__98CCDE62_5B87_4ED9_B355_D1A92DCBA97A__INCLUDED_)
