#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT", "S?", "V?"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  //feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  feat.zcr = compute_zcr(x,N,16000);
  feat.am = compute_am(x,N);
  feat.p = compute_power(x,N);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate, float a0, float a1, float a2, float min_zcr, float min_silence_time, float max_mv_time) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->a0 = a0;
  vad_data->a1 = a1;
  vad_data->a2 = a2;
  vad_data->min_zcr = min_zcr;
  vad_data->min_silence_time = min_silence_time;
  vad_data->max_mv_time = max_mv_time;
  /*vad_data->a0 = 4;
  vad_data->a1 = 6.7;
  vad_data->a2 = 11.1;
  vad_data->min_zcr = 50;
  vad_data->min_silence_time = 11;
  vad_data->max_mv_time = 10;*/
  vad_data->amplitude0 = 0.00095;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = ST_SILENCE;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x) {

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
  case ST_INIT:    
    if(f.p > vad_data->k0 + vad_data->a0 && f.zcr > vad_data->min_zcr && vad_data->N != 0) {
      vad_data->k1 = vad_data->k0 + vad_data->a1;
      vad_data->k2 = vad_data->k0 + vad_data->a2;
      vad_data->state = ST_MAYBE_VOICE;
      break;
    }
    vad_data->pot += pow(10,f.p/10);
    vad_data->N++;
    vad_data->k0 = 10*log10(vad_data->pot/vad_data->N);
    break;

  case ST_SILENCE:
    if (f.p > vad_data->k1 && f.am > vad_data->amplitude0)
      vad_data->state = ST_MAYBE_VOICE;
    break;

  case ST_VOICE:
    if (f.p < vad_data->k1 - 0.3)
      vad_data->state = ST_MAYBE_SILENCE;
    break;

  case ST_MAYBE_SILENCE:
    vad_data->silence_time++;
    if(vad_data->silence_time > vad_data->min_silence_time) {
        vad_data->state = ST_SILENCE;
        vad_data->silence_time = 0;
    }
    if (f.p > vad_data->k1 && f.am > vad_data->amplitude0 && f.zcr > vad_data->min_zcr)
      vad_data->state = ST_VOICE;
    break;

  case ST_MAYBE_VOICE:
    vad_data->voice_time++;
    if(f.p > vad_data->k2 && f.am > vad_data->amplitude0) {
      vad_data->state = ST_VOICE;
      vad_data->voice_time = 0;
    }
    else if(vad_data->voice_time > vad_data->max_mv_time) {
      vad_data->state = ST_SILENCE;
      vad_data->voice_time = 0;
    }
    break;
    
  case ST_UNDEF:
    break;
  }

  if (vad_data->state == ST_INIT)
    return ST_SILENCE;
  if (vad_data->state == ST_SILENCE || vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
