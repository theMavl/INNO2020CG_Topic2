#include "mt_algorithm.h"


Sphere::Sphere(float3 center, float radius) :
	center(center), radius(radius)
{
}

Sphere::~Sphere()
{
}

IntersectableData Sphere::Intersect(const Ray& ray) const
{
	float3 oc = ray.position - center;
	float a = dot(ray.direction, ray.direction);
	float b = 2.0f * dot(ray.direction, oc);
	float c = dot(oc, oc) - radius * radius;

	float d = b * b - 4 * a * c;
	if (d < 0)
		return IntersectableData(d);

	float x0 = (-b - sqrtf(d)) / (2.f * a);
	float x1 = (-b + sqrtf(d)) / (2.f * a);

	float t = std::min(x0, x1);
	if (t < 0)
		t = std::max(x0, x1);

	return IntersectableData(t);
}



MTAlgorithm::MTAlgorithm(short width, short height) : RayGenerationApp(width, height)
{
}

MTAlgorithm::~MTAlgorithm()
{
}

int MTAlgorithm::LoadGeometry(std::string filename)
{
	objects.push_back(new Sphere(float3{ -0, 0, -1 }, 0.4f));

	Vertex a(float3{ -0.5f, -0.5f, -1 }, float3{ 0, 0, 1 }, float3(), float3{ 1, 0, 0 });
	Vertex b(float3{ 0.5f, -0.5f, -1 }, float3{ 0, 0, 1 }, float3(), float3{ 1, 0, 0 });
	Vertex c(float3{ 0,  0.5f, -1 }, float3{ 0, 0, 1 }, float3(), float3{ 1, 0, 0 });
	objects.push_back(new Triangle(a, b, c));

	return 0;
}

Payload MTAlgorithm::TraceRay(const Ray& ray, const unsigned int max_raytrace_depth) const
{
	IntersectableData closest_hit_data = IntersectableData(t_max);
	for (auto& object : objects) {
		IntersectableData data = object->Intersect(ray);
		if (data.t > t_min && data.t < closest_hit_data.t)
			closest_hit_data = data;
	}
	if (closest_hit_data.t < t_max)
		return Hit(ray, closest_hit_data);
	else
		return Miss(ray);
}

Payload MTAlgorithm::Hit(const Ray& ray, const IntersectableData& data) const
{
	Payload payload;

	//float far = 1.0f-(data.t - 2.0f / 0.4f + 1.5f);

	payload.color = data.baricentric;//* far;

	return payload;
}

Triangle::Triangle(Vertex a, Vertex b, Vertex c) :
	a(a), b(b), c(c)
{
	ba = b.position - a.position;
	ca = c.position - a.position;
}

Triangle::Triangle() :
	a(float3{ 0, 0 ,0 }), b(float3{ 0, 0 ,0 }), c(float3{ 0, 0 ,0 })
{
}

Triangle::~Triangle()
{
}

IntersectableData Triangle::Intersect(const Ray& ray) const
{
	float3 pvec = cross(ray.direction, ca);
	float d = dot(ba, pvec);

	if (d > -1e-8 && d < 1e-8)
		return IntersectableData(-1);

	float inv_det = 1.f / d;

	float3 tvec = ray.position - a.position;
	float u = dot(tvec, pvec) * inv_det;
	if (u < 0 || u > 1)
		return IntersectableData(-1);

	float3 qvec = cross(tvec, ba);
	float v = dot(ray.direction, qvec) * inv_det;

	if (v < 0 || v > 1 || u + v > 1)
		return IntersectableData(-1);

	float t = dot(ca, qvec) * inv_det;

	return IntersectableData(t, float3{ 1.f - u - v, u, v });
}
