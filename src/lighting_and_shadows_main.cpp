#include "lighting_and_shadows.h"
#include <chrono>
using namespace std;
int main(int argc, char* argv[])
{
	
	LightingAndShadows* render = new LightingAndShadows(1920, 1080);
	int result = render->LoadGeometry("models/CornellBox-Original.obj");
	if (result)
	{
		return result;
	}
	render->SetCamera(float3{ 0, 1.1f, 2 }, float3{ 0, 1, -1 }, float3{ 0, 1, 0 });
	render->AddLight(new Light(float3{ 0, 1.98f, -0.06f }, float3{ 0.78f, 0.78f, 0.78f }));
	render->Clear();
	auto start = chrono::high_resolution_clock::now();
	render->DrawScene();
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> diff = end - start;
	cout << "DrawScene: " << diff.count() << " s\n";
	result = render->Save("results/lighting.png");
	return result;
}