import speech_commands.input_data as id
import tensorflow as tf

#sox -V -r 16000 -n -b 16 -c 2 sin.wav synth 30 sin 100 vol -3dB

model_settings = {
    "fingerprint_size" : 40,
    "fingerprint_width" : 49,
    "window_size_samples" : 480,
    "window_stride_samples" : 320,
    "average_window_width": 512,
    "sample_rate" : 16000,
    "preprocess" : 'micro',
}

summaries_dir = "."

model_settings["desired_samples"] = 160000


audio_processor = id.AudioProcessor(
    None, #data_url , , , unknown_percentage,
    None, #data_dir
    50, #silence_percentage
    10, #unknown_percentage
    0, #wanted_words,
    50, #validation_percentage
    50, #testing_percentage,
    model_settings,
    summaries_dir
    )

sess = tf.compat.v1.InteractiveSession()
input_wav = "sin.wav"
results = audio_processor.get_features_for_wav(input_wav, model_settings,
                                               sess)
features = results[0]
print(features.shape)
print(features)
