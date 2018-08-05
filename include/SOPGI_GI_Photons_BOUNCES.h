// SOPGI RAY BOUNCE
// You can keep appending this node, one after the other
// to keep bouncing in the scene, this is call Raytrace.
// just remember to add the attribute you want to collect.
// I initially wanted to put everything together into one clean node/code
// but end up in keeping this modularity as it turn out to be 
// more efficient and fast.

// We need this to later identifie the new points
i@gi_pt = 0;
    
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

// Get probabilities coefficients
float src_Pr = @gi_Pr;
float src_Pa = @gi_Pa;
float src_Pd = @gi_Pd;
float src_Ps = @gi_Ps;

// RUSSIAN ROULETTE
// We will set up a set of rules to help us optimize and speed up the GI calculation.
// In the real world, photons that belong to a Specular reflection function
// will get used mostly for the generation of caustics.(usually done in a second pass anyway)
// We will get rid of most of those Specular reflection photons as well as defining an Absorbtion condition for the remaining one.
// and we will focus only on the photons that belong to the diffuse relection function.

// Define Decision Coefficient
float decisionCoefficient = @gi_Ps + @gi_Pd;

// Define random number for casuality decisions
float gi_rand = nrandom('twister');

for(int i=0; i<=rays; i+=rayapprox)
    {
    // Lets check if a photon is absorbed
    if(decisionCoefficient<=gi_rand<=1) i++;
    // Lets check if a photon is a specular reflection
    //if(@gi_Pd<=gi_rand<=decisionCoefficient) i++;
    
    if(ch("method")==0){
        if(ch("stoc")==0)
            raydir = sample_hemisphere((vector)@N, vector2(set(rand(1.23456+i*4.56789),rand(1.23456+i+1*4.56789))));
        else 
            raydir = sample_hemisphere((vector)@N, vector2(set(nrandom('twister'), nrandom('mersenne'))));
            }
    else if(ch("method")==1){
        if (ch("stoc")==0)
           raydir = sample_direction_cone((vector)@N, cone, vector2(set(rand(1.23456+i*4.56789),rand(1.23456+i+1*4.56789))));
        else 
            raydir = sample_direction_cone((vector)@N, cone, vector2(set(nrandom('twister'), nrandom('mersenne'))));   
            }
        
        if (ch("stoc")==0)
            hitprim = intersect(@OpInput2, @P+(raydir*offset), raydir*maxdist*fit01(rand(i+4),raydistmin,1), hit, u, v);
        else
            hitprim = intersect(@OpInput2, @P+(raydir*offset), raydir*maxdist*fit01(nrandom('mersenne'),raydistmin,1), hit, u, v);
        
        // Store the original source point color
        vector tmp_src_cd = @Cd;
        // Store the original GI multiplier
        float tmp_src_gi_mult = (float)@gi_mult;
        // Store the bounce count from the current point
        int tmp_bounce = (int)@gi_bounce;
        // Store the point the rays are coming from.
        int tmp_ptnum = @ptnum;
        // Store its position also.
        vector tmp_src_pos = @P;
        
            if(hitprim==-1)
                raymiss++;
            else
            {
                // Create a point at the ray's hit position.
                int hitpt = addpoint(0, hit);
                // store the distance from the source point
                // So the closest samples will carry more GI power
                float tmp_dist = 1 - length(tmp_src_pos - hit)/maxdist;
                // Steal informations from the hit surface location
                vector prpt_n = primuv(1, "N", hitprim, vector(set(u,v,0)));
                float tmp_gi_mult = primuv(1, "gi_mult", hitprim, vector(set(u,v,0)));
                vector prpt_cd = primuv(1, "Cd", hitprim, vector(set(u,v,0)));
                float prpt_gi_mult_emitted = primuv(1, "gi_mult_emitted", hitprim, vector(set(u,v,0)));
                // Emission power
                prpt_cd *= prpt_gi_mult_emitted;
                
                
                // Get the light intensity/color
                vector tmp_light = primuv(2, "light", hitprim, vector(set(u,v,0)));
                
                // Get current BRDF and probabilities coefficients
                float tmp_D = primuv(2, "diffuse", hitprim, vector(set(u,v,0)));
                float tmp_S = primuv(2, "specular", hitprim, vector(set(u,v,0)));
                float tmp_Pd = primuv(2, "Pd", hitprim, vector(set(u,v,0)));
                float tmp_Ps = primuv(2, "Ps", hitprim, vector(set(u,v,0)));
                // Store new probabilities coefficients
                setpointattrib(0, "gi_Pd", hitpt, tmp_Pd, "set");
                setpointattrib(0, "gi_Ps", hitpt, tmp_Ps, "set");
                // Build condition
                float giRand = nrandom('twister');      // decision maker
                float giCondition = @gi_Pd + @gi_Ps;    // condition for decision
                
                // Solve for diffuse and specular
                vector tmp_gi_diff = tmp_src_cd * tmp_D / tmp_Pd;   // Solve for Diffuse
                vector tmp_gi_spec = tmp_src_cd * tmp_S / tmp_Ps;   // Solve for Specular
                
                // Build final photon color/intensity
                vector new_gi_cd = {0,0,0};
                if(@gi_Pd<=giRand<=giCondition)
                    new_gi_cd = ((v@gi_src_pt_cd/(float)tmp_bounce) + (tmp_src_gi_mult*v@gi_src_pt_cd)) + (tmp_gi_diff + tmp_gi_spec);
                else
                    new_gi_cd = ((v@gi_src_pt_cd/(float)tmp_bounce) + (tmp_src_gi_mult*v@gi_src_pt_cd)) + tmp_gi_diff;
                
                // Lets store the length of the photon's color 
                float tmp_new_gi_cd_length = length(new_gi_cd);
                
                // Calculate the reflection vector for the next ray bounce
                vector dir = reflect((normalize(tmp_src_pos - hit)*-1), prpt_n);
                
                // set all the attributes.
                setpointattrib(0, "gi_pt", hitpt, 1, "set");
                // Add 1 to @gi_bounce
                // This way we can reduse the GI power every time the ray bounce in the scene.
                setpointattrib(0, "gi_bounce", hitpt, tmp_bounce+1, "set");
                // GI_MULT is set in the geometry we are processing.
                // This way we can increase and decrease the GI power perObect or per Primitives
                setpointattrib(0, "gi_mult", hitpt, tmp_gi_mult, "set");
                // We set N equal to the reflected direction so the next node will pick it up automatically
                setpointattrib(0, "N", hitpt, dir, "set");
                setpointattrib(0, "Cd", hitpt, prpt_cd, "set");
                setpointattrib(0, "gi_src_pt", hitpt, tmp_ptnum, "set");
                setpointattrib(0, "gi_src_pt_n", hitpt, prpt_n, "set");
                setpointattrib(0, "gi_src_pt_pos", hitpt, tmp_src_pos, "set");
                setpointattrib(0, "gi_src_pt_cd", hitpt, new_gi_cd, "set");
                setpointattrib(0, "gi_src_pt_cd_length", hitpt, tmp_new_gi_cd_length, "set");
                setpointattrib(0, "gi_src_pt_dist", hitpt, tmp_dist, "set");                
            }
            
            // Lets get rid of the preview photons
            if(@gi_pt == 0)
                removepoint(0, @ptnum);
            // If a point's rays miss to hit this many time it will call it a day ;)
            if (raymiss==misslimit) break;

     }

