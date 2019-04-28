#ifndef _RW_COMM
#define _RW_COMM
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RWCOMM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RWCOMM_API functions as being imported from a DLL, wheras this DLL sees symbols

// defined with this macro as being exported.
#ifdef RWCOMM_EXPORTS
#define RWCOMM_API __declspec(dllexport)
#else
#define RWCOMM_API __declspec(dllimport)
#endif



/**
 * \mainpage RenderWare Graphics - Open Export API Reference
 *
 * <a href="http://www.renderware.com">
 * \image html rwglogo.png
 * </a>
 *
 * <h1> The RenderWare Open Export Framework </h1>
 *
 * The Open Export framework is an open architectured SDK, allowing users to export their art work into
 * RenderWare Graphics. Unlike the previous releases of RenderWare Graphics exporters that were standalone
 * exporter plugins, the framework allows users to customize the default exporter to suit their own needs.
 * Game developers often want to add their own set of art optimization utilities, and organize their art data
 * in their own application specific way. The framework allows users to add their own custom objects and
 * utilities, whilst still reusing the standard RenderWare Graphics main exporter functionality.
 *
 * <h2> Who should use the SDK </h2>
 * The RenderWare Graphics art tool exporters contain a rich set of functionality and data optimization
 * algorithms for exporting 3D art work. Game programmers, wishing to change different aspects of the export
 * process, whilst still reusing the main export functionality, can achieve this using the framework.
 * The framework allows users to create their own custom plugins that can modify and extend the RenderWare
 * Graphics exporters. 
 *
 * <h2> What you should know </h2>
 * This user guide makes some assumptions about your level of proficiency with real time 3D graphics and
 * 3D modeling tools. It is not aimed at complete newcomers to the fields of computer graphics and 3D 
 * scene graphs.
 * In addition, in order to use the SDK you need to know how to program in C++. You should also be familiar
 * with object-oriented programming concepts.
 *
 * <h2> Where to get started </h2>
 * The first step in understanding the Open Export architecture should be reading the \ref getstart section.
 * It contains an overview of the basic principles of the architecture and an introduction of how to
 * extend the exporters.
 * \li \ref oeworkflow "Basic Workflow" Explains the basic workflow of the export process
 * \li \ref oeacrh "Open Export Architecture" Libraries of the Open Export architecture
 * \li \ref commplugins "Custom Plugins" How to make an Open Export plugin
 *
 * If you are not new to RenderWare Graphics and Open Export framework, then you should go to the \ref
 * commpluginsp6 section. This is a brief summery of API changes between the two releases,
 * that would require you to recompile and make changes to your plugins.
 *
 * <h2> How the API Reference is split </h2>
 * This API Reference contains the following sections: 
 *  \li \ref rwcomm - Containing the abstract base classes for the scene graph, the scene graph builder and 
 *              the scene graph exporter.
 *  \li \ref rwexp - The RenderWare specific implementation library of the scene graph exporter mechanism. This
 *              library is responsible for exporting the RwComm scene graph to RenderWare binary files (rws/rp2/rg1/rx1).
 *  \if 3dsmax_build
 *  \li \ref rwmax - The 3ds max specific implementation library of the scene graph and the scene graph builder
 *              mechanism. This library is responsible for building the RwMax scene graph (derived from
 *              the RwComm scene graph), from a 3ds max scene.
 *  \endif
 *  \if maya_build 
 *  \li \ref rwmaya - The Maya specific implementation library of the scene graph and the scene graph builder
 *              mechanism. This library is responsible for building the RwMaya scene graph (derived from
 *              the RwComm scene graph), from a Maya scene.
 *  \endif
 *  \li \ref rwmi - A modeler-independent implementation library of the scene graph. This library only contains
 *              a generic scene graph implementation, without the builder mechanism.
 *  \li \ref rwrf3 -    The rf3 specific implementation of the scene graph builder mechanism and the scene graph
 *              exporter mechanism. This library is responsible for building an RwMI scene graph from an rf3
 *              file, and exporting an RwComm scene graph to an rf3 file.
 *  \li \ref rwexpmgr - The export manager library. This library contains the main class that manages the entire
 *              export process.
 *
 * You shouldn't need to know all the layers in the architecture to solve your specific problem. To get
 * an idea which sections should you read in more details refer to the \ref getstart pages.
 *
 * <h2> What you should use to build </h2>
 * The OpenExport libs and dlls are built with Visual C v6 (service pack 5 with the Processor Pack).
 * Any OpenExport plugins you create should be built with the same compiler. Run-Time Type Information
 * (RTTI) should be enabled and you should build against the multi-threaded dll run time libraries.
 *
 *  The Open Export framework is now shipped with debug libraries. All debug libraries have an extra "d" at
 *	the end. Users, developing plugins, can link against the debug libraries as long as the main hosting 
 *  application also links against the debug libraries. For 3dsmax and Maya, there is a debug version of the
 *  main plugin under the debug_plugins folder. Make sure to copy it to the 3dsMax/Maya plugin folder in order
 *  to use it with your debug plugins.
 *  Since the rf3cc tool also loads Open Export plugins, make sure to rebuild
 *  the rf3cc in debug mode, in order for the tool to pick up your plugins.
 *  By default, the plugin manager checks that debug plugins will not load with release applications and vice
 *  versa. In order to disable this check, compile your plugin with the _NODBGCHECK flag.
 *  
 *
 * <h2> Copyright Notice </h2>
 *
 * The information in this document is subject to change without notice
 * and does not represent a commitment on the part of Criterion Software
 * Ltd. The software described in this document is furnished under a
 * license agreement or a non-disclosure agreement. The software may be
 * used or copied only in accordance with the terms of the agreement.
 * It is against the law to copy the software on any medium except as
 * specifically allowed in the license or non-disclosure agreement.
 *
 * No part of this documentation may be reproduced or transmitted in
 * any form or by any means for any purpose without the express written
 * permission of Criterion Software Ltd.
 *
 * Copyright © 1993 - 2003 Criterion Software Ltd. All rights reserved.
 *
 * Canon and RenderWare are registered trademarks of Canon Inc. Nintendo is
 * a registered trademark and NINTENDO GAMECUBE a trademark of Nintendo Co.,
 * Ltd. Microsoft is a registered trademark and Xbox is a trademark of
 * Microsoft Corporation. PlayStation is a registered trademark of Sony Computer
 * Entertainment Inc. 3d studio max, character studio are registered trademarks 
 * and Discreet is a trademark of Autodesk/Discreet in the USA and/or other 
 * countries.
 *
 * Copyright © 2002 ImageMagick Studio LLC, a non-profit organization dedicated to
 * making software imaging solutions freely available.
 *
 * All other trademarks mentioned herein are the property of their respective
 * companies.
 *
 */

/**
*   \ingroup rwcomm  
*
*  \page RwComm Overview
*
*   The RwComm library contains a set of abstract classes, which together form an abstract 3D scene graph
*   architecture. This scene graph encapsulates common 3D functionality, shared by most modeling applications,
*   such as 3ds max, Maya and SoftImage. The scene graph contains a neutral set of interfaces, independent of
*   a specific modeling application, but rich enough to encapsulate most of the relevant functionality.
*   The scene graph acts as a database holding all of the 3D world we are exporting. It does not contain any
*   rendering, modeling or geometrical functionality and shouldn't be confused with other 3D scene 
*   graph architectures. Its soul purpose is to encapsulate 3D data for exporting.
*   
*   The RwComm library is an abstract low level library and does not contain any specific implementation.
*   It does not know anything about RenderWare Graphics, 3ds max, Maya or any other specific modeling tool.
*   It creates a generic framework for exporting 3D data from any modeling tool to another specific 3D format.
*   The basic workflow of the export process begins by building the scene graph, which is a snapshot of the
*   3D world we are about to export. The next phase in the process involves traversing this scene graph and
*   exporting a set of 3D elements of interest. This section consists of the various scene graph
*   elements, the scene graph builder mechanism and the scene graph exporter mechanism.
*/

#define RWCOMM_VERSION_STRING   3,5,0,2
#define RWCOMM_VERSION_FUNC     "RwCommGetVersion"
#define RWCOMM_MAKE_VERSION(v1,v2,v3,v4) ((v1 << 24) | (v2 << 20) | (v3 << 16) | v4)

#if (!defined(DOXYGEN))

// This class is exported from the RwComm.dll
class RWCOMM_API CRwComm {
public:
    CRwComm(void);
    // TODO: add your methods here.
};

#endif // (!defined(DOXYGEN))

extern RWCOMM_API int nRwComm;

RWCOMM_API int fnRwComm(void);

#endif 

