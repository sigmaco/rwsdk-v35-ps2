========================================================================
       DYNAMIC LINK LIBRARY : ScaleAnim
========================================================================

This example adds support for animated scale to the RwExp layer.

The RwExpFrame and RwExpMatrixAnimation creators are overridden and the
CalcTransform method is modified to leave scale in place.

A new RpHAnim keyframe type is registered that supports scale.

The RwExpHAnimHierarchy and RwExpBuildHAnimAnimation actions are overriden in order
to ensure the RpHAnimHierarchy and RpHAnimAnimation created make use of the new
keyframe type.

The RwExpScaleFilter and RwExpOffsetFilter vertex filters are overriden so that
scale is not burnt into geometries.


RenderWare Graphics Run-Time Usage
==================================

Support for the scale animation implemented in this exporter example
is not included by default in RenderWare Graphics, however adding support
is relatively simple. The files scalekey.c and scalekey.h contain all the
necessary code to allow usage of scale animations in RenderWare Graphics
applications.
 
To use this support you must compile scalekey.c into your application
and call HAnimFullKeyFrameRegister from the scalekey source after
initialization of RenderWare Graphics and prior to loading any scalekey
based assets. Once this has been completed animations exported containing
scale information can be treated in the same way all other animations are
treated.
 
/////////////////////////////////////////////////////////////////////////////
