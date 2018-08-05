//GI Multiplier
//This will increase the power of any photons that bounce on this surface.
f@gi_mult = chf("gimultr");
//This will increase the power of any photons that come from this surface.
f@gi_mult_emitted = chf("gimulte");;

//BLINN PHONG parameters
//ALL the diffuse parameters will apply to the COOK TORRANCE shading model as well.
//(The diffuse coefficient is derived from a Lambertian function and it is shared from both the shading models)
v@diffuseColor = set(ch("difcolorr"), ch("difcolorg"), ch("difcolorb")) + extraL(123456, 0.001);;
f@diffusePower = chf("difpower");
f@diffuseExponent = chf("diffexp");
f@diffuseBlend = chf("diffblned");
f@orenRoughness = chf("orenrough");
//The specular color also is shared between both the shading models
v@specularColor = set(ch("speccolorr"), ch("speccolorg"), ch("speccolorb")) + extraL(456789, 0.001);;
//specular only for BlinnPhong
f@specularPower = chf("blinnspecpower");
f@specularHardness = chf("blinnspechard");
//COOK TORRANCE parameters (Specular)
f@roughness = chf("cookroughness");
f@F = chf("cookfresnelReflectance");
f@ggxroughness = chf("ggxrough");
f@ggxF = chf("ggxf");

// REFLECTIONS
i@refldo = ch("reflections");
if(ch("refractions")==1)
    {
    i@refldo = 0;
    f@reflvalue = 1;
    }
else
    f@reflvalue = ch("reflectionvalue");
// REFRACTIONS
i@refrdo = ch("refractions");
if(i@refrdo==0)
    f@IOR = 1;
else
    f@IOR = ch("ior");

f@addrefl = ch("addrefl");
if(ch("reflections")==0)
    f@addrefl=0;
    
// RECURSIVE SAMPLES
i@recsamples = chi("recsamples");
i@recsamplesblur = ch("recsamplesblur");

i@diffType = 0;
if(ch("diffmodel") == 0) @diffType = 0;
else if(ch("diffmodel") == 1) @diffType = 1;
i@specType = 0; //Witch Shading Model am I ??
    //BLINN PHONG
    if(ch("shadingmodel") == 0) @specType = 0;       // BLINN PHONG
    //COOK TORRANCE
    else if(ch("shadingmodel") == 1) @specType = 1;  // COOK TORRANCE
    else if(ch("shadingmodel") == 2) @specType = 2;  // GGX
    else if(ch("shadingmodel") == 3) @specType = 3;  // GGX1
    else if(ch("shadingmodel") == 4) @specType = 4;  // GGX2
    //else if(ch("shadingmodel") == 3) @shdType = 4;
    //Add new one in here....
    
//Set color. This is important as we are going to use it for the GI calculation.
//Since this is not a rasterized render the only thing we have is the base wireframe color.
@Cd = v@diffuseColor;