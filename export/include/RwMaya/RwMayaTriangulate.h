#ifndef RW_MAYA_TRIANGULATE_H
#define RW_MAYA_TRIANGULATE_H

typedef pair<int, int>                  vertEdge;
typedef set<vertEdge>                   meshEdges;
typedef set<vertEdge>::iterator         meshEdgesIt;
typedef pair<set<int>::iterator, bool>  intSetPair;

#if (!defined(DOXYGEN))

struct polyFlipEdge
{
    int         id;
    vertEdge    edge;
};

// =============================================================================
// --------------------------  RwTriangulate CLASS  ----------------------------
// =============================================================================

class RWMAYA_API RwTriangulate : public MPxCommand
{
public:
                            RwTriangulate()                 {};
    virtual                 ~RwTriangulate()                {}; 
                            
    static void             *creator();
    bool                    isUndoable() const;
    MStatus                 doIt(const MArgList&);
    MStatus                 redoIt();
    MStatus                 undoIt();

private:
    MSelectionList          selList;
    MGlobal::MSelectionMode selMode;
    MDagPath                meshDagPath;
    MObject                 meshComps;
    set<int>                meshPolys;
    vector<meshEdges>       allFlipEdges;
                            
    bool                    BuildMeshQuadList();
    bool                    BuildMeshFlipEdgeList();
};

#endif // (!defined(DOXYGEN))

#endif // RW_MAYA_TRIANGULATE_H