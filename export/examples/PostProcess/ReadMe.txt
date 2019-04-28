========================================================================
       DYNAMIC LINK LIBRARY : Stream
========================================================================


This example demonstrates how to post process the entire list of exported RenderWare assets,  and how to customize the stream process for streaming them out. 

It does this by overriding the Export Manager class, and implementing its StreamAssets()
method. The method traverses the asset list, looking for RpClumps. Each time a clump is
encountered in the list, it is cloned and modified by some trivial modification function
(that just changes all the RpMaterial colors in the clump). The modified clump is added
to a new asset list and is streamed out. We stream out multiple reprsenations of the clump,
each one containing different data (in our case - different color).

This mechanism can be used , for example, for generating multiple LOD representations of the
same mesh data. Each representation can be streamed out to a seperate file, where it can
be loaded dynamically, at runtime, according to the LOD view criteria.


/////////////////////////////////////////////////////////////////////////////
