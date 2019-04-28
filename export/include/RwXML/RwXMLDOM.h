/*****************************************************************************

    File:   RwXMLDOM.h

*****************************************************************************/

/*************************** Compilation defines ****************************/
#ifndef RWXMLDOM_H
#define RWXMLDOM_H

/****************************** Include files *******************************/

#pragma warning(disable : 4251)

#include "RwXmlDll.h"
#include "RwXML.h"

#include <string>
#include <list>

using namespace std;

class RwXMLNode;

// a smart pointer to RwXMLNode
class RWXML_API RwXMLNodePtr
{
public:
    RwXMLNodePtr();
    RwXMLNodePtr(RwXMLNode* ptr);
    RwXMLNodePtr(const RwXMLNodePtr& ptr);
    ~RwXMLNodePtr();
    RwXMLNode* operator->();
    RwXMLNode* operator=(RwXMLNode* targetNode);
    RwXMLNode* GetPtr() {  return nodePtr; }
    operator RwXMLNode*() const { return nodePtr; }
private:
    RwXMLNode* nodePtr;
};

typedef list<RwXMLNodePtr> RwXMLNodeList;
typedef RwXMLNodeList::iterator RwXMLNodeListItor;


/************************ Constants and Definitions *************************/

typedef bool (*XMLEnumProc)(class RwXMLNode*, void*);

/********************************** Macros **********************************/

/************************** Typedefs and Classes ****************************/

/*****************************************************************************

RwXMLAttribute

Description:
XML keypair-style attribute class

Parent classes:
None

Child classes:
None

******************************************************************************/
class RWXML_API RwXMLAttribute
{
    friend class RwXMLNode;

public:
    //default constructor
    RwXMLAttribute();                        
    
    //data constructor
    RwXMLAttribute( const string& lpcName, const string& lpcValue );
    
    //sets the name of this attribute
    bool SetName( const string& lpcName );

    //sets the value of this attribute
    bool SetValue( const string& lpcValue );

    //gets the name of this attribute
    string GetName() const;

    //gets the value of this attribute
    string GetValue() const;

    // traversal operations
    inline const RwXMLAttribute* GetNextAttribute( void ) const { return mpNext; }
    inline const RwXMLAttribute* GetPreviousAttribute( void ) const { return mpPrev; }
    inline RwXMLAttribute* GetNextAttribute( void ) { return mpNext; }
    inline RwXMLAttribute* GetPreviousAttribute( void ) { return mpPrev; }

protected:       
    RwXMLAttribute* mpNext;
    RwXMLAttribute* mpPrev;

    string nodeName;
    string nodeValue;
};

/*****************************************************************************

RwXMLNode

Description:
XML data node. Contains a list of attributes and child nodes.

Parent classes:
None

Child classes:
None

******************************************************************************/
/**
*   \ingroup rf3helper 
*
*   This class represents an XML node. An XML node has a name, a set of attributes and
*   a text section. 
*/
class RWXML_API RwXMLNode
{
friend class RwXMLParser;

public:

    // ----- Node operations

    // constructors
    /** 
    *   Construct the node and attach it to its parent.
    *
    *   \param parentNode the parent node
    */
    RwXMLNode( RwXMLNode* parentNode = NULL );

    /** 
    *   Construct the node with the given name and attach it to its parent.
    *
    *   \param name the node's name
    *   \param parentNode the parent node
    */
    RwXMLNode( const string& name, RwXMLNode* parentNode = NULL );

    // destructor
    virtual ~RwXMLNode();
    
    // clone this node
//    RwXMLNode* CloneNode( bool deep );

    /** 
    *   Set the node's name.
    *
    *   \param newName the node's name
    */
    bool SetName( const string& newName );

    /** \returns the node's name */
    string GetName( void ) const;

    /**
    *   Set the node's text.
    *
    *   \param newText the input text string
    */
    bool SetText( const string& newText );

    /** \returns the node's text string.*/
    string GetText( void ) const;

    // ----- Child operations

    /** 
    *   Append a child to this node
    *
    *   \param newChild a pointer to the child to append
    */
    RwXMLNode* AppendChild( RwXMLNode* newChild );

    // insert a child before a given node
//    RwXMLNode* InsertBefore( RwXMLNode* newChild, RwXMLNode* refChild );

    // replace a child
//    RwXMLNode* ReplaceChild( RwXMLNode* newChild, RwXMLNode* oldChild );

    /** 
    *   Remove a child.
    *
    *   \param oldChild pointer to the child to remove
    */
    RwXMLNode* RemoveChild( RwXMLNode* oldChild );

    // has this node got child nodes
    bool HasChildNodes() const { return( NULL != mpFirstChild ); }

    // Create a new child and append - NON W3C!
    RwXMLNode* AddChild( const string& name );

    /** \returns the number of children nodes.*/
    unsigned int GetNumChildren(void) const;

    // ----- Reference Counting

    /** Bump the reference count.*/
    unsigned int AddRef(void);

    /**
    * Decrement the reference from this node.
    * deletes the node when count reaches zero.
    */
    unsigned int Release(void);

    // ----- Attribute operations

    /** 
    * Get a string attribute by name.
    *
    *   \param name the name of the attribute
    *   \param value the attribute's returned value
    *
    *   \returns true if the attribute exists, false otherwise
    */       
    bool GetAttribute( const string& name, string& value ) const;

    /** 
    * Get an int attribute by name.
    *
    *   \param name the name of the attribute
    *   \param value the attribute's returned value
    *
    *   \returns true if the attribute exists, false otherwise
    */     
    bool GetAttribute( const string& name, int& value ) const;

    /** 
    * Get a float attribute by name.
    *
    *   \param name the name of the attribute
    *   \param value the attribute's returned value
    *
    *   \returns true if the attribute exists, false otherwise
    */     
    bool GetAttribute( const string& name, float& value ) const;
    
    /**
    * Set a string attribute.
    *
    *   \param name the attribute's name
    *   \param value the value to set
    */
    void SetAttribute( const string& name, const string& value );

    /**
    * Set an int attribute.
    *
    *   \param name the attribute's name
    *   \param value the value to set
    */
    void SetAttribute( const string& name, int value );

    /**
    * Set a float attribute.
    *
    *   \param name the attribute's name
    *   \param value the value to set
    */
    void SetAttribute( const string& name, float value );


    // remove the attribute with the matching name
//    void RemoveAttribute( string& name );

    // Retrieves an attribute by name. Returns the attribute with the 
    // specified attribute name or NULL if there is no such attribute.
//    RwXMLAttribute* GetAttributeNode( string& name );

    // Retrieves an attribute by index. Returns the attribute with the 
    // specified index or NULL if there is no such attribute.
    RwXMLAttribute* GetAttributeNode( unsigned int index ) const;

    // Adds a new attribute. If an attribute with that name is
    // already present in this node, it is replaced by the new one.
    // If the newAttr attribute replaces an existing attribute with the
    // same name, the previously existing attribute is returned, otherwise
    // NULL is returned.
//    RwXMLAttribute* SetAttributeNode( RwXMLAttribute* newAttr );

    // Removes the specified attribute.
    // The attribute that was removed. If the attribute is not found 
    // NULL is returned. 
//    RwXMLAttribute* RemoveAttributeNode( RwXMLAttribute* oldAttr );

    /** \returns the number of attributes.*/
    unsigned int GetNumAttributes( void ) const;

    inline const RwXMLAttribute* GetFirstAttribute( void ) const { return mpFirstAttribute; }
    inline const RwXMLAttribute* GetListAttribute( void ) const { return mpLastAttribute; }

    inline RwXMLAttribute* GetFirstAttribute( void ) { return mpFirstAttribute; }
    inline RwXMLAttribute* GetListAttribute( void ) { return mpLastAttribute; }

    // ----- Traversal operations
    
    /** \returns the node's parent.*/
    inline const RwXMLNode* GetParent( void ) const { return mpParent; }

    /** \returns the first child in the child list.*/
    inline const RwXMLNode* GetFirstChild( void ) const { return mpFirstChild; }

    /** \returns the last child in the child list.*/
    inline const RwXMLNode* GetLastChild( void ) const { return mpLastChild; }

    /** 
    * Sibling iterator.
    *   \returns the next sibling. */
    inline const RwXMLNode* GetNextNode( void ) const { return mpNext; }

    /** 
    * Sibling iterator.
    * \returns the previous sibling. */
    inline const RwXMLNode* GetPreviousNode( void ) const { return mpPrev; }

    /** \returns the node's parent.*/
    inline RwXMLNode* GetParent( void ) { return mpParent; }

    /** \returns the first child in the child list.*/
    inline RwXMLNode* GetFirstChild( void ) { return mpFirstChild; }

    /** \returns the last child in the child list.*/
    inline RwXMLNode* GetLastChild( void )  { return mpLastChild; }

    /** Sibling iterator. 
    * \returns the next sibling. */
    inline RwXMLNode* GetNextNode( void )  { return mpNext; }

    /** Sibling iterator. 
    * \returns the previous sibling. */
    inline RwXMLNode* GetPreviousNode( void ) { return mpPrev; }

    // generate a list of node with tag matching name, "*" matches all
    RwXMLNodeList* GetChildrenByTagName( const string& name, bool deep = false ) const;
    
    bool GetChildrenByTagName( RwXMLNodeList& nodeList, const string& namw, bool deep = false ) const;

    bool EnumerateByTagName( const string& name, XMLEnumProc lpfXMLEnumProc,
                        bool deep = false, void* param = NULL ) const;

    // ---- saving operation
   
    /** 
    * Save the node and its descendants to the given xml file.
    *
    *   \param filename the XML file name
    *   \param lbCompressed flag indicating whether to remove all redundant spacing
    */
    bool Save( const string& filename, bool lbCompressed = true );

protected:

    string          nodeName;             //node name
    string          nodeText;             //node text
    RwXMLNode*       mpParent;             //node parent pointer
    RwXMLNode*       mpFirstChild;         //first child node
    RwXMLNode*       mpLastChild;          //last child node
    RwXMLNode*       mpNext;               //next child node
    RwXMLNode*       mpPrev;               //previous child node

    RwXMLAttribute*  mpFirstAttribute;     //first attribute    
    RwXMLAttribute*  mpLastAttribute;      //last attribute
    unsigned int    mRefCounter;          //reference counter    
};


/*****************************************************************************

RwXMLParser

Description:
XML parser class

Parent classes:
None

Child classes:
None

******************************************************************************/
class RWXML_API RwXMLParser
{
public:
    
    //default constructor
    RwXMLParser();

    //destructor
    ~RwXMLParser();

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

private:        

    static void openTagCB(void *userdata, const char *el, const char **attr);
    static void closeTagCB(void *userdata, const char *el);
    static void tagTextCB(void *userdata, const char *t);

    string errorMessage;
    unsigned int lineCount;
};

enum XMLCompressionType
{
    NORMAL = 0,
    NOSPACE,
    BINARY
};

// xml saving class
class RWXML_API RwXMLSaver
{
public:
    //default constructor
    RwXMLSaver();

    //destructor
    ~RwXMLSaver();

    // save node to xml file
    bool Save( RwXMLNode* node, const string& filename );

//    bool SetEncoding(XMLEncoding encoding);

    void SetCompression( XMLCompressionType comp ) { compressionType = comp; }

private:

    XMLCompressionType compressionType;

    //expands tokens into their xml escape codes
    static bool ExpandSubstitutes( char* lpcDst, const char* lpcSrc, bool lbInText );

    typedef struct XMLWriteInfoTag XMLWriteInfo;

    struct XMLWriteInfoTag
    {
        FILE*   mpFile;                   //file pointer to write to
        int     mnIndent;                 //current indent level
        bool  mbCompressed;               //save compressed?
        char* mpcWorkBuffer;              //string munging work buffer
        RwXMLSaver* mpSaver;
    };

    //writes this node to the given file
    bool WriteNode( RwXMLNode* lpNode, XMLWriteInfoTag* lpWriteInfo );

    //enumation callback function
    friend static bool _XMLEnumWriteNode( RwXMLNode* lpNode, void* lpParam );
};

/*************************** Function Prototypes ****************************/

/******************************* Global Data ********************************/

/******************************** Functions *********************************/

#endif //RWXMLDOM_H
