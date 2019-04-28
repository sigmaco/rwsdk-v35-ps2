
/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * pakfile.c
 *
 * Copyright (C) 2000 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate the use of PAK files for the storing and 
 *          retrieval of RenderWare data.
 *****************************************************************************/

#ifndef PAKFILE_H
#define PAKFILE_H

#include "rwcore.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

extern RwBool PakFileOpen(RwChar *filename);
extern RwBool PakFileOpenExt(RwChar *filename, RwChar *directory);
extern RwBool PakFileClose(void);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* PAKFILE_H */

