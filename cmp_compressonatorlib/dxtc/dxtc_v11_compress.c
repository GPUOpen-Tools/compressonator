/*
 * ===============================================================================
 *  Copyright (c) 2004-2006 ATI Technologies Inc.
 * ===============================================================================
 * 
 * dxtc_v11_compress.c : A high-performance, reasonable quality DXTC compressor
 *
 * PREFACE:
 *
 * All DXTC compressors have a big issue in trading off performance versus image quality. A
 * very high quality image is typically obtained by very many stepwise refinements,
 * looking for the output that contains the lowest error metric. These type of compressors
 * suffer from two major performance issues: first, the need to process each block very many
 * times - in some implementations, the entire block computation is repeated on each iteration -
 * is inherently slow; and secondly, that the performance is not necessarily predictable (in
 * worst-case images, both poor quality and slow results will be obtained.
 *
 *
 *
 * TOWARDS A BETTER SOLUTION:
 *
 * This compressor is an attempt to improve performance by generating a block based on a (mostly)
 * mathematical method (and a pretty simple one at that). The worst-case performance bound of this
 * algorithm is not much more than a factor of 2 worse than the best case (given that it does not hit
 * inherent slowdown conditions such as a lot of denormal operands to the FPU, which is not a major
 * issue).
 *
 * Stepwise refinement is a part of this method, used in a controlled manner to solve a
 * particularly difficult analytical problem (that of the correct mapping of the endpoints to
 * best represent the line). By reducing the refinement to a 1D process this makes it more
 * amenable to rapid implementation (at the cost of a more limited ability to match the source).
 *
 *
 *
 * COMPRESSION ALGORITHM
 *
 * The DXTC compression problem - for compressors avoiding an exhaustive search - is largely
 * divided into two main issues
 *        -    Find the best axis for representing the pixels in the block (the compressed block's
 *            pixels must obviously all lie along this axis)
 *        -    Map the points in the block onto this axis in the most efficient manner.
 *
 * Different compressors choose to attack the problem in different ways. In this compressor the
 * two problems are tackled largely totally separately.
 *
 *
 * Finding the axis is performed by a simplistic line-fitting method (using the average and the
 * absolute centred average to find two points and calling this the axis). This provides good
 * results in many circumstances, although it can end up producing poor results where the axis
 * ends up not representative of any of the individual pixels in the block.
 *
 * The pixels are then mapped onto the axis by a nearest-point-of-approach method. The critical
 * value stored is the distance along the axis.
 *
 * In order to correct for cases when the axis has been poorly mapped, the points are clustered
 * (as they would be for the final image) and the average axis mapping error for each cluster
 * is compared. If the error looks bad (typically, in these cases it is large and highly asymmetrical
 * between the two clusters), then the axis is recomputed using the average axis between
 * the two end clusters. This process could be further repeated but this appears reasonably
 * unnecessary - frankly, in the cases where refinement helps it seems unlikely that any very rapid
 * compressor can provide sufficient quality for the image to be definitively usable. In fact,
 * the 'guess' axis has proved sufficiently good in most cases that the limited form I implement
 * this refinement in hurts quality as often as not, and the refinement costs 10-20% in performance
 * so is not worthwhile.
 *
 *
 * Several methods were experimented with for determining the axis endpoints. The best general
 * initial guess was found to be to set the extreme points on the axis to be the resultant block
 * colours and represent those precisely. Although this is not the general 'best solution' it is
 * a very important best solution from a human eye point of view.
 *
 * There is then a stepwise refinement performed that significantly reduces noise by moving these
 * inital endpoints (inwards) and finding the value of minimum error. The compressor can experiment
 * with other modes of this movement, but this doesn't in general provide much higher image quality
 * and does significantly increase compression time.
 *
 *
 * Finally, the RGB565 colour values are computed and the final output is generated. Generating the
 * RGB involves a rounding process that must match up to the inverse unrounding process performed
 * in the decompressor. This also forces the block to be opaque (in the current implementation only
 * 4-colour blocks are generated).
 *
 *
 *
 * OUTPUT BLOCK QUALITY DETERMINATION
 *
 * In order to compute if the compression was sucessful, three error metrics are computed:
 *
 * 1. the axis mapping error produced when mapping from the arbitrary points to the selected
 *        axis. This is representative of how poor the colour distortion inherent from the compression is.
 *        Since the axis generation bug was fixed, this is actually usually small. Seeing a large axis
 *        mapping error almost invariably means that the image is extremely hard to compress.
 * 2. the cluster mapping error, which is the error produced when mapping the points on the axis
 *        into the final clusters. This frequently overreacts on highly noisy images.
 * 3. antialiased images fail to trip cluster error, but frequently come out 'lumpy'. This is
 *        detected by analysing the clusters on a geometric basis; images that have many blocks with
 *        clusters appearing in geometrically ordered patterns tend to be antialiased images.
 *
 *
 *
 * CONCLUSIONS
 *
 * This compressor rapidly produces results that vary from very good on most images highly amenable
 * to compression, to adequate/poor on images not suited to compression. It is never as good as an
 * extremely high quality offline decoder, and should not be considered a substitute for such (although
 * it may be superior to the average offline decoder).
 *
 */

// Further work:
// Add transparent block support
// Add special cases for 1, 2 and some 3 colour blocks (mostly for speed reasons)
// Possibly test if 3 colour blocks would generate better results in some cases (done, not
// apparent in most test images)

#include <math.h>
#include <assert.h>
#include <string.h>

#include "dxtc_v11_compress.h"

static void DXTCDecompressBlock(DWORD block_32[16], DWORD block_dxtc[2])
{
    DWORD c0, c1, c2, c3;
    DWORD r0, g0, b0, r1, g1, b1;
    int i;

    c0 = block_dxtc[0] & 0xffff;
    c1 = block_dxtc[0]>>16;

    if (c0 > c1)
    {
        r0 = ((block_dxtc[0]&0xf800) >> 8);
        g0 = ((block_dxtc[0]&0x07e0) >> 3);
        b0 = ((block_dxtc[0]&0x001f) << 3);


        r1 = ((block_dxtc[0]&0xf8000000) >> 24);
        g1 = ((block_dxtc[0]&0x07e00000) >> 19);
        b1 = ((block_dxtc[0]&0x001f0000) >> 13);

        // Apply the lower bit replication to give full dynamic range
        r0 += (r0>>5); r1 += (r1>>5);
        g0 += (g0>>6); g1 += (g1>>6);
        b0 += (b0>>5); b1 += (b1>>5);

        c0 = (r0<<16) | (g0<<8) | b0;
        c1 = (r1<<16) | (g1<<8) | b1;
        c2 = (((2*r0+r1)/3)<<16) | (((2*g0+g1)/3)<<8) | (((2*b0+b1)/3));
        c3 = (((2*r1+r0)/3)<<16) | (((2*g1+g0)/3)<<8) | (((2*b1+b0)/3));


        for(i=0; i<16; i++)
        {
            switch((block_dxtc[1]>>(2*i)) & 3)
            {
            case 0:
                block_32[i] = c0;
                break;
            case 1:
                block_32[i] = c1;
                break;
            case 2:
                block_32[i] = c2;
                break;
            case 3:
                block_32[i] = c3;
                break;
            }
        }
    }
    else
    {
        // Dont support transparent decode, but have to handle the case when they're both the same
        {
            r0 = ((block_dxtc[0]&0xf800) >> 8);
            g0 = ((block_dxtc[0]&0x07e0) >> 3);
            b0 = ((block_dxtc[0]&0x001f) << 3);


            r1 = ((block_dxtc[0]&0xf8000000) >> 24);
            g1 = ((block_dxtc[0]&0x07e00000) >> 19);
            b1 = ((block_dxtc[0]&0x001f0000) >> 13);

            // Apply the lower bit replication to give full dynamic range
            r0 += (r0>>5); r1 += (r1>>5);
            g0 += (g0>>6); g1 += (g1>>6);
            b0 += (b0>>5); b1 += (b1>>5);

            c0 = (r0<<16) | (g0<<8) | b0;
            c1 = (r1<<16) | (g1<<8) | b1;
            c2 = (((r0+r1)/2)<<16) | (((g0+g1)/2)<<8) | (((b0+b1)/2));


            for(i=0; i<16; i++)
            {
                switch((block_dxtc[1]>>(2*i)) & 3)
                {
                case 0:
                    block_32[i] = c0;
                    break;
                case 1:
                    block_32[i] = c1;
                    break;
                case 2:
                    block_32[i] = c2;
                    break;
                case 3:
                    block_32[i] = 0xff00ff;
                    break;
                }

            }
        }
    }
}

static void DXTCDecompressAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2])
{
    BYTE v[8];

    DWORD t;
    int i;

    v[0] = (BYTE) (block_dxtc[0] & 0xff);
    v[1] = (BYTE) ((block_dxtc[0]>>8) & 0xff);

    if (v[0] > v[1])
    {
        // 8-colour block
        for(i=1; i<7; i++)
        {
            t = ((7-i)*v[0] + i*v[1] + 3) / 7;
            v[i+1] = (BYTE) t;
        }

        for(i=0; i<16; i++)
        {
            if (i > 5)
                t = (block_dxtc[1]>>((3*(i-6))+2) & 7);
            else if (i == 5)
                t = ((block_dxtc[1] & 3)<<1) + ((block_dxtc[0] >> 31)&1);
            else
                t = (block_dxtc[0]>>((3*i)+16) & 7);

            block_8[i] = v[t];
        }
    }
    else if (v[0] == v[1])
    {
        for(i=0; i<16; i++)
            block_8[i] = v[0];
    }
    else
    {
        assert(0);
    }
}

//#define TRY_3_COLOR

#define MARK_BLOCK    { /*v_r = v_b = v_g = 0.0f;*/ average_r = 255.0f;    /*average_g = 128.0f; average_b = 0.0f;*/ }

//#define AVERAGE_UNIQUE_PIXELS_ONLY        // Even with this the performance increase is minor

//#define AXIS_RGB        1
//#define AXIS_YCbCr    1        // Pretty poor in most cases, probably shouldn't be used
//#define AXIS_Y_ONLY    1        // Use for testing; generates greyscale output
#define AXIS_MUNGE        1        // Raises priority of G at expense of B - seems slightly better than no munging (needs exhaustive testing)
//#define AXIS_MUNGE2    1        // Raises priority of G further at expense of R and B - maybe slightly better again...

#if AXIS_RGB

#define CS_RED(r, g, b)        (r)
#define CS_GREEN(r, g, b)    (g)
#define CS_BLUE(r, g, b)    (b)
#define DCS_RED(r, g, b)    (r)
#define DCS_GREEN(r, g, b)    (g)
#define DCS_BLUE(r, g, b)    (b)

#elif AXIS_YCbCr
/*
 *    Y  =  0.29900 * R + 0.58700 * G + 0.11400 * B
 *    Cb = -0.16874 * R - 0.33126 * G + 0.50000 * B  + CENTER
 *    Cr =  0.50000 * R - 0.41869 * G - 0.08131 * B  + CENTER
 *
 *    R = Y                + 1.40200 * Cr
 *    G = Y - 0.34414 * Cb - 0.71414 * Cr
 *    B = Y + 1.77200 * Cb
 */
#define CS_GREEN(r, g, b)    (0.299f*(r) + 0.587f*(g) + 0.114f*(b))
#define CS_RED(r, g, b)        (0.5f*(r) - 0.41869f*(g) - 0.08131f*(b) + 128.0f)
#define CS_BLUE(r, g, b)    (-0.16874f*(r) - 0.33126f*(g) + 0.5f*(b) + 128.0f)
#define DCS_RED(r, g, b)    ((g) + 1.402f*((r)-128.0f))
#define DCS_GREEN(r, g, b)    ((g) - 0.34414f*((b) - 128.0f) - 0.71414f*((r)-128.0f))
#define DCS_BLUE(r, g, b)    ((g) + 1.772f*((b) - 128.0f))

#elif AXIS_Y_ONLY
#define CS_GREEN(r, g, b)    (0.299f*(r) + 0.587f*(g) + 0.114f*(b))
#define CS_RED(r, g, b)        (128.0f)
#define CS_BLUE(r, g, b)    (128.0f)
#define DCS_RED(r, g, b)    ((g) + 1.402f*((r)-128.0f))
#define DCS_GREEN(r, g, b)    ((g) - 0.34414f*((b) - 128.0f) - 0.71414f*((r)-128.0f))
#define DCS_BLUE(r, g, b)    ((g) + 1.772f*((b) - 128.0f))

#elif AXIS_MUNGE

#define CS_RED(r, g, b)        (r)
#define CS_GREEN(r, g, b)    (g)
#define CS_BLUE(r, g, b)    ((b+g)*0.5f)
#define DCS_RED(r, g, b)    (r)
#define DCS_GREEN(r, g, b)    (g)
#define DCS_BLUE(r, g, b)    ((2.0f*b)-g)

#elif AXIS_MUNGE2

#define CS_RED(r, g, b)        ((r+g)*0.5f)
#define CS_GREEN(r, g, b)    (g)
#define CS_BLUE(r, g, b)    ((b+3.0f*g)*0.25f)
#define DCS_RED(r, g, b)    ((2.0f*r)-g)
#define DCS_GREEN(r, g, b)    (g)
#define DCS_BLUE(r, g, b)    ((4.0f*b)-(3.0f*g))

#else
#error No axis type defined
#endif

#define ROUND_AND_CLAMP(v, shift)    \
{\
    if (v < 0) v = 0;\
    else if (v > 255) v = 255;\
    else v += (0x80>>shift) - (v>>shift);\
}

void DXTCV11CompressExplicitAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2])
{
    int i;
    block_dxtc[0] = block_dxtc[1] = 0;
    for (i = 0; i<16; i++)
    {
        int v = block_8[i];
        v = (v + 7 - (v >> 4));
        v >>= 4;
        if (v<0)
            v = 0;
        if (v>0xf)
            v = 0xf;
        if (i<8)
            block_dxtc[0] |= v << (4 * i);
        else
            block_dxtc[1] |= v << (4 * (i - 8));
    }
}

/**************

#if !defined(_WIN64) && defined(_WIN32) 
// This compressor can only create opaque, 4-colour blocks
void DXTCV11CompressBlock(DWORD block_32[16], DWORD block_dxtc[2])
{
    int i, k;
    float r, g, b;

    float uniques[16][3];                    // The list of unique colours
    int unique_pixels;                        // The number of unique pixels
    float unique_recip;                        // Reciprocal of the above for fast multiplication
    int index_map[16];                        // The map of source pixels to unique indices
    int pixel_count[16];                    // The number of occurrences of each unique

    float average_r, average_g, average_b;    // The centrepoint of the axis
    float v_r, v_g, v_b;                    // The axis

    float pos_on_axis[16];                    // The distance each unique falls along the compression axis
    float dist_from_axis[16];                // The distance each unique falls from the compression axis
    float left=0, right=0, centre=0;                // The extremities and centre (average of left/right) of uniques along the compression axis
    float axis_mapping_error=0;                // The total computed error in mapping pixels to the axis

    int retry;                                // Retry count for axis mapping
    int swap;                                // Indicator if the RGB values need swapping to generate an opaque result
    int blocktype=0;


    // -------------------------------------------------------------------------------------
    // Find the array of unique pixel values and sum them to find their average position
    // -------------------------------------------------------------------------------------
    {
        // Find the array of unique pixel values and sum them to find their average position
        float *up = (float *)uniques;
        int current_pixel, firstdiff;

        current_pixel = unique_pixels = 0;
        average_r = average_g = average_b = 0;
        firstdiff = -1;
        for(i=0; i<16; i++)
        {
            for(k=0; k<i; k++)
                if (!((block_32[k] ^ block_32[i]) & 0xf8fcf8))
                    break;

#ifdef AVERAGE_UNIQUE_PIXELS_ONLY
            // Only add non-identical pixels to the list
            if (k == i)
#endif
            {
                float tr, tg, tb;

                index_map[i] = current_pixel++;
                pixel_count[i] = 1;

                r = (float)((block_32[i] >> 16) & 0xff);
                g = (float)((block_32[i] >> 8)  & 0xff);
                b = (float)((block_32[i] >> 0)  & 0xff);

                tr = CS_RED(r, g, b);
                tg = CS_GREEN(r, g, b);
                tb = CS_BLUE(r, g, b);

                *up++ = tr;
                *up++ = tg;
                *up++ = tb;

                if (k == i)
                {
                    unique_pixels++;
                    if ((i != 0) && (firstdiff < 0)) firstdiff = i;
                }

                average_r += tr;
                average_g += tg;
                average_b += tb;
            }
#ifdef AVERAGE_UNIQUE_PIXELS_ONLY
            else
            {
                index_map[i] = index_map[k];
                pixel_count[k]++;
            }
#endif
        }

#ifndef AVERAGE_UNIQUE_PIXELS_ONLY
        unique_pixels = 16;
#endif

        // Compute average of the uniques
        unique_recip = 1.0f / (float) unique_pixels;
        average_r *= unique_recip;
        average_g *= unique_recip;
        average_b *= unique_recip;
    }


    // -------------------------------------------------------------------------------------
    // For each component, reflect points about the average so all lie on the same side
    // of the average, and compute the new average - this gives a second point that defines the axis
    // To compute the sign of the axis sum the positive differences of G for each of R and B (the
    // G axis is always positive in this implementation
    // -------------------------------------------------------------------------------------
    // An interesting situation occurs if the G axis contains no information, in which case the RB
    // axis is also compared. I am not entirely sure if this is the correct implementation - should
    // the priority axis be determined by magnitude?
    {

        float rg_pos, bg_pos, rb_pos;

        v_r = v_g = v_b = 0;
        rg_pos = bg_pos = rb_pos = 0;
        for(i=0; i<unique_pixels; i++)
        {
            r = uniques[i][0] - average_r;
            g = uniques[i][1] - average_g;
            b = uniques[i][2] - average_b;
            v_r += (float)fabs(r);
            v_g += (float)fabs(g);
            v_b += (float)fabs(b);

            if (r > 0) { rg_pos += g; rb_pos += b; }
            if (b > 0) bg_pos += g;
        }
        v_r *= unique_recip;
        v_g *= unique_recip;
        v_b *= unique_recip;
        if (rg_pos < 0) v_r = -v_r;
        if (bg_pos < 0) v_b = -v_b;
        if ((rg_pos == bg_pos) && (rg_pos == 0))
            if (rb_pos < 0) v_b = -v_b;

    }



#if 1    // Experimental code for infinite partition compression
    {
        // Decide which partition to use
        // We perform the partition based on a plane in the 3D colourspace. The plane is defined by
        // a point and a normal.

        // Finding this plane is, unsurprisingly for me, based off a hand-waving argument that seems to
        // solve all the simple cases and is usually bafflingly found to do OK on the difficult ones.

        // We shall say that the average of all the points is the point.

        // The normal is the old compression axis

        // This gives the direction of the normal vector but not the signs of each component. There
        // are 8 possibilities; for each we calculate the sum of the distances from the plane of all
        // considered points. There are two cases of interest, that for which the sum of distances is
        // maximum, and that for which the sum of distances is minimum.

        // These two possibilities define two different situations. If there are two groups of points,
        // widely spaced, that each compress reasonably we should pick the axis with the maximum
        // sum of distances. If all the points together are close to a single line, splitting by
        // this method will just generate two nearly identical lines. Instead, we should pick the axis
        // with the minimum sum of distances, which will create two close but not identical lines,
        // probably parallel in the colourspace.

        // Which of these cases is in use can be determined by the ratio of minimum to maximum. If the
        // minimum and maximum are 'close' then we are in a case where the maximum plane is the best used. If
        // the minimum is only a small fraction of the maximum then the minimum plane is best used. Intermediate
        // cases imply that neither is a particularly good representation of the block (this indicates
        // a possible compression metric for the new system).

        // It is not known what ratio will work for this, it will probably be determined by trial-and-error.
        // It is possible the ratio of choice will vary depending on the number of pixels in the
        // groups (for example, one highly different pixel compared to a reasonably uniform line should
        // likely be represented by maximum plane).

        // The minimum plane can be ignored initially. It is never erroneous to only use the
        // maximum plane, but using the minimum plane gives the ability to improve quality on those
        // blocks on which DXTC is already very good. (It seems likely that Andy's exhaustive search
        // algorithm is finding these cases already, and as such attempts to compare this algorithm with
        // exhaustive search will likely not do too well until the minimum case is also handled.)


        // Compute point - no need, we already have it (average_X)
        // Compute plane normal - no need, we already have it (v_X)


        // Test each of the possible 8 compression planes
        float maxsum, minsum, sum, dist, ratio;
        DWORD maxvector=0, minvector=0, vector=0;

        assert(unique_pixels == 16);    // vector is confusing to interpret if this isn't true...

        maxsum = 0;
        minsum = 1000000000;
        for(i=0; i<8; i++)
        {
            float n_r, n_g, n_b;
            n_r = (i&1) ? v_r : -v_r;
            n_g = (i&2) ? v_g : -v_g;
            n_b = (i&4) ? v_b : -v_b;

            sum = 0;
            vector = 0;
            for(k=0; k<unique_pixels; k++)
            {
                dist =  (uniques[k][0] - average_r) * n_r +
                        (uniques[k][1] - average_g) * n_g +
                        (uniques[k][2] - average_b) * n_b;

                sum += (float)fabs(dist);
                if (dist < 0) vector |= (1<<k);
            }

            if (sum > maxsum) { maxsum = sum; maxvector = vector; }
            if (sum < minsum) { minsum = sum; minvector = vector; }
        }

        // Decide whether to split based on the max or the min
        ratio = minsum / maxsum;

        if (ratio > 0.20) vector = maxvector; else vector = minvector;

        // A refinement might be to check if there are any points which have ended up just on one side
        // of the partition when they would be better on the other. I think this is unlikely to be a
        // major issue

    }
#endif

    // Note:warning C4702: unreachable code
    // Axis projection and remapping
    for(retry = 0; retry<2; retry++)
    {
        float v2_recip;

        // Normalise the axis for simplicity of future calculation
        v2_recip = (v_r*v_r + v_g*v_g + v_b*v_b);
        if (v2_recip > 0)
            v2_recip = 1.0f / (float)sqrt(v2_recip);
        else
            v2_recip = 1.0f;
        v_r *= v2_recip;
        v_g *= v2_recip;
        v_b *= v2_recip;


        // the line joining (and extended on either side of) average and axis
        // defines the axis onto which the points will be projected

        // Project all the points onto the axis, calculate the distance along
        // the axis from the centre of the axis (average)

        // From Foley & Van Dam: Closest point of approach of a line (P + v) to a point (R) is
        //                            P + ((R-P).v) / (v.v))v
        // The distance along v is therefore (R-P).v / (v.v)
        // (v.v) is 1 if v is a unit vector.
        //
        // Calculate the extremities at the same time - these need to be reasonably accurately
        // represented in all cases
        //
        // In this first calculation, also find the error of mapping the points to the axis - this
        // is our major indicator of whether or not the block has compressed well - if the points
        // map well onto the axis then most of the noise introduced is high-frequency noise

        left = 10000.0f;
        right = -10000.0f;
        axis_mapping_error = 0;
        for(i=0; i<unique_pixels; i++)
        {
            // Compute the distance along the axis of the point of closest approach
            pos_on_axis[i] = ((uniques[i][0]-average_r) * v_r) +
                             ((uniques[i][1]-average_g) * v_g) +
                             ((uniques[i][2]-average_b) * v_b);

            // Compute the actual point and thence the mapping error
            r = uniques[i][0] - (average_r + pos_on_axis[i]*v_r);
            g = uniques[i][1] - (average_g + pos_on_axis[i]*v_g);
            b = uniques[i][2] - (average_b + pos_on_axis[i]*v_b);
            dist_from_axis[i] = r*r + g*g + b*b;
            axis_mapping_error += dist_from_axis[i];

            // Work out the extremities
            if (pos_on_axis[i] < left)
                left = pos_on_axis[i];
            if (pos_on_axis[i] > right)
                right = pos_on_axis[i];
        }


//#define REFINE_AXIS
// Latest enhancements / bug fixes seem to have rendered this redundant
#ifdef REFINE_AXIS
        if (retry == 0)
        {
            // Check if our axis is actually a reasonable one, by clustering the two end
            // point groups (of the four that would be present in the final map). If there
            // is a wildly differing axis distance average between the two clusters then we
            // haven't got a good axis

            float division;
            float cluster_r[4], cluster_g[4], cluster_b[4];
            float cluster_error[4];
            int cluster_points[4];
            int cluster;
            float left_recip, right_recip;
            float error;
            int new_left, new_right;

            centre = (left+right)/2;
            if (metrics->r300_mode)
                division = right*5.0f/8.0f;
            else
                division = right*2.0f/3.0f;

            for(cluster=0; cluster<4; cluster++)
            {
                cluster_error[cluster] = 0;
                cluster_points[cluster] = 0;
                cluster_r[cluster] = 0;
                cluster_g[cluster] = 0;
                cluster_b[cluster] = 0;
            }
            for(i=0; i<unique_pixels; i++)
            {
                b = pos_on_axis[i];

                // Endpoints (indicated by block > average) are 0 and 1, while
                // interpolants are 2 and 3
                if (fabs(b) < division)
                    cluster = 2;
                else
                    cluster = 0;

                // Positive is in the latter half of the block
                if (b >= centre)
                    cluster++;

                // Sum up the clusters
                cluster_error[cluster] += dist_from_axis[i];
                cluster_points[cluster]++;
                cluster_r[cluster] += uniques[i][0];
                cluster_g[cluster] += uniques[i][1];
                cluster_b[cluster] += uniques[i][2];
            }

            if (!cluster_points[0] || !cluster_points[1])
            {
                // Oh yeah, this is a BAD axis, we failed to map
                // any points into at least one of the end clusters
                // Select the furthest in distance of the two remaining
                // clusters
                if (cluster_points[0])        new_left = 0;
                else if (cluster_points[2])    new_left = 2;
                else if (cluster_points[3])    new_left = 3;
                else                        new_left = 1;

                if (cluster_points[1])        new_right = 1;
                else if (cluster_points[3])    new_right = 3;
                else if (cluster_points[2])    new_right = 2;
                else                        new_right = 0;

            }
            else
            {
                new_left = 0;
                new_right = 1;
            }

            // If we completely failed to find a new axis we can't use this
            // method - in which case we should find something else to do of
            // course...
            if (new_left != new_right)
            {
                // Check the error bound between these two clusters
                left_recip = (1.0f / (float)cluster_points[new_left]);
                right_recip = (1.0f / (float)cluster_points[new_right]);
                cluster_error[new_left] *= left_recip;
                cluster_error[new_right] *= right_recip;

                // Now need a metric for if these errors are 'reasonable'
                // Say the square of the difference?
                error = (float) (fabs(cluster_error[new_left]) - fabs(cluster_error[new_right]));
                error *= error;
                if (error > 50000.0f)        // Trial-and-error value...
                {
                    // It's not good - find a new axis
                    cluster_r[new_right] *= right_recip;
                    cluster_g[new_right] *= right_recip;
                    cluster_b[new_right] *= right_recip;
                    cluster_r[new_left] *= left_recip;
                    cluster_g[new_left] *= left_recip;
                    cluster_b[new_left] *= left_recip;

                    average_r = cluster_r[new_left];
                    average_g = cluster_g[new_left];
                    average_b = cluster_b[new_left];
                    v_r = cluster_r[new_right] - cluster_r[new_left];
                    v_g = cluster_g[new_right] - cluster_g[new_left];
                    v_b = cluster_b[new_right] - cluster_b[new_left];
                }
                else    // It's not bad, so keep the first guess
                    break;
            }
        }
#else
        break;
#endif
    }



    // -------------------------------------------------------------------------------------
    // Now we have a good axis and the basic information about how the points are mapped
    // to it
    // Our initial guess is to represent the endpoints accurately, by moving the average
    // to the centre and recalculating the point positions along the line
    // -------------------------------------------------------------------------------------
    {
        centre = (left + right) / 2;
        average_r += centre*v_r;
        average_g += centre*v_g;
        average_b += centre*v_b;
        for(i=0; i<unique_pixels; i++)
            pos_on_axis[i] -= centre;
        right -= centre;
        left -= centre;

        // Accumulate our final resultant error
        axis_mapping_error *= unique_recip * (1/255.0f);

    }


//#define EXACT_ENDPOINT_DETERMINATOR
#ifdef EXACT_ENDPOINT_DETERMINATOR
    {
        // Exact endpoint determination

        // G == the extent of the position of the endpoint either side of the centre
        // P(n) == abs(pos_on_axis[n]), sorted into ascending order
        // i == the number of endpoints below the 2/3 threshold
        // E == the error in cluster mapping

        // For any value of i:
        // E = SUM(SQUARED(Pn - G)) + SUM(SQUARED(Pm - G/3)) where n goes from 0 to (i-1) and m goes from (i) to 15
        // E = SUM(SQUARED(Pn)) - 2*SUM(t1*Pn*G) + 16*t2*SQUARED(G) where n goes from 0 to 15
        // (t1 and t2 are the form factors to take accound of the relative numbers of the G and G/3 comparators)
        // E is a minimum when dE/dG == 0
        // dE/dG = 2*t2*G - 2*t1*16*SUM(Pn)
        // G == SUM(Pn) * (t1/t2) / 16 == a constant * the average value of Pn

        // 2G/3 then needs to be clamped between P(i) and P(i+1). This may be impossible if P(i) and P(i+1) have
        // the same value; in these cases there is no valid result.

        // We can then measure E in all 16 possible cases


        // Performance:
        // The evaluation of the minimum error point is easy as it is simply (constant * average). Calculating
        // the actual error at that point is a little more expensive but not prohibitively so. However, to check
        // if the configuration is valid will require a sorted array of points, the generation of which will
        // certainly be relatively expensive, but can be fairly easily and very quickly be implemented with a
        // binary sort using a temporary array.
        // We can probably also reuse this information when building the clusters later.

        float average_p, average_p_squared;
        float p[16];
        int ref[16];
        float e, G, min_e, min_G, split_point;
        int min_i;

        float min_error_point_table[17];
        float t1_table[17];
        float t2_table[17];
        for(i=0; i<=16; i++)
        {
            t1_table[i] = 1 - (2.0f/(3.0f*16.0f))*(float)i;
            t2_table[i] = 1 - (8.0f/(9.0f*16.0f))*(float)i;
            min_error_point_table[i] = t1_table[i] / t2_table[i];
        }


        min_e = 10000000.0f;

        average_p = average_p_squared = 0;
        for(i=0; i<16; i++)
        {
            ref[i] = i;        // Keep the references; we can then use this to help with the clustering
            p[i] = (float)fabs(pos_on_axis[i]);
            average_p += p[i];
            average_p_squared += p[i]*p[i];
        }
        average_p *= (1.0f/16.0f);
        average_p_squared *= (1.0f/16.0f);

        {
            float pt[16];
            int rt[16];
            int a, b, j;

            // Sort into pairs
            for(i=0; i<16; i+=2)
            {
                if (p[i] > p[i+1])
                {
                    pt[i] = p[i+1];
                    rt[i] = ref[i+1];
                    pt[i+1] = p[i];
                    rt[i+1] = ref[i];
                }
                else
                {
                    pt[i] = p[i];
                    rt[i] = ref[i];
                    pt[i+1] = p[i+1];
                    rt[i+1] = ref[i+1];
                }
            }

            // Sort/interleave pairs to quads
            for(i=0; i<16; i+=4)
            {
                a = 0;
                b = 2;
                for(j=0; j<4; j++)
                {
                    if ((b >= 4) || ((a < 2) && (pt[i+a] < pt[i+b])))
                    {
                        p[i+j] = pt[i+a];
                        ref[i+j] = rt[i+(a++)];
                    }
                    else
                    {
                        p[i+j] = pt[i+b];
                        ref[i+j] = rt[i+(b++)];
                    }
                }
            }

            // Sort/interleave quads to 8s
            for(i=0; i<16; i+=8)
            {
                a = 0;
                b = 4;
                for(j=0; j<8; j++)
                {
                    if ((b >= 8) || ((a < 4) && (p[i+a] < p[i+b])))
                    {
                        pt[i+j] = p[i+a];
                        rt[i+j] = ref[i+(a++)];
                    }
                    else
                    {
                        pt[i+j] = p[i+b];
                        rt[i+j] = ref[i+(b++)];
                    }
                }
            }

            // Sort/interleave 8s to 16s
            a = 0;
            b = 8;
            for(j=0; j<16; j++)
            {
                if ((b >= 16) || ((a < 8) && (pt[i+a] < pt[i+b])))
                {
                    p[j] = pt[a];
                    ref[j] = rt[(a++)];
                }
                else
                {
                    p[j] = pt[b];
                    ref[j] = rt[(b++)];
                }
            }

        }


        // Sorting done. After that, the algorithm is much easier
#define epsilon (0.01f)
        for(i=0; i<16; i++)
        {
            // If this cannot be clamped to a valid configuration, dump it straight away.
            // (Actually not sure if this is necessary. The maxima/minima rules may automatically
            // eliminate these cases)
            if ((i != 0) && (i != 16) && (p[i] == p[i-1]))
                continue;

            G = average_p * min_error_point_table[i];

            // Clamp to interval
            split_point = G*(2.0f/3.0f);
            if ((i != 0) && (p[i-1] > split_point))
                G = (3.0f/2.0f)*(p[i-1]+epsilon);
            else if ((i != 16) && (p[i] < split_point))
                G = (3.0f/2.0f)*(p[i]-epsilon);

            // Calculate the actual error
            e = average_p_squared - 2*t1_table[i]*average_p*G + t2_table[i]*G*G;
            if (e < -0.1f)
                __asm int 3;

            if (e < min_e)
            {
                min_G = G;
                min_e = e;
                min_i = i;
            }
        }

        left = -min_G;
        right = min_G;
    }
#endif


#define PROGRESSIVE_REFINEMENT
#ifdef PROGRESSIVE_REFINEMENT
    {
        // Attempt a (simple) progressive refinement step to reduce noise in the
        // output image by trying to find a better overall match for the endpoints
        // than the first-guess solution found so far (which is just to take the ends.

        // The method is to move the endpoints inwards until a local minima is found.
        // This provides quite a significant improvement in image quality.

        // In fact there are six degrees of freedom that could be tested here - inc/dec
        // left, inc/dec right, move both in, move both out). Move both out will never
        // need to be tested as we already start at this worst case (both as far out as
        // possible). Similarly, if every 'either in, both in' case is tested individually
        // then that also eliminates all 'move out' cases where a single step is all that
        // is needed

        // Checking the additional modes beyond the first does not provide much enhancement

        // Changing the stepsize to be additive instead of multiplicative makes visual image
        // quality very marginally worse, for about the same performance.

        float error4, maxerror, v[5];
        float stepsize = 0.95f;            // Compromise value, seems to produce good results in most cases while only being a small speed hit
        DWORD bit4;
        float division4;
        float oldleft, oldright;
        int mode, bestmode;
        int first;
#ifdef TRY_3_COLOR
        float division3, error3;
        DWORD bit3;
#endif

        oldleft = left;
        oldright = right;
        maxerror = 10000000.0f;
        first = 1;
        do
        {
            for(bestmode=-1,mode=0; mode<1; mode++)        // In my experience modes other than mode 0 hardly ever get a hit, and it costs nearly 30% performance to test the other two
            {
                if (!first)
                {
                    switch(mode)
                    {
                    case 0:
                        left = oldleft*stepsize;
                        right = oldright*stepsize;
                        break;
                    case 1:
                        right = oldright*stepsize;
                        break;
                    case 2:
                        left = oldleft*stepsize;
                        break;
                    }
                    if (left > right)
                        break;
                }

                centre = (left+right)/2;
                division4 = (right-centre)*2.0f/3.0f;
                v[0] = left;
                v[2] = centre-((right-centre)/3.0f);
                v[3] = centre+((right-centre)/3.0f);
                v[1] = right;
                error4 = 0;
#ifdef TRY_3_COLOR
                division3 = (right-centre)/3;
                error3 = 0;
                v[4] = centre;
#endif

                for(i=0; i<unique_pixels; i++)
                {
                    b = pos_on_axis[i];

#if TRY_3_COLOR
                    // Endpoints (indicated by block > average) are 0 and 1, while
                    // interpolants are 2 and 3
                    if (fabs(b) >= division4)
                        bit4 = 0;
                    else
                        bit4 = 2;

                    // Positive is in the latter half of the block
                    if (b >= centre)
                        bit4 += 1;

#ifdef TRY_3_COLOR
                    if (fabs(b) < division3)
                        bit3 = 5;
                    else if (b >= centre)
                        bit3 = 1;
                    else
                        bit3 = 0;
#endif
#elif 0
                    {
                        float d;
                        d = ((float)fabs(b) - division4);
                        bit4 = (*(DWORD *)&d>>30) & 2;
                        d = (b - centre);
                        bit4 |= (*(DWORD *)&d>>31)^1;
                    }
#else    // Almost exactly the same as the above in performance
                    __asm {
                        mov eax, [b]
                        mov ecx, eax
                        and eax, 0x7fffffff
                        shr ecx, 1
                        sub eax, [division4]
                        and eax, 0x80000000
                        and ecx, 0x40000000
                        add eax, ecx
                        shr eax, 30
                        xor eax, 1
                        mov [bit4], eax
                    }
#endif

                    r = (b - v[bit4]);
                    error4 += r*r;
#ifdef TRY_3_COLOR
                    r = (b - v[bit3]);
                    error3 += r*r;
#endif
                }

                if (error4 < maxerror)
                {
                    maxerror = error4;
                    bestmode = mode;
                    blocktype = 4;
                }
#ifdef TRY_3_COLOR
                if (error3 < maxerror)
                {
                    maxerror = error3;
                    bestmode = mode;
                    blocktype = 3;
                }
#endif
                if (bestmode != -1);
                    maxerror -= 5.0f;    // Errors smaller than a certain epsilon should be ignored - this prevents unnecessary and infinite loops

                if (first)
                    break;
            }

            if (!first)
                switch(bestmode)
                {
                default:
                    bestmode = -1;
                    break;
                case -1:
                    break;
                case 0:
                    oldleft *= stepsize;
                    oldright *= stepsize;
                    break;
                case 1:
                    oldright *= stepsize;
                    break;
                case 2:
                    oldleft *= stepsize;
                    break;
                }
            first = 0;

        } while(bestmode != -1);

        left = oldleft;
        right = oldright;
    }
#else
    blocktype = 4;
#endif

    // -------------------------------------------------------------------------------------
    // Calculate the high and low output colour values
    // Involved in this is a rounding procedure which is undoubtedly slightly twitchy. A
    // straight rounded average is not correct, as the decompressor 'unrounds' by replicating
    // the top bits to the bottom.
    // In order to take account of this process, we don't just apply a straight rounding correction,
    // but base our rounding on the input value (a straight rounding is actually pretty good in terms of
    // error measure, but creates a visual colour and/or brightness shift relative to the original image)
    // The method used here is to apply a centre-biased rounding dependent on the input value, which was
    // (mostly by experiment) found to give minimum MSE while preserving the visual characteristics of
    // the image.
    // rgb = (average_rgb + (left|right)*v_rgb);
    // -------------------------------------------------------------------------------------
    {
        DWORD c0, c1, t;
        int rd, gd, bd;

        r = (average_r + left*v_r);
        g = (average_g + left*v_g);
        b = (average_b + left*v_b);
        rd = (int) DCS_RED(r, g, b);
        gd = (int) DCS_GREEN(r, g, b);
        bd = (int) DCS_BLUE(r, g, b);
        ROUND_AND_CLAMP(rd, 5);
        ROUND_AND_CLAMP(gd, 6);
        ROUND_AND_CLAMP(bd, 5);
        c0 = ((rd&0xf8)<<8) + ((gd&0xfc)<<3) + ((bd&0xf8)>>3);

        r = (average_r + right*v_r);
        g = (average_g + right*v_g);
        b = (average_b + right*v_b);
        rd = (int) DCS_RED(r, g, b);
        gd = (int) DCS_GREEN(r, g, b);
        bd = (int) DCS_BLUE(r, g, b);
        ROUND_AND_CLAMP(rd, 5);
        ROUND_AND_CLAMP(gd, 6);
        ROUND_AND_CLAMP(bd, 5);
        c1 = (((rd&0xf8)<<8) + ((gd&0xfc)<<3) + ((bd&0xf8)>>3));

        // Force to be a 4-colour opaque block - in which case, c0 is greater than c1
        if (blocktype == 4)
        {
            if (c0 < c1)
            {
                t = c0;
                c0 = c1;
                c1 = t;
                swap = 1;
            }
            else if (c0 == c1)
            {
                // This block will always be encoded in 3-colour mode
                // Need to ensure that only one of the two points gets used,
                // avoiding accidentally setting some transparent pixels into the block
                for(i=0; i<unique_pixels; i++)
                    pos_on_axis[i] = left;
                swap = 0;
            }
            else
                swap = 0;
        }
        else
        {
            if (c0 < c1)
                swap = 0;
            else if (c0 == c1)
            {
                // This block will always be encoded in 3-colour mode
                // Need to ensure that only one of the two points gets used,
                // avoiding accidentally setting some transparent pixels into the block
                for(i=0; i<unique_pixels; i++)
                    pos_on_axis[i] = left;
                swap = 0;
            }
            else
            {
                t = c0;
                c0 = c1;
                c1 = t;
                swap = 1;
            }

        }

        block_dxtc[0] = c0 | (c1<<16);
    }

    // -------------------------------------------------------------------------------------
    // Final clustering, creating the 2-bit values that define the output
    // -------------------------------------------------------------------------------------
    {
        DWORD bit;
        float division;
        float cluster_x[4];
        float cluster_y[4];
        int cluster_count[4];

        if (blocktype == 4)
        {
            block_dxtc[1] = 0;
            division = right*2.0f/3.0f;
            centre = (left+right)/2;        // Actually, this code only works if centre is 0 or approximately so

            for(i=0; i<4; i++)
            {
                cluster_x[i] = cluster_y[i] = 0.0f;
                cluster_count[i] = 0;
            }


            for(i=0; i<16; i++)
            {
                b = pos_on_axis[index_map[i]];

                // Endpoints (indicated by block > average) are 0 and 1, while
                // interpolants are 2 and 3
                if (fabs(b) >= division)
                    bit = 0;
                else
                    bit = 2;

                // Positive is in the latter half of the block
                if (b >= centre)
                    bit += 1;

                // Set the output, taking swapping into account
                block_dxtc[1] |= (bit^swap)<<(2*i);

                // Average the X and Y locations for each cluster
                cluster_x[bit] += (float)(i&3);
                cluster_y[bit] += (float)(i>>2);
                cluster_count[bit]++;
            }

            for(i=0; i<4; i++)
            {
                float cr;
                if (cluster_count[i])
                {
                    cr = 1.0f / cluster_count[i];
                    cluster_x[i] *= cr;
                    cluster_y[i] *= cr;
                }
                else
                {
                    cluster_x[i] = cluster_y[i] = -1;
                }
            }


            // SEVERAL METHODS OF ANTIALIAS BLOCK DETECTION INCLUDED HERE
            // Mostly rejected.

            // patterns in axis position detection
            // (same algorithm as used in the SSE version)
            if ((block_dxtc[0]&0xffff) != (block_dxtc[0]>>16))
            {
                DWORD i1, k1;
                DWORD x=0, y=0;
                int xstep=0, ystep=0;

                // Find a corner to search from
                #define POS(x,y) (pos_on_axis[(x)+(y)*4])
                for(k1=0; k1<4; k1++)
                {
                    switch(k1)
                    {
                    case 0:
                        x = 0; y = 0; xstep = 1; ystep = 1;
                        break;
                    case 1:
                        x = 0; y = 3; xstep = 1; ystep = -1;
                        break;
                    case 2:
                        x = 3; y = 0; xstep = -1; ystep = 1;
                        break;
                    case 3:
                        x = 3; y = 3; xstep = -1; ystep = -1;
                        break;
                    }

                    for(i1=0; i1<4; i1++)
                    {
                        if ((POS(x            , y+ystep*i1)    < POS(x+  xstep, y+ystep*i1)) ||
                            (POS(x+  xstep    , y+ystep*i1)    < POS(x+2*xstep, y+ystep*i1)) ||
                            (POS(x+2*xstep    , y+ystep*i1)    < POS(x+3*xstep, y+ystep*i1))
                            )
                            break;
                        if ((POS(x+xstep*i1, y)                < POS(x+xstep*i1, y+  ystep)) ||
                            (POS(x+xstep*i1, y+ystep)        < POS(x+xstep*i1, y+2*ystep)) ||
                            (POS(x+xstep*i1, y+2*ystep)        < POS(x+xstep*i1, y+3*ystep))
                            )
                            break;
                    }
                    if (i1 == 4)
                        break;
                }
            }


        }
        else    // if (blocktype == 4)
        {
            block_dxtc[1] = 0;
            division = right/3;
            centre = (left+right)/2;        // Actually, this code only works if centre is 0 or approximately so

            for(i=0; i<16; i++)
            {
                b = pos_on_axis[index_map[i]];

                // Endpoints (indicated by block > average) are 0 and 1, while
                // interpolants are 2
                if (fabs(b) < division)
                    bit = 2;
                else if (b >= centre)
                    bit = 1^swap;
                else
                    bit = swap;

                // Set the output, taking swapping into account
                block_dxtc[1] |= bit<<(2*i);
            }
        }
    }
    // done
}

// This compressor can only create opaque, 4-colour blocks
void DXTCV11CompressBlockMinimal(DWORD block_32[16], DWORD block_dxtc[2])
{
    int i, k;
    float r, g, b;

    float uniques[16][3];                  // The list of unique colours
    int unique_pixels;                     // The number of unique pixels
    float unique_recip;                    // Reciprocal of the above for fast multiplication
    int index_map[16];                     // The map of source pixels to unique indices
    int pixel_count[16];                   // The number of occurrences of each unique

    float average_r, average_g, average_b; // The centrepoint of the axis
    float v_r, v_g, v_b;                   // The axis

    float pos_on_axis[16];                 // The distance each unique falls along the compression axis
    float dist_from_axis[16];              // The distance each unique falls from the compression axis
    float left = 0, right = 0, centre = 0; // The extremities and centre (average of left/right) of uniques along the compression axis
    float axis_mapping_error = 0;          // The total computed error in mapping pixels to the axis

    int swap;                              // Indicator if the RGB values need swapping to generate an opaque result


    // -------------------------------------------------------------------------------------
    // (3) Find the array of unique pixel values and sum them to find their average position
    // -------------------------------------------------------------------------------------
    {
        // Find the array of unique pixel values and sum them to find their average position
        float *up = (float *)uniques;
        int current_pixel, firstdiff;

        current_pixel = unique_pixels = 0;
        average_r = average_g = average_b = 0;
        firstdiff = -1;
        for (i = 0; i<16; i++)
        {
            for (k = 0; k<i; k++)
                if (!((block_32[k] ^ block_32[i]) & 0xf8fcf8))
                    break;

#ifdef AVERAGE_UNIQUE_PIXELS_ONLY
            // Only add non-identical pixels to the list
            if (k == i)
#endif
            {
                float tr, tg, tb;

                index_map[i] = current_pixel++;
                pixel_count[i] = 1;

                r = (float)((block_32[i] >> 16) & 0xff);
                g = (float)((block_32[i] >> 8) & 0xff);
                b = (float)((block_32[i] >> 0) & 0xff);

                tr = CS_RED(r, g, b);
                tg = CS_GREEN(r, g, b);
                tb = CS_BLUE(r, g, b);

                *up++ = tr;
                *up++ = tg;
                *up++ = tb;

                if (k == i)
                {
                    unique_pixels++;
                    if ((i != 0) && (firstdiff < 0)) firstdiff = i;
                }

                average_r += tr;
                average_g += tg;
                average_b += tb;
            }
#ifdef AVERAGE_UNIQUE_PIXELS_ONLY
            else
            {
                index_map[i] = index_map[k];
                pixel_count[k]++;
            }
#endif
        }

#ifndef AVERAGE_UNIQUE_PIXELS_ONLY
        unique_pixels = 16;
#endif

        // Compute average of the uniques
        unique_recip = 1.0f / (float)unique_pixels;
        average_r *= unique_recip;
        average_g *= unique_recip;
        average_b *= unique_recip;
    }

    // -------------------------------------------------------------------------------------
    // (4) For each component, reflect points about the average so all lie on the same side
    // of the average, and compute the new average - this gives a second point that defines the axis
    // To compute the sign of the axis sum the positive differences of G for each of R and B (the
    // G axis is always positive in this implementation
    // -------------------------------------------------------------------------------------
    // An interesting situation occurs if the G axis contains no information, in which case the RB
    // axis is also compared. I am not entirely sure if this is the correct implementation - should
    // the priority axis be determined by magnitude?
    {

        float rg_pos, bg_pos, rb_pos;

        v_r = v_g = v_b = 0;
        rg_pos = bg_pos = rb_pos = 0;
        for (i = 0; i<unique_pixels; i++)
        {
            r = uniques[i][0] - average_r;
            g = uniques[i][1] - average_g;
            b = uniques[i][2] - average_b;
            v_r += (float)fabs(r);
            v_g += (float)fabs(g);
            v_b += (float)fabs(b);

            if (r > 0) { rg_pos += g; rb_pos += b; }
            if (b > 0) bg_pos += g;
        }
        v_r *= unique_recip;
        v_g *= unique_recip;
        v_b *= unique_recip;
        if (rg_pos < 0) v_r = -v_r;
        if (bg_pos < 0) v_b = -v_b;
        if ((rg_pos == bg_pos) && (rg_pos == 0))
            if (rb_pos < 0) v_b = -v_b;

    }

    // -------------------------------------------------------------------------------------
    // (5) Axis projection and remapping
    // -------------------------------------------------------------------------------------
    {
        float v2_recip;

        // Normalise the axis for simplicity of future calculation
        v2_recip = (v_r*v_r + v_g*v_g + v_b*v_b);
        if (v2_recip > 0)
            v2_recip = 1.0f / (float)sqrt(v2_recip);
        else
            v2_recip = 1.0f;
        v_r *= v2_recip;
        v_g *= v2_recip;
        v_b *= v2_recip;
    }

    // -------------------------------------------------------------------------------------
    // (6) Map the axis
    // -------------------------------------------------------------------------------------
    // the line joining (and extended on either side of) average and axis
    // defines the axis onto which the points will be projected
    // Project all the points onto the axis, calculate the distance along
    // the axis from the centre of the axis (average)
    // From Foley & Van Dam: Closest point of approach of a line (P + v) to a point (R) is
    //                            P + ((R-P).v) / (v.v))v
    // The distance along v is therefore (R-P).v / (v.v)
    // (v.v) is 1 if v is a unit vector.
    //
    // Calculate the extremities at the same time - these need to be reasonably accurately
    // represented in all cases
    //
    // In this first calculation, also find the error of mapping the points to the axis - this
    // is our major indicator of whether or not the block has compressed well - if the points
    // map well onto the axis then most of the noise introduced is high-frequency noise
    {
        left = 10000.0f;
        right = -10000.0f;
        axis_mapping_error = 0;
        for (i = 0; i < unique_pixels; i++)
        {
            // Compute the distance along the axis of the point of closest approach
            pos_on_axis[i] = ((uniques[i][0] - average_r) * v_r) +
                ((uniques[i][1] - average_g) * v_g) +
                ((uniques[i][2] - average_b) * v_b);

            // Compute the actual point and thence the mapping error
            r = uniques[i][0] - (average_r + pos_on_axis[i] * v_r);
            g = uniques[i][1] - (average_g + pos_on_axis[i] * v_g);
            b = uniques[i][2] - (average_b + pos_on_axis[i] * v_b);
            dist_from_axis[i] = r*r + g*g + b*b;
            axis_mapping_error += dist_from_axis[i];

            // Work out the extremities
            if (pos_on_axis[i] < left)
                left = pos_on_axis[i];
            if (pos_on_axis[i] > right)
                right = pos_on_axis[i];
        }
    }
    
    // -------------------------------------------------------------------------------------
    // (7) Now we have a good axis and the basic information about how the points are mapped
    // to it
    // Our initial guess is to represent the endpoints accurately, by moving the average
    // to the centre and recalculating the point positions along the line
    // -------------------------------------------------------------------------------------
    {
        centre = (left + right) / 2;
        average_r += centre*v_r;
        average_g += centre*v_g;
        average_b += centre*v_b;
        for (i = 0; i<unique_pixels; i++)
            pos_on_axis[i] -= centre;
        right -= centre;
        left -= centre;

        // Accumulate our final resultant error
        axis_mapping_error *= unique_recip * (1 / 255.0f);

    }

    // -------------------------------------------------------------------------------------
    // (8) Calculate the high and low output colour values
    // Involved in this is a rounding procedure which is undoubtedly slightly twitchy. A
    // straight rounded average is not correct, as the decompressor 'unrounds' by replicating
    // the top bits to the bottom.
    // In order to take account of this process, we don't just apply a straight rounding correction,
    // but base our rounding on the input value (a straight rounding is actually pretty good in terms of
    // error measure, but creates a visual colour and/or brightness shift relative to the original image)
    // The method used here is to apply a centre-biased rounding dependent on the input value, which was
    // (mostly by experiment) found to give minimum MSE while preserving the visual characteristics of
    // the image.
    // rgb = (average_rgb + (left|right)*v_rgb);
    // -------------------------------------------------------------------------------------
    {
        DWORD c0, c1, t;
        int rd, gd, bd;

        r = (average_r + left*v_r);
        g = (average_g + left*v_g);
        b = (average_b + left*v_b);
        rd = (int)DCS_RED(r, g, b);
        gd = (int)DCS_GREEN(r, g, b);
        bd = (int)DCS_BLUE(r, g, b);
        ROUND_AND_CLAMP(rd, 5);
        ROUND_AND_CLAMP(gd, 6);
        ROUND_AND_CLAMP(bd, 5);
        c0 = ((rd & 0xf8) << 8) + ((gd & 0xfc) << 3) + ((bd & 0xf8) >> 3);

        r = (average_r + right*v_r);
        g = (average_g + right*v_g);
        b = (average_b + right*v_b);
        rd = (int)DCS_RED(r, g, b);
        gd = (int)DCS_GREEN(r, g, b);
        bd = (int)DCS_BLUE(r, g, b);
        ROUND_AND_CLAMP(rd, 5);
        ROUND_AND_CLAMP(gd, 6);
        ROUND_AND_CLAMP(bd, 5);
        c1 = (((rd & 0xf8) << 8) + ((gd & 0xfc) << 3) + ((bd & 0xf8) >> 3));

        // Force to be a 4-colour opaque block - in which case, c0 is greater than c1
        // blocktype == 4
        {
            if (c0 < c1)
            {
                t = c0;
                c0 = c1;
                c1 = t;
                swap = 1;
            }
            else if (c0 == c1)
            {
                // This block will always be encoded in 3-colour mode
                // Need to ensure that only one of the two points gets used,
                // avoiding accidentally setting some transparent pixels into the block
                for (i = 0; i<unique_pixels; i++)
                    pos_on_axis[i] = left;
                swap = 0;
            }
            else
                swap = 0;
        }


        block_dxtc[0] = c0 | (c1 << 16);
    }

    // -------------------------------------------------------------------------------------
    // (9) Final clustering, creating the 2-bit values that define the output
    // -------------------------------------------------------------------------------------
    {
        DWORD bit;
        float division;
        float cluster_x[4];
        float cluster_y[4];
        int cluster_count[4];

        // (blocktype == 4)
        {
            block_dxtc[1] = 0;
            division = right*2.0f / 3.0f;
            centre = (left + right) / 2;        // Actually, this code only works if centre is 0 or approximately so

            for (i = 0; i<4; i++)
            {
                cluster_x[i] = cluster_y[i] = 0.0f;
                cluster_count[i] = 0;
            }


            for (i = 0; i<16; i++)
            {
                b = pos_on_axis[index_map[i]];

                // Endpoints (indicated by block > average) are 0 and 1, while
                // interpolants are 2 and 3
                if (fabs(b) >= division)
                    bit = 0;
                else
                    bit = 2;

                // Positive is in the latter half of the block
                if (b >= centre)
                    bit += 1;

                // Set the output, taking swapping into account
                block_dxtc[1] |= (bit^swap) << (2 * i);

                // Average the X and Y locations for each cluster
                cluster_x[bit] += (float)(i & 3);
                cluster_y[bit] += (float)(i >> 2);
                cluster_count[bit]++;
            }

            for (i = 0; i<4; i++)
            {
                float cr;
                if (cluster_count[i])
                {
                    cr = 1.0f / cluster_count[i];
                    cluster_x[i] *= cr;
                    cluster_y[i] *= cr;
                }
                else
                {
                    cluster_x[i] = cluster_y[i] = -1;
                }
            }

            // patterns in axis position detection
            // (same algorithm as used in the SSE version)
            if ((block_dxtc[0] & 0xffff) != (block_dxtc[0] >> 16))
            {
                DWORD i1, k1;
                DWORD x = 0, y = 0;
                int xstep = 0, ystep = 0;

                // Find a corner to search from
                #define POS(x,y) (pos_on_axis[(x)+(y)*4])
                for (k1 = 0; k1<4; k1++)
                {
                    switch (k1)
                    {
                    case 0:
                        x = 0; y = 0; xstep = 1; ystep = 1;
                        break;
                    case 1:
                        x = 0; y = 3; xstep = 1; ystep = -1;
                        break;
                    case 2:
                        x = 3; y = 0; xstep = -1; ystep = 1;
                        break;
                    case 3:
                        x = 3; y = 3; xstep = -1; ystep = -1;
                        break;
                    }

                    for (i1 = 0; i1<4; i1++)
                    {
                        if ((POS(x, y + ystep*i1)                < POS(x + xstep, y + ystep*i1)) ||
                            (POS(x + xstep, y + ystep*i1)        < POS(x + 2 * xstep, y + ystep*i1)) ||
                            (POS(x + 2 * xstep, y + ystep*i1)    < POS(x + 3 * xstep, y + ystep*i1))
                            )
                            break;
                        if ((POS(x + xstep*i1, y)                < POS(x + xstep*i1, y + ystep)) ||
                            (POS(x + xstep*i1, y + ystep)        < POS(x + xstep*i1, y + 2 * ystep)) ||
                            (POS(x + xstep*i1, y + 2 * ystep)    < POS(x + xstep*i1, y + 3 * ystep))
                            )
                            break;
                    }
                    if (i1 == 4)
                        break;
                }
            }
        }

    }
    // done
}

#endif

***************/

#ifndef _WIN64

void DXTCV11CompressAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2])
{
    int i;

    float pos[16];                    // The list of colours
    int blocktype;
    float b;
    DWORD n, bit;
    float step, rstep, offset;
    int count_0, count_255;
    float average_inc, average_ex;
    float max_ex, min_ex;

    max_ex = 0;
    min_ex = 255;
    average_inc = average_ex = 0;
    count_0 = count_255 = 0;
    for(i=0; i<16; i++)
    {
        int ex = 0;
        if (block_8[i] == 0) { count_0++; ex = 1; }
        else if (block_8[i] == 255) { count_255++; ex = 1; }

        pos[i] = (float)block_8[i];

        average_inc += pos[i];
        if (!ex)
        {
            average_ex += pos[i];
            if (pos[i] > max_ex) max_ex = pos[i];
            if (pos[i] < min_ex) min_ex = pos[i];
        }
    }


    // Make assumptions
    if (!count_0 && !count_255)
        blocktype = 8;
    else
    {
        // There are 0 or 255 blocks and we need to represent them
        blocktype = 8;        // Actually should probably try both here


        if (count_0) min_ex = 0;
        if (count_255) max_ex = 255;
    }


    // Start out assuming our endpoints are the min and max values we've determined
    // If the minimum is 0, it must stay 0, otherwise we shall move inwards cf. the colour compressor


    // Progressive refinement makes very little difference averaged across a whole image, but in certain
    // tricky areas can be noticeably better.

//#define ALPHA_PROGRESSIVE_REFINEMENT
#ifdef ALPHA_PROGRESSIVE_REFINEMENT
    {
        // Attempt a (simple) progressive refinement step to reduce noise in the
        // output image by trying to find a better overall match for the endpoints
        // than the first-guess solution found so far (which is just to take the ends.

        float error, maxerror;
        float oldmin, oldmax;
        int mode, bestmode;
        int first;
        float r, v;

        maxerror = 10000000.0f;
        oldmin = min_ex;
        oldmax = max_ex;
        first = 1;
        do
        {

            for(bestmode=-1,mode=0; mode<6; mode++)        // Other modes seem more important for alpha block compression, 4-6 seem broken atm
            {
                if (!first)
                {
                    switch(mode)
                    {
                    case 0:
                        min_ex = oldmin+1.0f;
                        max_ex = oldmax-1.0f;
                        break;
                    case 1:
                        max_ex = oldmax-1.0f;
                        break;
                    case 2:
                        min_ex = oldmin+1.0f;
                        break;
                    case 3:
                        min_ex = oldmin-1.0f;
                        max_ex = oldmax+1.0f;
                        break;
                    case 4:
                        max_ex = oldmax+1.0f;
                        break;
                    case 5:
                        min_ex = oldmin-1.0f;
                        break;
                    }
                    if ((min_ex + 1.0f) > max_ex)
                        break;
                    if ((min_ex < 0.0f) || (max_ex > 255.0f))
                        continue;
                }

                error = 0;
                step = (max_ex - min_ex) / (float) (blocktype-1);
                rstep = 1.0f / step;
                offset = min_ex - (step*0.5f);

                for(i=0; i<16; i++)
                {
                    b = pos[i];
                    if ((blocktype == 6) && ((b == 0) || (b == 255)))
                        continue;

                    // Work out which value in the block this selects
                    n = (int)((b - offset) * rstep);

                    if (n < 0) n = 0;
                    if (n > 7) n = 7;

                    // Compute the interpolated value
                    v = ((float)n * step) + offset;

                    // And accumulate the error
                    r = (b - v);
                    error += r*r;
                }

                if (error < maxerror)
                {
                    maxerror = error;
                    bestmode = mode;
                }

                if (first)
                    break;
            }
            if (!first)
                switch(bestmode)
                {
                default:
                    bestmode = -1;
                    break;
                case -1:
                    break;
                case 0:
                    oldmin += 1.0f;
                    oldmax -= 1.0f;
                    break;
                case 1:
                    oldmax -= 1.0f;
                    break;
                case 2:
                    oldmin += 1.0f;
                    break;
                case 3:
                    oldmin -= 1.0f;
                    oldmax += 1.0f;
                    break;
                case 4:
                    oldmax += 1.0f;
                    break;
                case 5:
                    oldmin -= 1.0f;
                    break;
                }
            first = 0;

        } while(bestmode != -1);

        min_ex = oldmin;
        max_ex = oldmax;

        metrics->sum_cluster_errors += maxerror;
        if (maxerror > 2000.0f)
            metrics->high_cluster_error_blocks++;
    }

#endif    // ALPHA_PROGRESSIVE_REFINEMENT


    // Generating the rounded values is slightly arcane.


    if (blocktype == 6)
        block_dxtc[0] = ((int)(min_ex + 0.5f)) | (((int)(max_ex + 0.5f))<<8);
    else
        block_dxtc[0] = ((int)(max_ex + 0.5f)) | (((int)(min_ex + 0.5f))<<8);


    step = (max_ex - min_ex) / (float) (blocktype-1);
    rstep = 1.0f / step;
    offset = min_ex - (step*0.5f);

    block_dxtc[1] = 0;
    for(i=0; i<16; i++)
    {
        b = pos[i];
        if (blocktype == 6)
        {
            if ((b == 0.0f) || (b == 255.0f))
            {
                if (b == 0) bit = 6;
                else bit = 7;
            }
            else
            {
                // Work out which value in the block this selects
                n = (int)((b - offset) * rstep);

                if (n <= 0) bit = 0;
                else if (n >= 5) bit = 1;
                else bit = n+1;
            }
        }
        else
        {
            // Blocktype 8
            // Work out which value in the block this selects
            n = (int)((b - offset) * rstep);

            if (n <= 0) bit = 1;
            else if (n >= 7) bit = 0;
            else bit = 8 - n;
        }

        if (i == 5)
        {
            block_dxtc[1] |= (bit>>1);
            block_dxtc[0] |= (bit&1)<<31;
        }
        else if (i < 5)
            block_dxtc[0] |= bit << (3*i+16);
        else
            block_dxtc[1] |= bit << (3*(i-6)+2);
    }

    // done
}

#endif // !_WIN64

