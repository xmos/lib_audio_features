import speech_commands.input_data as id

model_settings = {
    "fingerprint_size" : 10,
    "fingerprint_width" : 1,
    "desired_samples" : 240,
    "window_size_samples" : 512,
    "window_stride_samples" : 240,
    "sample_rate" : 16000,
    "preprocess" : 'average',
}
summaries_dir = "."

my_ap = id.AudioProcessor(
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

my_ap.prepare_processing_graph(model_settings, summaries_dir)