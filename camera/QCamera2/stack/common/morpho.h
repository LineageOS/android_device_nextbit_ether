/* Copyright (c) 2017, The LineageOS Project. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __QCAMERA_MORPHO_H__
#define __QCAMERA_MORPHO_H__

// from libmorpho_image_stab4.so
extern int morpho_ImageStabilizer4_addImageEx(int, int);
extern int morpho_ImageStabilizer4_finalize(int);
extern int morpho_ImageStabilizer4_getBufferSize(int, int, int, int);
extern int morpho_ImageStabilizer4_getResult(int);
extern int morpho_ImageStabilizer4_initialize(int, int, int, int, int);
extern int morpho_ImageStabilizer4_setChromaNoiseReductionLevel(int, int);
extern int morpho_ImageStabilizer4_setCoreSetting(int, int, int);
extern int morpho_ImageStabilizer4_setImageFormat(int, int);
extern int morpho_ImageStabilizer4_setInputISO(int, int);
extern int morpho_ImageStabilizer4_setLumaNoiseReductionLevel(int, int);
extern int morpho_ImageStabilizer4_setNumberOfMergeImages(int, int);
extern int morpho_ImageStabilizer4_setSharpnessEnhanceLevel(int, int);
extern int morpho_ImageStabilizer4_startEx(int, int, int);
// from libmorpho_movie_stab4.so
extern int morpho_MovieStabilizer4_finalize(int);
extern int morpho_MovieStabilizer4_getBufferSize(void);
extern int morpho_MovieStabilizer4_getFcode(int, int);
extern int morpho_MovieStabilizer4_initialize(int, int, int);
extern int morpho_MovieStabilizer4_process(int, int, int, int);
extern int morpho_MovieStabilizer4_setDelayFrameNum(int, int);
extern int morpho_MovieStabilizer4_setFixLevel(int, int);
extern int morpho_MovieStabilizer4_setFrameRate(int, int);
extern int morpho_MovieStabilizer4_setImageGyroTimeLag(int, int);
extern int morpho_MovieStabilizer4_setMovingSubjectLevel(int, int);
extern int morpho_MovieStabilizer4_setNoMovementLevel(int, int);
extern int morpho_MovieStabilizer4_setOutputImageSize(int, int, int);
extern int morpho_MovieStabilizer4_setRollingShutterCoeff(int, int);
extern int morpho_MovieStabilizer4_setScanlineOrientation(int, int);
extern int morpho_MovieStabilizer4_setViewAngle(int, int, int, int);
extern int morpho_MovieStabilizer4_start(int);
extern int morpho_MovieStabilizer4_updateGyroData(int, int, int, int);

#endif /* __QCAMERA_MORPHO_H__ */
