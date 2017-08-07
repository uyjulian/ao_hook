#ifndef _AUDIO_H_ccdfbab6_9f02_4da2_b7df_d81114391052
#define _AUDIO_H_ccdfbab6_9f02_4da2_b7df_d81114391052


_ML_C_HEAD_

typedef struct
{
    UInt32 RIFF;                    // TAG4('RIFF')
    UInt32 Size;
    UInt32 WAVE;                    // TAG4('WAVE')
    UInt32 fmt;                     // TAG4('fmt ')
    UInt32 FormatLength;
    UInt16 FormatTag;
    UInt16 Channels;
    UInt32 SamplesPerSec;
    UInt32 AvgBytesPerSec;
    UInt16 BlockAlign;
    UInt16 BitsPerSample;
    UInt32 data;                    // TAG4('data')
    UInt32 DataSize;

} AUDIO_WAVE_HEADER, *PAUDIO_WAVE_HEADER;

#if CPP_DEFINED

#include <MMSystem.h>

inline
Bool
FASTCALL
InitWaveHeader(
    AUDIO_WAVE_HEADER  *Header,
    const WAVEFORMATEX *WaveFormat,
    UInt32              WaveLength
)
{
    if (Header == NULL || WaveFormat == NULL)
        return False;

    Header->RIFF = TAG4('RIFF');
    Header->fmt  = TAG4('fmt ');
    Header->WAVE = TAG4('WAVE');
    Header->data = TAG4('data');
    Header->FormatLength = 16;

    Header->FormatTag = WaveFormat->wFormatTag;
    Header->Channels  = WaveFormat->nChannels;
    Header->SamplesPerSec = WaveFormat->nSamplesPerSec;
    Header->AvgBytesPerSec = WaveFormat->nAvgBytesPerSec;
    Header->BlockAlign = WaveFormat->nBlockAlign;
    Header->BitsPerSample = WaveFormat->wBitsPerSample;
    Header->DataSize = WaveLength;
    Header->Size = WaveLength + sizeof(*Header) - 8;

    return True;
}

#endif // CPP_DEFINED

EXTCPP
inline
Bool
FASTCALL
InitWaveHeader(
    AUDIO_WAVE_HEADER  *Header,
    UInt16              Channels,
    UInt32              SampleRate,
    UInt16              BitsPerSample,
    UInt32              WaveLength
)
{
    if (Header == NULL)
        return False;

    Header->RIFF = TAG4('RIFF');
    Header->fmt  = TAG4('fmt ');
    Header->WAVE = TAG4('WAVE');
    Header->data = TAG4('data');
    Header->FormatTag = WAVE_FORMAT_PCM;
    Header->FormatLength = 16;
    Header->Channels = Channels;
    Header->BitsPerSample = BitsPerSample;
    Header->BlockAlign = Header->Channels * BitsPerSample / 8;
    Header->SamplesPerSec = SampleRate;
    Header->AvgBytesPerSec = Header->BlockAlign * Header->SamplesPerSec;
    Header->DataSize = WaveLength;
    Header->Size = WaveLength + sizeof(*Header) - 8;

    return True;
}

#if MY_OS_WIN32

// #if !ML_AMD64
//     #pragma INCLUDE_LIB(ucadec.lib)
// #endif

EXTC_IMPORT int STDCALL UCADecode(const void *src, int srclen, void** dst, int* dstlen);
EXTC_IMPORT int STDCALL UCADecode2(const void *src, int srclen, void** dst, int* dstlen, int sample);
EXTC_IMPORT int STDCALL UCAFree(void* p);

#endif // MY_OS_WIN32

_ML_C_TAIL_

#endif // _AUDIO_H_ccdfbab6_9f02_4da2_b7df_d81114391052