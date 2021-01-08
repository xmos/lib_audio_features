from tensorflow.python.ops import gen_audio_ops as audio_ops
import tensorflow as tf
# from tensorflow.python import raw_ops
import numpy as np
from tensorflow.lite.experimental.microfrontend.python.ops import audio_microfrontend_op as frontend_op

model_settings = {
    "fingerprint_size" : 10,
    "fingerprint_width" : 10, #N DCTs
    "average_window_width" : 6, #256 - 256/6 = 43 groups
    "desired_samples" : 240,
    "window_size_samples" : 512,
    "window_stride_samples" : 240,
    "sample_rate" : 16000,
    "preprocess" : 'micro',
}

input_data = np.random.random_sample((1000000, 1))
print(input_data.shape, input_data)

""" From input_data.py
  if model_settings['preprocess'] == 'average':
    features_min = 0.0
    features_max = 127.5
  elif model_settings['preprocess'] == 'mfcc':
    features_min = -247.0
    features_max = 30.0
  elif model_settings['preprocess'] == 'micro':
    features_min = 0.0
"""


def get_features(model_settings):
    if model_settings['preprocess'] == 'micro':
        window_size_ms = (model_settings['window_size_samples'] *
                          1000) / model_settings['sample_rate']
        window_step_ms = (model_settings['window_stride_samples'] *
                          1000) / model_settings['sample_rate']
        int16_input = tf.cast(tf.multiply(input_data, 32768), tf.int16)
        # print(int16_input.shape)

        # https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/experimental/microfrontend/python/ops/audio_microfrontend_op.py
        micro_frontend = frontend_op.audio_microfrontend(
            int16_input,
            sample_rate=model_settings['sample_rate'],
            window_size=window_size_ms,
            window_step=window_step_ms,
            num_channels=model_settings['fingerprint_width'],
            out_scale=1,
            out_type=tf.float32)
        output = tf.multiply(micro_frontend, (10.0 / 256.0))
        return output

    elif model_settings['preprocess'] == 'mfcc':
        # https://www.tensorflow.org/api_docs/python/tf/raw_ops/AudioSpectrogram
        spectrogram = audio_ops.audio_spectrogram(
                  input_data,
                  window_size=model_settings['window_size_samples'],
                  stride=model_settings['window_stride_samples'],
                  magnitude_squared=True)
        output = audio_ops.mfcc(
                spectrogram,
                model_settings['sample_rate'],
                dct_coefficient_count=model_settings['fingerprint_width'])
        return output[0,:,:] #just return channel 0 as 2D tensor

    elif model_settings['preprocess'] == 'average':
        spectrogram = audio_ops.audio_spectrogram(
                  input_data,
                  window_size=model_settings['window_size_samples'],
                  stride=model_settings['window_stride_samples'],
                  magnitude_squared=True)
        output = tf.nn.pool(
                  input=tf.expand_dims(spectrogram, -1),
                  window_shape=[1, model_settings['average_window_width']],
                  strides=[1, model_settings['average_window_width']],
                  pooling_type='AVG',
                  padding='SAME')
        return output[0,:,:,0] #just return channel 0 as 2D tensor

    else:
        raise ValueError(f'Unknown model setting: {model_settings["preprocess"]}')


for preprocess_mode in ("average", "micro", "mfcc"):
    model_settings["preprocess"] = preprocess_mode
    print(f"Using mode: {model_settings['preprocess']}")

    output = get_features(model_settings)
    print(output.shape, output.dtype)
    print(tf.reduce_max(output))