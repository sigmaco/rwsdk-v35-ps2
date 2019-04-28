========================================================================
       DYNAMIC LINK LIBRARY : PreLight
========================================================================


This is an example of a vertex filter which pre-lights a scene. It uses a very
basic phong lighting equation:

I = Ia*Ka + Id*Kd(N*L), where

Ia - light's ambient intensity
Id - light's diffuse intensity
Ka - material's ambient valiue
Kd - material's diffuse value
N - the vertex's normal
L - Vector form the vertex facing the light

It calcualtes the color value for each vertex and bakes it in each
vertex's color.


/////////////////////////////////////////////////////////////////////////////
