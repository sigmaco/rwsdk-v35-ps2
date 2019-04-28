// RwExpMgrExportManager.h: interface for the RwExpMgrExportManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPMGREXPORTMANAGER_H__85D44548_3589_4D0A_B80E_9E3B5EE60A6F__INCLUDED_)
#define AFX_RWEXPMGREXPORTMANAGER_H__85D44548_3589_4D0A_B80E_9E3B5EE60A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RwExpMgrExportManager;

#define INIT_EXPORT_MANAGER_FUNC    "InitExportManager"
#define SET_EXPORT_MANAGER_FUNC     "SetCustomExportManager"

typedef void (*RwInitExportManagerFunc)  (RwExpMgrExportManager *pExportManager);

typedef void (*RwSetExportManagerFunc)(RwExpMgrExportManager *&pExportManager);


/**
*
*  \ingroup exportmanager
*
*   The RwExpMgrExportManager class is repsonsible for managing the entire export process including
*   building the scene graph, exporting and launching a viewer. You can export to or from rf3
*   files, define your own custom builders and exporters, customize options, saving of
*   the files, viewing etc.
*
*   \see RwCommBuilder, RwExpExporter RwRf3Builder, RwRf3Exporter
*/
class RWEXPMGR_API RwExpMgrExportManager  
{
public:
	virtual ~RwExpMgrExportManager();
    /**
    *   \return static instance of the class. RwExpMgrExportManager is a singleton.
    */
    static RwExpMgrExportManager *TheInstance();

    /**
    *   \return the registered RwExpAnmExporter.
    */
    RwCommExporter          *GetAnmExporter();
    /**
    *   \return the registered RwExpBspExporter.
    */
    RwCommExporter          *GetBspExporter();
    /**
    *   \return the registered RwExpDffExporter.
    */
    RwCommExporter          *GetDffExporter();
    /**
    *   \return the registered RwExpSplExporter.
    */
    RwCommExporter          *GetSplExporter();
    /**
    *   This method returns the exporter, mapped to the given asset type.
    *   \param assetType the asset type of the exporter
    *   \returns the exporter if successful. NULL otherwise.
    */
    RwCommExporter          *GetExporter(int assetType);

    /**
    *   \return the registered builder.
    */
    RwCommBuilder           *GetBuilder()     { return m_pBuilder; };
    /**
    *   \return the registered RwRf3Exporter.
    */
    RwRf3Exporter           *GetRf3Exporter() { return m_pRf3Exporter; };
    /**
    *   \return the registered RwRf3Builder.
    */
    RwRf3Builder            *GetRf3Builder()  { return m_pRf3Builder; };

    /**
    *   This function registers an anm exporter with the export manager.
    *   \param anmExporter RwExpAnmExporter to register
    */
    void                    SetAnmExporter(RwCommExporter *anmExporter);
    /**
    *   This function registers a bsp exporter with the export manager.
    *   \param bspExporter RwExpBspExporter to register
    */
    void                    SetBspExporter(RwCommExporter *bspExporter);
    /**
    *   This function registers a dff exporter with the export manager.
    *   \param dffExporter RwExpDffExporter to register
    */
    void                    SetDffExporter(RwCommExporter *dffExporter);
    /**
    *   This function registers an spl exporter with the export manager.
    *   \param splExporter RwExpSplExporter to register
    */
    void                    SetSplExporter(RwCommExporter *splExporter);
    /**
    *   This function registers a custom exporter with the export manager. Users can assign their own
    *   custom exporters to an asset type.
    *   \param assetType the type of the asset to map the exporter to
    *   \param pExporter a pointer to the exporter
    */
    void                    SetExporter(int assetType, RwCommExporter *pExporter);
    /**
    *   This function registers a builder with the export manager.
    *   \param pBuilder builder to register
    */
    void                    SetBuilder(RwCommBuilder *pBuilder);
    /**
    *   This function registers an rf3 exporter with the export manager.
    *   \param pRf3Exporter rf3 exporter to register
    */
    void                    SetRf3Exporter(RwRf3Exporter *pRf3Exporter);
    /**
    *   This function registers an rf3 builder with the export manager.
    *   \param pRf3Builder rf3 builder to register
    */
    void                    SetRf3Builder(RwRf3Builder *pRf3Builder);

    /**
    *   \return True if export is in progress. This is based on m_bExportInProgress which is set to true
    *           while export process is on.
    */
    bool                    GetExportInProgress() { return m_bExportInProgress; };

    /**
    *   \param bInProgress value to set the internal variable. When set to true analysis of the 3d
    *          application scene is disabled.
    */
    void                    SetExportInProgress(bool bInProgress) { m_bExportInProgress = bInProgress; };

    /**
    *   This functions exports a set of assets. It first calls ProcessOuputLocation() and ProcessProjectTemplate()
    *   to setup the export manager state for the export. It then calls BuildScene() to create an RwCommScene.
    *   It then calls PreExport(), ExportScene() and finally PostExport() to perform the actual export.
    *
    *   \param vAssets a vector of assets to export
    *   \param sProjectTemplate name of the project template
    *   \param sSceneName name of the file to save
    *   \param sExportPath path where to save the file
    *   \param pOptionOverrides an optional set of option overrides. If non NULL these options are added to the final set of export options.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     Export(const vector<RwCommAsset *> &vAssets, const string &sProjectTemplate,
                            const string &sSceneName, const string &sExportPath = "",
                            RwCommExportOptions *pOptionOverrides = NULL);

    /**
    *   This functions exports a set of Rf3 files. It first calls ProcessOuputLocation() and ProcessProjectTemplate()
    *   to setup the export manager state for the export. It then calls PreExport() to start the RenderWare
    *   engine and allocate asset lists. It then iterates through all the rf3 files and builds an RwCommScene
    *   for each using ReadRF3File() and exports it by by calling ExportScene(). Finally it calls PostExport()
    *   to stream assets and close the RenderWare engine.
    *
    *   \param vFiles a vector of rf3 files to export
    *   \param sProjectTemplate name of the project template
    *   \param sSceneName name of the file to save
    *   \param sExportPath path where to save the file
    *   \param pOptionOverrides an optional set of option overrides. If non NULL these options are added to the final set of export options.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     Export(const vector<string> &vFiles, const string &sProjectTemplate,
                            const string &sSceneName, const string &sExportPath = "",
                            RwCommExportOptions *pOptionOverrides = NULL);

    /**
    *   This function performs an export and view of a set of assets.
    *   It first ensures that previous temporary view files are removed by calling
    *   DeleteTempViewFiles(). It then sets up  m_viewProjectOptions by calling SetViewProjectOptions().
    *   It then performs the export to a temporary location using the Export() function and views it
    *   using the ViewFile() function. It finally clears m_viewProjectOptions by calling ClearViewProjectOptions(). 
    *
    *   \param vAssets a vector of asset to view
    *   \param sProjectTemplate name of the project template
    *   \param pOptionOverrides an optional set of option overrides. If non NULL these options are added to the final set of export options.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     View(const vector<RwCommAsset *> &vAssets, const string &sProjectTemplate,
                            RwCommExportOptions *pOptionOverrides = NULL);

    /**
    *   This function performs an export and view of a set of rf3 files.
    *   The function first ensures that previous temporary view files are removed by calling
    *   DeleteTempViewFiles(). It then sets up the m_viewProjectOptions by calling SetViewProjectOptions().
    *   It then performs the export to a temporary location using the Export() function and views it
    *   using the ViewFile function. It finally clears m_viewProjectOptions by calling ClearViewProjectOptions(). 
    *
    *   \param vFiles a vector rf3 files to view
    *   \param sProjectTemplate name of the project template
    *   \param pOptionOverrides an optional set of option overrides. If non NULL these options are added to the final set of export options.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     View(const vector<string> &vFiles, const string &sProjectTemplate,
                            RwCommExportOptions *pOptionOverrides = NULL);

    /**
    *   Initialize all per asset options such as Sub-Hierarchy Animation, Dynamic Key Frame Generation,
    *   Start Frame, End Frame, etc to their default values. This function is called from ProcessProjectTemplate().
    *   
    *   \param options to initialize
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     InitializePerAssetOptions(RwCommExportOptions &options);

protected:
    RwExpMgrExportManager();

    // Overridables
    /**
    *   This function is called in the main export functions before ExportScene is called. It start the
    *   RenderWare Engine and calls AllocateAssetLists.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     PreExport();
    /**
    *   This function is called at the end of the main export functions and is responsible for streaming
    *   the asset data out, pre-instancing platform specific data and closing the RenderWare Engine.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     PostExport();

    /**
    *   This function simply uses the RwExpStreamAssetList::StreamToFile. The TOC paramater is setup
    *   based on the RWS_EMBEDTOC option from the current project template.
    *   \param pAssetList an asset list to stream out
    *   \param filename destination file name
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     StreamAssets(RwExpStreamAssetList *pAssetList, const string &filename);
    /**
    *   Create an RwExpStreamAssetList for each platform we'll be exporting to. Called from PreExport()
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     AllocateAssetLists();
    /**
    *   Delete the RwExpStreamAssetList vector we created in AllocateAssetLists().
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     FreeAssetLists();

    /**
    *   Do the actual export. Called from Export(). Loops all platforms and all assets.
    *   and call ExportAsset().
    *
    *   \param pScene a pointer to the scene graph to export
    *   \param vAssets a vector of all the assets to export from the scene graph
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     ExportScene(RwCommScene *pScene, const vector<RwCommAsset *> vAssets);
    /**
    *   Export an asset. Calls RwCommExporter::Export() for the appropriate registered exporter.
    *
    *   \param pScene a pointer to the scene graph to export
    *   \param pAsset a pointer to an assets to export from the scene graph
    *   \param options for the export process
    *   \param pAssetList where to store the exported data
    *   \param Platform to which we export
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     ExportAsset(RwCommScene *pScene, RwCommAsset *pAsset,
                                RwCommExportOptions &options, RwExpStreamAssetList *pAssetList, const string &Platform);
    /**
    *   Export the ambient light from a scene.
    *
    *   \param pScene a pointer to the scene graph to export
    *   \param pAssetList where to store the exported data
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     ExportAmbientLight(const RwCommScene *pScene, 
                                RwExpStreamAssetList *pAssetList);

    /**
    *   Build a legacy file name for the asset. It will append "_PS2" "_Xbox" "_GameCube" if
    *   the platform is not generic.
    *
    *   \param sPath location where to save the asset
    *   \param pAsset a pointer to an assets to export from the scene graph
    *   \param assetType type of the asset, animated hierarchy, static world etc.
    *   \param Platform to which we export
    *
    *   \return The full path for the legacy file.
    */
    virtual string          BuildLegacyFileName(const string &sPath, const RwCommAsset *pAsset, const int assetType, const string &Platform);
    /**
    *   Append the right extension depending on which platform we're export for ".rp2", ".rx1", ".rg1" or ".rws".
    *
    *   \param sPath location in which to save the asset
    *   \param sName a file name without the extension and the path
    *   \param sPlatform to which we are exporting
    *
    *   \return The full path for the rws file.
    */
    virtual string          BuildRwsName(const string &sPath, const string &sName, const string &sPlatform);

    /**
    *   This function attempts to pre-instance the given file for the given platform. This is done
    *   using the Instance function from RwInstance.dll. On the Xbox platform DDS format textures are
    *   extracted from the asset list using GetTypeTextureNames and passed explicitly to the instancer.
    *
    *   \param sFileName full path of the file to instance.
    *   \param sPlatform the platform to instance on.
    *   \param pAssetList the current platform asset list.
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     PreInstanceFile(const string &sFileName, const string &sPlatform, RwExpStreamAssetList *pAssetList);
    /**
    *   View an rws file. This is called from View() with a file that is saved in a
    *   temporary location.
    *
    *   \param sFileName full path of the rws file to view
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     ViewFile(const string &sFileName);

    /**
    *   Create a partition object if any Static World asset requires
    *   partition visualization
    *
    *   \param vAsset a vector of assets to process for partition visualization
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    virtual RwCommError     CreateAssetPartitionObject( const vector<RwCommAsset *> &vAssets );

protected:
    /**
    *   Build the scene graph from an rf3 file using the registered RwRf3Builder.
    *   
    *   \param sceneFile full path of the rf3 file
    *   \param pScene returns a scene graph
    *   \param vAssets returns a vector of assets
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     ReadRF3File(const string &sceneFile, RwCommScene *&pScene,
                            vector<RwCommAsset *> &vAssets);
    /**
    *   Export an rf3 file from the scene graph using the registered RwRf3Exporter. It first
    *   constructs a vector of RwRf3AssetInfo objects from the passed assets using the 
    *   ProcessAssetOptions function. Depending on the RF3_SPLITBYASSET option it then
    *   makes a single or multiple calls to RwRf3Exporter::ExportFromAssets.
    *   
    *   \param pScene pointer to the scene graph
    *   \param vAssets vector of assets to export
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     SaveRF3File(RwCommScene *pScene, const vector<RwCommAsset *> &vAssets);

    /**
    *   Retrieve the selection for this asset. If the asset has parents the selection should include
    *   the parent selections.
    *
    *   \param pAsset to retrieve the selection from
    *   \param pScene pointer to the scene graph
    *   \param vSelection vector of selected objects in the scene to be filled in
    *
    */
    GetAssetSelection(RwCommAsset *pAsset, RwCommScene *pScene, vector<RwCommTransform *> &vSelection);

    /**
    *   This function sets up export data that relies on the contents of the project template.
    *   It first extracts the platforms to be targetted using RwExpMgrTemplateManager::GetTemplatePlatforms
    *   and stores them in the m_vPlatforms member. It then sets up the default template options using
    *   RwExpMgrTemplateManager::GetTemplateOptions. It then adds the common options from the specified
    *   project template using the same function. It finally extracts the platform specific options from the
    *   project template and stores the concatenated options in the m_vPlatformProjectOptions member.
    *
    *   \param sProjectTemplate the project template file to be processed.
    *   \param pOptionOverrides an optional set of option overrides. If non NULL these options are added to the final set of export options.
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     ProcessProjectTemplate(const string &sProjectTemplate, RwCommExportOptions *pOptionOverrides = NULL);
    /**
    *   Get all the options for this asset. The functions first recurses up the hierarchy if the
    *   asset has parents. It then extracts common followed by platform specifc options from the asset
    *   template linked to by the asset (RwCommAsset::GetAssetTemplateName). The template manager,
    *   RwExpMgrTemplateManager, is used to extract options from the template file. The functions finally
    *   adds in any per-asset artists options returned from RwCommAsset::GetOptions. The order in which
    *   options are extracted is important as the RwCommExportOptions implementation of operator+ overides
    *   options that already exist.
    *
    *   \param pAsset asset to get the options for
    *   \param sPlatform platform that we export to
    *   \param pScene pointer to the scene graph
    *   \param options returns all merged options
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     ProcessAssetOptions(const RwCommAsset *pAsset, const string &sPlatform,
                            RwCommScene *pScene, RwCommExportOptions &options);
    
    /**
    *   This function checks the per-asset options for any problems. Currently it checks that the
    *   animation end frame is later than the start frame. If this is not the case the function
    *   sets the end frame to the start frame + 1. The function also replaces the "Start" and "End"
    *   animation frame special cases with the current scene start and end frames.
    *
    *   \param options the options to be checked
    *   \param pScene pointer to the scene graph
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     CheckAssetOptions(RwCommExportOptions &options, RwCommScene *pScene);

    /** This function sets up the m_sSceneName and m_sExportPath member variables. It ensures
    *   these variables have sensible defaults in the case of empty strings. Forward slashes
    *   are convertes to back slashes.
    *
    *   \param sSceneName the output scene name.
    *   \param sExportPath the export path.
    */
    RwCommError     ProcessOuputLocation(const string &sSceneName, const string &sExportPath);

    /** Set the project option overrides used when viewing. The options should be stored in the
    *   m_viewProjectOptions member and will used to override the normal options when performing views.
    *   By default these options will disable all file exports other than .rws. It also enables
    *   texture and effect dictionaries and disables pre-instancing.
    */
    void            SetViewProjectOptions();
    
    /** Clear all viewing options */
    void            ClearViewProjectOptions();

    /**
    *   Outputs statistics on the texture dictionary using RwExpOutputTextureDictionaryStats and
    *   then embeds the Platform Indpendent dictionary returned by RwExpStreamAssetList ::GetTextureDictionary.
    *   \param pAssetList an asset list where to embed the texture dictionary.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     EmbedTextureDictionary(RwExpStreamAssetList *pAssetList);
    /**
    *   Creates an effect dictionary using RpMTEffectDictCreate and then add the
    *   unique effect file names from RwExpStreamAssetList ::GetEffectFiles. This
    *   effect dictionary is then added to the asset list.
    *   \param pAssetList an asset list in which to embed the effect dictionary.
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     EmbedEffectDictionary(RwExpStreamAssetList *pAssetList);
    /**
    *   This function creates an ambient RenderWare light clump and adds a new clump entry to the asset list.
    *   \param pAssetList an asset list in which to embed the ambient light
    *   \param ambientColor ambient light color
    *
    *   \return RWCOMM_SUCCESS if successful, RWCOMM_FAIL otherwise.
    */
    RwCommError     EmbedAmbientLight(RwExpStreamAssetList *pAssetList, const RwCommVec3 &ambientColor);

    /**Build the scene graph using the registered builder */
    RwCommScene     *BuildScene();

    /**
    *   Set up parameters for the progress bar. The progress bar iteration count is setup based on
    *   m_vPlatforms * vAssets.size().
    *   \param vAssets the assets being exported.
    */
    void            SetupProgressBar(const vector<RwCommAsset *> &vAssets);
    
    /** Start timing of the export process. Prints an output message and initialises the m_tStart member.*/
    void            StartTimer();
    
    /** End timing of the export process. Stores the end time in m_tEnd and outputs the export duration.*/
    void            EndTimer();

    /** Fills in a vector of enabled assets.
    *   \param vAssets an vector of assets which may be enabled or disabled.
    *   \param vEnabledAssets a vector of the enabled assets.
    */
    void            GetEnabledAssets(const vector<RwCommAsset *> &vAssets,
                                     vector<RwCommAsset *> &vEnabledAssets);
    /**
    *   Some assets are organized in a hierarchy like Animated Hierarchy and Animations.
    *   This function flattens this hierarchy so that subsequent code does not need to
    *   travserse the asset hierarchy.
    *   \param vAssets a vector of source assets
    *   \param vFlatAssets a flattened output vector of assets.
    */
    void            FlattenAssetVector(const vector<RwCommAsset *> &vAssets,
                                       vector<RwCommAsset *> &vFlatAssets);
    /**
    *   Add an asset and its children to a vector. If the asset is already a member of the vector
    *   it is not added.
    *   \param pAsset the asset to be added.
    *   \param vAssets an output vector of assets.
    */
    void            AddAssetToVector(RwCommAsset *pAsset, vector<RwCommAsset *> &vAssets);
    /**
    *   Run through all the textures in the asset list and extract the ones with the
    *   specified extension.
    *   \param pAssetList the asset list to search.
    *   \param extension the file extension to be matched.
    *   \param matchingNames an output vector of matching names.
    */
    const void      GetTypeTextureNames(RwExpStreamAssetList *pAssetList, string &extension,
                                           vector<string> &matchingNames);

    /**
    *   View an rws file. This is called from View() with a file that is saved in a
    *   temporary location.
    *
    *   \param sTmpFiles files to delete. Wildcards  a allowed. For example c:\temp.*
    *   \param sPath files to delete. Wildcards  a allowed. For example c:\temp.*
    */
    void            DeleteTempViewFiles(const string &sTmpFiles, const string &sPath);
    /**
    *   This function checks whether the passed file exists or not. This is used to check
    *   the export completed sucessfully before starting the viewer.
    *   \param sTmpFiles location of the file to check
    *
    *   \return True if the file exists, false otherwise.
    */
    bool            TempViewFilesExist(const string &sTmpFiles) const;

    RwCommScene     *WriteAndReadRf3(RwCommScene *pScene, vector<RwCommAsset *> &vAssets);

protected:

    string                          m_sSceneName;
    string                          m_sExportPath;
    time_t                          m_tStart, m_tEnd;

    RwCommExportOptions             m_projectOptions;
    RwCommExportOptions             m_viewProjectOptions;
    vector<string>                  m_vPlatforms;
    vector<RwExpStreamAssetList *>  m_vPlatformAssetLists;
    vector<RwCommExportOptions>     m_vPlatformProjectOptions;
    bool                            m_bViewOp;
    bool                            m_bExportInProgress;
    bool                            m_bExportAborted;

private:

    static RwExpMgrExportManager    *m_pInstance;

    RwCommBuilder                   *m_pBuilder;
    
    RwRf3Builder                    *m_pRf3Builder;
    RwRf3Exporter                   *m_pRf3Exporter;

    map<int, RwCommExporter*>       m_mExporters;
};

#endif // !defined(AFX_RWEXPMGREXPORTMANAGER_H__85D44548_3589_4D0A_B80E_9E3B5EE60A6F__INCLUDED_)
