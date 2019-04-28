// RwRf3XMLParser.h: interface for the RwRf3XMLParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RWRF3XMLPARSER_H
#define RWRF3XMLPARSER_H

#include "RwXMLDOM.h"

/**
*   \ingroup rf3helper 
*
*   Classes, deriving from the RwXMLNode, should be registered with the RwRf3XMLParser by deriving
*   from the RwRf3XMLNodeCreator and implementing its RwRf3XMLNodeCreator::Create() method.
*
*   \see RwRf3XMLParser
*/
class RwRf3XMLNodeCreator
{
public:
    RwRf3XMLNodeCreator();
    virtual ~RwRf3XMLNodeCreator();

    /**
    *   This method should create the custom RwXMLNode and return a pointer to it.
    *
    *   \param parentNode the XML parent node. Classes, implementing this method, should attach the created
    *           node to its parent node
    *
    *   \returns the created custom RwXMLNode
    */
    virtual RwXMLNode* Create( RwXMLNode* parentNode = NULL ) const = 0;
};


/**
*   \ingroup rf3helper 
*
*   This is the XML parser class. The class reads in an XML file and returns a tree of RwXMLNode objects.
*   Users can derive their own classes from the RwXMLNode and register them with the parser using the
*   RwRf3XMLNodeCreator class. The parser will then create their custom RwXMLNode objects from the XML file
*   once these objects are registered.
*
*   \see RwRf3XMLNodeCreator
*/
class RwRf3XMLParser
{
public:
    
    //default constructor
    RwRf3XMLParser();

    //destructor
    ~RwRf3XMLParser();

    // load an xml file into a xml node
    bool Load( RwXMLNode* node, const string& filename );

    // load an xml file
    RwXMLNode* Load( const string& filename );

    //parses the given buffer into the given node
//    bool ParseFrom( RwXMLNode& lNode, const char* lpcBuffer, unsigned int length );

    //creates a new node from a given buffer
//    RwXMLNode* ParseFrom( const char* lpcBuffer, unsigned int length );

    //gets the parser error message
    const char* GetErrorMessage() const;

    unsigned int GetLineCount() const { return lineCount; }

    //expands tokens into their xml escape codes
//    static bool ExpandSubstitutes( char* lpcDst, const char* lpcSrc, bool lbInText );

    void MapNode(RwXMLNode* pNode);
    RwXMLNode *GetNodeByName(const string &name);

    bool RegisterCreator(const string &tag, RwRf3XMLNodeCreator *pCreator);

private:        

    static void openTagCB(void *userdata, const char *el, const char **attr);
    static void closeTagCB(void *userdata, const char *el);
    static void tagTextCB(void *userdata, const char *t);

    string errorMessage;
    unsigned int lineCount;

    map<string, RwXMLNode*>  m_mNameTable;
    map<string, RwRf3XMLNodeCreator*>  m_mCreators;
};


#endif // RWRF3XMLPARSER_H