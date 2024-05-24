#version 330

const float sw = 800.0;
const float sh = 600.0;

in vec2 fragTexCoord;

out vec4 fragColor;

uniform vec2 zoom;
uniform vec2 offset;

float f(float x, float y) {
	return                               ;//reserved space
}

void main() {
	vec2 z = vec2(zoom.x, -zoom.y) * (fragTexCoord - offset);
	float dx = 1.0 / sw * zoom.x, dy = 1.0 / sh * zoom.y;
	float s1 = sign(f(z.x, z.y)), s2 = sign(f(z.x + dx, z.y)),
	      s3 = sign(f(z.x, z.y - dy)), s4 = sign(f(z.x + dx, z.y - dy));
	if(s1 == s2 && s2 == s3 && s3 == s4) {
		fragColor = vec4(0.0);
		return;
	}
	fragColor = vec4(1.0);
}
