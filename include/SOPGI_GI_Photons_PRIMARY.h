// SOPGI PRIMARY RAY
// This node is for the first ray bounce.
// It create and gather all the attribute you need.
// See the next one in the chain to keep bouncing in the scene.

// Create all the necessary attributes. Add new one here
// and remember to set the new one later.

// We need "gi_pt" this to later identifie the new points
i@gi_pt = 0;



// Set the bounce attribute
i@gi_bounce;
i@gi_src_pt = 0;
f@gi_src_pt_dist = 0;
v@gi_src_pt_cd = {0,0,0};
f@gi_src_pt_cd_length = 0;
v@gi_src_pt_n = {0,0,0};
v@gi_src_pt_pos = {0,0,0};
f@gi_prpt_gi_mult = 0;
f@gi_ao_dir_samples = 1;

v@gi_light = {0,0,0};

f@gi_Pr = 0;
f@gi_Pa = 0;
f@gi_Pd = 0;
f@gi_Ps = 0;

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
int raymiss = -1;

for(int i=0; i<=rays; i+=rayapprox)
    {
    // Gather all the directions we will shoot the Rays to.
    if(ch("method")==0){ // If hemisphere method
        if(ch("stoc")==0)
            raydir = sample_hemisphere(@N, vector2(set(rand(1.23456+i*4.56789),rand(1.23456+i+1*4.56789))));
        else 
            raydir = sample_hemisphere(@N, vector2(set(nrandom('twister'), nrandom('mersenne'))));
            }
    else if(ch("method")==1){ // if Cone Angle method
        if (ch("stoc")==0)
           raydir = sample_direction_cone(@N, cone, vector2(set(rand(1.23456+i*4.56789),rand(1.23456+i+1*4.56789))));
        else 
            raydir = sample_direction_cone(@N, cone, vector2(set(nrandom('twister'), nrandom('mersenne'))));   
            }
        // Calculte the intersections between the rays and the geometry we are using as a Ray Collision
        if (ch("stoc")==0)
            hitprim = intersect(@OpInput2, @P+(raydir*offset), raydir*@maxdist*fit01(rand(i+4),raydistmin,1), hit, u, v);
            //hitprim = intersect(@OpInput2, @P+(raydir*offset), raydir*maxdist*fit01(rand(i+4),raydistmin,1), hit, u, v);
        else
            hitprim = intersect(@OpInput2, @P+(raydir*offset), raydir*@maxdist*fit01(nrandom('mersenne'),raydistmin,1), hit, u, v);
            //hitprim = intersect(@OpInput2, @P+(raydir*offset), raydir*maxdist*fit01(nrandom('mersenne'),raydistmin,1), hit, u, v);
        
        // store the point the rays are coming from.
        int tmp_ptnum = @ptnum;
        // store its position also.
        vector tmp_src_pos = @P;
        
            if(hitprim==-1)
                raymiss++;
            else
            {
                // Create a point at the ray's hit position.
                int hitpt = addpoint(0, hit);
                // store the distance from the source point
                // So the closest samples will carry more GI power
                float dist = 1 - length(tmp_src_pos - hit)/maxdist;
                vector prpt_n = primuv(1, "N", hitprim, vector(set(u,v,0)));
                vector prpt_cd = primuv(1, "diffuse", hitprim, vector(set(u,v,0)));
                float prpt_gi_mult_emitted = primuv(1, "gi_mult_emitted", hitprim, vector(set(u,v,0)));
                prpt_cd *= prpt_gi_mult_emitted;
                float tmp_gi_mult = primuv(1, "gi_mult", hitprim, vector(set(u,v,0)));
                
                // Get the light intensity/color
                vector tmp_light = primuv(1, "light", hitprim, vector(set(u,v,0)));
                
                // Get the probabilities coefficents
                float tmp_Pr = primuv(1, "Pr", hitprim, vector(set(u,v,0)));
                float tmp_Pa = primuv(1, "Pa", hitprim, vector(set(u,v,0)));
                float tmp_Pd = primuv(1, "Pd", hitprim, vector(set(u,v,0)));
                float tmp_Ps = primuv(1, "Ps", hitprim, vector(set(u,v,0)));
                
                vector new_gi_cd = prpt_cd;
                
                float tmp_new_gi_cd_length = length(new_gi_cd);
                
                vector dir = reflect((normalize(tmp_src_pos - hit)*-1), prpt_n);
                // set all the attributes.
                setpointattrib(0, "gi_ao_dir_samples", hitpt, (@gi_ao_dir_samples * 1-(1/float(rays))), "set");
                setpointattrib(0, "gi_pt", hitpt, 1, "set");
                // Set the bounce to 1. From now on, all the next node will increase the bounce by 1 automatically.
                // This way we can reduse the GI power every time the ray bounce in the scene.
                setpointattrib(0, "gi_bounce", hitpt, 1, "set");
                // GI_MULT is set in the geometry we are processing.
                // This way we can increase and decrease the GI power perObect or per Primitives
                setpointattrib(0, "gi_mult", hitpt, tmp_gi_mult, "set");
                setpointattrib(0, "N", hitpt, dir, "set");
                setpointattrib(0, "Cd", hitpt, prpt_cd, "set");
                setpointattrib(0, "gi_src_pt", hitpt, tmp_ptnum, "set");
                setpointattrib(0, "gi_src_pt_n", hitpt, prpt_n, "set");
                setpointattrib(0, "gi_src_pt_pos", hitpt, tmp_src_pos, "set");
                setpointattrib(0, "gi_src_pt_cd", hitpt, new_gi_cd, "set");
                setpointattrib(0, "gi_src_pt_cd_length", hitpt, tmp_new_gi_cd_length, "set");
                setpointattrib(0, "gi_src_pt_dist", hitpt, dist, "set");                
                // Store the light intensity/color
                setpointattrib(0, "gi_light", hitpt, tmp_light, "set");
                
                // Store the ptobability coefficients
                setpointattrib(0, "gi_Pr", hitpt, tmp_Pr, "set");
                setpointattrib(0, "gi_Pa", hitpt, tmp_Pa, "set");
                setpointattrib(0, "gi_Pd", hitpt, tmp_Pd, "set");
                setpointattrib(0, "gi_Ps", hitpt, tmp_Ps, "set");
            }
            // Lets skip the iterations if a point ray miss this many time
            if (raymiss==misslimit) break;
            // lets get rid of what we dnt need
            if(@gi_pt == 0)
                removepoint(0, @ptnum);
     }