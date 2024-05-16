#version 330

//const int iterations = 15;
const float circle_radius = 0.1;

in vec2 fragTexCoord;

out vec4 fragColor;

uniform int iterations;

uniform vec2 offset;
uniform vec2 zoom;

uniform vec2 r1;
uniform vec2 r2;
uniform vec2 r3;

uniform vec2 c3;
uniform vec2 c2;
uniform vec2 c1;
uniform vec2 c0;

// (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
vec2 cm(vec2 z1, vec2 z2) {
	return vec2(z1.x * z2.x - z1.y * z2.y, z1.x * z2.y + z1.y * z2.x);
}

// (a + bi) / (c + di) = 1/(c^2 + d^2)(a + bi)(c - di) = 
// 1/(...) (ac + bd) + (bc - ad)i
vec2 cd(vec2 a, vec2 b) {
	return vec2((a.x*b.x+a.y*b.y)/(b.x*b.x+b.y*b.y),(a.y*b.x-a.x*b.y)/(b.x*b.x+b.y*b.y));
}

vec2 newton(vec2 x) {
	for(int i = 0; i < iterations ; ++i) {
		vec2 f = cm(cm(cm(c3, x), x), x) + cm(cm(c2, x), x) + cm(c1, x) + c0;
		vec2 df = cm(cm(3.0 * c3, x), x) + cm(2.0 * c2, x) + c1;
		x -= cd(f, df);
	}
	return x;
}

void main()
{
	vec2 z = vec2(zoom.x, -zoom.y) * (fragTexCoord - offset);
	if(length(r1 - z) < circle_radius || 
	   length(r2 - z) < circle_radius || 
	   length(r3 - z) < circle_radius) {
		fragColor = vec4(1.0);
		return;
	}
	z = newton(z);
	float min_dist = length(r1 - z);
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	if(length(r3 - z) < min_dist) {
		fragColor = vec4(0.0, 0.0, 1.0, 1.0);
		min_dist = length(r3 - z);
	}
	if(length(r2 - z) < min_dist) {
		fragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
}
