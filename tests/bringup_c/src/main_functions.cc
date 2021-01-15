/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <iostream>
#include <fstream>
#include <cassert>
std::ofstream features_c;
std::ifstream input_pcm;

#include "tensorflow/lite/micro/examples/micro_speech/main_functions.h"

#include "tensorflow/lite/micro/examples/micro_speech/audio_provider.h"
#include "tensorflow/lite/micro/examples/micro_speech/command_responder.h"
#include "tensorflow/lite/micro/examples/micro_speech/feature_provider.h"
#include "tensorflow/lite/micro/examples/micro_speech/micro_features/micro_model_settings.h"
#include "tensorflow/lite/micro/examples/micro_speech/micro_features/model.h"
#include "tensorflow/lite/micro/examples/micro_speech/recognize_commands.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

using namespace tflite;

// Globals, used for compatibility with Arduino-style sketches.
namespace {
// namespace micro{
// namespace tflite{
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
FeatureProvider* feature_provider = nullptr;
RecognizeCommands* recognizer = nullptr;
int32_t previous_time = 0;

// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
int8_t feature_buffer[kFeatureElementCount];
int8_t* model_input_buffer = nullptr;
}  // namespace
// }
// }



#define TOTAL_SAMPLES (16000 * 10)
int16_t g_captured_audio_buffer[TOTAL_SAMPLES] = {0};

int32_t* sample_buf = NULL;
bool g_is_audio_initialized = false;
int16_t g_audio_output_buffer[kMaxAudioSampleSize] = {0};


// Initialization for receiving audio data
TfLiteStatus InitAudioRecording(tflite::ErrorReporter *error_reporter) {
 
  return kTfLiteOk;
}

namespace tflite{
  int32_t GetCurrentTimeTicks() {
    // hwtimer_t hwtimer = hwtimer_alloc();
    // int32_t ticks = hwtimer_get_time(hwtimer);
    // hwtimer_free(hwtimer);
    return 0;
  }
}

extern "C"{
  void DebugLog(const char* s) { printf("%s", s); }
}

int32_t LatestAudioTimestamp(void){
  static int32_t time_now = 0;
  time_now += (16000 * 30 / 1000) * 2; //480 * 2
  return time_now;
}

void load_input_pcm(void){
    input_pcm.open("input.raw", std::ios::in|std::ios::binary);
    if (input_pcm.is_open()){
        input_pcm.seekg(0, std::ios::end);
        std::streampos size = input_pcm.tellg();
        assert(size >= sizeof(g_captured_audio_buffer) && "Input file not large enough to fill buffer");
        input_pcm.seekg(0, std::ios::beg);
        input_pcm.read(reinterpret_cast<char*>(g_captured_audio_buffer), sizeof(g_captured_audio_buffer));
        std::cout << "Reading: " << sizeof(g_captured_audio_buffer) << " bytes from: " << "input.raw" << std::endl;
        input_pcm.close();
    }else{
        assert(0 && "Cannot open input file");
    }
}

//This gets called by the feature provider
TfLiteStatus GetAudioSamples(tflite::ErrorReporter* error_reporter,
                             int start_ms, int duration_ms,
                             int* audio_samples_size, int16_t** audio_samples) {

  const int start_offset = start_ms * (kAudioSampleFrequency / 1000);
  const int duration_sample_count = duration_ms * (kAudioSampleFrequency / 1000);
  const int next_frame_sample = start_offset + duration_sample_count;

  if(next_frame_sample >= TOTAL_SAMPLES){
    *audio_samples_size = 0;
    return kTfLiteOk;
  }

  for (int i = 0; i<duration_sample_count; i++) {
    const int capture_index = (start_offset + i);
    assert(i < TOTAL_SAMPLES);
    g_audio_output_buffer[i] = g_captured_audio_buffer[capture_index];
  }

  *audio_samples_size = kMaxAudioSampleSize;
  *audio_samples = g_audio_output_buffer;

  // std::cout << "GetAudioSamples " << start_ms << "ms, " << duration_ms << "ms, " 
  //   << *audio_samples_size << ", " << *audio_samples << ", " << next_frame_sample << std::endl;

  return kTfLiteOk;
}

// The name of this function is important for Arduino compatibility.
void setup() {
  features_c.open ("features_c.bin", std::ios::out|std::ios::binary);

  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Prepare to access the audio spectrograms from a microphone or other source
  // that will provide the inputs to the neural network.
  // NOLINTNEXTLINE(runtime-global-variables)
  static FeatureProvider static_feature_provider(kFeatureElementCount,
                                                 feature_buffer);
  feature_provider = &static_feature_provider;

  previous_time = 0;

  load_input_pcm();

  for(int i = 0; i < 1024; i++){
      // std::cout << g_captured_audio_buffer[i] << std::endl;
  }
}

// The name of this function is important for Arduino compatibility.
void loop() {
  // Fetch the spectrogram for the current time.
  const int32_t current_time = LatestAudioTimestamp();
  int how_many_new_slices = 1;

  // GetAudioSamples(error_reporter, )

  TfLiteStatus feature_status = feature_provider->PopulateFeatureData(
      error_reporter, previous_time, current_time, &how_many_new_slices);
  if (feature_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Feature generation failed");
    // std::cout << "how_many_new_slices == 0" << std::endl;
    features_c.close();
    exit(0);
  }
  previous_time = current_time;
  // If no new audio samples have been received since last time, don't bother
  // running the network model.
  std::cout << "how_many_new_slices = " << how_many_new_slices << std::endl;

  // Copy feature buffer to input tensor
  for (int i = 0; i < kFeatureElementCount; i++) {
    model_input_buffer[i] = feature_buffer[i];
  }

  for( int f = 0; f < kFeatureSliceSize; f++){
    for( int c = 0; c < kFeatureSliceCount; c++){
      int idx = c + f * kFeatureSliceCount;
      int8_t feature = static_cast<int8_t>(feature_buffer[idx]);
      // std::cout << static_cast<int32_t>(feature) << ", ";
      features_c << feature;
    }
    // std::cout << std::endl << "Col.." << std::endl;
  }
}