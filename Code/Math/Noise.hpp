#ifndef _NOISE_HPP
#define _NOISE_HPP 1

#include "Utilities/Array2D.hpp"

void fn_PerlinNoise2D(const Array2D<float>& fSeed, int nOctaves, float fBias, Array2D<float>& fOutput)
{
    fOutput.mt_Resize(fSeed.mt_Get_Width(), fSeed.mt_Get_Height());

    for (int x = 0; x < fSeed.mt_Get_Width(); x++)
    {
        for (int y = 0; y < fSeed.mt_Get_Height(); y++)
        {
            float fNoise = 0.0f;
            float fScaleAcc = 0.0f;
            float fScale = 1.0f;

            for (int o = 0; o < nOctaves; o++)
            {
                int nPitch = nWidth >> o;
                int nSampleX1 = (x / nPitch) * nPitch;
                int nSampleY1 = (y / nPitch) * nPitch;

                int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
                int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

                float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
                float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

                float fSampleT = (1.0f - fBlendX) * fSeed(nSampleX1, nSampleY1) + fBlendX * fSeed(nSampleX2, nSampleY1);
                float fSampleB = (1.0f - fBlendX) * fSeed(nSampleX1, nSampleY2) + fBlendX * fSeed(nSampleX2, nSampleY2);

                fScaleAcc += fScale;
                fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
                fScale = fScale / fBias;
            }

            // Scale to seed range
            fOutput(x, y) = fNoise / fScaleAcc;
        }
    }

}



#endif // _NOISE_HPP
