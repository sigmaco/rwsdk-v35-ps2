#if !defined(RW_MAYA_TRAVERSE_ANIMATIONS_H)
#define RW_MAYA_TRAVERSE_ANIMATIONS_H

#pragma warning(disable : 4786)
#pragma warning(disable : 4251)


class RwMayaBuilder;
class RwCommCreationData;

/**
*  \ingroup mayabuilder 
*
*  This class iterates over all upstream motionPath and animCurve dependendency nodes
*  and and builds RwComm nodes for them. 
*
*/
class RWMAYA_API RwMayaTraverseAnimations  
{
public:
	RwMayaTraverseAnimations() {};
	virtual ~RwMayaTraverseAnimations() {};

	virtual void Traverse(RwCommBuilder &rBuilder, RwCommCreationData *pCreationData, RwCommObject *pParent);
};

/**
*  \ingroup mayabuilder 
*
*  This class iterates over all parents of the supplied dag path and traverses
*  animations from each (using RwMayaTraverseAnimations).
*
*   \see RwMayaTraverseAnimations
*
*/
class RWMAYA_API RwMayaTraverseParentAnimations  
{
public:
    RwMayaTraverseParentAnimations() {};
	virtual ~RwMayaTraverseParentAnimations() {};

	virtual void Traverse(RwCommBuilder &rBuilder, RwCommCreationData *pCreationData, RwCommObject *pParent);
};

/**
*  \ingroup mayabuilder 
*
*  This class traverses all animation curves related to the given dag
*  path through IK.
*
*/
class RWMAYA_API RwMayaTraverseIKAnimations  
{
public:
	RwMayaTraverseIKAnimations() {};
	virtual ~RwMayaTraverseIKAnimations() {};

	virtual void Traverse(RwCommBuilder &rBuilder, RwCommCreationData *pCreationData, RwCommObject *pParent);

};

/**
*  \ingroup mayabuilder 
*
*  This class traverses all animation curves related to the given dag
*  path by Maya constraint nodes.
*
*/
class RWMAYA_API RwMayaTraverseConstraintAnimations
{
public:
	RwMayaTraverseConstraintAnimations() {};
	virtual ~RwMayaTraverseConstraintAnimations() {};

	virtual void Traverse(RwCommBuilder &rBuilder, RwCommCreationData *pCreationData, RwCommObject *pParent);

private:
    set<string> visitedObjects;
};

#endif // !defined(RW_MAYA_TRAVERSE_ANIMATIONS_H)
