#ifndef RWMAXUTILS_H
#define RWMAXUTILS_H

#include "stdafx.h"

#include "RwCommError.h"

#include "RwMax.h"
#include "RwCommPrimitives.h"

class RwCommUserData;
class RwCommUserDataCollection;

RWMAX_API RwCommReal        RwMaxTicksToFrames(RwCommReal ticks);
RWMAX_API RwCommVec3        RwMaxPoint3ToRwExpVec3(const Point3& p);
RWMAX_API RwCommVec3        RwMaxColorToRwExpVec3(const Color& c);
RWMAX_API RwCommMatrix      RwMaxMat3ToRwExpMatrix(const Matrix3& m);
RWMAX_API Matrix3           RwMaxRwCommMatrixToMatrix3(const RwCommMatrix& m);
RWMAX_API string            RwMaxGetFileVersion(const char *filename, WORD languageID );
RWMAX_API void              RwMaxCenterDialogWithinOwner(HWND hwndDlg, HWND hWndInsertAfter = HWND_TOP);
RWMAX_API RwCommError       RwMaxGetUserData(ICustAttribContainer* pAttribs,
                                        RwCommUserDataCollection &userDataColl);
RWMAX_API void              RwMaxRoundZeroValues(Matrix3& m);

#endif