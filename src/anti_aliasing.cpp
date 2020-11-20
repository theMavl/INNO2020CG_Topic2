#include "anti_aliasing.h"

AntiAliasing::AntiAliasing(short width, short height) : AccelerationStructures(width, height)
{
}

AntiAliasing::~AntiAliasing()
{
}

void AntiAliasing::DrawScene()
{
	camera.SetRenderTargetSize(width * 2, height * 2);
	for (auto x = 0; x < width; x++) {
#pragma omp parallel for
		for (auto y = 0; y < height; y++) {
			Ray camera_ray1 = camera.GetCameraRay(2 * x, 2 * y);
			Payload payload1 = TraceRay(camera_ray1, raytracing_depth);

			Ray camera_ray2 = camera.GetCameraRay(2 * x + 1, 2 * y);
			Payload payload2 = TraceRay(camera_ray2, raytracing_depth);

			Ray camera_ray3 = camera.GetCameraRay(2 * x, 2 * y + 1);
			Payload payload3 = TraceRay(camera_ray3, raytracing_depth);

			Ray camera_ray4 = camera.GetCameraRay(2 * x + 1, 2 * y + 1);
			Payload payload4 = TraceRay(camera_ray4, raytracing_depth);

			float3 color = (payload1.color + payload2.color + payload3.color + payload4.color) / 4.f;

			SetPixel(x, y, color);
		}
	}
}
