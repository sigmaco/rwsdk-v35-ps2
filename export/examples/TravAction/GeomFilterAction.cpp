// GeomFilterAction.cpp: implementation of the GeomFilterAction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GeomFilterAction.h"

#include "RwCommMesh.h"
#include "RwCommMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GeomFilterAction::GeomFilterAction() : m_filterPattern("XXXX_PATTERN")
{

}

GeomFilterAction::~GeomFilterAction()
{

}

void
GeomFilterAction::Destroy()
{
    delete this;
}

bool 
GeomFilterAction::Init(const RwCommExportOptions *pExportOptions)
{
    return true;   
}

RwCommError 
GeomFilterAction::Apply(vector<RwExpTraverseObject> &vTraverseList, const RwCommExportOptions *pExportOptions)
{
    int nObjectsToOptimize = 0;
    set<RwCommObject*> sObjectsToFilter;
    vector<RwExpTraverseObject> vFilteredList;

    PRINT_MESSAGE("----------------------------------------------------------------", "", 2);
    PRINT_MESSAGE("Applying Geometry Filtering!!!", "", 2);
    PRINT_MESSAGE("----------------------------------------------------------------", "", 2);

    // First, get all the meshes in the traverse list and check if their name
    // includes our filter pattern
    for (int i = 0; i < vTraverseList.size(); i++)
    {
        RwCommMesh *pMesh = dynamic_cast<RwCommMesh*>(vTraverseList[i].GetObject());
        if (pMesh)
        {
            const string &meshName = pMesh->GetName();
            if (meshName.find(m_filterPattern) != string::npos)
            {
                sObjectsToFilter.insert(pMesh);
                nObjectsToOptimize++;
            }
        }
    }
    
    while (nObjectsToOptimize > 0)
    {
        set<RwCommObject*>::iterator iter;

        // Copy vTraverseList to vFilteredList and clear vTraverseList
        vFilteredList.clear();
        vFilteredList = vTraverseList;
        vTraverseList.clear();

        // Remove all objects, contained in the sObjectsToFilter set from the traverse list
        for (int i = 0; i < vFilteredList.size(); i++)
        {
            if ((iter = sObjectsToFilter.find(vFilteredList[i].GetObject())) == sObjectsToFilter.end())
            {
                vTraverseList.push_back(vFilteredList[i]);
            }
        }

        // Now, take care of all objects that are children of the objects we've just removed.
        // Add these objects to the sObjectsToFilter set, so they can be filtered in the next iteration
        nObjectsToOptimize = 0;
        for (i = 0; i < vTraverseList.size(); i++)
        {
            if ((iter = sObjectsToFilter.find(vTraverseList[i].GetParent())) != sObjectsToFilter.end())
            {
                sObjectsToFilter.insert(vTraverseList[i].GetObject());
                nObjectsToOptimize++;
            }
        }


    }

    return RWCOMM_SUCCESS;
}
