#include "allegro.h"
#include "math.h"
#include "xtrig.h"

float shortest_arc(float a1, float a2) {
	float q1 = a2 - a1;
	if (fabs(q1) <= M_PI) return q1;
	else if (q1>0) return ( q1 - (2*M_PI));
	else return ( q1 + (2*M_PI));
}

float afix(float tang) {
	tang = fmodf(tang, 2. * M_PI );
	if (tang > M_PI)  tang -= (2.0 * M_PI);
	if (tang < -M_PI) tang += (2.0 * M_PI);
    return tang;
}

int quadratic(float A, float B, float C, float &z1, float &z2) {
	float disc = B * B - 4. * A * C;
	if (disc < 0.) return 0;
	float sqrtd = sqrt(disc);
	float twoA = 2. * A;
	float q1 = (-B - sqrtd) / twoA;
	float q2 = (-B + sqrtd) / twoA;
	z1 = MIN(q1,q2);  z2 = MAX(q1,q2);
	return (disc == 0. ? 1 : 2);
}

float lpquadratic(float A, float B, float C) {
	float disc = B * B - 4. * A * C;
	if (disc < 0.) return -1.;
	float sqrtd = sqrt(disc);
	float twoA = 2. * A;
	float q1 = (-B - sqrtd) / twoA;
	float q2 = (-B + sqrtd) / twoA;
	return (q1 < 0. ? q2 : MIN(q1,q2));
}


