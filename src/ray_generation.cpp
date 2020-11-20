#include "ray_generation.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

RayGenerationApp::RayGenerationApp(short width, short height) :
	width(width),
	height(height)
{
	camera.SetRenderTargetSize(width, height);
}

RayGenerationApp::~RayGenerationApp()
{
}

void RayGenerationApp::SetCamera(float3 position, float3 direction, float3 approx_up)
{
	camera.SetPosition(position);
	camera.SetDirection(direction);
	camera.SetUp(approx_up);
}

void RayGenerationApp::Clear()
{
	frame_buffer.resize(width * height);
}

void RayGenerationApp::DrawScene()
{
	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			Ray camera_ray = camera.GetCameraRay(x, y);
			Payload payload = TraceRay(camera_ray, raytracing_depth);
			SetPixel(x, y, payload.color);
		}
	}
}

int RayGenerationApp::Save(std::string filename) const
{
	int result = stbi_write_png(filename.c_str(), width, height, 3, frame_buffer.data(), width * 3);

	if (result == 1)
		system((std::string("start ") + filename).c_str());

	return (1 - result); // convert stb OK code to the most usable
}

Payload RayGenerationApp::TraceRay(const Ray& ray, const unsigned int max_raytrace_depth) const
{
	return Miss(ray);
}

Payload RayGenerationApp::Miss(const Ray& ray) const
{
	Payload payload;
	float blue = 0.5 * (ray.direction.y + 1.0f);
	payload.color = { 0.f, 0.f, blue };

	return payload;
}

void RayGenerationApp::SetPixel(unsigned short x, unsigned short y, float3 color)
{
	frame_buffer[static_cast<size_t>(y)* static_cast<size_t>(width) + static_cast<size_t>(x)] =
		byte3{ static_cast<uint8_t>(255 * color.x), static_cast<uint8_t>(255 * color.y), static_cast<uint8_t>(255 * color.z) };
}

Camera::Camera():
	height(0), width(0)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float3 position)
{
	this->position = position;
}

void Camera::SetDirection(float3 direction)
{
	this->direction = normalize(direction - this->position);
}

void Camera::SetUp(float3 approx_up)
{
	this->right = cross(this->direction, normalize(approx_up));
	this->up = cross(this->right, this->direction);
}

void Camera::SetRenderTargetSize(short width, short height)
{
	this->width = width;
	this->height = height;
}

Ray Camera::GetCameraRay(short x, short y) const
{
	return Camera::GetCameraRay(x, y, float3{ 0.f });
}

Ray Camera::GetCameraRay(short x, short y, float3 jitter) const
{
	float u = (2.0f * (x + 0.5) / static_cast<float>(width) - 1) * static_cast<float>(width) / static_cast<float>(height);
	float v = 2.0f * (y + 0.5) / static_cast<float>(height) - 1;

	float3 ray_direction = this->direction + u * this->right - v * this->up + jitter;

	return Ray(this->position, ray_direction);
}
