/*
 ==============================================================================
 
 This file is part of HOSIRR
 Copyright (c) 2020 - Leo McCormack
 
 HOSIRR is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 HOSIRR is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with HOSIRR.  If not, see <http://www.gnu.org/licenses/>.
 
 ==============================================================================
 */

/**
 * @file hosirr_internal.c
 * @brief A C-port of the Higher-order Spatial Impulse Response Rendering
 *        (HO-SIRR) Matlab toolbox: https://github.com/leomccormack/HO-SIRR
 *
 * HO-SIRR is a rendering method, which can synthesise output loudspeaker array
 * room impulse responses (RIRs) using input spherical harmonic (Ambisonic/
 * B-Format) RIRs of arbitrary order. The method makes assumptions regarding
 * the composition of the sound-field and extracts spatial parameters over time,
 * which allows it to map the input to the output in an adaptive and informed
 * manner.
 *
 * The idea is that you then convolve a monophonic source with this loudspeaker
 * array RIR, and it will be reproduced and exhibit the spatial characteristics
 * of the captured space more faithfully (when compared to linear methods such
 * as Ambisonics).
 *
 * Dependencies: Spatial_Audio_Framework
 * (https://github.com/leomccormack/Spatial_Audio_Framework)
 *
 * @see [1] McCormack, L., Politis, A., Scheuregger, O., and Pulkki, V. (2019).
 *          "Higher-order processing of spatial impulse responses". In
 *          Proceedings of the 23rd International Congress on Acoustics, 9--13
 *          September 2019 in Aachen, Germany.
 *
 * @author Leo McCormack
 * @date 04.01.2020
 */

#include "hosirr_internal.h"

const int hosirrlib_defaultNumLoudspeakers = 24;

/* First 24 loudspeakers are for a 24 point t-design, the rest are for a 64 point sphere covering */
const float hosirrlib_defaultLoudspeakerDirections[HOSIRR_MAX_NUM_OUTPUTS][2] =
{ { 26.0011675216559f,    15.4641512961471f},
  { -26.0011675216559f,    -15.4641512961471f},
  { 17.1086452559122f,    -24.9937030546433f},
  { -17.1086452559122f,    24.9937030546433f},
  { 153.998832478344f,    -15.4641512961471f},
  { -153.998832478344f,    15.4641512961471f},
  { 162.891354744088f,    24.9937030546433f},
  { -162.891354744088f,    -24.9937030546433f},
  { 72.8913547440879f,    24.9937030546433f},
  { 107.108645255912f,    -24.9937030546433f},
  { 116.001167521656f,    15.4641512961471f},
  { 63.9988324783441f,    -15.4641512961471f},
  { -107.108645255912f,    24.9937030546433f},
  { -72.8913547440879f,    -24.9937030546433f},
  { -63.9988324783441f,    15.4641512961471f},
  { -116.001167521656f,    -15.4641512961471f},
  { 32.2544599366034f,    60.0253819510733f},
  { -147.745540063397f,    60.0253819510733f},
  { -57.7455400633966f,    60.0253819510733f},
  { 122.254459936603f,    60.0253819510733f},
  { -32.2544599366034f,    -60.0253819510733f},
  { 147.745540063397f,    -60.0253819510733f},
  { 57.7455400633966f,    -60.0253819510733f},
  { -122.254459936603f,    -60.0253819510733f},
  { 30.6985057053144f,    -5.05761304917880f},
  { 143.812406577837f,    15.9133298019635f},
  { 130.794805472464f,    -7.91483898193719f},
  { -119.215328432870f,    -75.3038430140441f},
  { 123.816179527771f,    61.1747037861180f},
  { 4.48104561993866f,    -71.1384398434430f},
  { -73.9344738836814f,    20.6946626023302f},
  { 63.5295603241057f,    -53.3423707266797f},
  { -135.252417118582f,    24.9408528220447f},
  { -107.870764089280f,    24.8291260519942f},
  { -166.449969063455f,    40.0245397366588f},
  { 57.9203035097749f,    30.1920110144187f},
  { 115.892173221112f,    -53.6236930040889f},
  { 48.1273088753989f,    79.5838377436714f},
  { 158.216565547426f,    -9.81533998838613f},
  { 47.1343730164372f,    -26.0266078438176f},
  { -54.8996700138452f,    -59.0031937425722f},
  { 112.311187001544f,    -27.9597674445890f},
  { -175.428217713155f,    -6.72022197908943f},
  { 169.452267909941f,    17.1830042759734f},
  { 165.561884481003f,    -59.0604895220853f},
  { -130.536974464656f,    50.8563068536070f},
  { 55.0686925634088f,    3.78684995535815f},
  { 81.0219618094497f,    -35.8614283972382f},
  { -14.4425471418627f,    60.3267262493244f},
  { -43.2560217011966f,    -30.2240966509461f},
  { -16.0262024876043f,    -43.7258470932039f},
  { -92.3951740427966f,    43.5820346866261f},
  { -173.205141468048f,    69.2190312297548f},
  { 99.1446168694376f,    -9.04700358511570f},
  { 29.9754329678593f,    21.9081872124173f},
  { -144.184829144672f,    -3.33461436766139f},
  { -20.4345397633408f,    11.4052978698742f},
  { -155.134052609622f,    -25.1104483294035f},
  { -92.2003683924521f,    3.70720882183497f},
  { -39.6687329458825f,    -6.36326927272292f},
  { -120.676370810454f,    -1.14614477337970f},
  { -97.3226110809217f,    -19.7750653411452f},
  { -51.8326269365099f,    16.5172273180314f},
  { 2.49465823999960f,    32.1039711767703f}};


void loadLoudspeakerArrayPreset
(
    LOUDSPEAKER_ARRAY_PRESETS preset,
    _Atomic_FLOAT32 dirs_deg[MAX_NUM_LOUDSPEAKERS_IN_PRESET][2],
    _Atomic_INT32* newNCH
)
{ 
    int ch, i, nCH;
    
    switch(preset){
        default:
        case LOUDSPEAKER_ARRAY_PRESET_DEFAULT:
        case LOUDSPEAKER_ARRAY_PRESET_5PX:
            nCH = 5;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __5pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_7PX:
            nCH = 7;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __7pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_8PX:
            nCH = 8;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __8pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_9PX:
            nCH = 9;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __9pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_10PX:
            nCH = 10;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __10pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_11PX:
            nCH = 11;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __11pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_11PX_7_4:
            nCH = 11;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __11pX_7_4_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_13PX:
            nCH = 13;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __13pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_22PX:
            nCH = 22;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __22pX_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_STEREO:
            nCH = 2;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] =  __protools_stereo_dirs_deg[__protools_mapping_discrete_to_stereo[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_LCR:
            nCH = 3;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_LCR_dirs_deg[__protools_mapping_discrete_to_LCR[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_QUAD:
            nCH = 4;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_Quad_dirs_deg[__protools_mapping_discrete_to_Quad[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_5_0:
            nCH = 5;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_5p0_dirs_deg[__protools_mapping_discrete_to_5p0[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_5_0_2:
            nCH = 7;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_5p0p2_dirs_deg[__protools_mapping_discrete_to_5p0p2[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_5_0_4:
            nCH = 9;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_5p0p4_dirs_deg[__protools_mapping_discrete_to_5p0p4[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0:
            nCH = 7;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_7p0_dirs_deg[__protools_mapping_discrete_to_7p0[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0_2:
            nCH = 9;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_7p0p2_dirs_deg[__protools_mapping_discrete_to_7p0p2[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0_4:
            nCH = 11;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_7p0p4_dirs_deg[__protools_mapping_discrete_to_7p0p4[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0_6:
            nCH = 13;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_7p0p6_dirs_deg[__protools_mapping_discrete_to_7p0p6[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_9_0_4:
            nCH = 13;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_9p0p4_dirs_deg[__protools_mapping_discrete_to_9p0p4[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_9_0_6:
            nCH = 15;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __protools_9p0p6_dirs_deg[__protools_mapping_discrete_to_9p0p6[ch]][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_AALTO_MCC:
            nCH = 45;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Aalto_MCC_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_AALTO_MCC_SUBSET:
            nCH = 37;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Aalto_MCCsubset_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_AALTO_APAJA:
            nCH = 29;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Aalto_Apaja_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_AALTO_LR:
            nCH = 13;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Aalto_LR_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_DTU_AVIL:
            nCH = 64;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __DTU_AVIL_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_ZYLIA_LAB:
            nCH = 22;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Zylia_Lab_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_4:
            nCH = 4;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Tdesign_degree_2_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_12:
            nCH = 12;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Tdesign_degree_4_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_24:
            nCH = 24;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Tdesign_degree_6_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_36:
            nCH = 36;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Tdesign_degree_8_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_48:
            nCH = 48;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Tdesign_degree_9_dirs_deg[ch][i];
            break;
        case LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_60:
            nCH = 60;
            for(ch=0; ch<nCH; ch++)
                for(i=0; i<2; i++)
                    dirs_deg[ch][i] = __Tdesign_degree_10_dirs_deg[ch][i];
            break;
    }
    
    /* Fill remaining slots with default coords */
    for(; ch<MAX_NUM_LOUDSPEAKERS_IN_PRESET; ch++)
        for(i=0; i<2; i++)
            dirs_deg[ch][i] = __default_LScoords64_rad[ch][i]* (180.0f/SAF_PI);
    
    /* specify new number of channels (for dynamically changing the number of TFT channels) */
    (*newNCH) = nCH;
}
