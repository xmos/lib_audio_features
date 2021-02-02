# Copyright (c) 2020, XMOS Ltd, All rights reserved
# We manually copy this to audio_features/aiot_sdk/tools/ai_tools/third_party/tensorflow/tensorflow/examples
# to allow access to the python model
import setuptools

setuptools.setup(
    name='tensorflow_examples',
    packages=setuptools.find_packages(),
    install_requires=[
    ],
    dependency_links=[
    ],
)
