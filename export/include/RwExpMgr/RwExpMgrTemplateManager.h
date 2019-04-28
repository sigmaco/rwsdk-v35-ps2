#ifndef RWEXPMGRTEMPLATEMANAGER_H
#define RWEXPMGRTEMPLATEMANAGER_H

#define RWEXP_TEMPLATE_VERSION  0

/**
*  \ingroup helperclasses 
*
*   Template types. Beside asset and project templates, there is  default template type.
*   It defines only one template file which lives in \Graphics\export\bin and is called
*   DefaultTemplate.rwt All other templates are generated from this one if not present
*   and it has all the options from all templates. You can't set the folder for this
*   template.
*/
enum RwExpMgrTemplateType
{
    /** asset template */
    assetTemplate,
    /** project template */
    projectTemplate,
    /** default template */
    defaultTemplate,
};

/**
*  \ingroup helperclasses 
*
*   Property structure is like an RwCommOption but much richer. It holds hierarchy,
*   descriptions, enums etc.
*/
struct RwExpMgrTemplateProperty
{
    /** Name of the property. */
    string                              m_name;
    /** Type of the property. It can be bool, int, enum, float, string, enumstring. */
    string                              m_type;
    /** Value of the property, stored as the string. */
    string                              m_value;
    /** Description of the property. */
    string                              m_description;
    /**
    *   Location of the property. It is a vector of string: gamecube, generic, ps2,
    *   xbox, anm, bsp, dff, spl. They define in which project/asset template this
    *   option belongs.
    */
    vector<string>                      m_location;
    /** Vector of enum definitions for the property. */
    vector<string>                      m_enums;
    /** Vector of all child properties. */
    vector<RwExpMgrTemplateProperty>    m_children;
};

// =============================================================================
// ----------------------  RwExpMgrTemplateManager CLASS  ----------------------
// =============================================================================
class RwExpMgrTemplateManager;

//disable warnings on extern before template instantiation

#include "patterns\RwObserver.h"

RW_IMPLEMENT_OBSERVER_LINKING(RwExpMgrTemplateManager, void*);

/**
*
*  \ingroup helperclasses 
*
*   RwExpMgrTemplateManager class is managing the template files. Template files
*   have .rwt extension and they are xml files that can be edited from any text
*   or xml editor. 
*
*   They hold all the export options for projects and assets that are divided into
*   sections. Top level sections are Output (only for project templates), Common
*   and Platforms sections. Platform section has a subsection for each platform
*   that can be enabled or disabled by an Enable option in the section. The
*   template structure can be mirrored in hierarchy of RwExpMgrTemplateProperty.
*/
class RWEXPMGR_API RwExpMgrTemplateManager : public RwObserverSubject<RwExpMgrTemplateManager>
{
public:

    enum ObserverUpdateMessages
    {
        AssetTemplatesDirty = 0,
        ProjectTemplatesDirty, 
    };

public:
    RwExpMgrTemplateManager();
    ~RwExpMgrTemplateManager();

    /**
    *   \return static instance of the class. RwExpMgrExportManager is a singleton.
    */
    static          RwExpMgrTemplateManager  &TheInstance();

    /**
    *   Get the template folder path from the registry.
    *
    *   \param type of the templates to get the folder from
    *   \param templateFolder returns the path where the templates of this type live
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     GetTemplateFolder(RwExpMgrTemplateType type, string &templateFolder);
    /**
    *   Set the template folder path. Path is saved in the registry.
    *
    *   \param type of the templates to set the folder for
    *   \param templateFolder path where the templates of this type live
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     SetTemplateFolder(RwExpMgrTemplateType type, const string &templateFolder);

    /**
    *   \param type of the templates to get
    *   \param templates returns all the templates' names of the specified type
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     GetTemplates(RwExpMgrTemplateType type, vector<string> &templates);
    
    // Per template methods.
    /**
    *   This will return all the common options plus platform specific ones if the platform
    *   is not an empty string.
    *
    *   \param type of the template to get the options from
    *   \param templateName name of the template to query
    *   \param platform specifies which section to query in the given template. If empty
    *                   string no platform specific options are queried.
    *   \param options returns the export options of the template for the specified platform
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     GetTemplateOptions(RwExpMgrTemplateType type, const string &templateName,
                                       const string &platform, RwCommExportOptions &options);
    /**
    *   Create a template file from the specified options. RwCommOption doesn't hold
    *   hierarchy, but first properties, which have the hierarchy, are loaded from the
    *   defaultTemplate and then options set to the specified values.
    *
    *   \param type of the template
    *   \param templateName name of the template to be saved
    *   \param options to save
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     SetTemplateOptions(RwExpMgrTemplateType type, const string &templateName,
                                       const RwCommExportOptions &options);
    /**
    *   Iterate over templates of specified type and check if there is one with given options.
    *
    *   \param templateName returns the template name if successful
    *   \param type of the templates to check
    *   \param options to check
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     FindTemplateFromOptions(string &templateName, RwExpMgrTemplateType type, 
                                            const RwCommExportOptions &options);

    /**
    *   Get enabled platforms from the specified template.
    *
    *   \param type of the template to query
    *   \param templateName name of the template file
    *   \param platforms returns all the string of the enabled platforms in the specified
    *                    template
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     GetTemplatePlatforms(RwExpMgrTemplateType type, const string &templateName,
                                         vector<string> &platforms);
    /**
    *   Load template properties from the specified template file
    *
    *   \param type of the template to load
    *   \param templateName name of the template file
    *   \param property returns top level property
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     LoadTemplateProperties(RwExpMgrTemplateType type, const string &templateName,
                                           RwExpMgrTemplateProperty &property);

    /**
    *   Rename the specified template.
    *
    *   \param type of the template
    *   \param templateName name of the template file to rename
    *   \param NewName for the template
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     RenameTemplateProperties(RwExpMgrTemplateType type, const string &templateName,
                                             string &NewName);

    /**
    *   Save properties to the template file.
    *
    *   \param type of the template to load
    *   \param templateName name of the template file
    *   \param property to save
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     SaveTemplateProperties(RwExpMgrTemplateType type, const string &templateName,
                                           RwExpMgrTemplateProperty &property);
    /**
    *   Create a basic hierarchy of top level properties. It will leave Output, Common, and
    *   all Platform, but without any options in them.
    *
    *   \param type of the template properties to create
    *   \param property returns the top level property
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     CreateTemplateProperties(RwExpMgrTemplateType type,
                                             RwExpMgrTemplateProperty &property);

    /**
    *   Check if the template file is writable or read-only.
    *
    *   \param type of the template
    *   \param templateName name of the template to check
    *   \param bWritable returns true if the file is writable, false otherwise
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     GetTemplateWritable(RwExpMgrTemplateType type, const string &templateName,
                                        bool &bWritable);
    /**
    *   Set if the template file is writable or read-only.
    *
    *   \param type of the template
    *   \param templateName name of the template to set
    *   \param bWritable true for writable, false for read-only
    *   
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     SetTemplateWritable(RwExpMgrTemplateType type, const string &templateName,
                                        bool bWritable);

    // Versioning.
    /**
    *   Get template version from the version string that is extracted from the template
    *   files. Version string also contains RenderWare version number.
    *
    *   \param strVersion version string
    *
    *   \return Template version number.
    */
    int             GetTemplateVersionFromVersionString(const string &strVersion);
    /**
    *   Create a version string we save in the template files, from the template version
    *   number. Version string also contains RenderWare version number.
    *
    *   \param templateVersion template version number
    *
    *   \return Version string that we stream out in the template files.
    */
    string          GetVersionStringFromTemplateVersion(int templateVersion);

    // Default template files.
    /**
    *   Create all default project and asset templates. This is executed on each start of
    *   the 3D application. If the files exist nothing is done. Missing files are recreated.
    */
    void            CreateDefaultTemplateFiles();

    /**
    *   \param type of the template
    *   \param templateName name of the template to get the path for.
    *   
    *   \return The full path of the specified template.
    */
    string          CreateTemplatePath(RwExpMgrTemplateType type, const string &templateName);

private:
    RwCommError     BuildProperties(RwXMLNodePtr rootNode, RwExpMgrTemplateProperty &rootProperty);
    RwCommError     BuildRwXMLNodes(RwExpMgrTemplateProperty rootProperty, RwXMLNode *rootNode);
    RwCommError     GetPropertiesOptions(RwExpMgrTemplateProperty rootProperty,
                                         RwCommExportOptions &options);

    void            FilterProperties(RwExpMgrTemplateProperty &rootProperty, const string &sLocation);
    void            FilterPropertiesFromOptions(RwExpMgrTemplateProperty &rootProperty, const string &sLocation,
                        RwExpMgrTemplateProperty *pTargetProp, const RwCommExportOptions &options, set<string> &sOptions); 
    RwExpMgrTemplateProperty*   FindProperty(RwExpMgrTemplateProperty &rootProperty, const string &name);
    bool            IsGroup(const RwExpMgrTemplateProperty &property);

    RwCommError     CreatePropertiesFromOptions(RwExpMgrTemplateProperty &props, RwExpMgrTemplateType type,
                            const RwCommExportOptions &options);
    void            SetPropertyFromOptions(RwExpMgrTemplateProperty &props, const RwCommExportOptions &options);
    bool            CompareProperties(RwExpMgrTemplateProperty &props1, RwExpMgrTemplateProperty &props2);
    void            SetPropertyTemplateType(RwExpMgrTemplateType type, RwExpMgrTemplateProperty &prop);
    void            RemoveOutputSection(RwExpMgrTemplateProperty &props);
    string          GetTemplateDescription(const string &sLocation);
};

#endif // RWEXPMGRTEMPLATEMANAGER_H