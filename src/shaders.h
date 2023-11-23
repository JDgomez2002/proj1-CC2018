#pragma once
#include <iostream>
#include <cmath>
#include <random>
#include "color.h"
#include "uniform.h"
#include "fragment.h"
#include "FastNoiseLite.h"

Vertex vertexShader(const Vertex& vertex, const Uniform& uniforms) {

  glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

  glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

  glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

  glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
  transformedNormal = glm::normalize(transformedNormal);

  return Vertex{
    glm::vec3(screenVertex),
    transformedNormal,
    vertex.position
  };
};

Fragment fragmentShaderShip(Fragment& fragment) {
  fragment.color = fragment.color * fragment.intensity;
  return fragment;
}

float rand(glm:: vec3 co) {
  return glm::fract(sin(glm::dot(co, glm::vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

float random_float_in_range(float min, float max) {
  float random_number = rand();
  float range = max - min;
  return random_number / (float)RAND_MAX * range + min;
}

Fragment fragmentShaderJupiter(Fragment& fragment) {
  Color color;

  glm::vec3 baseColor = glm::vec3(1.0f, 0.6f, 0.2f);

  glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

  FastNoiseLite noiseGenerator;
  noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  float shadowIntensity = 0.1f; // Reduced shadow intensity

  float ox1 = 500.0f;
  float oy1 = 700.0f;
  float zoom1 = 150.0f;
  float noiseValue1 = noiseGenerator.GetNoise((uv.x + ox1) * zoom1, (uv.y + oy1) * zoom1);
  glm::vec3 jupiterLayer = glm::vec3(0.9f, 0.7f, 0.4f) * noiseValue1 * (1.0f - shadowIntensity);

  float altitude = uv.y * 2.0f - 1.0f;
  glm::vec3 altitudeColor = glm::vec3(0.9f, 0.7f, 0.4f) * altitude * (1.0f - shadowIntensity);

  glm::vec3 finalColor = baseColor + jupiterLayer + altitudeColor;

  color = Color(static_cast<int>(finalColor.x * 255), static_cast<int>(finalColor.y * 255), static_cast<int>(finalColor.z * 255));

  fragment.color = color;

  return fragment;
}

Fragment fragmentShaderSun(Fragment& fragment) {
  Color color;

  // Centro de la pantalla (suponiendo que el centro es (0, 0))
  glm::vec2 center = glm::vec2(0.0f, 0.0f);

  // Radio del sol
  float sunRadius = 0.1f;

  // Calcula la distancia del fragmento al centro de la pantalla
  glm::vec2 fragmentPos = glm::vec2(fragment.original.x, fragment.original.y);
  float distanceToCenter = glm::length(fragmentPos - center);

  // Dirección de la luz (puedes ajustarla según tus necesidades)
  glm::vec2 lightDirection = glm::normalize(glm::vec2(1.0f, 1.0f)); // Diagonal superior derecha

  // Calcula el producto punto entre la dirección de la luz y la dirección al fragmento
  float dotProduct = glm::dot(glm::normalize(fragmentPos), lightDirection);

  float shadowFactor = 0.55f; // Factor de sombra (ajusta según tus necesidades)
  // Calcula un color más oscuro utilizando el producto punto como factor de sombra
  int r = static_cast<int>(255 * (2.5f - shadowFactor * dotProduct) + 5.0f);
  int g = static_cast<int>(70 * (2.5f - shadowFactor * dotProduct) + 3.5f);
  int b = static_cast<int>( (shadowFactor * dotProduct) + 0.5f);
  color = Color(r+200, g, b);

  fragment.color = color;

  return fragment;
}

Fragment fragmentShaderEarth(Fragment& fragment) {
  Color color;

  glm::vec3 groundColor = glm::vec3(0.44f, 0.51f, 0.33f);
  glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
  glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

  glm::vec3 uv = glm::vec3(
    fragment.original.x,
    fragment.original.y,
    fragment.original.z
  );

  FastNoiseLite noiseGenerator;
  noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  float ox = 1200.0f;
  float oy = 3000.0f;
  float zoom = 200.0f;

  float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom, uv.z * zoom);

  glm::vec3 tmpColor = (noiseValue < 0.5f) ? oceanColor : groundColor;

  float oxc = 3400.0f;
  float oyc = 9800.0f;
  float zoomc = 100.0f;

  float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc, uv.z * zoomc);

  float random_float = random_float_in_range(0.5, 0.8);

  if (noiseValueC > random_float) {
    tmpColor = cloudColor;
  }

  color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

  fragment.color = color * fragment.intensity;

  return fragment;
}

Fragment fragmentShaderMars(Fragment& fragment) {
  Color color;

  glm::vec3 baseColor = glm::vec3(0.7f, 0.1f, 0.1f); // Color base similar a Marte (rojo/rocoso)

  glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

  FastNoiseLite noiseGenerator;
  noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  // Capa de textura roja para áreas rocosas
  float ox1 = 500.0f;
  float oy1 = 700.0f;
  float zoom1 = 150.0f;
  float noiseValue1 = noiseGenerator.GetNoise((uv.x + ox1) * zoom1, (uv.y + oy1) * zoom1);
  glm::vec3 rockyLayer = glm::vec3(0.6f, 0.3f, 0.2f) * noiseValue1;

  // Añade variaciones de color basadas en la altitud
  float altitude = uv.y * 2.0f - 1.0f; // Suponemos que uv.y está en el rango [0, 1]
  glm::vec3 altitudeColor = glm::vec3(0.1f, 0.1f, 0.1f) * altitude;

  // Añade todas las capas de textura y variación de color
  glm::vec3 finalColor = baseColor + rockyLayer + altitudeColor;

  color = Color(static_cast<int>(finalColor.x * 255), static_cast<int>(finalColor.y * 255), static_cast<int>(finalColor.z * 255));

  fragment.color = color;

  return fragment;
}

Fragment fragmentShaderPluto(Fragment& fragment) {

  glm::vec3 baseColor = glm::vec3(0.4f, 0.4f, 1.0f); // Lighter blue base color resembling Pluto

  glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

  FastNoiseLite noiseGenerator;
  noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  // Red texture layer for rocky areas
  float ox1 = 500.0f;
  float oy1 = 700.0f;
  float zoom1 = 150.0f;
  float noiseValue1 = noiseGenerator.GetNoise((uv.x + ox1) * zoom1, (uv.y + oy1) * zoom1);
  glm::vec3 rockyLayer = glm::vec3(0.6f, 0.3f, 0.2f) * noiseValue1;

  // Adding color variations based on altitude
  float altitude = uv.y * 2.0f - 1.0f; // Assuming uv.y is in the range [0, 1]
  glm::vec3 altitudeColor = glm::vec3(0.15f, 0.15f, 0.15f) * altitude;

  // Combine texture layers and color variation
  glm::vec3 finalColor = baseColor + rockyLayer + altitudeColor;

  Color color = Color(static_cast<int>(finalColor.x * 255), static_cast<int>(finalColor.y * 255), static_cast<int>(finalColor.z * 255));

  fragment.color = color;

  return fragment;

};