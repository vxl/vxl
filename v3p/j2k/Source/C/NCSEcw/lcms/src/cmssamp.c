//
//  Little cms
//  Copyright (C) 1998-2004 Marti Maria
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "lcms.h"


// ---------------------------------------------------------------------------------

// Quantize a value 0 <= i < MaxSamples

WORD _cmsQuantizeVal(double i, int MaxSamples)
{
       double x;

       x = ((double) i * 65535.) / (double) (MaxSamples - 1);

       return (WORD) floor(x + .5);
}


// Is a table linear?

int cmsIsLinear(WORD Table[], int nEntries)
{
       register int i;
       int diff;

       for (i=0; i < nEntries; i++) {

           diff = abs((int) Table[i] - (int) _cmsQuantizeVal(i, nEntries));              
           if (diff > 3)
                     return 0;
       }

       return 1;
}



// pow() restricted to integer

static
int ipow(int base, int exp)
{
        int res = base;

        while (--exp)
               res *= base;

        return res;
}


// Given n, 0<=n<=clut^dim, returns the colorant.

static
int ComponentOf(int n, int clut, int nColorant)
{
        if (nColorant <= 0)
                return (n % clut);

        n /= ipow(clut, nColorant);

        return (n % clut);
}



// This routine does a sweep on whole input space, and calls its callback
// function on knots. returns TRUE if all ok, FALSE otherwise.

BOOL LCMSEXPORT cmsSample3DGrid(LPLUT Lut, _cmsSAMPLER Sampler, LPVOID Cargo, DWORD dwFlags)
{
   int i, t, nTotalPoints, Colorant, index;
   WORD In[MAXCHANNELS], Out[MAXCHANNELS];

   nTotalPoints = ipow(Lut->cLutPoints, Lut -> InputChan);

   index = 0;
   for (i = 0; i < nTotalPoints; i++) {

        for (t=0; t < (int) Lut -> InputChan; t++) {

                Colorant =  ComponentOf(i, Lut -> cLutPoints, (Lut -> InputChan - t  - 1 ));
                In[t]    = _cmsQuantizeVal(Colorant, Lut -> cLutPoints);
        }


        if (dwFlags & SAMPLER_HASTL1) {

                 for (t=0; t < (int) Lut -> InputChan; t++)
                     In[t] = cmsReverseLinearInterpLUT16(In[t],
                                                Lut -> L1[t],
                                                &Lut -> In16params);
        }


        // if (dwFlags & SAMPLER_INSPECT) {

             for (t=0; t < (int) Lut -> OutputChan; t++)
                        Out[t] = Lut->T[index + t];
        // }


        if (!Sampler(In, Out, Cargo))
                return FALSE;

        if (!(dwFlags & SAMPLER_INSPECT)) {
            
            if (dwFlags & SAMPLER_HASTL2) {

                for (t=0; t < (int) Lut -> OutputChan; t++)
                     Out[t] = cmsReverseLinearInterpLUT16(Out[t],
                                                   Lut -> L2[t],
                                                   &Lut -> Out16params);
                }

        
            for (t=0; t < (int) Lut -> OutputChan; t++)
                        Lut->T[index + t] = Out[t];

        }

        index += Lut -> OutputChan;

    }

    return TRUE;
}






// choose reasonable resolution
int _cmsReasonableGridpointsByColorspace(icColorSpaceSignature Colorspace, DWORD dwFlags)
{
    int nChannels;

    // Already specified?
    if (dwFlags & 0x00FF0000) {
            // Yes, grab'em
            return (dwFlags >> 16) & 0xFF;
    }

    nChannels = _cmsChannelsOf(Colorspace);

    // HighResPrecalc is maximum resolution

    if (dwFlags & cmsFLAGS_HIGHRESPRECALC) {

        if (nChannels > 4) 
                return 7;       // 7 for Hifi

        if (nChannels == 4)     // 23 for CMYK
                return 23;
    
        return 49;      // 49 for RGB and others        
    }


    // LowResPrecal is stripped resolution

    if (dwFlags & cmsFLAGS_LOWRESPRECALC) {
        
        if (nChannels > 4) 
                return 6;       // 6 for Hifi

        if (nChannels == 1) 
                return 33;      // For monochrome

        return 17;              // 17 for remaining
    }

    // Default values

    if (nChannels > 4) 
                return 7;       // 7 for Hifi

    if (nChannels == 4)
                return 17;      // 17 for CMYK

    return 33;                  // 33 for RGB
    
}

// Sampler implemented by another transform. This is a clean way to
// precalculate the devicelink 3D CLUT for almost any transform

static
int XFormSampler(register WORD In[], register WORD Out[], register LPVOID Cargo)
{
        cmsDoTransform((cmsHTRANSFORM) Cargo, In, Out, 1);
        return TRUE;
}

// This routine does compute the devicelink CLUT containing whole
// transform. Handles any channel number.

LPLUT _cmsPrecalculateDeviceLink(cmsHTRANSFORM h, DWORD dwFlags)
{
       _LPcmsTRANSFORM p = (_LPcmsTRANSFORM) h;
       LPLUT Grid;
       int nGridPoints;
       DWORD dwFormatIn, dwFormatOut;
       int ChannelsIn, ChannelsOut;
       LPLUT SaveGamutLUT;

       // Remove any gamut checking
       SaveGamutLUT = p ->Gamut;
       p ->Gamut = NULL;

       ChannelsIn   = _cmsChannelsOf(p -> EntryColorSpace);
       ChannelsOut  = _cmsChannelsOf(p -> ExitColorSpace);
               
       nGridPoints = _cmsReasonableGridpointsByColorspace(p -> EntryColorSpace, dwFlags);
     
       Grid =  cmsAllocLUT();
       if (!Grid) return NULL;

       Grid = cmsAlloc3DGrid(Grid, nGridPoints, ChannelsIn, ChannelsOut);

       // Compute device link on 16-bit basis
       dwFormatIn   = (CHANNELS_SH(ChannelsIn)|BYTES_SH(2));
       dwFormatOut  = (CHANNELS_SH(ChannelsOut)|BYTES_SH(2));

       p -> FromInput = _cmsIdentifyInputFormat(p, dwFormatIn);
       p -> ToOutput  = _cmsIdentifyOutputFormat(p, dwFormatOut);

       // Fix gamut & gamma possible mismatches. 
           
       if (!(dwFlags & cmsFLAGS_NOPRELINEARIZATION)) {

           cmsHTRANSFORM hOne[1];
           hOne[0] = h;
                
           _cmsComputePrelinearizationTablesFromXFORM(hOne, 1, Grid);
       }
        
            
       // Attention to this typecast! we can take the luxury to
       // do this since cmsHTRANSFORM is only an alias to a pointer
       // to the transform struct.

       if (!cmsSample3DGrid(Grid, XFormSampler, (LPVOID) p, Grid -> wFlags)) {

                cmsFreeLUT(Grid);
                return NULL;
       }
      
      
       p ->Gamut = SaveGamutLUT;
       return Grid;
}





// Fix broken LUT. just to obtain other CMS compatibility

static
void PatchLUT(LPLUT Grid, WORD At[], WORD Value[],
                     int nChannelsOut, int nChannelsIn)
{
       LPL16PARAMS p16  = &Grid -> CLut16params;
       double     px, py, pz, pw;
       int        x0, y0, z0, w0;
       int        i, index;


       if (Grid ->wFlags & LUT_HASTL1) return;  // There is a prelinearization

       px = ((double) At[0] * (p16->Domain)) / 65535.0;
       py = ((double) At[1] * (p16->Domain)) / 65535.0;
       pz = ((double) At[2] * (p16->Domain)) / 65535.0;
       pw = ((double) At[3] * (p16->Domain)) / 65535.0;

       x0 = (int) floor(px);
       y0 = (int) floor(py);
       z0 = (int) floor(pz);
       w0 = (int) floor(pw);

       if (nChannelsIn == 4) {

              if (((px - x0) != 0) ||
                  ((py - y0) != 0) ||
                  ((pz - z0) != 0) ||
                  ((pw - w0) != 0)) return; // Not on exact node

              index = p16 -> opta4 * x0 +
                      p16 -> opta3 * y0 +
                      p16 -> opta2 * z0 +
                      p16 -> opta1 * w0;
       }
       else 
	   if (nChannelsIn == 3) {

              if (((px - x0) != 0) ||
                  ((py - y0) != 0) ||
                  ((pz - z0) != 0)) return;  // Not on exact node

              index = p16 -> opta3 * x0 +
                      p16 -> opta2 * y0 +
                      p16 -> opta1 * z0;
       }
	   else 
       if (nChannelsIn == 1) {

		      if (((px - x0) != 0)) return;	// Not on exact node
		                  
			  index = p16 -> opta1 * x0;	
	   }
       else {
           cmsSignalError(LCMS_ERRC_ABORTED, "(internal) %d Channels are not supported on PatchLUT", nChannelsIn);
           return;
       }

       for (i=0; i < nChannelsOut; i++)
              Grid -> T[index + i] = Value[i];

}



BOOL _cmsFixWhiteMisalignment(_LPcmsTRANSFORM p)
{

       WORD *WhitePointIn, *WhitePointOut, *BlackPointIn, *BlackPointOut;
       int nOuts, nIns;


       if (!p -> DeviceLink) return FALSE;
       
       if (p ->Intent == INTENT_ABSOLUTE_COLORIMETRIC) return FALSE;
       if ((p ->PreviewProfile != NULL) && 
           (p ->ProofIntent == INTENT_ABSOLUTE_COLORIMETRIC)) return FALSE;


       if (!_cmsEndPointsBySpace(p -> EntryColorSpace,
                                 &WhitePointIn, &BlackPointIn, &nIns)) return FALSE;
       

       if (!_cmsEndPointsBySpace(p -> ExitColorSpace,
                                   &WhitePointOut, &BlackPointOut, &nOuts)) return FALSE;
       
       // Fix white only

       PatchLUT(p -> DeviceLink, WhitePointIn, WhitePointOut, nOuts, nIns);
       // PatchLUT(p -> DeviceLink, BlackPointIn, BlackPointOut, nOuts, nIns);

       return TRUE;
}

