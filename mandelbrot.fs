#version 330

const int iterations = 50;

in vec2 fragTexCoord;

out vec4 fragColor;

uniform vec2 zoom;
uniform vec2 offset;

vec2 cm(vec2 z1, vec2 z2) {
	return vec2(z1.x * z2.x - z1.y * z2.y, z1.x * z2.y + z1.y * z2.x);
}

void main() {
	vec2 c = vec2(zoom.x, -zoom.y) * (fragTexCoord - offset);
	vec2 z = vec2(0.0);
	for(int i = 0; i < iterations; ++i) {
		z = cm(z, z) + c;
		if(dot(z, z) >= 4.0) {
			fragColor = vec4(0.0);
			return;
		}
	}
	fragColor = vec4(1.0);
}
