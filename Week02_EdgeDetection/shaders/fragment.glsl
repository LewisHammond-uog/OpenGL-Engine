#version 330

//inputs from the previous pipeline stage
in vec4 vColour;
in vec2 vTexCoord;

//output from the fragment stage
out vec4 fragColour;

uniform sampler2D u_Texture;
uniform float u_Width;
uniform float u_Height;

//Sample Pass 1 - Vertical
// 1 2 1
// 0 x 0
// 1 2 1

//Sample Pass 2 - Horizontal
// 1 0 1
// 2 x 2
// 1 0 1

float Intensity(vec3 a_rgb)
{
	//Get the magnitude of a rgb value
	return sqrt(dot(a_rgb, a_rgb));
}


//Gets the RGB of the pixels surrounding the provided pixel
void GetTexelKernel(out float a_pixel[8], in vec2 a_uv, in sampler2D a_texure)
{
	//What fraction to do we need to move over by to get one pixel over?
	float w = 1.0 / u_Width;
	float h = 1.0 / u_Height;
	
	//Sample the pixels from around the center a_uv coordinate
	a_pixel[0] = Intensity(texture(a_texure, a_uv + vec2( -w, h)).rgb);
	a_pixel[1] = Intensity(texture(a_texure, a_uv + vec2(0.0, h)).rgb);
	a_pixel[2] = Intensity(texture(a_texure, a_uv + vec2( w,  h)).rgb);
	a_pixel[3] = Intensity(texture(a_texure, a_uv + vec2(-w,  0.0)).rgb);
	a_pixel[4] = Intensity(texture(a_texure, a_uv + vec2( w,  0.0)).rgb);
	a_pixel[5] = Intensity(texture(a_texure, a_uv + vec2(-w,  -h)).rgb);
	a_pixel[6] = Intensity(texture(a_texure, a_uv + vec2(0.0, -h)).rgb);
	a_pixel[7] = Intensity(texture(a_texure, a_uv + vec2( w,  -h)).rgb);
}

void main()
{
	float pixels[8];

	GetTexelKernel(pixels, vTexCoord, u_Texture);
	vec3 currentPixelRGB = texture(u_Texture, vTexCoord).rgb;

	//Do vertical sample
	float sobelVertical = pixels[5] + (2 * pixels[6]) + pixels[7] - (pixels[0] + (2 * pixels[1]) + pixels[2]);
	//Do horizontal sample
	float sobelHorizontal = pixels[2] + (2 * pixels[4]) + pixels[7] - (pixels[0] + (2 * pixels[3]) + pixels[5]);

	float sobel = sqrt((sobelHorizontal * sobelHorizontal) + (sobelVertical * sobelVertical));

	sobel = step(0.6, sobel);

	fragColour = vec4(currentPixelRGB + (sobel), 1.0);
}