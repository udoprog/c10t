// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "image/color.hpp"

#include <sstream>
#include <string>

float color_i_to_f[] = {
    0.0000f, 0.0039f, 0.0078f, 0.0118f, 0.0157f, 0.0196f, 0.0235f, 0.0275f,
    0.0314f, 0.0353f, 0.0392f, 0.0431f, 0.0471f, 0.0510f, 0.0549f, 0.0588f,
    0.0627f, 0.0667f, 0.0706f, 0.0745f, 0.0784f, 0.0824f, 0.0863f, 0.0902f,
    0.0941f, 0.0980f, 0.1020f, 0.1059f, 0.1098f, 0.1137f, 0.1176f, 0.1216f,
    0.1255f, 0.1294f, 0.1333f, 0.1373f, 0.1412f, 0.1451f, 0.1490f, 0.1529f,
    0.1569f, 0.1608f, 0.1647f, 0.1686f, 0.1725f, 0.1765f, 0.1804f, 0.1843f,
    0.1882f, 0.1922f, 0.1961f, 0.2000f, 0.2039f, 0.2078f, 0.2118f, 0.2157f,
    0.2196f, 0.2235f, 0.2275f, 0.2314f, 0.2353f, 0.2392f, 0.2431f, 0.2471f,
    0.2510f, 0.2549f, 0.2588f, 0.2627f, 0.2667f, 0.2706f, 0.2745f, 0.2784f,
    0.2824f, 0.2863f, 0.2902f, 0.2941f, 0.2980f, 0.3020f, 0.3059f, 0.3098f,
    0.3137f, 0.3176f, 0.3216f, 0.3255f, 0.3294f, 0.3333f, 0.3373f, 0.3412f,
    0.3451f, 0.3490f, 0.3529f, 0.3569f, 0.3608f, 0.3647f, 0.3686f, 0.3725f,
    0.3765f, 0.3804f, 0.3843f, 0.3882f, 0.3922f, 0.3961f, 0.4000f, 0.4039f,
    0.4078f, 0.4118f, 0.4157f, 0.4196f, 0.4235f, 0.4275f, 0.4314f, 0.4353f,
    0.4392f, 0.4431f, 0.4471f, 0.4510f, 0.4549f, 0.4588f, 0.4627f, 0.4667f,
    0.4706f, 0.4745f, 0.4784f, 0.4824f, 0.4863f, 0.4902f, 0.4941f, 0.4980f,
    0.5020f, 0.5059f, 0.5098f, 0.5137f, 0.5176f, 0.5216f, 0.5255f, 0.5294f,
    0.5333f, 0.5373f, 0.5412f, 0.5451f, 0.5490f, 0.5529f, 0.5569f, 0.5608f,
    0.5647f, 0.5686f, 0.5725f, 0.5765f, 0.5804f, 0.5843f, 0.5882f, 0.5922f,
    0.5961f, 0.6000f, 0.6039f, 0.6078f, 0.6118f, 0.6157f, 0.6196f, 0.6235f,
    0.6275f, 0.6314f, 0.6353f, 0.6392f, 0.6431f, 0.6471f, 0.6510f, 0.6549f,
    0.6588f, 0.6627f, 0.6667f, 0.6706f, 0.6745f, 0.6784f, 0.6824f, 0.6863f,
    0.6902f, 0.6941f, 0.6980f, 0.7020f, 0.7059f, 0.7098f, 0.7137f, 0.7176f,
    0.7216f, 0.7255f, 0.7294f, 0.7333f, 0.7373f, 0.7412f, 0.7451f, 0.7490f,
    0.7529f, 0.7569f, 0.7608f, 0.7647f, 0.7686f, 0.7725f, 0.7765f, 0.7804f,
    0.7843f, 0.7882f, 0.7922f, 0.7961f, 0.8000f, 0.8039f, 0.8078f, 0.8118f,
    0.8157f, 0.8196f, 0.8235f, 0.8275f, 0.8314f, 0.8353f, 0.8392f, 0.8431f,
    0.8471f, 0.8510f, 0.8549f, 0.8588f, 0.8627f, 0.8667f, 0.8706f, 0.8745f,
    0.8784f, 0.8824f, 0.8863f, 0.8902f, 0.8941f, 0.8980f, 0.9020f, 0.9059f,
    0.9098f, 0.9137f, 0.9176f, 0.9216f, 0.9255f, 0.9294f, 0.9333f, 0.9373f,
    0.9412f, 0.9451f, 0.9490f, 0.9529f, 0.9569f, 0.9608f, 0.9647f, 0.9686f,
    0.9725f, 0.9765f, 0.9804f, 0.9843f, 0.9882f, 0.9922f, 0.9961f, 1.0000f
};

float alpha_over_c(float u, float o, float ua, float oa);

/**
 * Takes two color values and does an alpha over blending without using floats.
 */
inline float alpha_over_c(float ac, float aa, float bc, float ba) {
  return (ac * aa) + (bc * ba - aa * bc * ba);
}

void color::blend(const color &other) {
  if (other.is_invisible()) return;

  if (other.is_opaque() || is_invisible()) {
    r = other.r;
    g = other.g;
    b = other.b;
    a = other.a;
    return;
  }
  
  r = alpha_over_c(other.r, other.a, r, a);
  g = alpha_over_c(other.g, other.a, g, a);
  b = alpha_over_c(other.b, other.a, b, a);
  a = a + other.a * (1.0f - a);
  r = ((r * 1.0f) / a);
  g = ((g * 1.0f) / a);
  b = ((b * 1.0f) / a);
}

// pull color down towards black
void color::darken(float f) {
  r = std::max(r - (r * f), 0.0f);
  g = std::max(g - (g * f), 0.0f);
  b = std::max(b - (b * f), 0.0f);
}

void color::darken(int f) {
  darken(color_i_to_f[uint8_t(f)]);
}

void color::lighten(float f) {
  r = std::min(r + (r * f), 1.0f);
  g = std::min(g + (g * f), 1.0f);
  b = std::min(b + (b * f), 1.0f);
}

void color::lighten(int f) {
  lighten(color_i_to_f[uint8_t(f)]);
}
