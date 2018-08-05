// SOPGI BRDF
// THis node will calculate proper BRDF shading models
// and output their Diffuse and Specular coefficients for GI calculation
// to be used also for an efficient RussianRoulette implementation.


// BLINN PHONG________________________________________________
//___________________________________________________________
float blinnPhong(vector N; vector V; vector L; float specHard)
{
    vector H = normalize(L + V);
    float NdotH = clamp(dot(N, H), 0, 1);
    return pow(NdotH, specHard);
}
//___________________________________________________________
// END BLINN PHONG____________________________________________


// OREN NAYAR_________________________________________________
//___________________________________________________________
float orenNayar(vector Nn, L, V; float m)
{
    float roughness = m;
    float roughness2 = m * m;
    vector rAdd = set(0.33, 0.09, 0);
    vector oren_nayar_fraction = roughness2 / (roughness2 + rAdd);
    vector range1 = set(1, 0, 0);
    vector range2 = set(-0.5, 0.45, 0);
    vector oren_nayar = range1 + range2 * oren_nayar_fraction;
    // Angles
    vector cos_theta = clamp(set(dot(Nn,L), dot(Nn,V), 0), 0, 1);
    vector cos_theta2 = cos_theta * cos_theta;
    float sin_theta = sqrt((1-cos_theta2.x) * (1-cos_theta2.y));
    vector light_plane = normalize(L - cos_theta.x * Nn);
    vector view_plane = normalize(V - cos_theta.y * Nn);
    float cos_phi = clamp(dot(light_plane, view_plane), 0, 1);
    // Build diffuse
    float diffuse_oren_nayar = cos_phi * sin_theta / max(cos_theta.x, cos_theta.y);
    float diffuse = cos_theta.x * (oren_nayar.x + oren_nayar.y * diffuse_oren_nayar);
    return diffuse;
    }
//___________________________________________________________
// END OREN NAYAR_____________________________________________


// COOK TORRANCE______________________________________________
//___________________________________________________________
// Solve for D :: Beckmann distribution
float cookD(vector Nn; vector H; float m)
{
    float NdotH = dot(Nn, H);
    float beta = acos(NdotH);
    float tanbeta = tan(beta);
    float tanbeta_over_m = tanbeta/m;
    float D = exp(-(tanbeta_over_m*tanbeta_over_m));
    return D;

}

// Solve for G :: Geometric contribution
float cookG(vector Nn; vector H; vector L; vector V)
{
    float NdotH = dot(Nn, H);
    float NdotV = dot(Nn, V);
    float NdotL = dot(Nn, L);
    float VdotH = dot(V, H);
    
    float masking = 2*NdotH*NdotV/VdotH;
    float shadowing = 2*NdotH*NdotL/VdotH;
    return min(1, min(masking, shadowing));
}

// Build final Specular
float cookTorrance(vector P; vector Lp; vector N; vector V; float eta, m, pi;)
{
    float cook = 0;
    vector L = Lp - P;
    vector Nn = normalize(N);
    
    if(dot(Nn, L)>0.0)
    {
        vector Ln = normalize(L);
        vector H = normalize(Ln+V);
        float D = cookD(Nn, H, m);
        float G = cookG(Nn, H, Ln, V);
    
        // Solve for F :: Fresnel
        float F, Kt;
        fresnel(V, Nn, eta, F, Kt);
    
        cook += D*G*F;
    
        float VdotN = dot(V, Nn);
        cook /= VdotN;
        cook /= pi;
    }
    return cook;
}
//___________________________________________________________
// END COOK TORRANCE__________________________________________


// GGX________________________________________________________
//___________________________________________________________
float G1V(float NdotV, k)
{
        return 1.0/(NdotV*(1.0-k)+k);
}

float GGX(vector N, V, L; float roughness, F0)
{
        float alpha = roughness*roughness;

        vector H = normalize(V+L);

        float NdotL = clamp(dot(N,L),0,1);
        float NdotV = clamp(dot(N,V),0,1);
        float NdotH = clamp(dot(N,H),0,1);
        float LdotH = clamp(dot(L,H),0,1);

        float F, D, VIS;

        // D
        float alphaSqr = alpha*alpha;
        //float PI = 3.14159265;
        float denom = NdotH * NdotH *(alphaSqr-1.0) + 1.0;
        D = alphaSqr/(PI * denom * denom);

        // F
        float LdotH5 = pow(1.0-LdotH,5);
        F = F0 + (1.0-F0)*(LdotH5);

        // V
        float k = alpha/2.0;
        VIS = G1V(NdotL,k)*G1V(NdotV,k);

        float specular = NdotL * D * F * VIS;
        return specular;
}
//___________________________________________________________
// END GGX____________________________________________________


// GGX1_______________________________________________________
//___________________________________________________________

float GGX1(vector N, V, L; float roughness, F0)
{
        float alpha = roughness*roughness;

        vector H = normalize(V+L);

        float NdotL = clamp(dot(N,L),0,1);
        float NdotV = clamp(dot(N,V),0,1);
        float NdotH = clamp(dot(N,H),0,1);
        float LdotH = clamp(dot(L,H),0,1);

        float F, D, VIS;

        // D
        float alphaSqr = alpha*alpha;
        //float PI = 3.14159265;
        float denom = NdotH * NdotH *(alphaSqr-1.0) + 1.0;
        D = alphaSqr/(PI * denom * denom);

        // F
        float LdotH5 = pow(1.0-LdotH,5);
        F = F0 + (1.0-F0)*(LdotH5);
        
        // V  : N_dot_L and N_dot_V are replaced with L_dot_H      
        float k = alpha/2.0;
        VIS = G1V(LdotH,k)*G1V(LdotH,k);

        float specular = NdotL * D * F * VIS;
        return specular;
        
}
//___________________________________________________________
// END GGX1___________________________________________________

// GGX2_______________________________________________________
//___________________________________________________________

float GGX2(vector N, V, L; float roughness, F0)
{
    vector H = normalize(V+L);

    float NdotL = clamp(dot(N,L),0,1);
    float NdotV = clamp(dot(N,V),0,1);
    float NdotH = clamp(dot(N,H),0,1);
    float LdotH = clamp(dot(L,H),0,1);

    float F, D, VIS;
    
    // D
    //float PI = 3.14159265;
    float a = 1/roughness*tan(NdotV);
    float alpha = roughness * roughness;
    float aaa = pow((1 + pow(tan(NdotH),2)/alpha),2);
    float NdotH4 = pow(cos(NdotH), 4);
    // This is powered to the 4th as it looked more cool.
    D = PI * pow((alpha * NdotH4 * aaa), 4);
    
    // F
    float LdotH5 = pow(1.0-LdotH,5);
    F = F0 + (1.0-F0)*(LdotH5);
    
    // V
    float a2 = tan(NdotV*NdotV);
    float denom = 1/roughness * a2;
    VIS = NdotV * (-1 + sqrt(1+(1/(denom*denom))))/2;

    // Build Specular
    float specular = NdotV * D * F * VIS;
    return specular;
}
//___________________________________________________________
// END GGX2___________________________________________________