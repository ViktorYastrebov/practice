import numpy as np
import cv2
import matplotlib.pyplot as plt

if __name__ == '__main__':
    img = cv2.imread('c:/Nanotronics Imaging/Scans/testScan/Scan_001/Tile_00001.png', cv2.IMREAD_GRAYSCALE)
    fft_mat: np.ndarray = np.fft.fft2(img).real
    # fft_mat.real[:] = np.where(fft_mat.real < fft_mat.real.max() / 7000, 0, fft_mat.real)

    img_print = cv2.normalize(np.log(np.abs(np.fft.fftshift(fft_mat.real +1))), None, norm_type=cv2.NORM_MINMAX ) * 255
    plt.imshow(img_print, cmap="jet")
    plt.show()
    cv2.imwrite("FFT.png", img_print)
    cv2.imshow("FFT", img_print)


    img = cv2.normalize(np.fft.ifft2(fft_mat).real, None, norm_type=cv2.NORM_MINMAX)
    cv2.imshow("Img", img)


    # hist = cv2.calcHist([fft_mat], [0], None, [fft_mat.max() - fft_mat.min()], [fft_mat.min(), fft_mat.max()])
    # hist = plt.hist(np.log(fft_mat.reshape(-1)), normed=True, bins=100)
    # plt.plot(hist)
    # plt.xlim([0, 256])

    # plt.show()
    # cv2.imshow("Img", hist)
    cv2.waitKey()

