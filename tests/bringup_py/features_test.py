import speech_commands.input_data as id
import tensorflow as tf
import numpy as np

model_settings = {
    "fingerprint_size" : 49, #Doesn't seem to affect generation of micro features
    "fingerprint_width" : 40,
    "window_size_samples" : 480,
    "window_stride_samples" : 320,
    "average_window_width": 512, #Doesn't seem to affect generation of micro features
    "sample_rate" : 16000,
    "preprocess" : 'micro',
}

summaries_dir = "."

model_settings["desired_samples"] = 16000 * 10


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
input_wav = "input.wav"
results = audio_processor.get_features_for_wav(input_wav, model_settings, sess)
features = results[0]
features = ((features / 26.0) * 256) - 128
print(features.shape)
features = np.clip(features, -128, 127)
byte_features = features.astype(np.int8)
# print(byte_features)
byte_features.tofile("features_py.bin")