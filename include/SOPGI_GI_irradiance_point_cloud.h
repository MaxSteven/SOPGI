// SOPGI COMPUTE GI NEW
// This method will produce more accurate result for the final Irradiance calculation
// as it will take into account the angle between the incoming photon direction
// and its reflection vector at the intersection point with the geometry.
// It will also use the projected area of the search sphere radius=>(furthest distance from the current location)

//float PI = 3.14159265;

float radius = chf("radius");
int maxpoints = chi("maxpoints");
string file = @OpInput2;

float A = 1/PI*(radius*radius); // Projected search sphere area onto the plane

int handle = pcopen(file, "P", @P, radius, maxpoints);
vector pc_cd = {0,0,0};
vector ptPos = {0,0,0};
vector ptNRaydir = {0,0,0};
vector tmpSrcPtPos = {0,0,0};
vector accum_pc_cd = {0,0,0};
vector pn = {0,0,0};
float NdotPN = 0;
float gi_dist = 0;
float gi_bounce = 0;
float pc_dist = 0;
int pointCount = -1;

if(@refrdo==0)
    {
    while(pciterate(handle))
        {
        if(@refldo==1)
            break;
        else
            {
            if(pointCount>=0) // Lets skip ourself
                {
                pcimport(handle, "gi_src_pt_cd", pc_cd);
                pcimport(handle, "P", ptPos);
                pcimport(handle, "N", ptNRaydir);
                pcimport(handle, "gi_src_pt_pos", tmpSrcPtPos);
                pcimport(handle, "point.distance", pc_dist);
                pcimport(handle, "gi_src_pt_dist", gi_dist);
                pcimport(handle, "gi_src_pt_n", pn);
                pcimport(handle, "gi_bounce", gi_bounce);
        
                // Calculate the incoming direction vector
                vector tmpRaydir = normalize(tmpSrcPtPos - ptPos); // incoming photon direction
                // Calculate the angle between the incoming direction and the reflected direction vectors
                float IdotR = fit(dot(tmpRaydir, ptNRaydir),-1, 1, 0, 1); // Lets refit the entire angle inside a [0,1] range
        
                // We dnt want to keep in consideration photons on a 90degree polygon from us.
                NdotPN = clamp(dot(@N, pn), 0, 1);
        
                //Accumulate all the GI values
                //Divide the original sample GI color by the gi_bounce number so we can simluate the lost in power.
                //Multiply the GI intensity by the distance is coming from so the closest a sample is the more power will have. (not sure this is correct but the result seem ok)
        
                // Here I'm making lots of assumption in calculating the final irradiance value.
                // First I am omitting completely the BRDF function as I am taking it into account
                // during the genration of the photon itself inside the ARAY node so I assume thats is enough.
                // For the same reason I am omitting from here the Photon power, it should all be already defined inside the 'photon Cd' attribute.
                accum_pc_cd += A * (((pc_cd/gi_bounce) * fit(gi_dist, 0, 1, (1-ch("distdecay")), 1.0)) * (1-(pc_dist/radius))) * IdotR * NdotPN;  
        
                pointCount ++;
                }
            else
                pointCount = 0;
            }
        }
    }

pcclose(handle);

float vmin = chf("vmin");
float vmax = chf("vmax");
float gamma = chf("gamma");

//Compute final GI value
v@gi_samples = accum_pc_cd/maxpoints;
@gi_samples = fit(@gi_samples,vmin,vmax,0,1) * gamma;
@Cd = @gi_samples;