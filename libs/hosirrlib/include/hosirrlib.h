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
 * @file hosirrlib.h
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

#ifndef __HOSIRRLIB_H_INCLUDED__
#define __HOSIRRLIB_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ========================================================================== */
/*                             Presets + Constants                            */
/* ========================================================================== */

/**
 * Available analysis/rendering order options
 */
#define HOSIRR_MAX_SH_ORDER ( 7 )
typedef enum _ANALYSIS_ORDERS{
    ANALYSIS_ORDER_FIRST = 1, /**< 1st-order rendering (4 channel input) */
    ANALYSIS_ORDER_SECOND,    /**< 2nd-order rendering (9 channel input) */
    ANALYSIS_ORDER_THIRD,     /**< 3rd-order rendering (16 channel input) */
    ANALYSIS_ORDER_FOURTH,    /**< 4th-order rendering (25 channel input) */
    ANALYSIS_ORDER_FIFTH,     /**< 5th-order rendering (36 channel input) */
    ANALYSIS_ORDER_SIXTH,     /**< 6th-order rendering (49 channel input) */
    ANALYSIS_ORDER_SEVENTH    /**< 7th-order rendering (64 channel input) */
    
}ANALYSIS_ORDERS;

/**
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

/**
 * Available Ambisonic channel ordering conventions
 *
 * @note CH_FUMA only supported for 1st order input.
 */
typedef enum _CH_ORDER {
    CH_ACN = 1, /**< Ambisonic Channel Numbering (ACN) */
    CH_FUMA     /**< (Obsolete) Furse-Malham/B-format (WXYZ) */

} CH_ORDER;

/** Number of channel ordering options */
#define HOSIRR_NUM_CH_ORDERINGS ( 2 )

/**
 * Available Ambisonic normalisation conventions
 *
 * @note NORM_FUMA only supported for 1st order input and does NOT have the
 *       1/sqrt(2) scaling on the omni.
 */
typedef enum _NORM_TYPES {
    NORM_N3D = 1,   /**< orthonormalised (N3D) */
    NORM_SN3D,      /**< Schmidt semi-normalisation (SN3D) */
    NORM_FUMA       /**< (Obsolete) Same as NORM_SN3D for 1st order */

} NORM_TYPES;

/** Number of normalisation options */
#define HOSIRR_NUM_NORM_TYPES ( 3 )
    
/**
 * Status of the ambisonic RIR
 */
typedef enum _AMBI_RIR_STATUS{
    AMBI_RIR_STATUS_LOADED = 0,    /**< An Ambisonic RIR has been loaded */
    AMBI_RIR_STATUS_NOT_LOADED,    /**< An Ambisonic RIR has NOT been loaded */
    AMBI_RIR_STATUS_INVALID_FORMAT /**< A file that does not have (N+1)^2
                                    *   channels was loaded, and will not be
                                    *   used for rendering */
}AMBI_RIR_STATUS;
    
/**
 * Statis of the loudspeaker RIR
 */
typedef enum _LS_RIR_STATUS{
    LS_RIR_STATUS_RENDERED = 0,        /**< Loudspeaker RIR has been rendered */
    LS_RIR_STATUS_RENDEREDING_ONGOING, /**< Loudspeaker RIR is currently being
                                        *   rendered */
    LS_RIR_STATUS_NOT_RENDERED         /**< Loudspeaker RIR has not yet been
                                        *   rendered */
}LS_RIR_STATUS;

#define HOSIRR_MAX_NUM_OUTPUTS ( 64 )
#define HOSIRR_PROGRESSTEXT_CHAR_LENGTH ( 256 )


/* ========================================================================== */
/*                               Main Functions                               */
/* ========================================================================== */

/**
 * Creates an instance of hosirrlib
 *
 * @param[in] phHS (&) address of hosirrlib handle
 */
void hosirrlib_create(void** const phHS);

/**
 * Destroys an instance of hosirrlib
 *
 * @param[in] phHS (&) address of hosirrlib handle
 */
void hosirrlib_destroy(void** const phHS);

/**
 * Analyses the input Ambisonic RIR and synthesises a Loudspeaker RIR, based
 * on the configured settings, using the HO-SIRR algorithm [1]
 *
 * @param[in] hHS hosirrlib handle
 *
 * @see [1] McCormack, L., Politis, A., Scheuregger, O., and Pulkki, V. (2019).
 *          "Higher-order processing of spatial impulse responses". In
 *          Proceedings of the 23rd International Congress on Acoustics, 9--13
 *          September 2019 in Aachen, Germany.
 */
void hosirrlib_render(void* const hHS);

    
/* ========================================================================== */
/*                                Set Functions                               */
/* ========================================================================== */
    
/**
 * Sets a flag, as to whether the renderer should isolate the first peak in the
 * Ambisonic RIR and process it based on broad-band analysis (0:disabled,
 * 1:enabled)
 *
 * This can help reduce timbral colouration in some cases.
 */
void hosirrlib_setBroadBandFirstPeakFLAG(void* const hHS, int newState);

/**
 * Sets the windowing length, in samples, used by the HOSIRR method
 */
void hosirrlib_setWindowLength (void* const hHS, int newValue);

/**
 * Sets the wet/dry balance; when 0: only dry (non-diffuse), 1: only wet
 * (diffuse)
 */
void hosirrlib_setWetDryBalance(void* const hHS, float newValue); 
    
/**
 * Load input Ambisonic (spherical harmonic) room impulse response (RIR) to be
 * rendered by hosirrlib.
 *
 * @param[in] hHS         hosirrlib handle
 * @param[in] H           The Ambisonic RIR; numChannels x numSamples
 * @param[in] numChannels Number of channels in H
 * @param[in] numSamples  Number of samples per channel in H
 * @param[in] sampleRate  Sample rate of the loaded H
 */
int hosirrlib_setAmbiRIR(void* const hHS,
                         const float** H,
                         int numChannels,
                         int numSamples,
                         int sampleRate);

/**
 * Sets the analysis/rendering order (see ANALYSIS_ORDERS enum)
 */
void hosirrlib_setAnalysisOrder(void* const hHS,  int newValue);
    
/**
 * Sets the azimuth of a specific loudspeaker
 */
void hosirrlib_setLoudspeakerAzi_deg(void* const hHS, int index, float newAzi_deg);

/**
 * Sets the elevation of a specific loudspeaker
 */
void hosirrlib_setLoudspeakerElev_deg(void* const hHS, int index, float newElev_deg);

/**
 * Sets the number of loudspeakers in the setup
 */
void hosirrlib_setNumLoudspeakers(void* const hHS, int new_nLoudspeakers);
 
/**
 * Sets the output loudspeaker preset
 *
 * For convenience, presets for several popular arrangements are included (see
 * LOUDSPEAKER_ARRAY_PRESETS enum).
 */
void hosirrlib_setOutputConfigPreset(void* const hHS, int newPresetID);

/**
 * Sets the Ambisonic channel ordering convention to use, which should match the
 * convention employed by the input RIR (see 'CH_ORDER' enum)
 */
void hosirrlib_setChOrder(void* const hHS, int newOrder);

/**
 * Sets the Ambisonic normalisation convention to use, which shoudl match the
 * convention employed by the input RIR (see 'NORM_TYPE' enum)
 */
void hosirrlib_setNormType(void* const hHS, int newType);

    
/* ========================================================================== */
/*                                Get Functions                               */
/* ========================================================================== */

/**
 * Returns the order of the loaded Ambisonic RIR
 */
int hosirrlib_getAmbiRIRinputOrder(void* const hHS);

/**
 * Returns the length, in samples, of the loaded Ambisonic RIR
 */
int hosirrlib_getAmbiRIRlength_samples(void* const hHS);

/**
 * Returns the length, in seconds, of the loaded Ambisonic RIR
 */
float hosirrlib_getAmbiRIRlength_seconds(void* const hHS);

/**
 * Returns the sampling rate of the loaded Ambisonic RIR
 */
int hosirrlib_getAmbiRIRsampleRate(void* const hHS);
    
/**
 * Returns a flag, dictating whether the renderer should isolate the first peak
 * in the Ambisonic RIR and process it based on broad-band analysis (0:disabled,
 * 1:enabled)
 */
int hosirrlib_getBroadBandFirstPeakFLAG(void* const hHS);

/**
 * Returns the windowing length, in samples, used by the HOSIRR method
 */
int hosirrlib_getWindowLength (void* const hHS);

/**
 * Returns the wet/dry balance; when 0: only dry (non-diffuse), 1: only wet
 * (diffuse)
 */
float hosirrlib_getWetDryBalance(void* const hHS);

/**
 * Returns the status of the Ambisonic RIR (see AMBI_RIR_STATUS enum)
 */
int hosirrlib_getAmbiRIRstatus(void* const hHS);

/**
 * Returns the status of the Loudspeaker RIR (see LS_RIR_STATUS enum)
 */
int hosirrlib_getLsRIRstatus(void* const hHS);

/**
 * Returns the current rendering progress (0: just started, 0<x<1: ongoing,
 * 1: finished)
 */
float hosirrlib_getProgress0_1(void* const hHS);

/**
 * Returns a string of length 'HOSIRR_PROGRESSTEXT_CHAR_LENGTH', which describes
 * the progress of the current render
 */
void hosirrlib_getProgressText(void* const hHS, char* text);
    
void hosirrlib_getLsRIR(void* const hHS, float** lsRIR);
    
/**
 * Returns the master/maximum decoding order (see 'ANALYSIS_ORDERS' enum)
 */
int hosirrlib_getAnalysisOrder(void* const hHS);
    
/**
 * Returns the loudspeaker azimuth for a given index
 */
float hosirrlib_getLoudspeakerAzi_deg(void* const hHS, int index);

/**
 * Returns the loudspeaker elevation for a given index
 */
float hosirrlib_getLoudspeakerElev_deg(void* const hHS, int index);

/**
 * Returns the number of loudspeakers in the current layout
 */
int hosirrlib_getNumLoudspeakers(void* const hHS);

/**
 * Returns the maximum number of loudspeakers supported by hosirrlib
 */
int hosirrlib_getMaxNumLoudspeakers(void);
 
/**
 * Returns the Ambisonic channel ordering convention currently being, which
 * should match the convention employed by the input RIR (see 'CH_ORDER' enum)
 */
int hosirrlib_getChOrder(void* const hHS);

/**
 * Returns the Ambisonic normalisation convention currently being used, which
 * should match the convention employed by the input RIR (see 'NORM_TYPE' enum)
 */
int hosirrlib_getNormType(void* const hHS);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __HOSIRRLIB_H_INCLUDED__ */
