
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
 * Copyright (c) 2001 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *                                                                         
 * pclone.c
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: Alexandre Hadjdaj.
 * Reviewed by: .
 *                                                                         
 * Purpose: Illustrate the use of the Standard Particle Plugin with the cloning 
 *          Pipeline.
 *                         
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"
#include "rpptank.h"

#include "skeleton.h"
#include "menu.h"

#include "pclone.h"

pCloneData PrtData;

#define SCALEF (0.25f)

/*
 *****************************************************************************
 */
static RpClump *
LoadClump(RwChar *filename)
{
    RwStream *stream;
    RpClump *clump = NULL;
    RwChar *path;

    path = RsPathnameCreate(filename);    
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
    RsPathnameDestroy(path);

    if( stream )
    {
        if( RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL) )
        {
            clump = RpClumpStreamRead(stream);
        }

        RwStreamClose(stream, NULL);
    }
    
    return clump;
} 

static RpAtomic *
getfirstatm(RpAtomic *atm, void *data)
{
    *((RpAtomic **)data) = atm;

    return NULL;
}    

static RpAtomic *
CreateAtomic(void)
{
    static RwChar _dff[] = RWSTRING("./models/globule.dff");
    
    RpClump *clump;
    RpAtomic *atm;
    RwFrame *frame;
    
    clump = LoadClump(_dff);

    RpClumpForAllAtomics(clump,getfirstatm,&atm);

    frame = RwFrameCreate();
    RwFrameSetIdentity(frame);
    RpAtomicSetFrame(atm,frame);
    
    return atm; 
} 

static RpAtomic *PtankCreate(RwUInt32 numPrt)
{
	RpAtomic *pTank;
	RwFrame *frame;
	
    pTank = RpPTankAtomicCreate(numPrt,
                            rpPTANKDFLAGMATRIX |
                            0,
                            rpPTANKSKYFLAGUSECLONEPIPELINE);
    frame = RwFrameCreate();
    RwMatrixSetIdentity(RwFrameGetMatrix(frame));
    RpAtomicSetFrame(pTank,frame);

    RpPTankAtomicSkyCloneSetReferenceAtomic(pTank,CreateAtomic());
    
    RpPTankAtomicSkyCloneSetAmbientLight(pTank,Ambient);
    RpPTankAtomicSkyCloneSetDirectionalLight(pTank,Directional);    
    
	return pTank;
}


/*
 *****************************************************************************
 */
void 
pCloneRender(void)
{
    static RwBool stateSet = FALSE;
    static RwUInt32 FogColor=RWRGBALONG(64,0,0,255);
    if( !stateSet )
    {
        /* 
         * Render state is persistent - only need to set it once...
         */
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
        
        RwRenderStateSet(rwRENDERSTATEFOGCOLOR,(void *)FogColor);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)TRUE);

        RwCameraSetFogDistance(Camera, 100.0f);

        RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void *)rwFOGTYPELINEAR);


    }
    
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)TRUE);

    RpAtomicRender(PrtData.atmEmitter);
    
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)FALSE);

    return;
}


/*
 *****************************************************************************
 */
RwBool
pCloneSetDefault(RwBool justcheck)
{
    RwV3d                       up, at, right;
    if( justcheck )
    {
        return TRUE;
    }

    /* Set the emitter's maximum particles */
    PrtData.EmtStd->maxPrt = 5000;

    /* Set the emitter's emission area */
    PrtData.EmtStd->emtSize.x = 10.0f;
    PrtData.EmtStd->emtSize.y = 10.0f;
    PrtData.EmtStd->emtSize.z = 0.0f;

    /* Set the particle's size */
    PrtData.EmtStd->prtSize.x = 1.0f;
    PrtData.EmtStd->prtSize.y = 1.0f;

    /* Set the particle emission gap : should not be bigger 
     *  than the batch size setted during the creation code
     */
    PrtData.EmtStd->emtPrtEmit = 2;
    PrtData.EmtStd->emtPrtEmitBias = 0;
    PrtData.EmtStd->emtEmitGap = 0.0f;
    PrtData.EmtStd->emtEmitGapBias = 0.0f;

    /* Set the particle's life span */
    PrtData.EmtStd->prtLife = 10.0f;
    PrtData.EmtStd->prtLifeBias = 0.0f;

    /* Set the particles emission speed */
    PrtData.EmtStd->prtInitVel = 5.0f;
    PrtData.EmtStd->prtInitVelBias = 5.00f;

    /* Set the particles emission Direction */
    PrtData.EmtStd->prtInitDir.x = 0.0f;
    PrtData.EmtStd->prtInitDir.y = 0.0f;
    PrtData.EmtStd->prtInitDir.z = -1.0f;

    PrtData.EmtStd->prtInitDirBias.x = 0.0f;
    PrtData.EmtStd->prtInitDirBias.y = 0.0f;
    PrtData.EmtStd->prtInitDirBias.z = 0.0f;

    /* Set the force emission Direction */
    PrtData.EmtStd->force.x = 0.0f;
    PrtData.EmtStd->force.y = 0.0f;
    PrtData.EmtStd->force.z = -5.0f;

    /* Set the default Color */
    PrtData.EmtStd->prtColor.red = 255;
    PrtData.EmtStd->prtColor.green = 255;
    PrtData.EmtStd->prtColor.blue = 255;
    PrtData.EmtStd->prtColor.alpha = 128;

    /* Set the default Texture coordinate */
    PrtData.EmtStd->prtUV[0].u = 0.0f;
    PrtData.EmtStd->prtUV[0].v = 0.0f;

    PrtData.EmtStd->prtUV[1].u = 1.0f;
    PrtData.EmtStd->prtUV[1].v = 1.0f;

    /* Set the texture */
    PrtData.EmtStd->texture = NULL;

	    
    PrtData.emtPrtMtx->prtPosMtxAt.x = (RwReal) 0.0;
    PrtData.emtPrtMtx->prtPosMtxAt.y = (RwReal) 0.0;
    PrtData.emtPrtMtx->prtPosMtxAt.z = (RwReal) 0.0;

    PrtData.emtPrtMtx->prtPosMtxAtBias.x = (RwReal) 1.0;
    PrtData.emtPrtMtx->prtPosMtxAtBias.y = (RwReal) 1.0;
    PrtData.emtPrtMtx->prtPosMtxAtBias.z = (RwReal) 1.0;

    PrtData.emtPrtMtx->prtPosMtxUp.x = (RwReal) 0.0;
    PrtData.emtPrtMtx->prtPosMtxUp.y = (RwReal) 0.0;
    PrtData.emtPrtMtx->prtPosMtxUp.z = (RwReal) 0.0;

    PrtData.emtPrtMtx->prtPosMtxUpBias.x = (RwReal) 1.0;
    PrtData.emtPrtMtx->prtPosMtxUpBias.y = (RwReal) 1.0;
    PrtData.emtPrtMtx->prtPosMtxUpBias.z = (RwReal) 1.0;

    PrtData.emtPrtMtx->flags = 0;

    RwMatrixSetIdentity(&PrtData.emtPrtMtx->prtCnsMtx);

    at = *RwMatrixGetAt(&PrtData.emtPrtMtx->prtCnsMtx);
    up = *RwMatrixGetUp(&PrtData.emtPrtMtx->prtCnsMtx);
    right = *RwMatrixGetRight(&PrtData.emtPrtMtx->prtCnsMtx);

    /*
    * ...do the rotations...
    */
    RwMatrixRotate(&PrtData.emtPrtMtx->prtCnsMtx, &at, 1.5*SCALEF, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(&PrtData.emtPrtMtx->prtCnsMtx, &up, 3.5*SCALEF, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(&PrtData.emtPrtMtx->prtCnsMtx, &right, -2.5*SCALEF, rwCOMBINEPOSTCONCAT);

    return TRUE;
}


/*
 *****************************************************************************
 */
RwBool 
pCloneInitialize(void)
{
    RwInt32 offset;
    RwFrame *frame;
    
    /* Create an emitter class suporting the standard 
       emitter properties and with a matrix per particles
       and owning and controling a PTank */
    PrtData.eClass = RpPrtStdEClassStdCreate(
        (rpPRTSTDEMITTERDATAFLAGSTANDARD |
		rpPRTSTDEMITTERDATAFLAGPRTMATRIX |
        rpPRTSTDEMITTERDATAFLAGPTANK));

    if( NULL == PrtData.eClass )
    {
        return FALSE;
    }

    /* Create a particle class suporting the standard 
       particle properties 
       plus a velocity per particles */
    PrtData.pClass = RpPrtStdPClassStdCreate(
        (rpPRTSTDPARTICLEDATAFLAGSTANDARD |
        rpPRTSTDPARTICLEDATAFLAGVELOCITY));

    if(NULL == PrtData.eClass )
    {
        return FALSE;
    }

    
    /* Create the Emitter's Atomic */
    PrtData.atmEmitter = RpPrtStdAtomicCreate(PrtData.eClass, NULL);
    if( NULL == PrtData.atmEmitter )
    {
        return FALSE;
    }

    /* Get the emitter Pointer */
    PrtData.emitter = RpPrtStdAtomicGetEmitter(PrtData.atmEmitter);

    /* Set particle class and particle batch size */
    RpPrtStdEmitterSetPClass(PrtData.emitter, PrtData.pClass, BATCHSIZE);

    /* Get the Emitter Standard properties pointer */
    offset = RpPrtStdPropTabGetPropOffset(
        PrtData.eClass->propTab, rpPRTSTDPROPERTYCODEEMITTERSTANDARD);

    PrtData.EmtStd = (RpPrtStdEmitterStandard *) (((RwChar *)PrtData.emitter) + offset);

    offset = RpPrtStdPropTabGetPropOffset(
        PrtData.eClass->propTab, rpPRTSTDPROPERTYCODEEMITTERPRTMATRIX);
        
    PrtData.emtPrtMtx = (RpPrtStdEmitterPrtMatrix *) (((RwChar *)PrtData.emitter) + offset);

    offset = RpPrtStdPropTabGetPropOffset(
        PrtData.eClass->propTab, rpPRTSTDPROPERTYCODEEMITTERPTANK);

    PrtData.EmtPtank = (RpPrtStdEmitterPTank *) (((RwChar *)PrtData.emitter) + offset);   
    PrtData.emitter->flag |= rpPRTSTDEMITTERFLAGNOBUFFERSWAP;
    
    /* Setup The Default Properties */
    pCloneSetDefault(FALSE);
    

    /* Setup a frame for the emitter */
    frame = RwFrameCreate();
    RwMatrixSetIdentity(RwFrameGetMatrix(frame));
    RpAtomicSetFrame(PrtData.atmEmitter, frame);

    /* Set the particle's texture*/
	PrtData.atmPTank = PtankCreate(5000);
	
	RpPrtStdEmitterLinkPTank(PrtData.emitter,PrtData.atmPTank);

    return TRUE;
}


/*
 *****************************************************************************
 */
void 
pCloneTerminate(void)
{

	if( NULL != PrtData.atmPTank )
	{
		RpAtomic *cloneSrc = 
		RpPTankAtomicSkyCloneGetReferenceAtomic(PrtData.atmPTank);

		if( NULL != cloneSrc )
		{
			RpClump *clump = RpAtomicGetClump(cloneSrc);
			
			RpClumpDestroy(clump);
		}
	}

    
    if( NULL != PrtData.atmEmitter )
    {
        RwFrame *frame = RpAtomicGetFrame(PrtData.atmEmitter);
        
        if( NULL != frame)
        {
             RpAtomicSetFrame(PrtData.atmEmitter, NULL);
             RwFrameDestroy(frame);
        }

        RpAtomicDestroy(PrtData.atmEmitter);
		PrtData.atmEmitter = NULL;
    }

	if( NULL != PrtData.eClass )
	{
	   RpPrtStdEClassDestroy(PrtData.eClass);
	}

	if( NULL != PrtData.pClass )
	{
	   RpPrtStdPClassDestroy(PrtData.pClass);
	}
	
    return;
}


/*
 *****************************************************************************
 */
void
pCloneCameraRotate(RwReal xAngle, RwReal yAngle)
{
    RwMatrix    *mtx;
    RwV3d       right, up, pos, at;
    RwReal      dist;

    mtx = RwFrameGetMatrix(RwCameraGetFrame(Camera));
    right = *RwMatrixGetRight(mtx);
    up = *RwMatrixGetUp(mtx);
    pos = *RwMatrixGetPos(mtx);

    /* Find the distance */
    dist = RwV3dLength(&pos);

    /*
     * First translate back to the origin...
     */
    RwV3dScale(&pos, &pos, -1.0f);
    RwMatrixTranslate(mtx, &pos, rwCOMBINEPOSTCONCAT);

    /*
     * ...do the rotations...
     */
    RwMatrixRotate(mtx, &up, xAngle, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(mtx, &right, yAngle, rwCOMBINEPOSTCONCAT);

    /*
     * ...and translate back...
     */
    at = *RwMatrixGetAt(mtx);
    RwV3dNormalize(&at, &at);

    RwV3dScale(&pos, &at, -dist);
    RwMatrixTranslate(mtx, &pos, rwCOMBINEPOSTCONCAT);
    RwFrameUpdateObjects(RwCameraGetFrame(Camera));

    return;
}

/*
 *****************************************************************************
 */
void
pCloneCameraTranslate(RwReal atDir)
{
    RwV3d       at;

    at = *RwMatrixGetAt(RwFrameGetMatrix(RwCameraGetFrame(Camera)));
    
    /*
     * translate...
     */
    RwV3dScale(&at,&at,atDir);
    RwFrameTranslate(RwCameraGetFrame(Camera), &at, rwCOMBINEPOSTCONCAT);

    RwFrameUpdateObjects(RwCameraGetFrame(Camera));

    return;
}
