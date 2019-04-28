========================================================================
       DYNAMIC LINK LIBRARY : OptTex
========================================================================


This is an example of a custom object exporter. The exporter derives from 
the RwExpRwTexture class, implementing its Export() and ExtractImages() methods.

The custom exporter optimizes the exported textures , by making sure that all texture sizes
are beneath a certain threshold.


/////////////////////////////////////////////////////////////////////////////
