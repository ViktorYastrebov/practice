import cv2
import matplotlib.pyplot as plt
import numpy as np

from scipy.signal import argrelextrema
from scipy.ndimage import gaussian_filter1d

# from utils.cv import show, norm_minmax

def norm_minmax(img: np.ndarray):
    img = img.astype(np.float32)
    img = (img - img.min()) / (img.max() - img.min())

    return img


def show(name: str, img: np.ndarray):
    img = norm_minmax(img) * 255

    cv2.imshow(name, img.astype(np.uint8))

    cv2.waitKey()


def preprocess(img: np.ndarray):
    img = norm_minmax(img)

    mask1 = img > np.mean(img) / 1.5
    mask2 = 1 - mask1

    show("mask1", mask1)
    show("mask2", mask2)

    img1 = img * mask1
    img2 = img * mask2

    std1, mean1 = np.std(img1), np.mean(img1)
    std2, mean2 = np.std(img2), np.mean(img2)

    img1 /= std1
    img2 /= std2

    diff = np.abs(mean1 - mean2)
    if mean1 > mean2:
        img2 += diff * mask2
    else:
        img1 += diff * mask1

    img3 = img1 + img2

    return img3


def roughness(img):
    s1 = cv2.Sobel(img, cv2.CV_32F, 1, 0, 5)
    s2 = cv2.Sobel(img, cv2.CV_32F, 0, 1, 5)
    sobel = 0.5 * s1 + 0.5 * s2

    sobel = np.abs(sobel)
    mean, std = np.mean(sobel), np.std(sobel)

    mask1 = sobel > (mean + std)
    mask2 = sobel < (mean - std)
    mask = np.logical_or(mask1, mask2)

    masked = sobel * mask
    non_zero = np.sum(masked > 0)

    rough = non_zero / np.prod(img.shape)

    return rough

def find_picks2(img):
    h = np.histogram(img.astype(np.int32).reshape(-1), bins=255)

    h_data = h[0]
    peaks = argrelextrema(h_data, np.greater)

    plt.hist(img.astype(np.int32).reshape(-1), bins=255)
    plt.show()


def find_pick3(img):
    hist, bin_edges = np.histogram(img.astype(np.int32).reshape(-1), bins=255)

    hist_smoothed = gaussian_filter1d(hist, 30)
    plt.plot(hist)
    plt.plot(hist_smoothed)
    plt.show()

    diff1 = np.diff(hist_smoothed, 1)
    plt.plot(diff1)
    plt.show()

    diff1 = np.where(diff1 <= 0, -1, 1)
    plt.plot(diff1)


    peaks = np.diff(diff1)
    plt.plot(peaks)
    plt.show()

    peaks = peaks == peaks.min()
    return np.where(peaks == 1)


def find_pick_opencv(img):
    hist, bin_edges = np.histogram(img.astype(np.int32).reshape(-1), bins=255)
    # plt.plot(hist)
    # plt.show()
    #hist2d = np.reshape(hist, newshape=)

    hist_median = cv2.medianBlur(hist.astype(np.float32), 21)
    hist_smoothed = cv2.GaussianBlur(hist_median, (21,21), sigmaX=30)
    hist_smoothed = hist_smoothed.reshape(-1)
    plt.plot(hist_smoothed)
    plt.show()

    diff1 = np.diff(hist_smoothed.astype(np.int32))
    diff1 = np.where(diff1 <= 1, -1, 1)
    plt.plot(diff1)
    plt.show()

    peaks = np.diff(diff1)
    plt.plot(peaks)
    plt.show()

    peaks = peaks == peaks.min()
    indexes = np.where(peaks == 1)
    return indexes


def find_picks(img):

    #img = cv2.medianBlur(img, 21)
    #show("blurred_image", img)

    hist_array, bin_edges = np.histogram(img.astype(np.int32).reshape(-1), bins=255)
    #n, bins, patches = plt.hist(img.astype(np.int32).reshape(-1), bins=255)

    diffArr1 = np.diff(hist_array, 1)
    diffArr = np.diff(hist_array, 2)

    plt.hist(img.astype(np.int32).reshape(-1), bins=255)
    plt.plot(diffArr1)
    plt.plot(diffArr)
    plt.show()

    sorted = np.sort(diffArr)
    revert_sorted = np.flip(sorted)

    hist_list = diffArr.tolist()
    idx1 = hist_list.index(revert_sorted[0])
    idx2 = hist_list.index(revert_sorted[1])


    #idx1 = hist_list.index(sorted[0])
    #idx2 = hist_list.index(sorted[1])

    sum = (idx1 + idx2) / 2

    #print(sum)
    #print(f"first = {revert_sorted[0]}, second = {revert_sorted[1]}")
    return sum


if __name__ == '__main__':
    img = cv2.imread("c:/work/imageDataSet/Surface Roughness/Surface Roughness/Tile_001_00131.png",
                     cv2.IMREAD_GRAYSCALE).astype(np.float32)

    # plt.hist(img.astype(np.int32).reshape(-1), bins=255)
    # plt.show()

    # find_picks(img)
    # find_picks2(img)
    # peaks = find_pick3(img)

    find_pick_opencv(img)

    show("orig", img)

    preprocessed = preprocess(img)
    show("preprocessed", preprocessed)

    percent = np.round(roughness(preprocessed), 2)
    print(f"roughness = {percent}")

    plt.show()
