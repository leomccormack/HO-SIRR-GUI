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
 * @file hosirrlib.c
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

void hosirrlib_create
(
    void ** const phHS
)
{
    hosirrlib_data* pData = (hosirrlib_data*)malloc1d(sizeof(hosirrlib_data));
    *phHS = (void*)pData;
    
    pData->progress0_1 = 0.0f;
    pData->progressText = malloc1d(HOSIRR_PROGRESSTEXT_CHAR_LENGTH*sizeof(char));
    strcpy(pData->progressText,"HOSIRR");
    
    /* input AmbiRIR */
    pData->shir = NULL;
    pData->ambiRIR_status = AMBI_RIR_STATUS_NOT_LOADED;
    pData->ambiRIRorder = -1;
    pData->ambiRIRlength_seconds = pData->ambiRIRlength_samples = 0.0f;
    pData->ambiRIRsampleRate = 0;
    
    /* output Loudspeaker array RIR */
    pData->lsir = NULL;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
    
    /* default user parameters */
    pData->analysisOrder = 1;
    loadLoudspeakerArrayPreset(LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_24, pData->loudpkrs_dirs_deg, &(pData->nLoudpkrs));
    pData->chOrdering = CH_ACN;
    pData->norm = NORM_SN3D;
    pData->broadBandFirstPeakFLAG = 1;
    pData->windowLength = DEFAULT_WINDOW_LENGTH;
    pData->wetDryBalance = 1.0f;
}

void hosirrlib_destroy
(
    void ** const phHS
)
{
    hosirrlib_data *pData = (hosirrlib_data*)(*phHS);
    
    if (pData != NULL) {
        free(pData->shir);
        free(pData->lsir);
        free(pData->progressText);
        free(pData);
        pData = NULL;
    }
}

void hosirrlib_render
(
    void  *  const hHS
)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    
    /* The leaning tower of variables */
    int i, j, k, n, ch, maxInd, dirwinsize, BB1stPeak, order, nSH, winsize, nLS;
    int N_gtable, N_tri, aziRes, elevRes, N_azi, aziIndex, elevIndex, idx3d;
    int numSec, order_sec, nSH_sec, delay, frameCount;
    int fftsize, hopsize, nBins_anl, nBins_syn, maxDiffFreq_Ind, rir_len;
    int o[MAX_SH_ORDER+2];
    int idx, jl, lSig, lSig_pad;
    float fs, wetDry, peakNorm, normSec, nearestVal, tmp, a2eNorm;
    float intensity[3], diff_intensity[3], energy, diff_energy, normSecIntensity;
    //float t60[6] = {0.07f, 0.07f, 0.06f, 0.04f, 0.02f, 0.01f};
    float t60[6] = {0.2f, 0.2f, 0.16f, 0.12f, 0.09f, 0.04};
    float fc[6] = {125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f};
    float IntensityBB[3] = {0.0f};
    float IntensityBB_XYZ[3];
    float* shir, *shir_tmp, *shir_tmp2, *direct_win, *shir_direct, *shir_pad, *gtable, *sec_dirs_deg, *sectorCoeffs_tmp;
    float* lsir_ndiff, *lsir_diff, *win, *Y_enc_tmp, *D_ls_tmp;
    float* prev_diff_energy, *prev_diff_intensity, *azim, *elev, *diffs;
    float* insig_win, *ndiffs_sqrt, *lsir_win, *M_ifft, *M_ifft_fl, *rir_filt;
    float_complex* A_xyz, *sectorCoeffs, *sectorCoeffs_syn, *Y_enc, *D_ls;
    float_complex* inspec_syn, *inspec_anl, *s_anl, *WXYZ_sec, *z_diff, *z_00;
    float_complex* ndiffgains_interp, *diffgains_interp, *ndiffgains, *diffs_sqrt;
    float_complex* outspec_ndiff, *outspec_diff, *a_diff;
    float_complex pvCOV[4][4];
    const float_complex calpha = cmplxf(1.0f, 0.0f), cbeta = cmplxf(0.0f, 0.0f);
    void* hFFT_syn, *hFFT_anl;
    NORM_TYPES norm;
    CH_ORDER chOrdering;
    
    /* Check if processing should actually go-ahead */
    if(pData->ambiRIR_status != AMBI_RIR_STATUS_LOADED ||
       pData->lsRIR_status == LS_RIR_STATUS_RENDERED ||
       pData->lsRIR_status == LS_RIR_STATUS_RENDEREDING_ONGOING)
        return;
    else
        pData->lsRIR_status = LS_RIR_STATUS_RENDEREDING_ONGOING;
    
    /* take a local copy of current configuration to be thread safe */
    fs = pData->ambiRIRsampleRate;
    order = SAF_MIN(pData->analysisOrder, pData->ambiRIRorder);
    nSH = (order+1)*(order+1);
    winsize = pData->windowLength;
    lSig = pData->ambiRIRlength_samples;
    BB1stPeak = pData->broadBandFirstPeakFLAG;
    wetDry = pData->wetDryBalance;
    nLS = pData->nLoudpkrs;
    norm = pData->norm;
    chOrdering = pData->chOrdering;
    
    /* make local copy of current Ambi RIR */
    shir = malloc1d(nSH * lSig * sizeof(float));
    switch(chOrdering){
        case CH_ACN:
            memcpy(shir, pData->shir, nSH * lSig * sizeof(float));
            break;
        case CH_FUMA:
            /* only for first-order, convert to ACN */
            assert(nSH==4);
            memcpy(&shir[0], &(pData->shir[0]), lSig * sizeof(float));
            memcpy(&shir[1*lSig], &(pData->shir[3*lSig]), lSig * sizeof(float));
            memcpy(&shir[2*lSig], &(pData->shir[1*lSig]), lSig * sizeof(float));
            memcpy(&shir[3*lSig], &(pData->shir[2*lSig]), lSig * sizeof(float));
            break;
    }
    
    /* account for input normalisation scheme */
    for(n=0; n<MAX_SH_ORDER+2; n++){  o[n] = n*n;  }
    switch(norm){
        case NORM_N3D:  /* already in N3D, do nothing */
            break;
        case NORM_FUMA: /* (same as converting SN3D->N3D for first-order) */
        case NORM_SN3D: /* convert to N3D */
            for (n = 0; n<order+1; n++)
                for (ch = o[n]; ch<o[n+1]; ch++)
                    for(i = 0; i<lSig; i++)
                        shir[ch*lSig+i] *= sqrtf(2.0f*(float)n+1.0f);
            break;
    }
    
    /* normalise such that peak of the omni is 1 */
    utility_simaxv(shir, lSig, &maxInd); /* index of max(abs(omni)) */
    peakNorm = 1.0f/fabsf(shir[maxInd]);
    utility_svsmul(shir, &peakNorm, nSH*lSig, shir);
    
    /* isolate first peak */
    if(BB1stPeak){
        utility_simaxv(shir, lSig, &maxInd); /* index of max(abs(omni)) */
        
        /* calculate window and extract peak */
        dirwinsize = 64;
        if(maxInd <dirwinsize/2)
            BB1stPeak=0;
        else{
            shir_tmp = malloc1d(nSH*lSig*sizeof(float));
            shir_tmp2 = malloc1d(nSH*lSig*sizeof(float));
            memcpy(shir_tmp, shir, nSH*lSig*sizeof(float));
            direct_win = calloc1d(nSH*lSig,sizeof(float));
            for(i=0; i<nSH; i++)
                getWindowingFunction(WINDOWING_FUNCTION_HANN, dirwinsize+1 /* force symmetry */, &direct_win[i*lSig + maxInd - dirwinsize/2]);
            shir_direct = malloc1d(nSH*lSig*sizeof(float));
            utility_svvmul(shir_tmp, direct_win, nSH*lSig, shir_direct);
            
            /* flip window and use it to remove peak from the input */
            for(i=0; i<nSH*lSig; i++)
                direct_win[i] = 1.0f-direct_win[i];
            utility_svvmul(shir_tmp, direct_win, nSH*lSig, shir_tmp2);
            memcpy(shir, shir_tmp2, nSH*lSig*sizeof(float));
            
            free(shir_tmp);
            free(shir_tmp2);
            free(direct_win);
        }
    }
    
    /* zero pad the signal's start and end for STFT */
    lSig_pad = winsize/2 + winsize*2 + lSig;
    shir_pad = calloc1d(nSH*lSig_pad, sizeof(float));
    for(i=0; i<nSH; i++)
        memcpy(&shir_pad[i*lSig_pad + winsize/2], &(shir[i*lSig]), lSig*sizeof(float));
 
    /* VBAP gain table */
    strcpy(pData->progressText,"Computing VBAP Gain Table");
    pData->progress0_1 = 0.0f;
    gtable = NULL;
    aziRes = 1;
    elevRes = 1;
    generateVBAPgainTable3D((float*)pData->loudpkrs_dirs_deg, nLS, aziRes, elevRes, 0, 0, 0.0f, &gtable, &N_gtable, &N_tri);
    
    /* Sector design */
    strcpy(pData->progressText,"Computing Sector Coefficients");
    numSec = order == 1 ? 1 : __Tdesign_nPoints_per_degree[2*order-1];
    sec_dirs_deg = (float*)__HANDLES_Tdesign_dirs_deg[2*order-1];
    order_sec = order-1;
    nSH_sec = (order_sec+1)*(order_sec+1);
    A_xyz = malloc1d(nSH*nSH_sec*3*sizeof(float_complex));
    computeVelCoeffsMtx(order_sec, A_xyz);
    sectorCoeffs_tmp = malloc1d((numSec*4)*nSH*sizeof(float));
    sectorCoeffs = malloc1d((numSec*4)*nSH*sizeof(float_complex));
    sectorCoeffs_syn = malloc1d((numSec*4)*nSH*sizeof(float_complex));
    normSec = computeSectorCoeffsEP(order_sec, A_xyz, SECTOR_PATTERN_PWD, sec_dirs_deg, numSec, sectorCoeffs_tmp);
    for(i=0; i<numSec*4*nSH; i++){
        sectorCoeffs[i] = cmplxf(sectorCoeffs_tmp[i], 0.0f); /* real->complex data type */
        sectorCoeffs_syn[i] = cmplxf(sectorCoeffs_tmp[i]/sqrtf(4.0f*SAF_PI), 0.0f);
    }
    free(sectorCoeffs_tmp);
    free(A_xyz);
    
    /* Inits */
    fftsize = winsize*2; 
    hopsize = winsize/2;       /* half the window size time-resolution */
    nBins_anl = winsize/2 + 1; /* nBins used for analysis */
    nBins_syn = fftsize/2 + 1; /* nBins used for synthesis */
    nearestVal = 10e5f;
    for(i=0; i<nBins_anl; i++){
        tmp = fabsf((float)i*(fs/(float)winsize) - MAX_DIFF_FREQ_HZ);
        if(tmp < nearestVal){
            nearestVal = tmp;
            maxDiffFreq_Ind = i;
        }
    }
    
    /* transform window (symmetric Hann - 'hanning' in matlab) */
    win = malloc1d(winsize*sizeof(float));
    for(i=0; i<winsize; i++)
        win[i] = powf(sinf((float)i*(SAF_PI/(float)winsize)), 2.0f);
    
    /* diffuse stream rendering intialisations */
    a2eNorm = (float)nLS/(sqrtf((float)nLS));
    if(order==1){
        D_ls_tmp = malloc1d(nLS*nSH*sizeof(float));
        getLoudspeakerDecoderMtx((float*)pData->loudpkrs_dirs_deg, nLS, LOUDSPEAKER_DECODER_SAD, order, 0, D_ls_tmp);
        utility_svsmul(D_ls_tmp, &a2eNorm, nLS*nSH, D_ls_tmp);
        D_ls = malloc1d(nLS*nSH*sizeof(float_complex));
        for(i=0; i<nLS*nSH; i++)
            D_ls[i] = cmplxf(D_ls_tmp[i], 0.0f);
    }
    else{
        Y_enc_tmp = malloc1d(nSH_sec*numSec*sizeof(float));
        getRSH(order_sec, sec_dirs_deg, numSec, Y_enc_tmp);
        D_ls_tmp = malloc1d(nLS*nSH_sec*sizeof(float));
        getLoudspeakerDecoderMtx((float*)pData->loudpkrs_dirs_deg, nLS, LOUDSPEAKER_DECODER_SAD, order_sec, 0, D_ls_tmp);
        utility_svsmul(D_ls_tmp, &a2eNorm, nLS*nSH_sec, D_ls_tmp);
        Y_enc = malloc1d(nSH_sec*numSec*sizeof(float_complex));
        D_ls = malloc1d(nLS*nSH_sec*sizeof(float_complex));
        for(i=0; i<nSH_sec*numSec; i++)
            Y_enc[i] = cmplxf(Y_enc_tmp[i], 0.0f);
        for(i=0; i<nLS*nSH_sec; i++)
            D_ls[i] = cmplxf(D_ls_tmp[i], 0.0f);
        free(Y_enc_tmp);
    }
    free(D_ls_tmp);
    
    /* mem alloc */
    saf_rfft_create(&hFFT_syn, fftsize);
    saf_rfft_create(&hFFT_anl, fftsize/2);
    lsir_ndiff = calloc1d(nLS * (lSig + (2*fftsize)), sizeof(float));
    lsir_diff  = calloc1d(nLS * (lSig + (2*fftsize)), sizeof(float));
    lsir_win = malloc1d(fftsize * sizeof(float));
    prev_diff_energy = calloc1d(numSec, sizeof(float));
    prev_diff_intensity = calloc1d(numSec*3, sizeof(float));
    azim = malloc1d(numSec*nBins_anl*sizeof(float));
    elev = malloc1d(numSec*nBins_anl*sizeof(float));
    diffs = malloc1d(numSec*nBins_anl*sizeof(float));
    ndiffs_sqrt = malloc1d(nBins_anl*sizeof(float));
    diffs_sqrt  = malloc1d(nBins_anl*sizeof(float_complex));
    insig_win = calloc1d(fftsize,sizeof(float));
    inspec_syn = calloc1d(nSH*nBins_syn,sizeof(float_complex));      //////ma->ca
    inspec_anl = calloc1d(nSH*nBins_anl,sizeof(float_complex));//////ma->ca
    s_anl = malloc1d(4*numSec*nBins_anl*sizeof(float_complex));
    WXYZ_sec = malloc1d(4*nBins_anl*sizeof(float_complex));
    z_diff = malloc1d(numSec*nBins_syn*sizeof(float_complex));
    z_00 = malloc1d(nBins_syn*sizeof(float_complex));
    a_diff = malloc1d(nSH*nBins_syn*sizeof(float_complex));
    M_ifft = malloc1d(winsize*sizeof(float));
    M_ifft_fl = calloc1d(fftsize, sizeof(float));
    ndiffgains = malloc1d(nLS*nBins_anl*sizeof(float_complex));
    ndiffgains_interp = malloc1d(nLS*nBins_syn*sizeof(float_complex));
    diffgains_interp = malloc1d(nBins_syn*sizeof(float_complex));
    outspec_ndiff = malloc1d(nLS*nBins_syn*sizeof(float_complex));
    outspec_diff = malloc1d(nLS*nBins_syn*sizeof(float_complex));
    
    /* Main processing loop */
    strcpy(pData->progressText,"HOSIRR - Rendering");
    idx = frameCount = 0;
    while (idx + winsize < lSig + 2*winsize){

        /* update progress */
        pData->progress0_1 = (float)idx/(float)(lSig + 2*winsize);
 
        /* Window input and transform to frequency domain */
        for(i=0; i<nSH; i++){
            for(j=0; j<winsize; j++)
                insig_win[j] = win[j] * shir_pad[i*lSig_pad+idx+j];
            saf_rfft_forward(hFFT_syn, insig_win, &inspec_syn[i*nBins_syn]);
            for(j=0, k=0; j<nBins_anl; j++, k+=fftsize/winsize)
                inspec_anl[i*nBins_anl+j] = inspec_syn[i*nBins_syn+k];
        }
        
        /* Form weighted pressure-velocity signals */
        cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, numSec*4, nBins_anl, nSH, &calpha,
                    sectorCoeffs, nSH,
                    inspec_anl, nBins_anl, &cbeta,
                    s_anl, nBins_anl);
        
        /* SIRR analysis for each sector */
        for(n=0; n<numSec; n++){
            for(i=0; i<4; i++)
                memcpy(&WXYZ_sec[i*nBins_anl], &s_anl[n*4*nBins_anl + i*nBins_anl], nBins_anl*sizeof(float_complex));
            
            /* compute Intensity vector for each frequency bin to estimate DoA */
            for(j=0; j<nBins_anl; j++){
                for(i=0; i<3; i++)
                    intensity[i] = crealf( ccmulf(conjf(WXYZ_sec[j]), WXYZ_sec[(i+1)*nBins_anl+j]));
                azim[n*nBins_anl+j] = atan2f(intensity[1], intensity[0])*180.0f/SAF_PI;
                elev[n*nBins_anl+j] = atan2f(intensity[2], sqrtf(powf(intensity[0], 2.0f) + powf(intensity[1], 2.0f)))*180.0f/SAF_PI;
            }
                
            /* Compute broad-band active-intensity vector */
            cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasConjTrans, 4, 4, maxDiffFreq_Ind+1, &calpha,
                        WXYZ_sec, nBins_anl,
                        WXYZ_sec, nBins_anl, &cbeta,
                        FLATTEN2D(pvCOV), 4);
            for(i=0; i<3; i++)
                intensity[i] = crealf(pvCOV[1+i][0]);
            energy = 0.0f;
            for(i=0; i<4; i++)
                energy += crealf(pvCOV[i][i])*0.5f;
            
            /* Estimating and time averaging of boadband diffuseness */
            normSecIntensity = 0.0f;
            for(i=0; i<3; i++){
                diff_intensity[i] = (1.0f-ALPHA_DIFF_COEFF)*intensity[i] + ALPHA_DIFF_COEFF*prev_diff_intensity[n*3+i];
                prev_diff_intensity[n*3+i] = diff_intensity[i];
                normSecIntensity += powf(fabsf(diff_intensity[i]), 2.0f);
            }
            diff_energy = (1.0f-ALPHA_DIFF_COEFF)*energy + ALPHA_DIFF_COEFF*prev_diff_energy[n];
            prev_diff_energy[n] = diff_energy;
            normSecIntensity = sqrtf(normSecIntensity);
            for(i=0; i<nBins_anl; i++)
                diffs[n*nBins_anl+i] = 1.0f - normSecIntensity/(diff_energy+2.23e-10f);
        }
        
        /* SIRR Synthesis for each sector */
        memset(outspec_ndiff, 0, nLS*nBins_syn*sizeof(float_complex));
        memset(outspec_diff,  0, nLS*nBins_syn*sizeof(float_complex));
        for(n=0; n<numSec; n++){
            for(i=0; i<nBins_anl; i++){
                ndiffs_sqrt[i] = sqrtf(1.0f-diffs[n*nBins_anl+i]);
                diffs_sqrt[i]  = cmplxf(sqrtf(diffs[n*nBins_anl+i]), 0.0f);
            }
            
            /* Gain factor computation */
            for(i=0; i<nBins_anl; i++){
                N_azi = (int)(360.0f / (float)aziRes + 0.5f) + 1;
                aziIndex = (int)(matlab_fmodf(azim[n*nBins_anl+i] + 180.0f, 360.0f) / (float)aziRes + 0.5f);
                elevIndex = (int)((elev[n*nBins_anl+i] + 90.0f) / (float)elevRes + 0.5f);
                idx3d = elevIndex * N_azi + aziIndex;
                for(j=0; j<nLS; j++)
                    ndiffgains[j*nBins_anl+i] = cmplxf(gtable[idx3d*nLS+j] * ndiffs_sqrt[i], 0.0f);
            }
            
            /* Interpolate panning filters  */
            for(i=0; i<nLS; i++){
                saf_rfft_backward(hFFT_anl, &ndiffgains[i*nBins_anl], M_ifft);
                /* flip */
                for(j=0; j<winsize/2; j++){
                    M_ifft_fl[j] = M_ifft[winsize/2+j];
                    M_ifft_fl[winsize/2+j] = M_ifft[j];
                }
                saf_rfft_forward(hFFT_syn, M_ifft_fl, &ndiffgains_interp[i*nBins_syn]);
            }
            
            /* Generate non-diffuse stream */
            cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 1, nBins_syn, nSH, &calpha,
                        &sectorCoeffs_syn[n*4*nSH], nSH,
                        inspec_syn, nBins_syn, &cbeta,
                        z_00, nBins_syn);
            for(i=0; i<nLS; i++)
                for(j=0; j<nBins_syn; j++)
                    outspec_ndiff[i*nBins_syn+j] = ccaddf(outspec_ndiff[i*nBins_syn+j],
                                                          ccmulf(ndiffgains_interp[i*nBins_syn+j], crmulf(z_00[j], sqrtf(normSec))) );
            
            /* Interpolate diffs  */
            saf_rfft_backward(hFFT_anl, diffs_sqrt, M_ifft);
            /* flip */
            for(j=0; j<winsize/2; j++){
                M_ifft_fl[j] = M_ifft[winsize/2+j];
                M_ifft_fl[winsize/2+j] = M_ifft[j];
            }
            saf_rfft_forward(hFFT_syn, M_ifft_fl, diffgains_interp);
            
            /* Generate diffuse stream */
            if(order==1){
                for(i=0; i<nSH; i++)
                    for(j=0; j<nBins_syn; j++)
                        a_diff[i*nBins_syn+j] = ccmulf(diffgains_interp[j], crmulf(inspec_syn[i*nBins_syn+j], 1.0f/sqrtf((float)nSH)));
            }
            else{
                for(j=0; j<nBins_syn; j++)
                    z_diff[n*nBins_syn+j] = crmulf(ccmulf(diffgains_interp[j], z_00[j]), 1.0f/sqrtf(numSec));
            }
        }
        
        /* Decode diffuse stream to loudspeakers */
        if(order==1){
            cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nLS, nBins_syn, nSH, &calpha,
                        D_ls, nSH,
                        a_diff, nBins_syn, &cbeta,
                        outspec_diff, nBins_syn);
        }
        else{
            cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nSH_sec, nBins_syn, numSec, &calpha,
                        Y_enc, numSec,
                        z_diff, nBins_syn, &cbeta,
                        a_diff, nBins_syn);
            cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nLS, nBins_syn, nSH_sec, &calpha,
                        D_ls, nSH_sec,
                        a_diff, nBins_syn, &cbeta,
                        outspec_diff, nBins_syn);
        }

        /* Overlap-add synthesis */
        for(i=0; i<nLS; i++){
            saf_rfft_backward(hFFT_syn, &outspec_ndiff[i*nBins_syn], lsir_win);
            for(jl=idx, k=0; jl<fftsize+idx; jl++, k++)
                lsir_ndiff[i*(lSig + (2*fftsize))+jl] += lsir_win[k];
            saf_rfft_backward(hFFT_syn, &outspec_diff[i*nBins_syn], lsir_win);
            for(jl=idx, k=0; jl<fftsize+idx; jl++, k++)
                lsir_diff[i*(lSig + (2*fftsize))+jl] += lsir_win[k];
        }
        
        idx += hopsize;
        frameCount++;
    }
    
    /* Remove delay caused by processing */
    delay = winsize; ///2;    // from winsize/2
    for(i=0; i<nLS; i++){
        memcpy(&lsir_ndiff[i*lSig], &lsir_ndiff[i*(lSig + (2*fftsize))+delay], lSig*sizeof(float));
        memcpy(&lsir_diff[i*lSig], &lsir_diff[i*(lSig + (2*fftsize))+delay], lSig*sizeof(float));
    }
    lsir_ndiff = realloc1d(lsir_ndiff, nLS*lSig*sizeof(float));
    lsir_diff = realloc1d(lsir_diff, nLS*lSig*sizeof(float));
    
    /* Convolution with exponential decaying noise to decorrelate the diffuse stream */
    strcpy(pData->progressText,"Decorrelating Diffuse Stream");
    rir_filt = NULL;
    synthesiseNoiseReverb(nLS, fs, t60, fc, 6, 1, &rir_filt, &rir_len);
    fftfilt(lsir_diff, rir_filt, lSig, rir_len, nLS, lsir_diff);
    
    /* Re-introduce first peak based on broadband analysis  */
    if(BB1stPeak){
        /* Broad-band intensity */
        for(i=0; i<3; i++)
            for(j=0; j<lSig; j++)
                IntensityBB[i] += shir_direct[0*lSig + j] * shir_direct[(i+1)*lSig + j]/sqrtf(3.0f); /* "sqrtf(3.0f)" for N3D->SN3D */
        IntensityBB_XYZ[0] = IntensityBB[2];
        IntensityBB_XYZ[1] = IntensityBB[0];
        IntensityBB_XYZ[2] = IntensityBB[1];
        
        /* Get gtable index for DoA */
        N_azi = (int)(360.0f / aziRes + 0.5f) + 1;
        aziIndex = (int)(matlab_fmodf( (atan2f(IntensityBB_XYZ[1], IntensityBB_XYZ[0]) *180.0f/SAF_PI) + 180.0f, 360.0f) / (float)aziRes + 0.5f);
        elevIndex = (int)(((atan2f(IntensityBB_XYZ[2], sqrtf(powf(IntensityBB_XYZ[0], 2.0f) + powf(IntensityBB_XYZ[1], 2.0f)))*180.0f/SAF_PI) + 90.0f) / (float)elevRes + 0.5f);
        idx3d = elevIndex * N_azi + aziIndex;
        
        /* pan omni and sum to non-diffuse stream */
        for(i=0; i<nLS; i++)
            for(j=0; j<lSig; j++)
                lsir_ndiff[i*lSig+j] += gtable[idx3d*nLS+i] * shir_direct[j];
        
        free(shir_direct);
    }
    
    /* Sum the two streams, store */
    pData->lsir = realloc1d(pData->lsir, nLS*lSig*sizeof(float));
    for(i=0; i<nLS; i++){
        for(j=0; j<lSig; j++){
            if(wetDry<1.0f)
                pData->lsir[i*lSig+j] = wetDry * lsir_ndiff[i*lSig+j] + lsir_diff[i*lSig+j];
            else
                pData->lsir[i*lSig+j] = lsir_ndiff[i*lSig+j] + (2.0f-wetDry) * lsir_diff[i*lSig+j];
        }
    }
    
    /* indicate that rendering is complete */
    pData->progress0_1 = 1.0f;
    pData->lsRIR_status = LS_RIR_STATUS_RENDERED;
 
    /* Clean-up */
    free(shir);
    free(shir_pad);
    free(gtable);
    free(sectorCoeffs);
    free(sectorCoeffs_syn);
    free(win);
    if(order>1)
        free(Y_enc);
    free(D_ls);
    saf_rfft_destroy(&hFFT_syn);
    saf_rfft_destroy(&hFFT_anl);
    free(lsir_ndiff);
    free(lsir_diff);
    free(lsir_win);
    free(prev_diff_energy);
    free(prev_diff_intensity);
    free(azim);
    free(elev);
    free(diffs);
    free(ndiffs_sqrt);
    free(diffs_sqrt);
    free(insig_win);
    free(inspec_syn);
    free(inspec_anl);
    free(s_anl);
    free(WXYZ_sec);
    free(z_diff);
    free(z_00);
    free(a_diff);
    free(M_ifft);
    free(M_ifft_fl);
    free(ndiffgains);
    free(ndiffgains_interp);
    free(diffgains_interp);
    free(outspec_ndiff);
    free(outspec_diff);
    free(rir_filt);
}


/* Set Functions */

void hosirrlib_setBroadBandFirstPeakFLAG(void* const hHS, int newState)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    pData->broadBandFirstPeakFLAG = newState;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setWindowLength(void* const hHS, int newValue)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    /* round to nearest multiple of 2 */
    pData->windowLength = newValue % 2 == 0 ? newValue : newValue + 2 - (newValue % 2);
    /* clamp within bounds */
    pData->windowLength = SAF_CLAMP(pData->windowLength, MIN_WINDOW_LENGTH, MAX_WINDOW_LENGTH);
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setWetDryBalance(void* const hHS, float newValue)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    pData->wetDryBalance = newValue;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}


int hosirrlib_setAmbiRIR
(
    void* const hHS,
    const float* const* H,
    int numChannels,
    int numSamples,
    int sampleRate
)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    int i;
    
    /* Check if input is actually in the SHD */
    if(fabsf(sqrtf((float)numChannels)-floorf(sqrtf((float)numChannels)))>0.0001f){
        pData->ambiRIR_status = AMBI_RIR_STATUS_NOT_LOADED;
        pData->ambiRIRorder = -1;
        pData->ambiRIRlength_seconds = pData->ambiRIRlength_samples = 0.0f;
        pData->ambiRIRsampleRate = 0;
        return (int)(pData->ambiRIR_status);
    }
    
    /* if it is, store RIR data */
    pData->ambiRIRorder = SAF_MIN(sqrt(numChannels-1), MAX_SH_ORDER);
    pData->analysisOrder = pData->ambiRIRorder;
    pData->ambiRIRlength_samples = numSamples;
    pData->ambiRIRsampleRate = sampleRate;
    pData->ambiRIRlength_seconds = (float)pData->ambiRIRlength_samples/(float)pData->ambiRIRsampleRate;
    pData->shir = realloc1d(pData->shir, numChannels * numSamples * sizeof(float));
    
    for(i=0; i<numChannels; i++)
        memcpy(&(pData->shir[i*numSamples]), H[i], numSamples * sizeof(float));
    
    /* set FLAGS */
    pData->ambiRIR_status = AMBI_RIR_STATUS_LOADED;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
    
    return (int)(pData->ambiRIR_status);
}

void hosirrlib_setAnalysisOrder(void* const hHS, int newValue)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    pData->analysisOrder = SAF_MIN(SAF_MAX(newValue,1), MAX_SH_ORDER);
    /* FUMA only supports 1st order */
    if(pData->analysisOrder!=ANALYSIS_ORDER_FIRST && pData->chOrdering == CH_FUMA)
        pData->chOrdering = CH_ACN;
    if(pData->analysisOrder!=ANALYSIS_ORDER_FIRST && pData->norm == NORM_FUMA)
        pData->norm = NORM_SN3D;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setLoudspeakerAzi_deg(void* const hHS, int index, float newAzi_deg)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    if(newAzi_deg>180.0f)
        newAzi_deg = -360.0f + newAzi_deg;
    newAzi_deg = SAF_MAX(newAzi_deg, -180.0f);
    newAzi_deg = SAF_MIN(newAzi_deg, 180.0f);
    pData->loudpkrs_dirs_deg[index][0] = newAzi_deg;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setLoudspeakerElev_deg(void* const hHS, int index, float newElev_deg)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    newElev_deg = SAF_MAX(newElev_deg, -90.0f);
    newElev_deg = SAF_MIN(newElev_deg, 90.0f);
    pData->loudpkrs_dirs_deg[index][1] = newElev_deg;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setNumLoudspeakers(void* const hHS, int new_nLoudspeakers)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    pData->nLoudpkrs = new_nLoudspeakers > MAX_NUM_LOUDSPEAKERS ? MAX_NUM_LOUDSPEAKERS : new_nLoudspeakers;
    pData->nLoudpkrs = SAF_MAX(MIN_NUM_LOUDSPEAKERS, pData->nLoudpkrs);
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void  hosirrlib_setOutputConfigPreset(void* const hHS, int newPresetID)
{
    hosirrlib_data *pData = ( hosirrlib_data*)(hHS);
    loadLoudspeakerArrayPreset(newPresetID, pData->loudpkrs_dirs_deg, &(pData->nLoudpkrs));
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setChOrder(void* const hHS, int newOrder)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    if((CH_ORDER)newOrder != CH_FUMA || pData->analysisOrder==ANALYSIS_ORDER_FIRST) /* FUMA only supports 1st order */
        pData->chOrdering = (CH_ORDER)newOrder;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}

void hosirrlib_setNormType(void* const hHS, int newType)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    if((NORM_TYPES)newType != NORM_FUMA || pData->analysisOrder==ANALYSIS_ORDER_FIRST) /* FUMA only supports 1st order */
        pData->norm = (NORM_TYPES)newType;
    pData->lsRIR_status = LS_RIR_STATUS_NOT_RENDERED;
}


/* Get Functions */

float hosirrlib_getProgress0_1(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->progress0_1;
}

void hosirrlib_getProgressText(void* const hHS, char* text)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    memcpy(text, pData->progressText, HOSIRR_PROGRESSTEXT_CHAR_LENGTH*sizeof(char));
}

int hosirrlib_getAmbiRIRstatus(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->ambiRIR_status;
}

int hosirrlib_getLsRIRstatus(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->lsRIR_status;
}

int hosirrlib_getAmbiRIRinputOrder(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->ambiRIRorder;
}

int hosirrlib_getAmbiRIRlength_samples(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->ambiRIRlength_samples;
}

float hosirrlib_getAmbiRIRlength_seconds(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->ambiRIRlength_seconds;
}

int hosirrlib_getAmbiRIRsampleRate(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->ambiRIRsampleRate;
}

int hosirrlib_getBroadBandFirstPeakFLAG(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->broadBandFirstPeakFLAG;
}

void hosirrlib_getLsRIR(void* const hHS, float* const* lsRIR)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    int i;
    if(pData->lsRIR_status == LS_RIR_STATUS_RENDERED)
        for(i=0; i< pData->nLoudpkrs; i++)
            memcpy(lsRIR[i], &(pData->lsir[i*(pData->ambiRIRlength_samples)]), pData->ambiRIRlength_samples*sizeof(float));
}

int hosirrlib_getWindowLength (void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->windowLength;
}

float hosirrlib_getWetDryBalance(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->wetDryBalance;
}

int hosirrlib_getAnalysisOrder(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->analysisOrder;
}

float hosirrlib_getLoudspeakerAzi_deg(void* const hHS, int index)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->loudpkrs_dirs_deg[index][0];
}

float hosirrlib_getLoudspeakerElev_deg(void* const hHS, int index)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->loudpkrs_dirs_deg[index][1];
}

int hosirrlib_getNumLoudspeakers(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return pData->nLoudpkrs;
}

int hosirrlib_getMaxNumLoudspeakers()
{
    return MAX_NUM_LOUDSPEAKERS;
}

int hosirrlib_getChOrder(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return (int)pData->chOrdering;
}

int hosirrlib_getNormType(void* const hHS)
{
    hosirrlib_data *pData = (hosirrlib_data*)(hHS);
    return (int)pData->norm;
}
