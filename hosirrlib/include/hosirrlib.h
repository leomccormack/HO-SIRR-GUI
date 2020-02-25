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

/*
 * Filename: hosirrlib.h (include header)
 * --------------------------------------
 * hosirrlib is a C-port of the Higher-order Spatial Impulse Response Rendering
 * (HO-SIRR) Matlab toolbox: https://github.com/leomccormack/HO-SIRR
 * HO-SIRR is a rendering method, which can synthesise output loudspeaker array
 * room impulse responses (RIRs) using input spherical harmonic (Ambisonic/
 * B-Format) RIRs of arbitrary order. The method makes assumptions regarding
 * the composition of the sound-field and extracts spatial parameters over time,
 * which allows it to map the input to the output in an adaptive and informed
 * manner.
 * The idea is that you then convolve a monophonic source with this loudspeaker
 * array RIR, and it will be reproduced and exhibit the spatial characteristics
 * of the captured space more faithfully (when compared to linear methods such
 * as Ambisonics).
 *
 * Dependencies:
 *     Spatial_Audio_Framework
 * Author, date created:
 *     Leo McCormack, 04.01.2020
 *
 * [1] McCormack, L., Politis, A., Scheuregger, O., and Pulkki, V. (2019).
 *     "Higher-order processing of spatial impulse responses". In Proceedings of
 *     the 23rd International Congress on Acoustics, 9--13 September 2019 in
 *     Aachen, Germany.
 */

#ifndef __HOSIRRLIB_H_INCLUDED__
#define __HOSIRRLIB_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ========================================================================== */
/*                             Presets + Constants                            */
/* ========================================================================== */

/*
 * Enum: ANALYSIS_ORDERS
 * ---------------------
 * Available analysis/rendering order options.
 *
 * Options:
 *     ANALYSIS_ORDER_FIRST   - 1st-order analysis/rendering (4 channel input)
 *     ANALYSIS_ORDER_SECOND  - 2nd-order analysis/rendering (9 channel input)
 *     ANALYSIS_ORDER_THIRD   - 3rd-order analysis/rendering (16 channel input)
 *     ANALYSIS_ORDER_FOURTH  - 4th-order analysis/rendering (25 channel input)
 *     ANALYSIS_ORDER_FIFTH   - 5th-order analysis/rendering (36 channel input)
 *     ANALYSIS_ORDER_SIXTH   - 6th-order analysis/rendering (49 channel input)
 *     ANALYSIS_ORDER_SEVENTH - 7th-order analysis/rendering (64 channel input)
 */
#define HOSIRR_MAX_SH_ORDER ( 7 )
typedef enum _ANALYSIS_ORDERS{
    ANALYSIS_ORDER_FIRST = 1,
    ANALYSIS_ORDER_SECOND,
    ANALYSIS_ORDER_THIRD,
    ANALYSIS_ORDER_FOURTH,
    ANALYSIS_ORDER_FIFTH,
    ANALYSIS_ORDER_SIXTH,
    ANALYSIS_ORDER_SEVENTH
    
}ANALYSIS_ORDERS;

/*
 * Enum: LOUDSPEAKER_ARRAY_PRESETS
 * -------------------------------
 * Available loudspeaker array presets
 */
typedef enum _LOUDSPEAKER_ARRAY_PRESETS{
    LOUDSPEAKER_ARRAY_PRESET_DEFAULT = 1,
    LOUDSPEAKER_ARRAY_PRESET_5PX,
    LOUDSPEAKER_ARRAY_PRESET_7PX,
    LOUDSPEAKER_ARRAY_PRESET_8PX,
    LOUDSPEAKER_ARRAY_PRESET_9PX,
    LOUDSPEAKER_ARRAY_PRESET_10PX,
    LOUDSPEAKER_ARRAY_PRESET_11PX,
    LOUDSPEAKER_ARRAY_PRESET_11PX_7_4,
    LOUDSPEAKER_ARRAY_PRESET_13PX,
    LOUDSPEAKER_ARRAY_PRESET_22PX,
    LOUDSPEAKER_ARRAY_PRESET_AALTO_MCC,
    LOUDSPEAKER_ARRAY_PRESET_AALTO_MCC_SUBSET,
    LOUDSPEAKER_ARRAY_PRESET_AALTO_APAJA,
    LOUDSPEAKER_ARRAY_PRESET_AALTO_LR,
    LOUDSPEAKER_ARRAY_PRESET_DTU_AVIL,
    LOUDSPEAKER_ARRAY_PRESET_ZYLIA_LAB,
    LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_4,
    LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_12,
    LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_24,
    LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_36,
    LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_48,
    LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_60
    
}LOUDSPEAKER_ARRAY_PRESETS;

/*
 * Enum: _CH_ORDER
 * ---------------
 * Available Ambisonic channel ordering conventions
 * Note: CH_FUMA only supported for 1st order input.
 * Further note: FuMa: CH_FUMA+NORM_FUMA, AmbiX: CH_ACN+NORM_SN3D
 *
 * Options:
 *     CH_ACN  - Ambisonic Channel Numbering (ACN)
 *     CH_FUMA - (Obsolete) Furse-Malham/B-format (WXYZ)
 */
#define HOSIRR_NUM_CH_ORDERINGS ( 2 )
typedef enum _CH_ORDER{
    CH_ACN = 1,
    CH_FUMA     /* first-order only */
}CH_ORDER;

/*
 * Enum: NORM_TYPES
 * ----------------
 * Available Ambisonic normalisation conventions
 * Note: NORM_FUMA only supported for 1st order input and does NOT have the
 * 1/sqrt(2) scaling on the omni.
 * Further note: FuMa: CH_FUMA+NORM_FUMA, AmbiX: CH_ACN+NORM_SN3D
 *
 * Options:
 *     NORM_N3D  - orthonormalised (N3D)
 *     NORM_SN3D - Schmidt semi-normalisation (SN3D)
 *     NORM_FUMA - (Obsolete) Same as NORM_SN3D for 1st order
 */
#define HOSIRR_NUM_NORM_TYPES ( 3 )
typedef enum _NORM_TYPES{
    NORM_N3D = 1,
    NORM_SN3D,
    NORM_FUMA   /* first-order only */
}NORM_TYPES;
    
    
/*
 * Enum: AMBI_RIR_STATUS
 * ---------------------
 *
 * Options:
 *     AMBI_RIR_STATUS_LOADED  -
 *     AMBI_RIR_STATUS_NOT_LOADED -
 *     AMBI_RIR_STATUS_INVALID_FORMAT -
 */
typedef enum _AMBI_RIR_STATUS{
    AMBI_RIR_STATUS_LOADED = 0,
    AMBI_RIR_STATUS_NOT_LOADED,
    AMBI_RIR_STATUS_INVALID_FORMAT
}AMBI_RIR_STATUS;
    
/*
 * Enum: LS_RIR_STATUS
 * -------------------
 *
 * Options:
 *     LS_RIR_STATUS_RENDERED  -
 *     LS_RIR_STATUS_NOT_RENDERED -
 */
typedef enum _LS_RIR_STATUS{
    LS_RIR_STATUS_RENDERED = 0,
    LS_RIR_STATUS_RENDEREDING_ONGOING,
    LS_RIR_STATUS_NOT_RENDERED
}LS_RIR_STATUS;

#define HOSIRR_MAX_NUM_OUTPUTS ( 64 )
#define HOSIRR_PROGRESSTEXT_CHAR_LENGTH ( 256 )


/* ========================================================================== */
/*                               Main Functions                               */
/* ========================================================================== */

/*
 * Function: hosirrlib_create
 * --------------------------
 * Creates an instance of hosirrlib
 *
 * Input Arguments:
 *     phHS - & address of hosirrlib handle
 */
void hosirrlib_create(void** const phHS);

/*
 * Function: hosirrlib_destroy
 * ---------------------------
 * Destroys an instance of hosirrlib
 *
 * Input Arguments:
 *     phHS - & address of hosirrlib handle
 */
void hosirrlib_destroy(void** const phHS);

/*
 * Function: hosirrlib_render
 * --------------------------
 * Decodes input spherical harmonic signals to the loudspeaker channels.
 *
 * Input Arguments:
 *     hHS       - hosirrlib handle
 *     inputs    - input channel buffers; 2-D array: nInputs x nSamples
 *     outputs   - Output channel buffers; 2-D array: nOutputs x nSamples
 *     nInputs   - number of input channels
 *     nOutputs  - number of output channels
 *     nSamples  - number of samples in 'inputs'/'output' matrices
 */
void hosirrlib_render(void* const hHS);

    
/* ========================================================================== */
/*                                Set Functions                               */
/* ========================================================================== */
    

void hosirrlib_setBroadBandFirstPeakFLAG(void* const hHS, int newState);
void hosirrlib_setWindowLength (void* const hHS, int newValue);
void hosirrlib_setWetDryBalance(void* const hHS, float newValue); 
    
/*
 * Function: hosirrlib_setAmbiRIR
 * ------------------------------
 * Load input spherical harmonic (SH) room impulse response (RIR) to be
 * rendered by hosirrlib.
 *
 * Input Arguments:
 *     hHS - hosirrlib handle
 */
int hosirrlib_setAmbiRIR(void* const hHS,
                         const float** H,
                         int numChannels,
                         int numSamples,
                         int sampleRate);

/*
 * Function: hosirrlib_setAnalysisOrder
 * ------------------------------------
 * Sets the analysis/rendering order.
 *
 * Input Arguments:
 *     hAmbi    - hosirrlib handle
 *     newValue - new analysis/rendering order (see 'ANALYSIS_ORDERS' enum)
 */
void hosirrlib_setAnalysisOrder(void* const hHS,  int newValue);
    
/*
 * Function: hosirrlib_setLoudspeakerAzi_deg
 * -----------------------------------------
 * Sets the azimuth of a specific loudspeaker
 *
 * Input Arguments:
 *     hHS      - hosirrlib handle
 *     index      - loudspeaker index
 *     newAzi_deg - new azimuth in DEGREES
 */
void hosirrlib_setLoudspeakerAzi_deg(void* const hHS, int index, float newAzi_deg);

/*
 * Function: hosirrlib_setLoudspeakerElev_deg
 * ------------------------------------------
 * Sets the elevation of a specific loudspeaker
 *
 * Input Arguments:
 *     hHS         - hosirrlib handle
 *     index       - loudspeaker index
 *     newElev_deg - new elevation in DEGREES
 */
void hosirrlib_setLoudspeakerElev_deg(void* const hHS, int index, float newElev_deg);

/*
 * Function: ambi_dec_setNumLoudspeakers
 * -------------------------------------
 * Sets the number of loudspeakers to decode to.
 *
 * Input Arguments:
 *     hHS               - hosirrlib handle
 *     new_nLoudspeakers - new number of loudspeakers
 */
void hosirrlib_setNumLoudspeakers(void* const hHS, int new_nLoudspeakers);
 
/*
 * Function: hosirrlib_setOutputConfigPreset
 * -----------------------------------------
 * Sets the output loudspeaker preset.
 * For convenience, presets for several popular arrangements are included (see
 * "PRESETS" enum).
 *
 * Input Arguments:
 *     hHS         - hosirrlib handle
 *     newPresetID - new preset (see "PRESETS" enum)
 */
void hosirrlib_setOutputConfigPreset(void* const hHS, int newPresetID);

/*
 * Function: hosirrlib_setChOrder
 * -----------------------------
 * Sets the Ambisonic channel ordering convention to decode with, in order to
 * match the convention employed by the input signals
 *
 * Input Arguments:
 *     hHS      - hosirrlib handle
 *     newOrder - convention to use (see 'CH_ORDER' enum)
 */
void hosirrlib_setChOrder(void* const hHS, int newOrder);

/*
 * Function: hosirrlib_setNormType
 * ------------------------------
 * Sets the Ambisonic normalisation convention to decode with, in order to match
 * with the convention employed by the input signals.
 *
 * Input Arguments:
 *     hHS     - hosirrlib handle
 *     newType - convention to use (see 'NORM_TYPE' enum)
 */
void hosirrlib_setNormType(void* const hHS, int newType);

    
/* ========================================================================== */
/*                                Get Functions                               */
/* ========================================================================== */
    
int hosirrlib_getAmbiRIRinputOrder(void* const hHS);
int hosirrlib_getAmbiRIRlength_samples(void* const hHS);
float hosirrlib_getAmbiRIRlength_seconds(void* const hHS);
int hosirrlib_getAmbiRIRsampleRate(void* const hHS);
    
    
int hosirrlib_getBroadBandFirstPeakFLAG(void* const hHS);
int hosirrlib_getWindowLength (void* const hHS);
float hosirrlib_getWetDryBalance(void* const hHS);
    
int hosirrlib_getAmbiRIRstatus(void* const hHS);
int hosirrlib_getLsRIRstatus(void* const hHS);
    
float hosirrlib_getProgress0_1(void* const hHS);
void hosirrlib_getProgressText(void* const hHS, char* text);
    
void hosirrlib_getLsRIR(void* const hHS, float** lsRIR);
    
/*
 * Function: hosirrlib_getAnalysisOrder
 * ------------------------------------
 * Returns the master/maximum decoding order.
 *
 * Input Arguments:
 *     hHS - hosirrlib handle
 * Returns:
 *     current analysis/rendering order (see 'ANALYSIS_ORDERS' enum)
 */
int hosirrlib_getAnalysisOrder(void* const hHS);
    
/*
 * Function: hosirrlib_getLoudspeakerAzi_deg
 * ----------------------------------------
 * Returns the loudspeaker azimuth for a given index.
 *
 * Input Arguments:
 *     hHS   - hosirrlib handle
 *     index - loudspeaker index
 * Returns:
 *     loudspeaker azimuth in DEGREES
 */
float hosirrlib_getLoudspeakerAzi_deg(void* const hHS, int index);

/*
 * Function: hosirrlib_getLoudspeakerElev_deg
 * ------------------------------------------
 * Returns the loudspeaker elevation for a given index.
 *
 * Input Arguments:
 *     hHS   - hosirrlib handle
 *     index - loudspeaker index
 * Returns:
 *     loudspeaker elevation in DEGREES
 */
float hosirrlib_getLoudspeakerElev_deg(void* const hHS, int index);

/*
 * Function: hosirrlib_getNumLoudspeakers
 * -------------------------------------
 * Returns the number of loudspeakers in the current layout
 *
 * Input Arguments:
 *     hHS - hosirrlib handle
 * Returns:
 *     number of loudspeakers
 */
int hosirrlib_getNumLoudspeakers(void* const hHS);

/*
 * Function: hosirrlib_getMaxNumLoudspeakers
 * -----------------------------------------
 * Returns the maximum number of loudspeakers supported by hosirrlib
 *
 * Returns:
 *     maximum number of loudspeakers
 */
int hosirrlib_getMaxNumLoudspeakers(void);
 
/*
 * Function: hosirrlib_getChOrder
 * ------------------------------
 * Returns the Ambisonic channel ordering convention currently being used to
 * decode with, which should match the convention employed by the input signals
 *
 * Input Arguments:
 *     hHS - hosirrlib handle
 * Returns:
 *     convention currently being used (see 'CH_ORDER' enum)
 */
int hosirrlib_getChOrder(void* const hHS);

/*
 * Function: hosirrlib_getNormType
 * -------------------------------
 * Returns the Ambisonic normalisation convention currently being usedto decode
 * with, which should match the convention employed by the input signals.
 *
 * Input Arguments:
 *     hHS - hosirrlib handle
 * Returns:
 *     convention currently being used (see 'NORM_TYPE' enum)
 */
int hosirrlib_getNormType(void* const hHS);
 
    
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __HOSIRRLIB_H_INCLUDED__ */
