#version 330

const int iterations = 10;

in vec2 fragTexCoord;

out vec4 fragColor;

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
vec2 cd(vec2 z1, vec2 z2) {
	return vec2(z1.x * z2.x + z1.y + z2.y, z1.y * z2.x - z1.x * z2.y) * (1.0 / dot(z2, z2));
}

vec2 newton(vec2 x) {
	for(int i = 0; i < iterations; ++i) {
		vec2 f = cm(cm(cm(c3, x), x), x) + cm(cm(c2, x), x) + cm(c1, x) + c0;
		vec2 df = cm(cm(3.0 * c3, x), x) + cm(2.0 * c2, x) + c1;
		x -= cd(f, df);
	}
	return x;
}

void main()
{
	vec2 z = vec2(zoom.x, zoom.y) * (fragTexCoord - offset);
	z = newton(z);
	float min_dist = length(z - r1);
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	if(length(z - r2) < min_dist) {
		fragColor = vec4(0.0, 1.0, 0.0, 1.0);
		min_dist = length(z - r2);
	}
	if(length(z - r3) < min_dist) {
		fragColor = vec4(0.0, 0.0, 1.0, 1.0);
	}
}
