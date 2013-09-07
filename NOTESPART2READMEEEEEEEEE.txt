notes for damage shader
parralax occlusion mapping with per pixel height map for light damage 
displacement map with its own seperate height map for heavy damage
each object will will have its own diffuse normal and specular map
damage accumulation technique each pass for a different map swapping render targets 
// note to self be careful with bullets when rendering we are going to likely have not fun frame rate
render object technique will move the verts and use the pixel hight map to render with parallax occlusion heigth mapping
include damage decal as well as the damaged texture set for the main model 
// note need to make the damaged texture set for the big cube thing
// idea 2 projectiles one inflicts ligth damage the other inflicts heavy damage
// additional idea 3rd projectile inflicts hybrid damage 