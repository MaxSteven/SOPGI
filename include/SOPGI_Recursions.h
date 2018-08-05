// SOPGI REFLECTIONS AND REFRACTIONS
// This node will calculate reflections and refractions rays recursively.
// There are some parts that have been coded quick and dirty but the ray's splitting
// for the recursive reflections and refractions works great
// witch was what I really wanted to do after all.


// So, for those who dnt know or never really tried to code recursions,
// A recursion is essentially a function that run inside itself.
// I end up in building a raycasting function that is recursive by itself so I just need to call it once
// every time I need to calculate reflections and refractions and it will trace rays and split them
// into the scene based on the recursion depth value and return me the final color, just in one call.

// CASTARAY START_______________________________________________________________________________________________________
// _______________________________________________________________________________________________________________________

vector castARaySTART(vector raydirStart, pos, N; float addrefl, IOR, recSamplesBlur; string collision, returnAttributeName, rflCeck, rfrCeck, iorCheck; int samples, rfl, rfr, getDataFrom, recursions, debugAddPoints)
{
    vector hit, raydir;
    float u, v;
    int hitprim;
    vector dir = {0,0,0};
    float offset = 0.0001;
    float angle = 0;
    float cone = radians(angle);
    float maxdist = 100000000;
    vector tmp_vector = {0,0,0};
    vector tmp = {0,0,0};
    
if(rfl || rfr)
    {
    for(int i=0; i<samples; i++)
    {
    raydir = sample_direction_cone((vector)raydirStart, recSamplesBlur, vector2(set(rand(pos+1.23456+i*4.56789),rand(pos+1.23456+i*4.56789))));

    hitprim = intersect(collision, pos+(raydir*offset), raydir*maxdist, hit, u, v);

    if(debugAddPoints==1)
        {
        int hitpt = addpoint(0, hit);
        tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0)));
        setpointattrib(0, "Cd", hitpt, tmp_vector, "set");
        }

    int refldoCheck = primuv(getDataFrom, "refldo", hitprim, vector(set(u,v,0)));
    int refrdoCheck = primuv(getDataFrom, "refrdo", hitprim, vector(set(u,v,0)));
    float addRFL = primuv(getDataFrom, "addrefl", hitprim, vector(set(u,v,0)));
    float tmp_ior = primuv(getDataFrom, iorCheck, hitprim, vector(set(u,v,0)));
    
    float NdotDIRStart = dot(N, raydir);
    if(NdotDIRStart<0)
        tmp_vector = 0;
    else
        tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0))) * addrefl;
    
    tmp = tmp_vector;
    vector src_pos = pos;

    if(hitprim==-1)
        {
        tmp_vector = {0,0,0};
        break;
        }
    else
        {
        for(int j=0; j<recursions; j++)
            {
            vector prpt_n = primuv(getDataFrom, "N", hitprim, vector(set(u,v,0)));
            float NdotDIR = dot(prpt_n, dir);

            if(refldoCheck==0 && refrdoCheck==0)
                {
                if(NdotDIRStart<0)
                    tmp_vector = 0;
                else
                    tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0)));
                tmp = tmp_vector;
                }
            
            if(refldoCheck)
                {
                dir = -reflect((normalize(src_pos - hit)), prpt_n);
                }
            else if(refrdoCheck)
                {
                dir = -reflect((normalize(src_pos - hit)), prpt_n);
                }
        
            src_pos = hit;
            hitprim = intersect(collision, hit+(dir*offset), dir*maxdist, hit, u, v);
        
            if(hitprim==-1)
                {
                tmp_vector = {0,0,0};
                break;
                }
            else
                {
                tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0))) * addRFL;
                if(NdotDIR<0)
                    tmp = 0;
                else
                    tmp = tmp_vector;
                
                refldoCheck = primuv(getDataFrom, rflCeck, hitprim, vector(set(u,v,0)));
                refrdoCheck = primuv(getDataFrom, rfrCeck, hitprim, vector(set(u,v,0)));
                addRFL = primuv(getDataFrom, "addrefl", hitprim, vector(set(u,v,0)));
                tmp_ior = primuv(getDataFrom, iorCheck, hitprim, vector(set(u,v,0)));
                
                if(debugAddPoints==1)
                    {
                    int hitpt2 = addpoint(0, hit);
                    setpointattrib(0, "Cd", hitpt2, tmp_vector, "set");
                    }
                }

            tmp = tmp_vector;
            }
    
        }

    }
    }
    else
        tmp = {0,0,0};
    return tmp;
}

// _______________________________________________________________________________________________________________________
// END CASTARAY START_____________________________________________________________________________________________________





// CASTARAY RECURSIONS_______________________________________________________________________________________________________
// _______________________________________________________________________________________________________________________

vector castARayRECURSIONS(vector raydirStart, pos, N, V; float addrefl, IOR, recSamplesBlur; string collision, returnAttributeName, rflCeck, rfrCeck, iorCheck; int samples, rfl, rfr, getDataFrom, recursions, debugAddPoints)
{
    vector hit, raydir;
    float u, v;
    int hitprim;
    vector dir = {0,0,0};
    vector dirRFL = {0,0,0};
    float offset = 0.0001;
    float angle = 0;
    float cone = radians(angle);
    float maxdist = 100000000;
    vector tmpRFL = {0,0,0};
    vector tmp_vector = {0,0,0};
    vector tmp = {0,0,0};
    vector raydirRefl = -reflect(V, N);
    float NdotV = dot(N,V);
    
if(rfl || rfr)
    {
    for(int i=0; i<samples; i++)
        {
        float NdotDIRStart = dot(N, raydirRefl);
    
        raydir = sample_direction_cone((vector)raydirStart, recSamplesBlur, vector2(set(rand(pos+1.23456+i*4.56789),rand(pos+1.23456+i*4.56789))));
        
        hitprim = intersect(collision, pos+(raydir*offset), raydir*maxdist, hit, u, v);

        if(debugAddPoints==1)
            {
            int hitpt = addpoint(0, hit);
            tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0)));
            setpointattrib(0, "Cd", hitpt, tmp_vector, "set");
            }

        int refldoCheck = primuv(getDataFrom, "refldo", hitprim, vector(set(u,v,0)));
        int refrdoCheck = primuv(getDataFrom, "refrdo", hitprim, vector(set(u,v,0)));
        float addRFL = primuv(getDataFrom, "addrefl", hitprim, vector(set(u,v,0)));
        float tmp_ior = primuv(getDataFrom, iorCheck, hitprim, vector(set(u,v,0)));
    
        NdotDIRStart = dot(N, raydir);
        if(NdotDIRStart<0)
            tmp_vector = 0;
        else
            tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0))) * addrefl;
    
        tmp = tmp_vector;
    
        vector src_pos = pos;

        if(hitprim==-1)
            {
            tmp_vector = {0,0,0};
            break;
            }
        else
            {
            for(int j=0; j<recursions; j++)
                {
                vector prpt_n = primuv(getDataFrom, "N", hitprim, vector(set(u,v,0)));
                float NdotDIR = dot(prpt_n, dir);

                if(refldoCheck==0 || refrdoCheck==0)
                    {
                    tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0)));
                    tmp = tmp_vector;
                    }
            
                if(refldoCheck)
                    {
                    dir = -reflect((normalize(src_pos - hit)), prpt_n);
                    }
                if(refrdoCheck)
                    {
                    if(NdotV>0)
                        dir = -refract((normalize(src_pos - hit)), prpt_n, tmp_ior);
                    if(addRFL>0)
                        {
                        dirRFL = -reflect((normalize(src_pos - hit)), prpt_n);
                        tmpRFL += castARaySTART(dirRFL, hit, prpt_n, addRFL, tmp_ior, cone, collision, "Cd", "refldo", "refrdo", "IOR", samples, refldoCheck, refrdoCheck, getDataFrom, recursions, debugAddPoints) * addRFL;
                        }
                    }
        
                src_pos = hit;
                hitprim = intersect(collision, hit+(dir*offset), dir*maxdist, hit, u, v);
        
                if(hitprim==-1)
                    {
                    tmp_vector = {0,0,0};
                    break;
                    }
                else
                    {
                    tmp_vector = primuv(getDataFrom, returnAttributeName, hitprim, vector(set(u,v,0))) * addRFL;
                
                    if(NdotDIR<0)
                        tmp = 0;
                    else
                        tmp = tmp_vector;
                
                    refldoCheck = primuv(getDataFrom, rflCeck, hitprim, vector(set(u,v,0)));
                    refrdoCheck = primuv(getDataFrom, rfrCeck, hitprim, vector(set(u,v,0)));
                    addRFL = primuv(getDataFrom, "addrefl", hitprim, vector(set(u,v,0)));
                    tmp_ior = primuv(getDataFrom, iorCheck, hitprim, vector(set(u,v,0)));
                
                    if(debugAddPoints==1)
                        {
                        int hitpt2 = addpoint(0, hit);
                        setpointattrib(0, "Cd", hitpt2, tmp_vector, "set");
                        }
                    }

                tmp += tmp_vector;
                }
            }
        }
    }
    else
        tmp = {0,0,0};
    return tmp + tmpRFL;
}

// _______________________________________________________________________________________________________________________
// END CASTARAY RECURSIONS_____________________________________________________________________________________________________



// FRESNEL__________________________________________________
// http://blog.demofox.org/2017/01/09/raytracing-reflection-refraction-fresnel-total-internal-reflection-and-beers-law/
//___________________________________________________________

float FresnelReflectAmount(float n1, n2, reflvalue; vector normal, incident)
{
        // Schlick aproximation
        float r0 = (n1-n2) / (n1+n2);
        r0 *= r0;
        float cosX = -dot(normal, incident);
        if (n1 > n2)
        {
            float n = n1/n2;
            float sinT2 = n*n*(1.0-cosX*cosX);
            // Total internal reflection
            if (sinT2 > 1.0)
                return 1.0;
            cosX = sqrt(1.0-sinT2);
        }
        float x = 1.0-cosX;
        float ret = r0+(1.0-r0)*x*x*x*x*x;
 
        // "reflvalue" is set into the geometry from the BRDF node
        ret = (reflvalue + (1.0-reflvalue) * ret);
        return ret;
}

//___________________________________________________________
// END FRESNEL______________________________________________



int recursionDepthMax = int(ch("../../refldepth"));

v@SOPGI_RECURSIONS = {0,0,0};
v@SOPGI_RFL = {0,0,0};

float IOR = @IOR;
int hitprim = 0;
int preview_hitprim = 0;
vector hit, raydir, dirRFL, raydirCone, tmp_recursiveColor;
vector tmpRFL = {0,0,0};
vector dir = {0,0,0};
float u, v, angle;
float offset = 0.0001;
vector raydirRefl = -reflect(v@V, v@N);
vector raydirRefr = -refract(v@V, v@N, IOR);

// Fresnel's IOR terms
float n1 = chf("n1");
float n2 = chf("n2");

vector H = normalize (v@N + v@V);
float NdotV = dot(v@N,v@V);

int samples = i@recsamples;

if(samples==1)
    angle = 0;
else
    angle = @recsamplesblur;
float cone = radians(angle);

if(i@refldo==1 || i@refrdo==1)
    {
    if(NdotV>0)
        {
        for(int i=0; i<samples; i++)
            {
            float NdotDIRStart = dot(@N, raydirRefl);
            
            if(@refldo)
                {
                raydirCone = sample_direction_cone(raydirRefl, cone, vector2(set(rand(@P+1.23456+i*4.56789),rand(@P+1.23456+i+1*4.56789))));
                }
            if(@refrdo)
                {
                raydirCone = sample_direction_cone(raydirRefr, cone, vector2(set(rand(@P+1.23456+i*4.56789),rand(@P+1.23456+i+1*4.56789))));
                if(@addrefl>0)
                    tmpRFL = castARayRECURSIONS(raydirRefl, @P, @N, @V, f@addrfl, f@IOR, cone, @OpInput1, "Cd", "refldo", "refrdo", "IOR", samples, i@refldo, i@refrdo, 0, recursionDepthMax, 0);
                }
            
            hitprim = intersect(@OpInput1, @P+(raydirCone*offset), raydirCone*100, hit, u, v);
            
            vector src_pos = @P;
            float tmp_addRFL = primuv(0, "addrefl", hitprim, vector(set(u,v,0)));
            float tmp_ior = primuv(0, "IOR", hitprim, vector(set(u,v,0)));
            
            int refldoCheck = primuv(0, "refldo", hitprim, vector(set(u,v,0)));
            int refrdoCheck = primuv(0, "refrdo", hitprim, vector(set(u,v,0)));
            
            if(hitprim==-1)
                {
                tmp_recursiveColor = {0,0,0};
                break;
                }
                
            else
                {
                for(int rfl=0; rfl<recursionDepthMax; rfl++)
                    {
                    if(refldoCheck==0 && refrdoCheck==0)
                        {
                        tmp_recursiveColor = primuv(0, "Cd", hitprim, vector(set(u,v,0)));
                        break;
                        }
                    
                        vector prpt_n = primuv(0, "N", hitprim, vector(set(u,v,0)));
                        float NdotDIR = dot(prpt_n, dir);
                        
                        if(refldoCheck)
                            {
                            dir = -reflect((normalize(src_pos - hit)), prpt_n);
                            }
                        else if(refrdoCheck)
                            {
                            dir = -refract((normalize(src_pos - hit)), prpt_n, tmp_ior);
                            if(tmp_addRFL>0)
                                {
                                dirRFL = -reflect((normalize(src_pos - hit)), prpt_n);
                                tmpRFL += castARayRECURSIONS(dirRFL, hit, prpt_n, @V, tmp_addRFL, tmp_ior, cone, @OpInput1, "Cd", "refldo", "refrdo", "IOR", samples, refldoCheck, refrdoCheck, 0, recursionDepthMax, 0) * tmp_addRFL;
                                }
                            }  
                        
                        src_pos = hit;
                        hitprim = intersect(@OpInput1, hit+(dir*offset), dir*100000, hit, u, v);
                        
                        if(hitprim==-1)
                            {
                            tmp_recursiveColor = {0,0,0};
                            break;
                            }
                        else
                            {
                            tmp_recursiveColor = primuv(0, "Cd", hitprim, vector(set(u,v,0)));
                            tmp_addRFL = primuv(0, "addrefl", hitprim, vector(set(u,v,0)));
                            tmp_ior = primuv(0, "IOR", hitprim, vector(set(u,v,0)));

                            refldoCheck = primuv(0, "refldo", hitprim, vector(set(u,v,0)));
                            refrdoCheck = primuv(0, "refrdo", hitprim, vector(set(u,v,0)));
                             }

                        }
                    v@SOPGI_RFL = tmpRFL; ///samples;
                    v@SOPGI_RECURSIONS += tmp_recursiveColor/samples;
                    }
                }
                
            // The following will apply a fresnel weigth to the reflections for transparent objects.
            // Its a cheap and lazy implementation as I'm moving SOPGI into a different plan for the future
            // but I wanted to see what it will look like and made this quick and dirty....
            
            float fresnel = 0;
            if(@refrdo)
                fresnel = FresnelReflectAmount(1, n2, @addrefl, v@N, v@V);
            else if(@refldo)
                fresnel = FresnelReflectAmount(0, n2, @addrefl, v@N, v@V);
            
            vector finalColor = {0,0,0};
            vector recursionsColor = {0,0,0};
            recursionsColor = v@SOPGI_RFL * fresnel;

            if(@refrdo)
                finalColor = (v@SOPGI_RECURSIONS * clamp(1-fresnel,0,1)) + recursionsColor;
            else
                finalColor = v@SOPGI_RECURSIONS + recursionsColor;
                
            // Lets store final recursions (reflections and arefractions together, feel free to split them if needed)
            v@SOPGI_RECURSIONS = finalColor;
            
            // Lets add back the specualr also
            @Cd = finalColor + v@specular;
            }
        }