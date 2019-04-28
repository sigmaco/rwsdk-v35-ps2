// RwCommProgressBar.h: interface for the RwCommProgressBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWCOMMPROGRESSBAR_H__8AE51A6D_E3D3_4BF0_9A91_D78E252D797D__INCLUDED_)
#define AFX_RWCOMMPROGRESSBAR_H__8AE51A6D_E3D3_4BF0_9A91_D78E252D797D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
using namespace std;

typedef map<string, float> RwCommProgressStageMap;

#include "RwComm.h"
#include "RwCommError.h"
#include "RwCommPrimitives.h"

/**
*  \ingroup commlowlevelobjects
*
*   This class implements progress bar functionality.
*
*   Each task is separated in stages where each stage has its own weight. For example
*   you might have two stages when exporting, the export process itself and process of
*   saving the data. First stage can have weight of 0.95 and the second one 0.05. Each
*   stage has a limit marking the state at which the stage is complete.
*
*   RwCommProgressBar also has a notion of iterations. It is a number of times the 
*   whole process is repeated. An equal percentage is given to all iterations.
*   
*   If you need to implement the GUI for the progress bar, the only functions
*   you need to implement are: StartGUI(), EndGUI(), UpdateGUI() and IsCancelled()
*/
class RWCOMM_API RwCommProgressBar  
{
public:
	RwCommProgressBar();
	virtual ~RwCommProgressBar();

    /**
    *   \param pInstance of the derived class to register. RwCommProgressBar is a
    *          singleton.
    */
    static void                 RegisterInstance(RwCommProgressBar *pInstance);
    /**
    *   \return static instance of the class. RwCommProgressBar is a singleton.
    */
    static RwCommProgressBar    *TheInstance();

    /**
     *  \param iterationCount number of iterations to set.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetIterationCount(const int iterationCount);
    /**
     *  \param iterationCount returns a number of iterations.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         GetIterationCount(int &iterationCount);
    /**
     *  \param iteration iteration to set as current.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetIteration(const int iteration);
    /**
     *  \param iteration returns the current iteration.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         GetIteration(int &iteration);
    /**
     *  Increment iteration number.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         IncrementIteration();

    /**
     *  \param stage name for which to set the weight
     *  \param weight of the stage to set
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetStageWeight(const string stage, const RwCommReal weight);
    /**
     *  \param stage name of the stage
     *  \param weight returns the stage weight
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         GetStageWeight(const string stage, RwCommReal &weight);
    /**
     *  \param stage name for which to set the limit
     *  \param limit of the stage to set
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetStageLimit(const string stage, const RwCommReal limit);
    /**
     *  \param stage name of the stage
     *  \param limit returns the stage limit
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         GetStageLimit(const string stage, RwCommReal &limit);
    /**
     *  Each stage has an increment value, that is the number of increment before the
     *  this stage initates progress bar update. This is to prevent too many updates
     *  and UI slowdowns.
     *
     *  \param stage name for which to set the increment
     *  \param increment of the stage to set
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetStageUpdateIncrement(const string stage, const RwCommReal increment);
    /**
     *  \param stage name of the stage
     *  \param increment returns the stage increment number
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     *
     *  \see SetStageUpdateIncrement() for more information on increments.
     */
    virtual RwCommError         GetStageUpdateIncrement(const string stage, RwCommReal &increment);
    /**
     *  \param stage name for which to set the value
     *  \param value of the stage to set
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetStageValue(const string stage, const RwCommReal value);
    /**
     *  \param stage name of the stage
     *  \param value returns the current stage value.
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         GetStageValue(const string stage, RwCommReal &value);
    /**
     *  \param stage name for which to increment the value
     *  \param increment value of the stage
     *
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         IncrementStageValue(const string stage, const RwCommReal increment);

    /**
     *  Normalize weights
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         NormaliseWeights();
    
    /**
    *   Calculate the current progressbar position, update the GUI and check if the process
    *   was cancelled.
    *  \return RWCOMM_SUCCESS if successful, RWCOMM_ABORT if the operation was aborted, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError         Update();
    
    /** Reset the all the stages, limits, weights, values and increments. */
    virtual RwCommError         ReInitialiseIteration();

    /**
     *  Start the progress bar.
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         StartGUI() { return RWCOMM_NOT_IMPLEMENTED; }
    
    /**
     *  End the progress bar.
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         EndGUI() { return RWCOMM_NOT_IMPLEMENTED; }
    
    /**
     *  Update the progress bar with a new total.
     *  \param total value to update the GUI to
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         UpdateGUI(const RwCommReal total) { return RWCOMM_NOT_IMPLEMENTED; }
    
    /**
     *  This is called from the Update()
     *  \param bCancelled returns true if the process is cancelled.
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         IsCancelled(bool &bCancelled) { return RWCOMM_NOT_IMPLEMENTED; }

    /** 
     *  Set the title message of the progress bar
     *  \param title to set
     *  \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
     */
    virtual RwCommError         SetTitle(string title);

protected:
    static RwCommProgressBar    *m_pTheInstance;
    string                      m_title;
    int                         m_iterationCount;
    int                         m_iteration;
    RwCommProgressStageMap      m_stageLimits;
    RwCommProgressStageMap      m_stageWeights;
    RwCommProgressStageMap      m_stageValues;
    RwCommProgressStageMap      m_stageUpdateIncrements;
    RwCommProgressStageMap      m_stageUpdateTotals;
};

#endif // !defined(AFX_RWCOMMPROGRESSBAR_H__8AE51A6D_E3D3_4BF0_9A91_D78E252D797D__INCLUDED_)
