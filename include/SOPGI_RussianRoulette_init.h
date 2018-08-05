// RUSSIAN ROULETTE___________________________________________
//___________________________________________________________
// The following will decide if a photon will be transmitted or absorbed
// Probability for reflection

f@Pr = max(@diffuse.x + @specular.x, @diffuse.y + @specular.y, @diffuse.z + @specular.z);
// Probability of absorbtion
f@Pa = 1 - @Pr;
// Probability for diffuse reflection
f@Pd = ((@diffuse.x + @diffuse.y + @diffuse.z) / (@diffuse.x + @diffuse.y + @diffuse.z + @specular.x + @specular.y + @specular.z)) * @Pr;
//Probability for specular reflection
f@Ps = @Pr - @Pd;

//___________________________________________________________
// END RUSSIAN ROULETTE_______________________________________