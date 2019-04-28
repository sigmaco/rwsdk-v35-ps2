// RwCommMessage.h: interface for the RwCommMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWCOMMMESSAGE_H__F65839FE_6BF2_4610_857F_D4C5E6BCDD66__INCLUDED_)
#define AFX_RWCOMMMESSAGE_H__F65839FE_6BF2_4610_857F_D4C5E6BCDD66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4251)

#include <string>
using namespace std;

#include "RwComm.h"
#include "RwCommError.h"

/** 
*  \ingroup commlowlevelobjects
*   \def PRINT_MESSAGE This macro is used to print output messages.
*/
#define PRINT_MESSAGE(x, y, z) RwCommMessage::TheInstance()->AddMessage(x, y, z)

/**
*  \ingroup commlowlevelobjects
*
*   This structure represents a message.
*/
struct RWCOMM_API RwCommMsgObj
{
    RwCommMsgObj(const string& m, const string& n, const int l) : msg(m), nodeName(n), level(l) {}

    /** Message body */
    string  msg;        
    /** Node name this message refers to */
    string  nodeName;   
    /** Verbosity level of the message */
    int     level;
};

/**
*  \ingroup commlowlevelobjects
*
*   This class displays and manages messages. It is a singleton class. Each message has
*   verbosity level for filtering. There is a separate array of summary messages
*   that should be display at the end.
*
*   \note For printing out the messages you should use PRINT_MESSAGE macro.
*/
class RWCOMM_API RwCommMessage  
{
public:
	virtual ~RwCommMessage();

    /**
    *   Register a static instance of the class. RwCommMessage is a singleton.
    */
    static void RegisterInstance(RwCommMessage *pInstance);

    /**
    *   \return static instance of the class. RwCommMessage is a singleton.
    */
    static RwCommMessage *TheInstance();

    /**
    *   Display a message.
    *
    *   \param msg message
    *   \param nodeName node name this message refers to
    *   \param level verbosity level for this message
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError AddMessage(const string &msg, const string &nodeName = "",
                            const int level = 0)    { return RWCOMM_NOT_IMPLEMENTED; }
    /**
    *   Set verbosity level. Messages with a higher level will be stored but not displayed.
    *   On changing the verbosity level all currently stored messages will be re-tested.
    *
    *   \param verbosity the new verbosity level
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetVerbosity(int verbosity) { return RWCOMM_NOT_IMPLEMENTED; }

    /**
    *   Set prefilter level. All messages with a higher level will not be stored or displayed.
    *
    *   \param preFilter the new prefilter level
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError SetPreFilter(int preFilter) { return RWCOMM_NOT_IMPLEMENTED; }

    /**
    *   Get preFilter level.
    *
    *   \param preFilter storage for the prefilter level
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError GetPreFilter(int &preFilter) { return RWCOMM_NOT_IMPLEMENTED; }

    /**
    *   Check if there is a message with the specified entry name in the summary.
    */
    bool    HasEntry(const string &entryName) const;
    /**
    *   Add a message to the summary. This is a list of separate messages 
    *   that should be displayed at the end.
    */
    void    AddToSummary(const string &msg, const string &nodeName = "",
                const int level = 0, const string &entryName = "");
    /**
    *   Display all the messages from the summary by calling the AddMessage(),
    *   then clear all of them.
    */
    void    FlushSummary();

protected:
    RwCommMessage();

private:
    vector<RwCommMsgObj>    m_vSummary;
    set<string>             m_sEntries;
    static RwCommMessage    *m_pTheInstance;
};

#endif // !defined(AFX_RWCOMMMESSAGE_H__F65839FE_6BF2_4610_857F_D4C5E6BCDD66__INCLUDED_)
