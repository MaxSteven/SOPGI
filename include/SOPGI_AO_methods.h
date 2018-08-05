// SOPGI AO BRUTE FORCE
// This will calculate a pure brute force Ambient occlusion
// although there are some optimization in it to speed it up a little.

f@gi_ao_samples = 1;
v@gi_src_pt_n = {0,0,0};

vector hit, raydir;
float u, v;
int hitprim;
float offset = 0.0001;
float angle = chf("angle");
float cone = radians(angle);
float maxdist = chf("maxdist");
int misslimit = chi("misslimit");
int rayapprox = chi("rayapprox");
int rays = chi("rays");
float raydistmin = chf("raydistmin");
float gamma = chf("gamma");
int aomiss = -1;

if(@refrdo==0)
{
for(int i=0; i<=rays; i+=rayapprox)
    {
    if(ch("method")==0){
        if(ch("stoc")==0)
            raydir = sample_hemisphere(@N, vector2(set(rand(@P+1.23456+i*4.56789),rand(@P+1.23456+i+1*4.56789))));
        else 
            raydir = sample_hemisphere(@N, vector2(set(nrandom('twister'), nrandom('mersenne'))));
            }
    else if(ch("method")==1){
        if (ch("stoc")==0)
           raydir = sample_direction_cone(@N, cone, vector2(set(rand(@P+1.23456+i*4.56789),rand(@P+1.23456+i+1*4.56789))));
        else 
            raydir = sample_direction_cone(@N, cone, vector2(set(nrandom('twister'), nrandom('mersenne'))));   
            }
        
        if (ch("stoc")==0)
            hitprim = intersect(@OpInput1, @P+(raydir*offset), raydir*maxdist*fit01(rand(i+4),raydistmin,1), hit, u, v);
        else
        hitprim = intersect(@OpInput1, @P+(raydir*offset), raydir*maxdist*fit01(nrandom('mersenne'),raydistmin,1), hit, u, v);
        
            if(hitprim==-1)
                aomiss++;
            else
            {
                //Create a point at the ray's hit position.
                int hitpt = addpoint(0, hit);
                vector prpt_n = primuv(0, "N", hitprim, vector(set(u,v,0)));
                //set all the attributes.
                setpointattrib(0, "gi_ao_samples", hitpt, (@gi_ao_samples * 1-(1/float(rays))), "set");
                setpointattrib(0, "gi_src_pt_n", hitpt, prpt_n, "set");
            
            }
            
            if (aomiss==misslimit) break;
            if(@hit_pt == 0)
                removepoint(0, @ptnum);
        }
    }


// SOPGI COMPUTE AO BRUTE FORCE PREPASS
// This will just transfer the preview AO BRUTE FORCE values into the geometry.

float radius = chf("radius");
int maxpoints = chi("maxpoints");
string file = @OpInput2;

int handle = pcopen(file, "P", @P, radius, maxpoints);
float pc_ao = 0;
float accum_pc_ao = 0;
vector pn = {0,0,0};
float dotw = 0;
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
                pcimport(handle, "gi_ao_samples", pc_ao);
                pcimport(handle, "point.distance", pc_dist);
        
                //pcimport(handle, "gi_src_pt_n", pn);
                //dotw = clamp(dot(@N, pn), 0, 1);
    
                //Accumulate all the AO values
                accum_pc_ao += (pc_ao*(1-(pc_dist/radius))); // * dotw;
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
//Compute final AO volue
@gi_ao_samples = pow(fit(1-accum_pc_ao/maxpoints,vmin,vmax,0,1), gamma);
@Cd = @gi_ao_samples;

// SOPGI VOLUME OBSCURANCE (NVidia)
// Its a method used for real time graphics in video games.
// Its a fast approach that use Normals and vector projections.
// Its great for corners and small details hard to catch with
// the brute force method.

int maxpoints = chi("maxPoints");
float radius = chf("SPHradius");
float k = chf("K");
string file = @OpInput1; 
    
int handle = pcopen(file, "P", @P, radius, maxpoints+1);
float heightTotal = 0.0; float heightMaxTotal = 0.0;
float pdist = 0;
vector importP = {0,0,0};
int count = -1;

if(@refrdo==0)
    {
    while(pciterate(handle))
        {
        if(i@refldo && i@refrdo)
            break;
        else
            {
            if(count>=0)    //Lets skip ourself
                {
                pcimport(handle, "P", importP);
                pcimport(handle, "point.distance", pdist);
        
                // Variable names self explanatory...
                vector neighborOffset = importP - @P;
                float height = dot(neighborOffset, @N);
                vector tangentProj = neighborOffset - @N * height;
                float distSqr = dot(tangentProj,tangentProj);
                float heightMax = sqrt(max(0, radius * radius - distSqr));
                height = clamp(height, -heightMax, heightMax)*k;
                heightTotal += height;
                heightMaxTotal += heightMax;
                }
            count += 1;
            }
        }
   }
pcclose(handle);

f@VO = clamp((1 - max(0, heightTotal / heightMaxTotal)),0,1);
@Cd = @VO;

