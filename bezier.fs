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

vec2 newton_bezier(vec2 t, vec2 p) {
	for(int i = 0; i < 40; ++i) {
		vec2 it = vec2(1.0) - t;
		//t.x -= (it.x * it.x * p0.x + 2 * it.x * t.x * p1.x + t.x * t.x * p2.x - p.x)/(2*it.x*(p1.x - p0.x) + 2*t.x*(p2.x - p1.x));
		//t.y -= (it.y * it.y * p0.y + 2 * it.y * t.y * p1.y + t.y * t.y * p2.y - p.y)/(2*it.y*(p1.y - p0.y) + 2*t.y*(p2.y - p1.y));
		t -= (it * it * p0 + 2 * it * t * p1 + t * t * p2 - p)/(2*it*(p1 - p0) + 2*t*(p2 - p1));
	}
	return t;
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
	/*
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
		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
		return;
	}
	*/
	vec2 t = newton_bezier(vec2(1.0, 1.0), z);
	if(0.0 <= t.x && t.x <= 1.0 && abs(t.x - t.y) < 0.01) {
		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
		return;
	}
	fragColor = vec4(0.5);
}
