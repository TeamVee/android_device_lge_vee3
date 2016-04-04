/*
** Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
** Not a Contribution, Apache license notifications and license are retained
** for attribution purposes only.
** Copyright 2008, The Android Open-Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <math.h>

//#define LOG_NDEBUG 0
#define LOG_TAG "AudioHardware7x27A"
#include <utils/Log.h>
#include <utils/String8.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <cutils/properties.h> // for property_get
// hardware specific functions

extern "C" {
#include "initialize_audcal8x25.h"
}
#include "AudioHardware.h"
#ifdef QCOM_FM_ENABLED
extern "C" {
#include "HardwarePinSwitching.h"
}
#endif
//#include <media/AudioRecord.h>

static int (*audcal_init)();
static void *libaudcal;

#define COMBO_DEVICE_SUPPORTED // Headset speaker combo device supported on this target
#define DUALMIC_KEY "dualmic_enabled"
#define TTY_MODE_KEY "tty_mode"
#define ECHO_SUPRESSION "ec_supported"
#define VOIPRATE_KEY "voip_rate"
namespace android_audio_legacy {

#ifdef SRS_PROCESSING
void*       SRSParamsG = NULL;
void*       SRSParamsW = NULL;
void*       SRSParamsC = NULL;
void*       SRSParamsHP = NULL;
void*       SRSParamsP = NULL;
void*       SRSParamsHL = NULL;

#define SRS_PARAMS_G 1
#define SRS_PARAMS_W 2
#define SRS_PARAMS_C 4
#define SRS_PARAMS_HP 8
#define SRS_PARAMS_P 16
#define SRS_PARAMS_HL 32
#define SRS_PARAMS_ALL 0xFF

#endif /*SRS_PROCESSING*/

static void * acoustic;
const uint32_t AudioHardware::inputSamplingRates[] = {
        8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
};

#ifdef SRS_PROCESSING
static void msm72xx_enable_srs(int flags, bool state);
#endif /*SRS_PROCESSING*/

static int post_proc_feature_mask = 0;
static bool hpcm_playback_in_progress = false;

static int snd_device = -1;

#define PCM_OUT_DEVICE "/dev/msm_pcm_out"
#define PCM_IN_DEVICE "/dev/msm_pcm_in"
#define PCM_CTL_DEVICE "/dev/msm_pcm_ctl"
#define PREPROC_CTL_DEVICE "/dev/msm_preproc_ctl"
#define VOICE_MEMO_DEVICE "/dev/msm_voicememo"
#ifdef QCOM_FM_ENABLED
#define FM_DEVICE  "/dev/msm_fm"
#endif
#define BTHEADSET_VGS "bt_headset_vgs"

/*SND Devices*/
static uint32_t SND_DEVICE_CURRENT = -1;
static uint32_t SND_DEVICE_HANDSET = 0x0;
static uint32_t SND_DEVICE_HEADSET = 0x3;
static uint32_t SND_DEVICE_SPEAKER = 0x6;
static uint32_t SND_DEVICE_TTY_HEADSET = 0x8;
static uint32_t SND_DEVICE_TTY_VCO = 0x9;
static uint32_t SND_DEVICE_TTY_HCO = 0xA;
static uint32_t SND_DEVICE_BT = 0xC;
static uint32_t SND_DEVICE_IN_S_SADC_OUT_HANDSET = 0x10;
static uint32_t SND_DEVICE_IN_S_SADC_OUT_SPEAKER_PHONE = 0x19;
static uint32_t SND_DEVICE_FM_DIGITAL_STEREO_HEADSET = 0x1A;
static uint32_t SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE = 0x1B;
static uint32_t SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET = 0x1C;
static uint32_t SND_DEVICE_STEREO_HEADSET_AND_SPEAKER = 0x1F;
static uint32_t SND_DEVICE_FM_ANALOG_STEREO_HEADSET = 0x23;
static uint32_t SND_DEVICE_FM_ANALOG_STEREO_HEADSET_CODEC = 0x24;

/*CAD Devices*/
static uint32_t CAD_HW_DEVICE_ID_NONE                 = -1;
static uint32_t CAD_HW_DEVICE_ID_HANDSET_SPKR         = -1;
static uint32_t CAD_HW_DEVICE_ID_HANDSET_MIC          = -1;
static uint32_t CAD_HW_DEVICE_ID_HEADSET_MIC          = -1;
static uint32_t CAD_HW_DEVICE_ID_HEADSET_SPKR_MONO    = -1;
static uint32_t CAD_HW_DEVICE_ID_HEADSET_SPKR_STEREO  = -1;
static uint32_t CAD_HW_DEVICE_ID_SPEAKER_PHONE_MIC    = -1;
static uint32_t CAD_HW_DEVICE_ID_SPEAKER_PHONE_MONO   = -1;
static uint32_t CAD_HW_DEVICE_ID_SPEAKER_PHONE_STEREO = -1;
static uint32_t CAD_HW_DEVICE_ID_BT_SCO_MIC           = -1;
static uint32_t CAD_HW_DEVICE_ID_BT_SCO_SPKR          = -1;
static uint32_t CAD_HW_DEVICE_ID_BT_A2DP_SPKR         = -1;
static uint32_t CAD_HW_DEVICE_ID_TTY_HEADSET_MIC      = -1;
static uint32_t CAD_HW_DEVICE_ID_TTY_HEADSET_SPKR     = -1;
static uint32_t CAD_HW_DEVICE_ID_HEADSET_STEREO_PLUS_SPKR_MONO_RX     = -1;
static uint32_t CAD_HW_DEVICE_ID_LP_FM_HEADSET_SPKR_STEREO_RX         = -1;
static uint32_t CAD_HW_DEVICE_ID_I2S_RX                               = -1;
static uint32_t CAD_HW_DEVICE_ID_SPEAKER_PHONE_MIC_ENDFIRE            = -1;
static uint32_t CAD_HW_DEVICE_ID_HANDSET_MIC_ENDFIRE                  = -1;
static uint32_t CAD_HW_DEVICE_ID_I2S_TX                               = -1;
static uint32_t CAD_HW_DEVICE_ID_LP_FM_HEADSET_SPKR_STEREO_PLUS_HEADSET_SPKR_STEREO_RX = -1;
static uint32_t CAD_HW_DEVICE_ID_FM_DIGITAL_HEADSET_SPKR_STEREO = -1;
static uint32_t CAD_HW_DEVICE_ID_FM_DIGITAL_SPEAKER_PHONE_MONO = -1;
static uint32_t CAD_HW_DEVICE_ID_FM_DIGITAL_SPEAKER_PHONE_MIC= -1;
static uint32_t CAD_HW_DEVICE_ID_FM_DIGITAL_BT_A2DP_SPKR = -1;
static uint32_t CAD_HW_DEVICE_ID_MAX                     = -1;

static uint32_t CAD_HW_DEVICE_ID_CURRENT_RX = -1;
static uint32_t CAD_HW_DEVICE_ID_CURRENT_TX = -1;
// ----------------------------------------------------------------------------

AudioHardware::AudioHardware() :
    mInit(false), mMicMute(true),
#ifdef QCOM_FM_ENABLED
    mFmFd(-1),
#endif
    mBluetoothNrec(true), mBluetoothVGS(false), mBluetoothId(0),
    mOutput(0),
    mCurSndDevice(-1), mCadEndpoints(NULL),
    mTtyMode(TTY_OFF)
#ifdef QCOM_FM_ENABLED
    ,FmA2dpStatus(-1)
#endif
{
    m7xsnddriverfd = open("/dev/msm_cad", O_RDWR);
    if (m7xsnddriverfd >= 0) {
        int rc = ioctl(m7xsnddriverfd, CAD_GET_NUM_ENDPOINTS, &mNumCadEndpoints);
        if (rc >= 0) {
            mCadEndpoints = new msm_cad_endpoint[mNumCadEndpoints];
            mInit = true;
            ALOGV("constructed (%d SND endpoints)", rc);
            struct msm_cad_endpoint *ept = mCadEndpoints;
            for (int cnt = 0; cnt < mNumCadEndpoints; cnt++, ept++) {
                ept->id = cnt;
                ioctl(m7xsnddriverfd, CAD_GET_ENDPOINT, ept);
                ALOGV("cnt = %d ept->name = %s ept->id = %d\n", cnt, ept->name, ept->id);
#define CHECK_FOR(desc) if (!strcmp(ept->name, #desc)) CAD_HW_DEVICE_ID_##desc = ept->id;
                CHECK_FOR(NONE);
                CHECK_FOR(HANDSET_SPKR);
                CHECK_FOR(HANDSET_MIC);
                CHECK_FOR(HEADSET_MIC);
                CHECK_FOR(HEADSET_SPKR_MONO);
                CHECK_FOR(HEADSET_SPKR_STEREO);
                CHECK_FOR(SPEAKER_PHONE_MIC);
                CHECK_FOR(SPEAKER_PHONE_MONO);
                CHECK_FOR(SPEAKER_PHONE_STEREO);
                CHECK_FOR(BT_SCO_MIC);
                CHECK_FOR(BT_SCO_SPKR);
                CHECK_FOR(BT_A2DP_SPKR);
                CHECK_FOR(TTY_HEADSET_MIC);
                CHECK_FOR(TTY_HEADSET_SPKR);
                CHECK_FOR(HEADSET_STEREO_PLUS_SPKR_MONO_RX);
                CHECK_FOR(I2S_RX);
                CHECK_FOR(SPEAKER_PHONE_MIC_ENDFIRE);
                CHECK_FOR(HANDSET_MIC_ENDFIRE);
                CHECK_FOR(I2S_TX);
#ifdef QCOM_FM_ENABLED
                CHECK_FOR(LP_FM_HEADSET_SPKR_STEREO_RX);
                CHECK_FOR(LP_FM_HEADSET_SPKR_STEREO_PLUS_HEADSET_SPKR_STEREO_RX);
                CHECK_FOR(FM_DIGITAL_HEADSET_SPKR_STEREO);
                CHECK_FOR(FM_DIGITAL_SPEAKER_PHONE_MONO);
                CHECK_FOR(FM_DIGITAL_SPEAKER_PHONE_MIC);
                CHECK_FOR(FM_DIGITAL_BT_A2DP_SPKR);
#endif
#undef CHECK_FOR
            }
        }
        else ALOGE("Could not retrieve number of MSM SND endpoints.");
    } else
        ALOGE("Could not open MSM SND driver.");

    libaudcal = ::dlopen("libaudcal.so", RTLD_NOW);
    if (libaudcal == NULL) {
       ALOGE("DLOPEN not successful for libaudcal");
    } else {
       ALOGD("DLOPEN successful for libaudcal");
       audcal_init = (int (*)())::dlsym(libaudcal,"audcal_initialize");
       if (audcal_init == NULL) {
           ALOGE("dlsym:Error:%s Loading audcal_initialize", dlerror());
        } else {
           audcal_init();
    }
}

    char fluence_key[PROPERTY_VALUE_MAX] = "none";
    property_get("ro.qc.sdk.audio.fluencetype",fluence_key,"0");

    if (0 == strncmp("fluencepro", fluence_key, sizeof("fluencepro"))) {
       ALOGE("FluencePro quadMic feature not supported");
    } else if (0 == strncmp("fluence", fluence_key, sizeof("fluence"))) {
       mDualMicEnabled = true;
       ALOGV("Fluence dualmic feature Enabled");
    } else {
       mDualMicEnabled = false;
       ALOGV("Fluence feature Disabled");
    }
}

AudioHardware::~AudioHardware()
{
    for (size_t index = 0; index < mInputs.size(); index++) {
        closeInputStream((AudioStreamIn*)mInputs[index]);
    }
    mInputs.clear();
    closeOutputStream((AudioStreamOut*)mOutput);
    delete [] mCadEndpoints;
    if (acoustic) {
        ::dlclose(acoustic);
        acoustic = 0;
    }
    if (m7xsnddriverfd > 0)
    {
      close(m7xsnddriverfd);
      m7xsnddriverfd = -1;
    }
    mInit = false;
}

status_t AudioHardware::initCheck()
{
    return mInit ? NO_ERROR : NO_INIT;
}

// default implementation calls its "without flags" counterpart
AudioStreamOut* AudioHardware::openOutputStreamWithFlags(uint32_t devices,
                                          audio_output_flags_t flags,
                                          int *format,
                                          uint32_t *channels,
                                          uint32_t *sampleRate,
                                          status_t *status)
{
    return openOutputStream(devices, format, channels, sampleRate, status);
}

AudioStreamOut* AudioHardware::openOutputStream(
        uint32_t devices,  int *format, uint32_t *channels,
        uint32_t *sampleRate, status_t *status)
{
     ALOGD("AudioHardware::openOutputStream devices %x format %d channels %d samplerate %d",
        devices, *format, *channels, *sampleRate);

     audio_output_flags_t flags = static_cast<audio_output_flags_t> (*status);

     if (!audio_is_output_device(devices))
        return 0;

    { // scope for the lock
        Mutex::Autolock lock(mLock);

        ALOGV(" AudioHardware::openOutputStream AudioStreamOutMSM72xx output stream \n");
        // only one output stream allowed
        if (mOutput) {
            if (status) {
                *status = INVALID_OPERATION;
            }
            ALOGE(" AudioHardware::openOutputStream Only one output stream allowed \n");
            return 0;
        }

        // create new output stream
        AudioStreamOutMSM72xx* out = new AudioStreamOutMSM72xx();
        status_t lStatus = out->set(this, devices, format, channels, sampleRate);
        if (status) {
            *status = lStatus;
        }
        if (lStatus == NO_ERROR) {
            mOutput = out;
        } else {
            delete out;
        }
        return mOutput;
    }
}

void AudioHardware::closeOutputStream(AudioStreamOut* out) {
    Mutex::Autolock lock(mLock);
    if ((mOutput == 0) || ((mOutput != out)))
        ALOGW("Attempt to close invalid output stream");
    else {
        delete mOutput;
        mOutput = 0;
    }
}

AudioStreamIn* AudioHardware::openInputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate, status_t *status,
        AudioSystem::audio_in_acoustics acoustic_flags)
{
    // check for valid input source
    ALOGD("AudioHardware::openInputStream devices %x format %d channels %d samplerate %d",
        devices, *format, *channels, *sampleRate);

    if (!audio_is_input_device(devices))
        return 0;

    mLock.lock();

    AudioStreamInMSM72xx* in72xx = new AudioStreamInMSM72xx();
    status_t lStatus = in72xx->set(this, devices, format, channels, sampleRate, acoustic_flags);
    if (status) {
        *status = lStatus;
    }
    if (lStatus != NO_ERROR) {
        ALOGE("Error creating Audio stream AudioStreamInMSM72xx \n");
        mLock.unlock();
        delete in72xx;
        return 0;
    }
    mInputs.add(in72xx);
    mLock.unlock();
    return in72xx;
}

void AudioHardware::closeInputStream(AudioStreamIn* in) {
    Mutex::Autolock lock(mLock);

    ssize_t index = mInputs.indexOf((AudioStreamInMSM72xx *)in);
    if (index >= 0) {
        ALOGV("closeInputStream AudioStreamInMSM72xx");
        mLock.unlock();
        delete mInputs[index];
        mLock.lock();
        mInputs.removeAt(index);
    } else {
        ALOGE("Attempt to close invalid input stream");
     }
}

status_t AudioHardware::setMode(int mode)
{
    status_t status = AudioHardwareBase::setMode(mode);
    if (status == NO_ERROR) {
        // make sure that doAudioRouteOrMute() is called by doRouting()
        // even if the new device selected is the same as current one.
        clearCurDevice();
    }
    return status;
}

status_t AudioHardware::setMasterMute(bool muted) {
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

int AudioHardware::createAudioPatch(unsigned int num_sources,
        const struct audio_port_config *sources,
        unsigned int num_sinks,
        const struct audio_port_config *sinks,
        audio_patch_handle_t *handle) {
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

int AudioHardware::releaseAudioPatch(audio_patch_handle_t handle) {
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

int AudioHardware::getAudioPort(struct audio_port *port) {
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

int AudioHardware::setAudioPortConfig(
        const struct audio_port_config *config) {
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

bool AudioHardware::checkOutputStandby()
{
    if (mOutput)
        if (!mOutput->checkStandby())
            return false;

    return true;
}

status_t AudioHardware::setMicMute(bool state)
{
    Mutex::Autolock lock(mLock);
    return setMicMute_nosync(state);
}

// always call with mutex held
status_t AudioHardware::setMicMute_nosync(bool state)
{
    if (mMicMute != state) {
        mMicMute = state;
        ALOGD("setMicMute_nosync calling voice mute with the mMicMute %d", mMicMute);
        msm_set_voice_tx_mute(mMicMute);
    }
    return NO_ERROR;
}

status_t AudioHardware::getMicMute(bool* state)
{
    *state = mMicMute;
    return NO_ERROR;
}

status_t AudioHardware::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    String8 key;

    const char BT_NREC_KEY[] = "bt_headset_nrec";
    const char BT_NAME_KEY[] = "bt_headset_name";
    const char BT_NREC_VALUE_ON[] = "on";
#ifdef SRS_PROCESSING
    int to_set=0;
    ALOGV("setParameters() %s", keyValuePairs.string());
    if(strncmp("SRS_Buffer", keyValuePairs.string(), 10) == 0) {
        int SRSptr = 0;
        String8 keySRSG  = String8("SRS_BufferG"), keySRSW  = String8("SRS_BufferW"),
          keySRSC  = String8("SRS_BufferC"), keySRSHP = String8("SRS_BufferHP"),
          keySRSP  = String8("SRS_BufferP"), keySRSHL = String8("SRS_BufferHL");
        if (param.getInt(keySRSG, SRSptr) == NO_ERROR) {
            SRSParamsG = (void*)SRSptr;
            to_set |= SRS_PARAMS_G;
        } else if (param.getInt(keySRSW, SRSptr) == NO_ERROR) {
            SRSParamsW = (void*)SRSptr;
            to_set |= SRS_PARAMS_W;
        } else if (param.getInt(keySRSC, SRSptr) == NO_ERROR) {
            SRSParamsC = (void*)SRSptr;
            to_set |= SRS_PARAMS_C;
        } else if (param.getInt(keySRSHP, SRSptr) == NO_ERROR) {
            SRSParamsHP = (void*)SRSptr;
            to_set |= SRS_PARAMS_HP;
        } else if (param.getInt(keySRSP, SRSptr) == NO_ERROR) {
            SRSParamsP = (void*)SRSptr;
            to_set |= SRS_PARAMS_P;
        } else if (param.getInt(keySRSHL, SRSptr) == NO_ERROR) {
            SRSParamsHL = (void*)SRSptr;
            to_set |= SRS_PARAMS_HL;
        }

        ALOGD("SetParam SRS flags=0x%x", to_set);

        if (hpcm_playback_in_progress)
            msm72xx_enable_srs(to_set, true);

        if (SRSptr)
            return NO_ERROR;

    }
#endif /*SRS_PROCESSING*/
    if (keyValuePairs.length() == 0) return BAD_VALUE;

    key = String8(BT_NREC_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == BT_NREC_VALUE_ON) {
            mBluetoothNrec = true;
        } else {
            mBluetoothNrec = false;
            ALOGI("Turning noise reduction and echo cancellation off for BT "
                 "headset");
        }
    }
    key = String8(BTHEADSET_VGS);
    if (param.get(key, value) == NO_ERROR) {
        if (value == BT_NREC_VALUE_ON) {
            mBluetoothVGS = true;
        } else {
            mBluetoothVGS = false;
        }
    }
    key = String8(BT_NAME_KEY);
    if (param.get(key, value) == NO_ERROR) {
        mBluetoothId = 0;
        for (int i = 0; i < mNumCadEndpoints; i++) {
            if (!strcasecmp(value.string(), mCadEndpoints[i].name)) {
                mBluetoothId = mCadEndpoints[i].id;
                ALOGI("Using custom acoustic parameters for %s", value.string());
                break;
            }
        }
        if (mBluetoothId == 0) {
            ALOGI("Using default acoustic parameters "
                 "(%s not in acoustic database)", value.string());
            doRouting(NULL);
        }
    }

    key = String8(DUALMIC_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == "true") {
            mDualMicEnabled = true;
            ALOGI("DualMike feature Enabled");
        } else {
            mDualMicEnabled = false;
            ALOGI("DualMike feature Disabled");
        }
        doRouting(NULL, 0);
    }

    key = String8(TTY_MODE_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == "full") {
            mTtyMode = TTY_FULL;
        } else if (value == "hco") {
            mTtyMode = TTY_HCO;
        } else if (value == "vco") {
            mTtyMode = TTY_VCO;
        } else {
            mTtyMode = TTY_OFF;
        }
        if(mMode != AUDIO_MODE_IN_CALL){
           return NO_ERROR;
        }
        ALOGI("Changed TTY Mode=%s", value.string());
        if((mMode == AUDIO_MODE_IN_CALL) &&
           (mCurSndDevice == SND_DEVICE_HEADSET))
           doRouting(NULL, 0);
    }
    return NO_ERROR;
}

String8 AudioHardware::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;

    String8 key = String8(DUALMIC_KEY);

    if (param.get(key, value) == NO_ERROR) {
        value = String8(mDualMicEnabled ? "true" : "false");
        param.add(key, value);
    }

    key = String8(BTHEADSET_VGS);
    if (param.get(key, value) == NO_ERROR) {
        if(mBluetoothVGS)
           param.addInt(String8("isVGS"), true);
    }

    key = String8(AUDIO_PARAMETER_KEY_FLUENCE_TYPE);
    if (param.get(key, value) == NO_ERROR) {
       if (mDualMicEnabled) {
            value = String8("fluence");
            param.add(key, value);
       } else {
            value = String8("none");
            param.add(key, value);
       }
    }

    ALOGV("AudioHardware::getParameters() %s", param.toString().string());
    return param.toString();
}


#ifdef SRS_PROCESSING
static void msm72xx_enable_srs(int flags, bool state)
{
    int fd = open(PCM_CTL_DEVICE, O_RDWR);
    if (fd < 0) {
        ALOGE("Cannot open PCM Ctl device for srs params");
        return;
    }

    ALOGD("Enable SRS flags=0x%x state= %d",flags,state);
    if (state == false) {
        if(post_proc_feature_mask & SRS_ENABLE) {
            post_proc_feature_mask &= SRS_DISABLE;
        }
        if(SRSParamsG) {
            unsigned short int backup = ((unsigned short int*)SRSParamsG)[2];
            ((unsigned short int*)SRSParamsG)[2] = 0;
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsG);
            ((unsigned short int*)SRSParamsG)[2] = backup;
        }
    } else {
        post_proc_feature_mask |= SRS_ENABLE;
        if(SRSParamsW && (flags & SRS_PARAMS_W))
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsW);
        if(SRSParamsC && (flags & SRS_PARAMS_C))
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsC);
        if(SRSParamsHP && (flags & SRS_PARAMS_HP))
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsHP);
        if(SRSParamsP && (flags & SRS_PARAMS_P))
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsP);
        if(SRSParamsHL && (flags & SRS_PARAMS_HL))
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsHL);
        if(SRSParamsG && (flags & SRS_PARAMS_G))
            ioctl(fd, AUDIO_SET_SRS_TRUMEDIA_PARAM, SRSParamsG);
    }

    if (ioctl(fd, AUDIO_ENABLE_AUDPP, &post_proc_feature_mask) < 0) {
        ALOGE("enable audpp error");
    }

    close(fd);
}

#endif /*SRS_PROCESSING*/

size_t AudioHardware::getInputBufferSize(uint32_t sampleRate, int format, int channelCount)
{
    ALOGD("AudioHardware::getInputBufferSize sampleRate %d format %d channelCount %d"
            ,sampleRate, format, channelCount);
    if ( (format != AUDIO_FORMAT_PCM_16_BIT) &&
         (format != AUDIO_FORMAT_AAC)) {
        ALOGW("getInputBufferSize bad format: 0x%x", format);
        return 0;
    }
    if (channelCount < 1 || channelCount > 2) {
        ALOGW("getInputBufferSize bad channel count: %d", channelCount);
        return 0;
    }

    if (format == AUDIO_FORMAT_AAC)
       return 2048;
    else
       return 2048*channelCount;
}

static status_t set_volume_rpc(uint32_t rx_device,
                               uint32_t tx_device,
                               uint32_t method,
                               uint32_t volume,
                               int m7xsnddriverfd)
{

    ALOGD("rpc_snd_set_volume(%d, %d, %d, %d)\n", rx_device, tx_device, method, volume);
    if (rx_device == -1UL && tx_device == -1UL) return NO_ERROR;

    if (m7xsnddriverfd < 0) {
        ALOGE("Can not open snd device");
        return -EPERM;
    }
    /* rpc_snd_set_volume(
     *     device,            # Any hardware device enum, including
     *                        # SND_DEVICE_CURRENT
     *     method,            # must be SND_METHOD_VOICE to do anything useful
     *     volume,            # integer volume level, in range [0,5].
     *                        # note that 0 is audible (not quite muted)
     *  )
     * rpc_snd_set_volume only works for in-call sound volume.
     */
     struct msm_cad_volume_config args;
     args.device.rx_device = rx_device;
     args.device.tx_device = tx_device;
     args.method = method;
     args.volume = volume;

     if (ioctl(m7xsnddriverfd, CAD_SET_VOLUME, &args) < 0) {
         ALOGE("snd_set_volume error.");
         return -EIO;
     }
     return NO_ERROR;
}

status_t AudioHardware::setVoiceVolume(float v)
{
    if (v < 0.0) {
        ALOGW("setVoiceVolume(%f) under 0.0, assuming 0.0\n", v);
        v = 0.0;
    } else if (v > 1.0) {
        ALOGW("setVoiceVolume(%f) over 1.0, assuming 1.0\n", v);
        v = 1.0;
    }
    // Added 0.4 to current volume, as in voice call Mute cannot be set as minimum volume(0.00)
    // setting Rx volume level as 2 for minimum and 7 as max level.
    v = 0.4 + v;

    int vol = lrint(v * 5.0);
    ALOGD("setVoiceVolume(%f)\n", v);
    ALOGI("Setting in-call volume to %d (available range is 2 to 7)\n", vol);

    if ((mCurSndDevice != -1) && ((mCurSndDevice == SND_DEVICE_TTY_HEADSET) || (mCurSndDevice == SND_DEVICE_TTY_VCO)))
    {
        vol = 1;
        ALOGI("For TTY device in FULL or VCO mode, the volume level is set to: %d \n", vol);
    }

    Mutex::Autolock lock(mLock);
    set_volume_rpc(CAD_HW_DEVICE_ID_CURRENT_RX, CAD_HW_DEVICE_ID_CURRENT_TX, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    return NO_ERROR;
}

#ifdef QCOM_FM_ENABLED
status_t AudioHardware::setFmVolume(float v)
{
    if (v < 0.0) {
        ALOGW("setFmVolume(%f) under 0.0, assuming 0.0\n", v);
        v = 0.0;
    } else if (v > 1.0) {
        ALOGW("setFmVolume(%f) over 1.0, assuming 1.0\n", v);
        v = 1.0;
    }

    int vol = lrint(v * 7.5);
    if (vol > 7)
        vol = 7;
    ALOGD("setFmVolume(%f)\n", v);
    Mutex::Autolock lock(mLock);
    set_volume_rpc(CAD_HW_DEVICE_ID_CURRENT_RX, CAD_HW_DEVICE_ID_CURRENT_TX, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    return NO_ERROR;
}
#endif

status_t AudioHardware::setMasterVolume(float v)
{
    Mutex::Autolock lock(mLock);
    int vol = ceil(v * 7.0);
    ALOGI("Set master volume to %d.\n", vol);
    set_volume_rpc(CAD_HW_DEVICE_ID_HANDSET_SPKR, CAD_HW_DEVICE_ID_HANDSET_MIC, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_SPEAKER_PHONE_MONO, CAD_HW_DEVICE_ID_SPEAKER_PHONE_MIC, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_BT_SCO_SPKR, CAD_HW_DEVICE_ID_BT_SCO_MIC, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_HEADSET_SPKR_STEREO, CAD_HW_DEVICE_ID_HEADSET_MIC, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_HANDSET_SPKR, CAD_HW_DEVICE_ID_HANDSET_MIC_ENDFIRE, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_SPEAKER_PHONE_MONO, CAD_HW_DEVICE_ID_SPEAKER_PHONE_MIC_ENDFIRE, SND_METHOD_VOICE, vol, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_TTY_HEADSET_SPKR, CAD_HW_DEVICE_ID_TTY_HEADSET_MIC, SND_METHOD_VOICE, 1, m7xsnddriverfd);
    set_volume_rpc(CAD_HW_DEVICE_ID_TTY_HEADSET_SPKR, CAD_HW_DEVICE_ID_HANDSET_MIC, SND_METHOD_VOICE, 1, m7xsnddriverfd);
    // We return an error code here to let the audioflinger do in-software
    // volume on top of the maximum volume that we set through the SND API.
    // return error - software mixer will handle it
    return -1;
}

static status_t do_route_audio_rpc(uint32_t device,
                                   bool ear_mute, bool mic_mute, int m7xsnddriverfd)
{

    ALOGW("rpc_snd_set_device(%d, %d, %d)\n", device, ear_mute, mic_mute);

    if (m7xsnddriverfd < 0) {
        ALOGE("Can not open snd device");
        return -EPERM;
    }
    // RPC call to switch audio path
    /* rpc_snd_set_device(
     *     device,            # Hardware device enum to use
     *     ear_mute,          # Set mute for outgoing voice audio
     *                        # this should only be unmuted when in-call
     *     mic_mute,          # Set mute for incoming voice audio
     *                        # this should only be unmuted when in-call or
     *                        # recording.
     *  )
     */
    struct msm_cad_device_config args;
    args.ear_mute = ear_mute ? SND_MUTE_MUTED : SND_MUTE_UNMUTED;
    args.device.pathtype = CAD_DEVICE_PATH_RX_TX;
    args.device.rx_device = -1;
    args.device.tx_device = -1;

    if(device == SND_DEVICE_HANDSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HANDSET_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_HANDSET_MIC;
        ALOGV("In HANDSET");
    }
    else if(device == SND_DEVICE_SPEAKER) {
        args.device.rx_device = CAD_HW_DEVICE_ID_SPEAKER_PHONE_MONO;
        args.device.tx_device = CAD_HW_DEVICE_ID_SPEAKER_PHONE_MIC;
        ALOGV("In SPEAKER");
    }
    else if(device == SND_DEVICE_HEADSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HEADSET_SPKR_MONO;
        args.device.tx_device = CAD_HW_DEVICE_ID_HEADSET_MIC;
        ALOGV("In HEADSET");
    }
    else if(device == SND_DEVICE_HEADSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HEADSET_SPKR_STEREO;
        args.device.tx_device = CAD_HW_DEVICE_ID_HEADSET_MIC;
        ALOGV("In HEADSET STEREO");
    }
    else if(device == SND_DEVICE_HEADSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HEADSET_SPKR_STEREO;
        args.device.tx_device = CAD_HW_DEVICE_ID_NONE;
        ALOGV("In HEADSET STEREO WITHOUT MIC");
    }
    else if(device == SND_DEVICE_IN_S_SADC_OUT_HANDSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HANDSET_SPKR;
	args.device.tx_device = CAD_HW_DEVICE_ID_HANDSET_MIC_ENDFIRE;
        ALOGV("In DUALMIC_HANDSET");
    }
    else if(device == SND_DEVICE_IN_S_SADC_OUT_SPEAKER_PHONE) {
        args.device.rx_device = CAD_HW_DEVICE_ID_SPEAKER_PHONE_MONO;
        args.device.tx_device = CAD_HW_DEVICE_ID_SPEAKER_PHONE_MIC_ENDFIRE;
        ALOGV("In DUALMIC_SPEAKER");
    }
    else if(device == SND_DEVICE_TTY_HEADSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_TTY_HEADSET_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_TTY_HEADSET_MIC;
        ALOGV("In TTY_FULL");
    }
    else if(device == SND_DEVICE_TTY_VCO) {
        args.device.rx_device = CAD_HW_DEVICE_ID_TTY_HEADSET_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_HANDSET_MIC;
        ALOGV("In TTY_VCO");
    }
    else if(device == SND_DEVICE_TTY_HCO) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HANDSET_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_TTY_HEADSET_MIC;
        ALOGV("In TTY_HCO");
    }
    else if(device == SND_DEVICE_BT) {
        args.device.rx_device = CAD_HW_DEVICE_ID_BT_SCO_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_BT_SCO_MIC;
        ALOGV("In BT_HCO");
    }
    else if(device == SND_DEVICE_BT) {
        args.device.rx_device = CAD_HW_DEVICE_ID_BT_A2DP_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_NONE;
        ALOGV("In BT");
    }
    else if(device == SND_DEVICE_STEREO_HEADSET_AND_SPEAKER) {
        args.device.rx_device = CAD_HW_DEVICE_ID_HEADSET_STEREO_PLUS_SPKR_MONO_RX;
        args.device.tx_device = CAD_HW_DEVICE_ID_HEADSET_MIC;
        ALOGV("In DEVICE_SPEAKER_HEADSET_AND_SPEAKER");
    }
    else if(device == SND_DEVICE_FM_ANALOG_STEREO_HEADSET_CODEC) {
        args.device.rx_device = CAD_HW_DEVICE_ID_LP_FM_HEADSET_SPKR_STEREO_PLUS_HEADSET_SPKR_STEREO_RX;
        args.device.tx_device = CAD_HW_DEVICE_ID_NONE;
        ALOGV("In SND_DEVICE_FM_ANALOG_STEREO_HEADSET_CODEC");
    }
    else if(device == SND_DEVICE_FM_ANALOG_STEREO_HEADSET) {
        args.device.pathtype = CAD_DEVICE_PATH_LB;
        args.device.rx_device = CAD_HW_DEVICE_ID_LP_FM_HEADSET_SPKR_STEREO_RX;
        args.device.tx_device = CAD_HW_DEVICE_ID_NONE;
        ALOGV("In SND_DEVICE_FM_ANALOG_STEREO_HEADSET");
    }
    else if (device == SND_DEVICE_FM_DIGITAL_STEREO_HEADSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_FM_DIGITAL_HEADSET_SPKR_STEREO;
        args.device.tx_device = CAD_HW_DEVICE_ID_NONE;
        ALOGV("In SND_DEVICE_FM_DIGITAL_STEREO_HEADSET");
    }
    else if (device == SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE) {
        args.device.rx_device = CAD_HW_DEVICE_ID_FM_DIGITAL_SPEAKER_PHONE_MONO;
        args.device.tx_device = CAD_HW_DEVICE_ID_FM_DIGITAL_SPEAKER_PHONE_MIC;
        ALOGV("In SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE");
    }
    else if (device == SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET) {
        args.device.rx_device = CAD_HW_DEVICE_ID_FM_DIGITAL_BT_A2DP_SPKR;
        args.device.tx_device = CAD_HW_DEVICE_ID_NONE;
        ALOGV("In SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET");
    }
    else if(device == SND_DEVICE_CURRENT)
    {
        args.device.rx_device = CAD_HW_DEVICE_ID_CURRENT_RX;
        args.device.tx_device = CAD_HW_DEVICE_ID_CURRENT_TX;
        ALOGV("In SND_DEVICE_CURRENT");
    }
    ALOGW("rpc_snd_set_device(%d, %d, %d, %d)\n", args.device.rx_device, args.device.tx_device, ear_mute, mic_mute);

    if(args.device.rx_device == -1 || args.device.tx_device == -1) {
	   ALOGE("Error in setting rx and tx device");
           return -1;
    }

    CAD_HW_DEVICE_ID_CURRENT_RX = args.device.rx_device;
    CAD_HW_DEVICE_ID_CURRENT_TX = args.device.tx_device;
    if((device != SND_DEVICE_CURRENT) && (!mic_mute)
#ifdef QCOM_FM_ENABLED
      &&(device != SND_DEVICE_FM_DIGITAL_STEREO_HEADSET)
      &&(device != SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE)
      &&(device != SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET)
#endif
       ) {
        //Explicitly mute the mic to release DSP resources
        args.mic_mute = SND_MUTE_MUTED;
        if (ioctl(m7xsnddriverfd, CAD_SET_DEVICE, &args) < 0) {
            ALOGE("snd_set_device error.");
            return -EIO;
        }
    }
    args.mic_mute = mic_mute ? SND_MUTE_MUTED : SND_MUTE_UNMUTED;
    if (ioctl(m7xsnddriverfd, CAD_SET_DEVICE, &args) < 0) {
        ALOGE("snd_set_device error.");
        return -EIO;
    }

    return NO_ERROR;
}

// always call with mutex held
status_t AudioHardware::doAudioRouteOrMute(uint32_t device)
{
    int rc;
    int nEarmute=true;

#ifdef QCOM_FM_ENABLED
    if(IsFmon()){
        /* FM needs both Rx path and Tx path to be unmuted */
        nEarmute = false;
        mMicMute = false;
    } else
#endif
    if (mMode == AUDIO_MODE_IN_CALL)
        nEarmute = false;
    rc = do_route_audio_rpc(device,
                              nEarmute , mMicMute, m7xsnddriverfd);
#ifdef QCOM_FM_ENABLED
    if ((
        (device == SND_DEVICE_FM_DIGITAL_STEREO_HEADSET) ||
        (device == SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE)  ||
        (device == SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET)) &&
        (device != mCurSndDevice)) {
        ALOGV("doAudioRouteOrMute():switch to FM mode");
        switch_mode(MODE_FM);
    } else if (((mCurSndDevice == SND_DEVICE_FM_DIGITAL_STEREO_HEADSET) ||
        (mCurSndDevice == SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE)  ||
        (mCurSndDevice == SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET)) &&
        (device != mCurSndDevice)) {
        ALOGV("doAudioRouteOrMute():switch to AUX PCM mode");
        switch_mode(MODE_BTSCO);
    }
#endif
    return rc;
}

#ifdef QCOM_FM_ENABLED
bool AudioHardware::isFMAnalog()
{
    char value[PROPERTY_VALUE_MAX];
    bool isAfm = false;

    if (property_get("hw.fm.isAnalog", value, NULL)
    && !strcasecmp(value, "true")){
        isAfm = true;
    }

    return isAfm;
}
#endif
status_t AudioHardware::doRouting(AudioStreamInMSM72xx *input, uint32_t outputDevices)
{
    /* currently this code doesn't work without the htc libacoustic */

    Mutex::Autolock lock(mLock);
    status_t ret = NO_ERROR;
    int new_snd_device = -1;
#ifdef QCOM_FM_ENABLED
    bool enableDgtlFmDriver = false;
#endif

    if (!outputDevices)
        outputDevices = mOutput->devices();

    ALOGD("outputDevices = %x", outputDevices);

    //int (*msm72xx_enable_audpp)(int);
    //msm72xx_enable_audpp = (int (*)(int))::dlsym(acoustic, "msm72xx_enable_audpp");

    if (input != NULL) {
        uint32_t inputDevice = input->devices();
        ALOGI("do input routing device %x\n", inputDevice);
        // ignore routing device information when we start a recording in voice
        // call
        // Recording will happen through currently active tx device
        if(inputDevice == AUDIO_DEVICE_IN_VOICE_CALL)
            return NO_ERROR;
        if (inputDevice != 0) {
            if (outputDevices &
                   (AUDIO_DEVICE_OUT_BLUETOOTH_SCO | AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET)) {
                    ALOGI("Routing audio to Bluetooth PCM\n");
                    new_snd_device = SND_DEVICE_BT;
            } else if (outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET) {
                    ALOGI("Routing audio to Wired Headset\n");
                    new_snd_device = SND_DEVICE_HEADSET;
            } else if (outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
                    ALOGI("Routing audio to No microphone Wired Headphone\n");
                    new_snd_device = SND_DEVICE_HEADSET;
            } else if (outputDevices & AUDIO_DEVICE_OUT_SPEAKER) {
                    ALOGI("Routing audio to Speakerphone\n");
                    new_snd_device = SND_DEVICE_SPEAKER;
            } else if (outputDevices & AUDIO_DEVICE_OUT_EARPIECE) {
                    ALOGI("Routing audio to Handset\n");
                    new_snd_device = SND_DEVICE_HANDSET;
#ifdef QCOM_FM_ENABLED
            } else if (inputDevice & AUDIO_DEVICE_IN_FM_RX_A2DP) {
                    ALOGI("Routing audio from FM to Bluetooth A2DP\n");
                    new_snd_device = SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET;
                    FmA2dpStatus = true;
            } else if (inputDevice & AUDIO_DEVICE_IN_FM_RX) {
                    ALOGI("Routing audio to FM\n");
                    enableDgtlFmDriver = true;
#endif
            }
        }
    }

    if (new_snd_device == -1) {
        if (outputDevices & (outputDevices - 1)) {
            if ((outputDevices & AUDIO_DEVICE_OUT_SPEAKER) == 0) {
                ALOGV("Hardware does not support requested route combination (%#X),"
                     " picking closest possible route...", outputDevices);
            }
        }

        if ((mTtyMode != TTY_OFF) && (mMode == AUDIO_MODE_IN_CALL) &&
                (outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET)) {
            if (mTtyMode == TTY_FULL) {
                ALOGI("Routing audio to TTY FULL Mode\n");
                new_snd_device = SND_DEVICE_TTY_HEADSET;
            } else if (mTtyMode == TTY_VCO) {
                ALOGI("Routing audio to TTY VCO Mode\n");
                new_snd_device = SND_DEVICE_TTY_VCO;
            } else if (mTtyMode == TTY_HCO) {
                ALOGI("Routing audio to TTY HCO Mode\n");
                new_snd_device = SND_DEVICE_TTY_HCO;
            }
            if ((mTtyMode != TTY_OFF) && (mMode == AUDIO_MODE_IN_CALL) &&
                       (outputDevices & AUDIO_DEVICE_OUT_SPEAKER)) {
            } else if (mTtyMode == TTY_HCO) {
                ALOGI("Routing audio to TTY HCO Mode with Speakerphone\n");
                new_snd_device = SND_DEVICE_TTY_HCO;
            }
#ifdef COMBO_DEVICE_SUPPORTED
        } else if ((outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET) &&
                   (outputDevices & AUDIO_DEVICE_OUT_SPEAKER)) {
            ALOGI("Routing audio to Wired Headset and Speaker\n");
            new_snd_device = SND_DEVICE_STEREO_HEADSET_AND_SPEAKER;
        } else if ((outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) &&
                   (outputDevices & AUDIO_DEVICE_OUT_SPEAKER)) {
            ALOGI("Routing audio to No microphone Wired Headset and Speaker (%d,%x)\n", mMode, outputDevices);
            new_snd_device = SND_DEVICE_STEREO_HEADSET_AND_SPEAKER;
#endif
#ifdef QCOM_FM_ENABLED
        } else if ((outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET) &&
                   (outputDevices & AUDIO_DEVICE_OUT_FM)) {
            if( !isFMAnalog() ){
                ALOGI("Routing FM to Wired Headset\n");
                new_snd_device = SND_DEVICE_FM_DIGITAL_STEREO_HEADSET;
                enableDgtlFmDriver = true;
            } else{
                ALOGW("Enabling Anlg FM + codec device\n");
                new_snd_device = SND_DEVICE_FM_ANALOG_STEREO_HEADSET_CODEC;
                enableDgtlFmDriver = false;
            }
        } else if ((outputDevices & AUDIO_DEVICE_OUT_SPEAKER) &&
                   (outputDevices & AUDIO_DEVICE_OUT_FM)) {
            ALOGI("Routing FM to Speakerphone\n");
            new_snd_device = SND_DEVICE_FM_DIGITAL_SPEAKER_PHONE;
            enableDgtlFmDriver = true;
        } else if ( (outputDevices & AUDIO_DEVICE_OUT_FM) && isFMAnalog()) {
            ALOGW("Enabling Anlg FM on wired headset\n");
            new_snd_device = SND_DEVICE_FM_ANALOG_STEREO_HEADSET;
            enableDgtlFmDriver = false;
#endif
        } else if (outputDevices &
                   (AUDIO_DEVICE_OUT_BLUETOOTH_SCO | AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET)) {
            ALOGI("Routing audio to Bluetooth PCM\n");
            new_snd_device = SND_DEVICE_BT;
        } else if (outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET) {
            ALOGI("Routing audio to Wired Headset\n");
            new_snd_device = SND_DEVICE_HEADSET;
        } else if (outputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
            ALOGI("Routing audio to No microphone Wired Headphone\n");
            new_snd_device = SND_DEVICE_HEADSET;
        } else if (outputDevices & AUDIO_DEVICE_OUT_SPEAKER) {
            ALOGI("Routing audio to Speakerphone\n");
            new_snd_device = SND_DEVICE_SPEAKER;
        } else if (outputDevices & AUDIO_DEVICE_OUT_EARPIECE) {
            ALOGI("Routing audio to Handset\n");
            new_snd_device = SND_DEVICE_HANDSET;
        }
    }

    if (mDualMicEnabled && (mMode == AUDIO_MODE_IN_CALL || mMode == AUDIO_MODE_IN_COMMUNICATION)) {
        if (new_snd_device == SND_DEVICE_HANDSET) {
            ALOGI("Routing audio to handset with DualMike enabled\n");
            new_snd_device = SND_DEVICE_IN_S_SADC_OUT_HANDSET;
        } else if (new_snd_device == SND_DEVICE_SPEAKER) {
            ALOGI("Routing audio to speakerphone with DualMike enabled\n");
            new_snd_device = SND_DEVICE_IN_S_SADC_OUT_SPEAKER_PHONE;
        }
    }
#ifdef QCOM_FM_ENABLED
    if((outputDevices  == 0) && (FmA2dpStatus == true))
       new_snd_device = SND_DEVICE_FM_DIGITAL_BT_A2DP_HEADSET;
#endif

    if (new_snd_device != -1 && new_snd_device != mCurSndDevice) {
        ret = doAudioRouteOrMute(new_snd_device);

        //disable post proc first for previous session
        if (hpcm_playback_in_progress) {
#ifdef SRS_PROCESSING
            msm72xx_enable_srs(SRS_PARAMS_ALL, false);
#endif /*SRS_PROCESSING*/
        }

        //enable post proc for new device
        snd_device = new_snd_device;

        if (hpcm_playback_in_progress) {
#ifdef SRS_PROCESSING
            msm72xx_enable_srs(SRS_PARAMS_ALL, true);
#endif /*SRS_PROCESSING*/
        }

        mCurSndDevice = new_snd_device;
    }

    return ret;
}

#ifdef QCOM_FM_ENABLED
status_t AudioHardware::enableFM()
{
    ALOGD("enableFM");
    status_t status = NO_INIT;
    status = ::open(FM_DEVICE, O_RDWR);
    if (status < 0) {
           ALOGE("Cannot open FM_DEVICE errno: %d", errno);
           goto Error;
    }
    mFmFd = status;

    status = ioctl(mFmFd, AUDIO_START, 0);

    if (status < 0) {
            ALOGE("Cannot do AUDIO_START");
            goto Error;
    }
    return NO_ERROR;

    Error:
    if (mFmFd >= 0) {
        ::close(mFmFd);
        mFmFd = -1;
    }
    return NO_ERROR;
}


status_t AudioHardware::disableFM()
{
    int status;
    ALOGD("disableFM");
    if (mFmFd >= 0) {
        status = ioctl(mFmFd, AUDIO_STOP, 0);
        if (status < 0) {
                ALOGE("Cannot do AUDIO_STOP");
        }
        ::close(mFmFd);
        mFmFd = -1;
    }

    return NO_ERROR;
}
#endif
status_t AudioHardware::checkMicMute()
{
    Mutex::Autolock lock(mLock);
    if (mMode != AUDIO_MODE_IN_CALL) {
        setMicMute_nosync(true);
    }

    return NO_ERROR;
}

status_t AudioHardware::dumpInternals(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioHardware::dumpInternals\n");
    snprintf(buffer, SIZE, "\tmInit: %s\n", mInit? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmMicMute: %s\n", mMicMute? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmBluetoothNrec: %s\n", mBluetoothNrec? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmBluetoothId: %d\n", mBluetoothId);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardware::dump(int fd, const Vector<String16>& args)
{
    dumpInternals(fd, args);
    for (size_t index = 0; index < mInputs.size(); index++) {
        mInputs[index]->dump(fd, args);
    }

    if (mOutput) {
        mOutput->dump(fd, args);
    }
    return NO_ERROR;
}

uint32_t AudioHardware::getInputSampleRate(uint32_t sampleRate)
{
    uint32_t i;
    uint32_t prevDelta;
    uint32_t delta;

    for (i = 0, prevDelta = 0xFFFFFFFF; i < sizeof(inputSamplingRates)/sizeof(uint32_t); i++, prevDelta = delta) {
        delta = abs(sampleRate - inputSamplingRates[i]);
        if (delta > prevDelta) break;
    }
    // i is always > 0 here
    return inputSamplingRates[i-1];
}

// getActiveInput_l() must be called with mLock held
AudioHardware::AudioStreamInMSM72xx *AudioHardware::getActiveInput_l()
{
    for (size_t i = 0; i < mInputs.size(); i++) {
        // return first input found not being in standby mode
        // as only one input can be in this state
        if (mInputs[i]->state() > AudioStreamInMSM72xx::AUDIO_INPUT_CLOSED) {
            return mInputs[i];
        }
    }

    return NULL;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

AudioHardware::AudioStreamOutMSM72xx::AudioStreamOutMSM72xx() :
    mHardware(0), mFd(-1), mStartCount(0), mRetryCount(0), mStandby(true), mDevices(0)
{
}

status_t AudioHardware::AudioStreamOutMSM72xx::set(
        AudioHardware* hw, uint32_t devices, int *pFormat, uint32_t *pChannels, uint32_t *pRate)
{
    int lFormat = pFormat ? *pFormat : 0;
    uint32_t lChannels = pChannels ? *pChannels : 0;
    uint32_t lRate = pRate ? *pRate : 0;

    mHardware = hw;

    // fix up defaults
    if (lFormat == 0) lFormat = format();
    if (lChannels == 0) lChannels = channels();
    if (lRate == 0) lRate = sampleRate();

    // check values
    if ((lFormat != format()) ||
        (lChannels != channels()) ||
        (lRate != sampleRate())) {
        if (pFormat) *pFormat = format();
        if (pChannels) *pChannels = channels();
        if (pRate) *pRate = sampleRate();
        ALOGE("AudioStreamOutMSM72xx: Setting up correct values");
        return NO_ERROR;
    }

    if (pFormat) *pFormat = lFormat;
    if (pChannels) *pChannels = lChannels;
    if (pRate) *pRate = lRate;

    mDevices = devices;

    return NO_ERROR;
}

AudioHardware::AudioStreamOutMSM72xx::~AudioStreamOutMSM72xx()
{
    if (mFd >= 0) close(mFd);
}

ssize_t AudioHardware::AudioStreamOutMSM72xx::write(const void* buffer, size_t bytes)
{
    //ALOGE("AudioStreamOutMSM72xx::write(%p, %u)", buffer, bytes);
    status_t status = NO_INIT;
    size_t count = bytes;
    const uint8_t* p = static_cast<const uint8_t*>(buffer);

    if (mStandby) {

        // open driver
        ALOGV("open driver");
        status = ::open("/dev/msm_pcm_out", O_RDWR);
        if (status < 0) {
            ALOGE("Cannot open /dev/msm_pcm_out errno: %d", errno);
            goto Error;
        }
        mFd = status;

        // configuration
        ALOGV("get config");
        struct msm_audio_config config;
        status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
        if (status < 0) {
            ALOGE("Cannot read config");
            goto Error;
        }

        ALOGV("set config");
        config.channel_count = AudioSystem::popCount(channels());
        config.sample_rate = sampleRate();
        config.buffer_size = bufferSize();
        config.buffer_count = AUDIO_HW_NUM_OUT_BUF;
        config.type = CODEC_TYPE_PCM;
        status = ioctl(mFd, AUDIO_SET_CONFIG, &config);
        if (status < 0) {
            ALOGE("Cannot set config");
            goto Error;
        }

        ALOGV("buffer_size: %u", config.buffer_size);
        ALOGV("buffer_count: %u", config.buffer_count);
        ALOGV("channel_count: %u", config.channel_count);
        ALOGV("sample_rate: %u", config.sample_rate);

        // fill 2 buffers before AUDIO_START
        mStartCount = AUDIO_HW_NUM_OUT_BUF;
        mStandby = false;
    }

    while (count) {
        ssize_t written = ::write(mFd, p, count);
        if (written >= 0) {
            count -= written;
            p += written;
        } else {
            if (errno != EAGAIN) return written;
            mRetryCount++;
            ALOGW("EAGAIN - retry");
        }
    }

    // start audio after we fill 2 buffers
    if (mStartCount) {
        if (--mStartCount == 0) {
            ioctl(mFd, AUDIO_START, 0);
            hpcm_playback_in_progress = true;
#ifdef SRS_PROCESSING
            msm72xx_enable_srs(SRS_PARAMS_ALL, true);
#endif /*SRS_PROCESSING*/
        }
    }
    return bytes;

Error:
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    // Simulate audio output timing in case of error
    usleep(bytes * 1000000 / frameSize() / sampleRate());

    return status;
}

status_t AudioHardware::AudioStreamOutMSM72xx::standby()
{
    status_t status = NO_ERROR;
    if (!mStandby && mFd >= 0) {
        //disable post processing
        hpcm_playback_in_progress = false;
        {
#ifdef SRS_PROCESSING
            msm72xx_enable_srs(SRS_PARAMS_ALL, false);
#endif /*SRS_PROCESSING*/
        }
        ::close(mFd);
        mFd = -1;
    }
    mStandby = true;
    return status;
}

status_t AudioHardware::AudioStreamOutMSM72xx::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioStreamOutMSM72xx::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannels: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmHardware: %p\n", mHardware);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmFd: %d\n", mFd);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmStartCount: %d\n", mStartCount);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmRetryCount: %d\n", mRetryCount);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmStandby: %s\n", mStandby? "true": "false");
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

bool AudioHardware::AudioStreamOutMSM72xx::checkStandby()
{
    return mStandby;
}


status_t AudioHardware::AudioStreamOutMSM72xx::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key;
    status_t status = NO_ERROR;
    int device;
    ALOGV("AudioStreamOutMSM72xx::setParameters() %s", keyValuePairs.string());

#ifdef QCOM_FM_ENABLED
    float fm_volume;
    key = String8(AUDIO_PARAMETER_KEY_FM_VOLUME);
    if (param.getFloat(key, fm_volume) == NO_ERROR) {
        mHardware->setFmVolume(fm_volume);
        param.remove(key);
    }

    key = String8(AUDIO_PARAMETER_KEY_HANDLE_FM);
    if (param.getInt(key, device) == NO_ERROR) {
        if (device & AUDIO_DEVICE_OUT_FM) {
            mDevices |= device;
            mHardware->enableFM();
        } else {
            mHardware->disableFM();
            mDevices &= device;
        }
        param.remove(key);
    }
#endif

    key = String8(AudioParameter::keyRouting);
    if (param.getInt(key, device) == NO_ERROR) {
        mDevices = device;
        ALOGV("set output routing %x", mDevices);
        status = mHardware->doRouting(NULL, device);
        param.remove(key);
    }

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioHardware::AudioStreamOutMSM72xx::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        ALOGV("get routing %x", mDevices);
        param.addInt(key, (int)mDevices);
    }

    ALOGV("AudioStreamOutMSM72xx::getParameters() %s", param.toString().string());
    return param.toString();
}

status_t AudioHardware::AudioStreamOutMSM72xx::getRenderPosition(uint32_t *dspFrames)
{
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

status_t AudioHardware::AudioStreamOutMSM72xx::getPresentationPosition(uint64_t *frames, struct timespec *timestamp)
{
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}


status_t AudioHardware::AudioStreamOutDirect::getPresentationPosition(uint64_t *frames, struct timespec *timestamp)
{
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

//.----------------------------------------------------------------------------
int AudioHardware::AudioStreamInMSM72xx::InstanceCount = 0;
AudioHardware::AudioStreamInMSM72xx::AudioStreamInMSM72xx() :
    mHardware(0), mFd(-1), mState(AUDIO_INPUT_CLOSED), mRetryCount(0),
    mFormat(AUDIO_HW_IN_FORMAT), mChannels(AUDIO_HW_IN_CHANNELS),
    mSampleRate(AUDIO_HW_IN_SAMPLERATE), mBufferSize(AUDIO_HW_IN_BUFFERSIZE),
    mAcoustics((AudioSystem::audio_in_acoustics)0), mDevices(0)
{
    AudioStreamInMSM72xx::InstanceCount++;
}

status_t AudioHardware::AudioStreamInMSM72xx::set(
        AudioHardware* hw, uint32_t devices, int *pFormat, uint32_t *pChannels, uint32_t *pRate,
        AudioSystem::audio_in_acoustics acoustic_flags)
{
    if(AudioStreamInMSM72xx::InstanceCount > 1)
    {
        ALOGE("More than one instance of recording not supported");
        return -EBUSY;
    }

    if ((pFormat == 0) ||
        ((*pFormat != AUDIO_HW_IN_FORMAT) &&
         (*pFormat != AUDIO_FORMAT_AMR_NB) &&
         (*pFormat != AUDIO_FORMAT_EVRC) &&
         (*pFormat != AUDIO_FORMAT_QCELP) &&
         (*pFormat != AUDIO_FORMAT_AAC)))
    {
        *pFormat = AUDIO_HW_IN_FORMAT;
        ALOGE("audio format bad value");
        return BAD_VALUE;
    }
    if (pRate == 0) {
        return BAD_VALUE;
    }
    uint32_t rate = hw->getInputSampleRate(*pRate);
    if (rate != *pRate) {
        *pRate = rate;
        ALOGE(" sample rate does not match\n");
        return BAD_VALUE;
    }

    if (pChannels == 0 || (*pChannels & (AUDIO_CHANNEL_IN_MONO | AUDIO_CHANNEL_IN_STEREO)) == 0)
    {
        *pChannels = AUDIO_HW_IN_CHANNELS;
        ALOGE(" Channel count does not match\n");
        return BAD_VALUE;
    }

    mHardware = hw;

    ALOGV("AudioStreamInMSM72xx::set(%d, %d, %u)", *pFormat, *pChannels, *pRate);
    if (mFd >= 0) {
        ALOGE("Audio record already open");
        return -EPERM;
    }

    struct msm_audio_config config;
    struct msm_audio_voicememo_config gcfg;
    memset(&gcfg,0,sizeof(gcfg));
    status_t status = 0;
    if(*pFormat == AUDIO_HW_IN_FORMAT)
    {
    // open audio input device
        status = ::open(PCM_IN_DEVICE, O_RDWR);
        if (status < 0) {
            ALOGE("Cannot open %s errno: %d", PCM_IN_DEVICE, errno);
            goto Error;
        }
        mFd = status;

        // configuration
        status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
        if (status < 0) {
            ALOGE("Cannot read config");
           goto Error;
        }

    ALOGV("set config");
    config.channel_count = AudioSystem::popCount(*pChannels);
    config.sample_rate = *pRate;
    config.buffer_size = bufferSize();
    config.buffer_count = 2;
        config.type = CODEC_TYPE_PCM;
    status = ioctl(mFd, AUDIO_SET_CONFIG, &config);
    if (status < 0) {
        ALOGE("Cannot set config");
        if (ioctl(mFd, AUDIO_GET_CONFIG, &config) == 0) {
            if (config.channel_count == 1) {
                *pChannels = AUDIO_CHANNEL_IN_MONO;
            } else {
                *pChannels = AUDIO_CHANNEL_IN_STEREO;
            }
            *pRate = config.sample_rate;
        }
        goto Error;
    }

    ALOGV("confirm config");
    status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
    if (status < 0) {
        ALOGE("Cannot read config");
        goto Error;
    }
    ALOGV("buffer_size: %u", config.buffer_size);
    ALOGV("buffer_count: %u", config.buffer_count);
    ALOGV("channel_count: %u", config.channel_count);
    ALOGV("sample_rate: %u", config.sample_rate);
    ALOGV("input device: %x", devices);

    mDevices = devices;
    mFormat = AUDIO_HW_IN_FORMAT;
    mChannels = *pChannels;
    mSampleRate = config.sample_rate;
    mBufferSize = config.buffer_size;
    }
    else if( (*pFormat == AUDIO_FORMAT_AMR_NB) ||
             (*pFormat == AUDIO_FORMAT_EVRC) ||
             (*pFormat == AUDIO_FORMAT_QCELP))
           {

      // open vocie memo input device
      status = ::open(VOICE_MEMO_DEVICE, O_RDWR);
      if (status < 0) {
          ALOGE("Cannot open Voice Memo device for read");
          goto Error;
      }
      mFd = status;
      /* Config param */
      if(ioctl(mFd, AUDIO_GET_CONFIG, &config))
      {
        ALOGE(" Error getting buf config param AUDIO_GET_CONFIG \n");
        goto  Error;
      }

      ALOGV("The Config buffer size is %d", config.buffer_size);
      ALOGV("The Config buffer count is %d", config.buffer_count);
      ALOGV("The Config Channel count is %d", config.channel_count);
      ALOGV("The Config Sample rate is %d", config.sample_rate);

      mDevices = devices;
      mChannels = *pChannels;
      mSampleRate = config.sample_rate;

      if (mDevices == AUDIO_DEVICE_IN_VOICE_CALL)
      {
        if ((mChannels & AUDIO_CHANNEL_IN_VOICE_DNLINK) &&
            (mChannels & AUDIO_CHANNEL_IN_VOICE_UPLINK)) {
          ALOGI("Recording Source: Voice Call Both Uplink and Downlink");
          gcfg.rec_type = RPC_VOC_REC_BOTH;
        } else if (mChannels & AUDIO_CHANNEL_IN_VOICE_DNLINK) {
          ALOGI("Recording Source: Voice Call DownLink");
          gcfg.rec_type = RPC_VOC_REC_FORWARD;
        } else if (mChannels & AUDIO_CHANNEL_IN_VOICE_UPLINK) {
          ALOGI("Recording Source: Voice Call UpLink");
          gcfg.rec_type = RPC_VOC_REC_REVERSE;
        }
      }
      else {
        ALOGI("Recording Source: Mic/Headset");
        gcfg.rec_type = RPC_VOC_REC_REVERSE;
      }

      gcfg.rec_interval_ms = 0; // AV sync
      gcfg.auto_stop_ms = 0;

      switch (*pFormat)
      {
        case AUDIO_FORMAT_AMR_NB:
        {
          ALOGI("Recording Format: AMR_NB");
          gcfg.capability = RPC_VOC_CAP_AMR; // RPC_VOC_CAP_AMR (64)
          gcfg.max_rate = RPC_VOC_AMR_RATE_1220; // Max rate (Fixed frame)
          gcfg.min_rate = RPC_VOC_AMR_RATE_1220; // Min rate (Fixed frame length)
          gcfg.frame_format = RPC_VOC_PB_AMR; // RPC_VOC_PB_AMR
          mFormat = AUDIO_FORMAT_AMR_NB;
          mBufferSize = 320;
          break;
        }

        case AUDIO_FORMAT_EVRC:
        {
          ALOGI("Recording Format: EVRC");
          gcfg.capability = RPC_VOC_CAP_IS127;
          gcfg.max_rate = RPC_VOC_1_RATE; // Max rate (Fixed frame)
          gcfg.min_rate = RPC_VOC_1_RATE; // Min rate (Fixed frame length)
          gcfg.frame_format = RPC_VOC_PB_NATIVE_QCP;
          mFormat = AUDIO_FORMAT_EVRC;
          mBufferSize = 230;
          break;
        }

        case AUDIO_FORMAT_QCELP:
        {
          ALOGI("Recording Format: QCELP");
          gcfg.capability = RPC_VOC_CAP_IS733; // RPC_VOC_CAP_AMR (64)
          gcfg.max_rate = RPC_VOC_1_RATE; // Max rate (Fixed frame)
          gcfg.min_rate = RPC_VOC_1_RATE; // Min rate (Fixed frame length)
          gcfg.frame_format = RPC_VOC_PB_NATIVE_QCP;
          mFormat = AUDIO_FORMAT_QCELP;
          mBufferSize = 350;
          break;
        }

        default:
        break;
      }

      gcfg.dtx_enable = 0;
      gcfg.data_req_ms = 20;

      /* Set Via  config param */
      if (ioctl(mFd, AUDIO_SET_VOICEMEMO_CONFIG, &gcfg))
      {
        ALOGE("Error: AUDIO_SET_VOICEMEMO_CONFIG failed\n");
        goto  Error;
      }

      if (ioctl(mFd, AUDIO_GET_VOICEMEMO_CONFIG, &gcfg))
      {
        ALOGE("Error: AUDIO_GET_VOICEMEMO_CONFIG failed\n");
        goto  Error;
      }

      ALOGV("After set rec_type = 0x%8x\n",gcfg.rec_type);
      ALOGV("After set rec_interval_ms = 0x%8x\n",gcfg.rec_interval_ms);
      ALOGV("After set auto_stop_ms = 0x%8x\n",gcfg.auto_stop_ms);
      ALOGV("After set capability = 0x%8x\n",gcfg.capability);
      ALOGV("After set max_rate = 0x%8x\n",gcfg.max_rate);
      ALOGV("After set min_rate = 0x%8x\n",gcfg.min_rate);
      ALOGV("After set frame_format = 0x%8x\n",gcfg.frame_format);
      ALOGV("After set dtx_enable = 0x%8x\n",gcfg.dtx_enable);
      ALOGV("After set data_req_ms = 0x%8x\n",gcfg.data_req_ms);
    }
    else if(*pFormat == AUDIO_FORMAT_AAC) {
      // open AAC input device
               status = ::open(PCM_IN_DEVICE, O_RDWR);
               if (status < 0) {
                     ALOGE("Cannot open AAC input  device for read");
                     goto Error;
               }
               mFd = status;

      /* Config param */
               if(ioctl(mFd, AUDIO_GET_CONFIG, &config))
               {
                     ALOGE(" Error getting buf config param AUDIO_GET_CONFIG \n");
                     goto  Error;
               }

      ALOGV("The Config buffer size is %d", config.buffer_size);
      ALOGV("The Config buffer count is %d", config.buffer_count);
      ALOGV("The Config Channel count is %d", config.channel_count);
      ALOGV("The Config Sample rate is %d", config.sample_rate);

      mDevices = devices;
      mChannels = *pChannels;
      mSampleRate = *pRate;
      mBufferSize = 2048;
      mFormat = *pFormat;

      config.channel_count = AudioSystem::popCount(*pChannels);
      config.sample_rate = *pRate;
      config.type = 1; // Configuring PCM_IN_DEVICE to AAC format

      if (ioctl(mFd, AUDIO_SET_CONFIG, &config)) {
             ALOGE(" Error in setting config of msm_pcm_in device \n");
                   goto Error;
        }
    }

    //mHardware->setMicMute_nosync(false);
    mState = AUDIO_INPUT_OPENED;

    //if (!acoustic)
    //    return NO_ERROR;

    return NO_ERROR;

Error:
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    return status;
}

//.----------------------------------------------------------------------------
AudioHardware::AudioStreamInMSM72xx::~AudioStreamInMSM72xx()
{
    ALOGV("AudioStreamInMSM72xx destructor");
    AudioStreamInMSM72xx::InstanceCount--;
    standby();
}

ssize_t AudioHardware::AudioStreamInMSM72xx::read( void* buffer, ssize_t bytes)
{
    //ALOGV("AudioStreamInMSM72xx::read(%p, %ld)", buffer, bytes);
    if (!mHardware) return -1;

    size_t count = bytes;
    size_t  aac_framesize= bytes;
    uint8_t* p = static_cast<uint8_t*>(buffer);
    uint32_t* recogPtr = (uint32_t *)p;
    uint16_t* frameCountPtr;
    uint16_t* frameSizePtr;

    if (mState < AUDIO_INPUT_OPENED) {
        AudioHardware *hw = mHardware;
        hw->mLock.lock();
        status_t status = set(hw, mDevices, &mFormat, &mChannels, &mSampleRate, mAcoustics);
        hw->mLock.unlock();
        if (status != NO_ERROR) {
            return -1;
        }
        mFirstread = false;
    }

    if (mState < AUDIO_INPUT_STARTED) {
        mState = AUDIO_INPUT_STARTED;
        // force routing to input device
#ifdef QCOM_FM_ENABLED
        if (mDevices != AUDIO_DEVICE_IN_FM_RX) {
            mHardware->clearCurDevice();
             mHardware->doRouting(this, 0);
        }
#endif
        if (ioctl(mFd, AUDIO_START, 0)) {
            ALOGE("Error starting record");
            standby();
            return -1;
        }
    }

    // Resetting the bytes value, to return the appropriate read value
    bytes = 0;
    if (mFormat == AUDIO_FORMAT_AAC)
    {
        *((uint32_t*)recogPtr) = 0x51434F4D ;// ('Q','C','O', 'M') Number to identify format as AAC by higher layers
        recogPtr++;
        frameCountPtr = (uint16_t*)recogPtr;
        *frameCountPtr = 0;
        p += 3*sizeof(uint16_t);
        count -= 3*sizeof(uint16_t);
    }
    while (count > 0) {

        if (mFormat == AUDIO_FORMAT_AAC) {
            frameSizePtr = (uint16_t *)p;
            p += sizeof(uint16_t);
            if(!(count > 2)) break;
            count -= sizeof(uint16_t);
        }

        ssize_t bytesRead = ::read(mFd, p, count);
        if (bytesRead > 0) {
            count -= bytesRead;
            p += bytesRead;
            bytes += bytesRead;

            if (mFormat == AUDIO_FORMAT_AAC){
                *frameSizePtr =  bytesRead;
                (*frameCountPtr)++;
            }

            if(!mFirstread)
            {
               mFirstread = true;
               break;
            }

        }
        else if(bytesRead == 0)
        {
         ALOGI("Bytes Read = %d ,Buffer no longer sufficient",bytesRead);
         break;
        } else {
            if (errno != EAGAIN) return bytesRead;
            mRetryCount++;
            ALOGW("EAGAIN - retrying");
        }
    }
    if (mFormat == AUDIO_FORMAT_AAC)
         return aac_framesize;

    return bytes;
}

status_t AudioHardware::AudioStreamInMSM72xx::standby()
{
    if (mState > AUDIO_INPUT_CLOSED) {
        if (mFd >= 0) {
            ::close(mFd);
            mFd = -1;
        }
        mState = AUDIO_INPUT_CLOSED;
    }
    if (!mHardware) return -1;
    // restore output routing if necessary
#ifdef QCOM_FM_ENABLED
    if (!mHardware->isFMAnalog() && !mHardware->IsFmon())
#endif
    {
        mHardware->clearCurDevice();
         mHardware->doRouting(this, 0);
    }
#ifdef QCOM_FM_ENABLED
    if(mHardware->IsFmA2dpOn())
        mHardware->SwitchOffFmA2dp();
#endif

    return NO_ERROR;
}

status_t AudioHardware::AudioStreamInMSM72xx::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioStreamInMSM72xx::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannels: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmHardware: %p\n", mHardware);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmFd count: %d\n", mFd);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmState: %d\n", mState);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmRetryCount: %d\n", mRetryCount);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardware::AudioStreamInMSM72xx::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    ALOGV("AudioStreamInMSM72xx::setParameters() %s", keyValuePairs.string());

    if (param.getInt(key, device) == NO_ERROR) {
        ALOGD("set input routing %x", device);
        if (device & (device - 1)) {
            status = BAD_VALUE;
        } else {
            mDevices = device;
            status = mHardware->doRouting(this, device);
        }
        param.remove(key);
    }

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioHardware::AudioStreamInMSM72xx::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        ALOGV("get routing %x", mDevices);
        param.addInt(key, (int)mDevices);
    }

    ALOGV("AudioStreamInMSM72xx::getParameters() %s", param.toString().string());
    return param.toString();
}

// ----------------------------------------------------------------------------

extern "C" AudioHardwareInterface* createAudioHardware(void) {
    return new AudioHardware();
}

}; // namespace android
