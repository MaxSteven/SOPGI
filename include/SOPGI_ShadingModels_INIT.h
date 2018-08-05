// EXTRA COLORS_______________________________________________
//___________________________________________________________
vector extraL(int ptid; float cd_mult, factor)
{
    float extraR = fit(rand(ptid+1), 0, 1, 0, cd_mult*factor);
    float extraG = fit(rand(ptid+2), 0, 1, 0, cd_mult*factor);
    float extraB = fit(rand(ptid+3), 0, 1, 0, cd_mult*factor);
    vector finalAdd = set(extraR, extraG, extraB);
    return finalAdd;
}
//___________________________________________________________
// END EXTRA COLORS___________________________________________


// ALL START HERE_____________________________________________
//___________________________________________________________

// Store the old color for GI computation
//v@oldColor = @Cd;

// Let initialize our BRDF coefficients
v@diffuse = {0,0,0};
v@specular = {0,0,0};

//float PI = 3.14159265;
// Camera position
vector cPos = point(2, "P", 0);
// Cam/View direction
vector Vd = normalize(cPos - @P);

// We will retrieve Lights information in a FOR LOOP
// But for now we initialize everything we need in here.
// Light position
vector lPos = {0,0,0};
// Light direction
vector Ld = {0,0,0};
// Light intensity
vector Li = {0,0,0};
// Light multiplier
float Lm = 0;

// Initialize these variables to store Diffuse and Specular accumulation.
vector accumulateDiffuse = {0,0,0};
vector accumulateSpecular = {0,0,0};

// How many lights do we have in the scene?
int lightCount = npoints(@OpInput2);

for (int i=0; i<lightCount; i++)
{
    // Light position
    lPos = point(1, "P", i);
    // Light direction
    Ld = lPos - @P;
    // Light intensity
    Li = point(1, "Cd", i);
    // Light multiplier
    Lm = point(1, "cd_mult", i);

    // Derive the distance
    float distance = length(Ld);
    distance *= distance;

    // Intensity of the diffuse light.
    // Refitting as we are using the wireframe as a comp BG for the GI and we dnt want pure black.
    float lambert = fit(clamp(dot(@N, normalize(Ld)), 0 ,1), 0, 1, @diffuseBlend, 1);
    float on = fit(orenNayar(@N, Ld, Vd, @orenRoughness), 0, 1, @diffuseBlend, 1);
    
    // Apply the diffuse coefficient
    if(@diffType == 0)// Lambert
        accumulateDiffuse += (lambert * Li * v@diffuseColor * @diffusePower / pow(distance, @diffuseExponent)) + extraL(456789, Lm, 0.0005);
    else if(@diffType == 1)// Oren Nayar
        accumulateDiffuse += (on * Li * v@diffuseColor * @diffusePower / pow(distance, @diffuseExponent)) + extraL(452854, Lm, 0.0005);
    
    if(@specType == 0){
        // BLINN PHONG specular coefficient
        accumulateSpecular += ((blinnPhong(@N, Vd, Ld, @specularHardness) * v@specularColor * @specularPower / distance) * Li) + extraL(821566, Lm, 0.0005);
        }
    else if(@specType == 1){
        // COOK TORRANCE specular coefficient
        float cook = cookTorrance(@P, lPos, @N, Vd, @F, @roughness, PI);
        accumulateSpecular += (Li * (v@specularColor * cook)) + extraL(739155, Lm, 0.0001);
        }
        // GGX specular coefficient
    else if(@specType == 2){
        float ggx = GGX(@N, Vd, Ld, @ggxroughness, @ggxF);
        accumulateSpecular += (Li * (v@specularColor * ggx)) + extraL(456321, Lm, 0.0001);
        }
        // GGX1 specular coefficient
    else if(@specType == 3){
        float ggx = GGX1(@N, Vd, Ld, @ggxroughness, @ggxF);
        accumulateSpecular += (Li * (v@specularColor * ggx)) + extraL(159369, Lm, 0.0001);
        }
        // GGX2 specular coefficient
    else if(@specType == 4){
        float ggx = GGX2(@N, Vd, Ld, @ggxroughness, @ggxF);
        accumulateSpecular += (Li * (v@specularColor * ggx)) + extraL(159369, Lm, 0.0001);
        }
    }

// Store final DIFFUSE and SPECULAR coefficients
@diffuse = accumulateDiffuse;
@specular = accumulateSpecular;

// store camera Direction for later use
v@V = Vd;
