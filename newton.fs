#version 330

const int iterations = 10;

in vec2 fragTexCoord;

out vec4 fragColor;

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
	return vec2(z1.x * z2.x + z1.y + z2.y, z1.y * z2.x - z1.x * z2.y) / (z2.x * z2.x + z2.y * z2.y);
}

vec2 newton(vec2 x) {
	for(int i = 0; i < iterations; ++i) {
		vec2 f = cm(cm(cm(c3, x) + c2, x) + c1, x) + c0;
		vec2 df = cm(cm(3.0 * c3, x) + 2.0 * c2, x) + c1;
		x -= cd(f, df);
	}
	return x;
}

void main()
{
	vec2 z = vec2(8.0, 6.0) * (fragTexCoord - vec2(0.5, 0.5));
	z = newton(z);
	float min_dist = dot(z - r1, z - r1);
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	if(dot(z - r2, z - r2) < min_dist) {
		fragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
	if(dot(z - r3, z - r3) < min_dist) {
		fragColor = vec4(0.0, 0.0, 1.0, 1.0);
	}
}
