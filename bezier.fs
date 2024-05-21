#version 330

const float thickness = 0.001;

in vec2 fragTexCoord;

out vec4 fragColor;

const vec2 p0 = {0.25, 0.33};
const vec2 p1 = {0.5, 0.66};
const vec2 p2 = {0.75, 0.33};

bool line(vec2 a, vec2 b, vec2 p) {
	vec2 ba = b - a;
	vec2 pa = p - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) <= thickness;
}

void main() {
	vec2 z = vec2(fragTexCoord.x, fragTexCoord.y * 0.75);
	if(length(p0 - z) < 1e-2 || length(p1 - z) < 1e-2 || length(p2 - z) < 1e-2) {
		fragColor = vec4(1.0);
		return;
	}
	if(line(p0, p1, z) || line(p1, p2, z)) {
		fragColor = vec4(1.0);
		return;
	}
	bool paint = false;
	vec2 p = p0;
	for(int i = 1; i <= 20; ++i) {
		float t = float(i) * 1.0 / 20.0;
		float it = 1.0 - t;
		vec2 b = it * it * p0 + 2 * it * t * p1 + t * t * p2;
		paint = paint || line(b, p, z);
		p = b;
	}
	if(paint) {
		fragColor = vec4(1.0);
		return;
	}
	fragColor = vec4(0.5);
}
