#include "sys_audio.h"

typedef struct {
	float left_phase;
	float right_phase;
} paTestData;

/* onAudio is the portaudio audio callback. */
/* TODO: */
int onAudio(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo *timeInfo,
            PaStreamCallbackFlags statusFlags, void *userData) {
	UNUSED(inputBuffer);
	UNUSED(timeInfo);
	UNUSED(statusFlags);

	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData *)userData;
	float *out = (float *)outputBuffer;
	unsigned int i;

	for (i = 0; i < framesPerBuffer; i++) {
		*out++ = data->left_phase;  /* left */
		*out++ = data->right_phase; /* right */
		/* Generate simple sawtooth phaser that ranges between -1.0 and
		 * 1.0. */
		data->left_phase += 0.01f;
		/* When signal reaches top, drop back down. */
		if (data->left_phase >= 1.0f)
			data->left_phase -= 2.0f;
		/* higher pitch so we can distinguish left and right. */
		data->right_phase += 0.03f;
		if (data->right_phase >= 1.0f)
			data->right_phase -= 2.0f;
	}
	return 0;
}

void InitAudioSystem() {
	int err;
	if ((err = Pa_Initialize()) != paNoError)
		derrorf("PortAudio error: %s", Pa_GetErrorText(err));
}

void DeinitAudioSystem() {
	int err;
	if ((err = Pa_Terminate()) != paNoError)
		derrorf("PortAudio error: %s", Pa_GetErrorText(err));
}
