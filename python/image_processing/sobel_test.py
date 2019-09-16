import cv2
import numpy as np
import matplotlib.pyplot as plt


def norm_minmax(img: np.ndarray):
    img = img.astype(np.float32)
    img = (img - img.min()) / (img.max() - img.min())

    return img


def show(name: str, img: np.ndarray):
    img = norm_minmax(img) * 255

    cv2.imshow(name, img.astype(np.uint8))

    cv2.waitKey()


def laplasian(img):
    lap = cv2.Laplacian(img, cv2.CV_16S, 3)
    lap = cv2.resize(lap, (800, 600))

    show("laplasian", lap)

    lap = cv2.normalize(lap, None, 0, 255, cv2.NORM_MINMAX)

    plt.hist(lap.reshape(-1).astype(np.uint8), normed=True, bins=255)
    plt.show()


def sobel(img):
    sobelx = cv2.Sobel(img, cv2.CV_32F, 1, 0, ksize=3)
    sobely = cv2.Sobel(img, cv2.CV_32F, 0, 1, ksize=3)

    sobel_xy = cv2.addWeighted(sobely, 0.5, sobelx, 0.5, 0)

    # sobel_xy = cv2.resize(sobel_xy, (800, 600))

    sobel_xy = norm_minmax(sobel_xy) * 255
    # sobel_xy -= np.mean(sobel_xy)
    # sobel_xy = np.where(sobel_xy < 0, 0, sobel_xy)

    show("sobelx", sobel_xy)

    # plt.hist(sobel_xy.reshape(-1).astype(np.uint8), normed=True, bins=255)
    # plt.show()

    mean, std = np.mean(sobel_xy), np.std(sobel_xy)

    cv_mean, cv_std = cv2.meanStdDev(sobel_xy)
    # min, max = sobel_xy.min, sobel_xy.max

    print(mean, std)

    # cond = sobel_xy.min() < (sobel_xy < mean - std)
    right_sigma1 = sobel_xy < mean - std
    right_sigma2 = sobel_xy > mean - 2 * std
    right_cond = np.logical_and(right_sigma1, right_sigma2)
    show("r_cond", right_cond)

    left_sigma1 = sobel_xy > mean + std
    left_sigma2 = sobel_xy < mean + 2 * std
    left_cond = np.logical_and(left_sigma1, left_sigma2)
    show("l_cond", left_cond)

    cond = np.logical_or(right_cond, left_cond)

    filtered3 = np.where(cond, sobel_xy, 0)

    plt.hist(filtered3.reshape(-1).astype(np.uint8), normed=True, bins=255)
    plt.show()

    show("filtered3", filtered3)

    nonZeroCount = np.sum(filtered3 > 0) / (filtered3.shape[0] *filtered3.shape[1]) * 100
    print(nonZeroCount)

    cv2.waitKey()


if __name__ == '__main__':
    img = cv2.imread('c:/work/imageDataSet/Surface Roughness/Surface Roughness/Tile_001_00157.png',
                     cv2.IMREAD_GRAYSCALE)
    sobel(img)
    # laplasian(img)
