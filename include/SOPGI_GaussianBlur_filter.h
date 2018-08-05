// SOPGI Irradiance Gaussian Blur filter.

string attribIn = chs("attributeInput");
float radius = chf("radius");
int maxpoints = chi("maxpoints");
string file = @OpInput1;
int handle = pcopen(file, "P", @P, radius, maxpoints);

int count = 0;
float pdist;
float decay = chf("decay");
vector tmp = 0;
vector pn = {0,0,0};
float dotw = 0;
vector accumulated_tmp = 0;
vector accumulate_gauss = 0;

if(@refrdo==0)
    {
    while(pciterate(handle))
        {
        if(@refldo==1)
            break;
        else
            {
            pcimport(handle, attribIn, tmp);
            pcimport(handle, "point.distance", pdist);
            pcimport(handle, "N", pn);
    
            dotw = clamp(dot(@N, pn), 0, 1);
    
            accumulated_tmp += (exp(-pow(pdist*(decay),2))*tmp) * dotw;
            accumulate_gauss += (exp(-pow(pdist*(decay),2))) * dotw;
            }
        }
    }
    
v@gi_samples_prepass = accumulated_tmp/accumulate_gauss;
@Cd = @gi_samples_prepass;