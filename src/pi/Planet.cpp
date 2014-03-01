// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Planet.h"
#include "Pi.h"
#include "WorldView.h"
#include "GeoSphere.h"
#include "perlin.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/RenderState.h"
#include "graphics/Graphics.h"
#include "graphics/Texture.h"
#include "graphics/VertexArray.h"
#include "Color.h"

#ifdef _MSC_VER
	#include "win32/WinMath.h"
	#define log1pf LogOnePlusX
#endif // _MSC_VER

using namespace Graphics;

Planet::Planet()
	: TerrainBody()
{
}

Planet::Planet(SystemBody *sbody)
	: TerrainBody(sbody)
{
	InitParams(sbody);
}

void Planet::InitParams(const SystemBody *sbody)
{
	double specificHeatCp;
	double gasMolarMass;
	if (sbody->GetSuperType() == SystemBody::SUPERTYPE_GAS_GIANT) {
		specificHeatCp=12950.0; // constant pressure specific heat, for a combination of hydrogen and helium
		gasMolarMass = 0.0023139903;
	} else {
		specificHeatCp=1000.5;// constant pressure specific heat, for the combination of gasses that make up air
		// XXX using earth's molar mass of air...
		gasMolarMass = 0.02897;
	}
	const double GAS_CONSTANT = 8.3144621;
	const double PA_2_ATMOS = 1.0 / 101325.0;

	// surface gravity = -G*M/planet radius^2
	m_surfaceGravity_g = -G*sbody->GetMass()/(sbody->GetRadius()*sbody->GetRadius());
	const double lapseRate_L = -m_surfaceGravity_g/specificHeatCp; // negative deg/m
	const double surfaceTemperature_T0 = sbody->GetAverageTemp(); //K

	double surfaceDensity, h; Color c;
	sbody->GetAtmosphereFlavor(&c, &surfaceDensity);// kg / m^3
	surfaceDensity/=gasMolarMass;			// convert to moles/m^3

	//P = density*R*T=(n/V)*R*T
	const double surfaceP_p0 = PA_2_ATMOS*((surfaceDensity)*GAS_CONSTANT*surfaceTemperature_T0); // in atmospheres
	if (surfaceP_p0 < 0.002) h = 0;
	else {
		//*outPressure = p0*(1-l*h/T0)^(g*M/(R*L);
		// want height for pressure 0.001 atm:
		// h = (1 - exp(RL/gM * log(P/p0))) * T0 / l
		double RLdivgM = (GAS_CONSTANT*lapseRate_L)/(-m_surfaceGravity_g*gasMolarMass);
		h = (1.0 - exp(RLdivgM * log(0.001/surfaceP_p0))) * surfaceTemperature_T0 / lapseRate_L;
//		double h2 = (1.0 - pow(0.001/surfaceP_p0, RLdivgM)) * surfaceTemperature_T0 / lapseRate_L;
//		double P = surfaceP_p0*pow((1.0-lapseRate_L*h/surfaceTemperature_T0),1/RLdivgM);
	}
	m_atmosphereRadius = h + sbody->GetRadius();

	SetPhysRadius(std::max(m_atmosphereRadius, GetMaxFeatureRadius()+1000));
	SetClipRadius(GetPhysRadius());
}

/*
 * dist = distance from centre
 * returns pressure in earth atmospheres
 * function is slightly different from the isothermal earth-based approximation used in shaders,
 * but it isn't visually noticeable.
 */
void Planet::GetAtmosphericState(double dist, double *outPressure, double *outDensity) const
{
	PROFILE_SCOPED()
#if 0
	static bool atmosphereTableShown = false;
	if (!atmosphereTableShown) {
		atmosphereTableShown = true;
		for (double h = -1000; h <= 100000; h = h+1000.0) {
			double p = 0.0, d = 0.0;
			GetAtmosphericState(h+this->GetSystemBody()->GetRadius(),&p,&d);
			Output("height(m): %f, pressure(hpa): %f, density: %f\n", h, p*101325.0/100.0, d);
		}
	}
#endif

	// This model has no atmosphere beyond the adiabetic limit
	// Note: some code duplicated in InitParams(). Check if changing.
	if (dist >= m_atmosphereRadius) {*outDensity = 0.0; *outPressure = 0.0; return;}

	double surfaceDensity;
	double specificHeatCp;
	double gasMolarMass;
	const SystemBody *sbody = this->GetSystemBody();
	if (sbody->GetSuperType() == SystemBody::SUPERTYPE_GAS_GIANT) {
		specificHeatCp=12950.0; // constant pressure specific heat, for a combination of hydrogen and helium
		gasMolarMass = 0.0023139903;
	} else {
		specificHeatCp=1000.5;// constant pressure specific heat, for the combination of gasses that make up air
		// XXX using earth's molar mass of air...
		gasMolarMass = 0.02897;
	}
	const double GAS_CONSTANT = 8.3144621;
	const double PA_2_ATMOS = 1.0 / 101325.0;

	// lapse rate http://en.wikipedia.org/wiki/Adiabatic_lapse_rate#Dry_adiabatic_lapse_rate
	// the wet adiabatic rate can be used when cloud layers are incorporated
	// fairly accurate in the troposphere
	const double lapseRate_L = -m_surfaceGravity_g/specificHeatCp; // negative deg/m

	const double height_h = (dist-sbody->GetRadius()); // height in m
	const double surfaceTemperature_T0 = sbody->GetAverageTemp(); //K

	Color c;
	sbody->GetAtmosphereFlavor(&c, &surfaceDensity);// kg / m^3
	// convert to moles/m^3
	surfaceDensity/=gasMolarMass;

	//P = density*R*T=(n/V)*R*T
	const double surfaceP_p0 = PA_2_ATMOS*((surfaceDensity)*GAS_CONSTANT*surfaceTemperature_T0); // in atmospheres

	// height below zero should not occur
	if (height_h < 0.0) { *outPressure = surfaceP_p0; *outDensity = surfaceDensity*gasMolarMass; return; }

	//*outPressure = p0*(1-l*h/T0)^(g*M/(R*L);
	*outPressure = surfaceP_p0*pow((1-lapseRate_L*height_h/surfaceTemperature_T0),(-m_surfaceGravity_g*gasMolarMass/(GAS_CONSTANT*lapseRate_L)));// in ATM since p0 was in ATM
	//                                                                               ^^g used is abs(g)
	// temperature at height
	double temp = surfaceTemperature_T0+lapseRate_L*height_h;

	*outDensity = (*outPressure/(PA_2_ATMOS*GAS_CONSTANT*temp))*gasMolarMass;
}
