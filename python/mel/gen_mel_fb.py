import sys
import numpy as np
import os

import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt

from feature_utils import mel_filterbank

PLOTTING = 1  # turn on/off plotting

VAD_MEL_MAX = 16777216

def print_full_mel(fbank):
    print(fbank.shape)
    old_nz = 0
    for bank in fbank.tolist():
        num_zeros = 0
        for value in bank:
            if value != 0.0:
                break;
            num_zeros += 1

        print(num_zeros, num_zeros - old_nz, bank[:40])
        old_nz = num_zeros


def mel_to_c_convert(mel_fb_py_scaled):
    ###################################### Convert Python FB to C FB format ############################################
    # prepare format of filters to match C code table format
    # select every 2nd filter (the rest will be reconstructed from the existing filters!)
    # remove all zeros except one in front of each non-zero value in each filter
    new_fb = [[] for x in range(mel_fb_py_scaled.shape[0])]
    for idx in range(mel_fb_py_scaled.shape[0]):
        tmp = [0] + [int(x) for x in mel_fb_py_scaled[idx] if x != 0]
        new_fb[idx].append(tmp)
    new_list = np.array([x[0] for x in new_fb])
    new_list = [x for k, x in enumerate(new_list) if k % 2 == 0]

    return new_list



################################################### Parameter settings #################################################
Fs = 16000
nfft = 64
n_filters = 6
f_low_list = [0, 31.25, 62.5, 93.75, 125, 155, 156.25, 187, 249, 280]
dir2save = os.path.join(os.path.dirname(__file__), "FB_tables")
if not os.path.exists(dir2save):
    os.makedirs(dir2save)


############################################## Compute filterbank ######################################################
for fdx, f_low in enumerate(f_low_list):
    # compute FB
    mel_fb_py = mel_filterbank(Fs=Fs, nfft=nfft, n_filters=n_filters, f_low=f_low)

    # rescale FB
    mel_fb_py_scaled = mel_fb_py * VAD_MEL_MAX

    # add half of the last filter
    idx_start = np.where(mel_fb_py_scaled[n_filters - 1] == np.max(mel_fb_py_scaled[n_filters - 1]))[0][0]
    new_mel_filter = np.zeros(nfft // 2 + 1)
    new_mel_filter[idx_start:] = VAD_MEL_MAX - mel_fb_py_scaled[n_filters - 1][idx_start:]

    # add extra filter at the end and update FB (needed for C code)
    mel_fb_py_scaled = np.vstack((mel_fb_py_scaled, new_mel_filter.T))

    ######################################## Save full Python filterbank to file #######################################
    # save complete filter to file (easy to copy-paste in C code)
    file2save = os.path.join(dir2save, "FB_full_" + str(f_low) + "Hz.npy")
    np.save(file2save, mel_fb_py_scaled)


    new_list = mel_to_c_convert(mel_fb_py_scaled)
    print(f_low, new_list)

    ############################################# Save C filterbank to file ############################################
    # save filter to file (easy to copy-paste in C code)
    with open(os.path.join(dir2save, "FB_new_" + str(f_low) + "Hz.txt"), "w") as f:
        for idx, x in enumerate(new_list):
            new = [str(y) + ", " for y in x]
            f.write("%s" % " ".join(new))
            f.write(" # %d\n" % idx)

    ############################################### Plot filterbanks ###################################################
    if PLOTTING:
        fig, axes = plt.subplots(1, ncols=1, figsize=[16, 6])
        fig.canvas.set_window_title("f_low = " + str(f_low))

        # axes.plot(mel_fb_c[str(f_low)], "b", label="FB_c")  # C filterbank
        flat_new_list = [item for sublist in new_list for item in sublist]
        print(flat_new_list)
        # print(mel_fb_c[str(f_low)])
        axes.plot(flat_new_list, "b", label="FB_c")  # C filterbank
        for idx in range(n_filters):  # Python filterbank
            if idx == 0:
                axes.plot(mel_fb_py_scaled[idx], "r--", label="FB_py")
            else:
                axes.plot(mel_fb_py_scaled[idx], "r--")

        axes.set_xlim(0, nfft / 2 + 1)
        axes.legend(loc="upper right")

        # save to file
        plt.savefig(os.path.join(dir2save, "FB_new_" + str(f_low) + "Hz.png"), dpi=200)

        # close figure
        plt.close()