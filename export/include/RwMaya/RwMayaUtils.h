#ifndef RW_MAYA_UTILS_H
#define RW_MAYA_UTILS_H


RWMAYA_API RwCommMatrix         RwMayaMatrixToCommMatrix(const MMatrix & m);
RWMAYA_API RwCommVec3           RwMayaPointToCommVec3(const MPoint & p);
RWMAYA_API RwCommVec3           RwMayaVectorToCommVec3(const MVector & v);
RWMAYA_API RwCommVec3           RwMayaColorToCommVec3(const MColor & c);
RWMAYA_API RwCommBoundingBox    RwMayaBoundingBoxToRwCommBoundingBox(MBoundingBox mayaBBox);
RWMAYA_API void                 RwMayaStripStringPath(string &inString);
RWMAYA_API void                 RwMayaStripStringExtension(string &inString);
RWMAYA_API void                 RwMayaStripStringFileName(string &inString);
RWMAYA_API void                 RwMayaConvertForwardSlashesToBack(string &inString);
RWMAYA_API string               RwMayaGetShortFileName(const string &filename);

#endif //RW_MAYA_UTILS_H