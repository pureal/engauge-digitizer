#ifndef POINT_MATCH_ALGORITHM_H
#define POINT_MATCH_ALGORITHM_H

#include "fftw3.h"
#include "PointMatchTriplet.h"
#include <QList>
#include <QPoint>

class DocumentModelPointMatch;
class QImage;
class QPixmap;

typedef QList<PointMatchTriplet> PointMatchList;

/// Algorithm returning a list of points that match the specified point. This returns a list of matches, from best to worst.
/// This is executed in a separate QThread so the gui thread is not blocked
class PointMatchAlgorithm
{
 public:
  /// Single constructor
  PointMatchAlgorithm(const QList<QPoint> &samplePointPixels,
                      const QImage &imageProcessed,
                      const DocumentModelPointMatch &modelPointMatch,
                      const QList<QPoint> &pointsExisting);

 private:
  PointMatchAlgorithm();

  enum PixelStates
  {
    PixelOff, // pixel is off
    PixelOnUnscanned, // pixel is on and has not yet been scanned
    PixelOnScanned // pixel is on and has been scanned and assigned to a maxima, so ignore it
  };

  // Allocate memory for an image array and phase array pair before calculations
  void allocateMemory(double** array,
                      fftw_complex** arrayPrime,
                      int width,
                      int height);

  // Find each local maxima that is the largest value in a region that is:
  //   1. as big as the the sample
  //   2. centered about that local maxima
  void assembleLocalMaxima(double* image,
                           double* sample,
                           double* convolution,
                           PointMatchList& listCreated,
                           int width,
                           int height,
                           int sampleXCenter,
                           int sampleYCenter,
                           int sampleXExtent,
                           int sampleYExtent);

  // Compute convolution in image space from phase space image and sample arrays
  void computeConvolution(fftw_complex* imagePrime,
                          fftw_complex* samplePrime,
                          int width,
                          int height,
                          double** convolution);

  // In-place replacement of matrix by its complex conjugate
  void conjugateMatrix(int width,
                       int height,
                       fftw_complex* matrix);

  // Correlate sample and image around specified screen location
  double correlation(double* image,
                     double* sample,
                     int width,
                     int height,
                     int x,
                     int y,
                     int sampleXCenter,
                     int sampleYCenter,
                     int sampleXExtent,
                     int sampleYExtent);

  // Load image and imagePrime arrays
  void loadImage(const QImage &imageProcessed,
                 const DocumentModelPointMatch &modelPointMatch,
                 const QList<QPoint> &pointsExisting,
                 int width,
                 int height,
                 double** image,
                 fftw_complex** imagePrime);

  // Load sample and samplePrime arrays, and compute center location and extent
  void loadSample(const QList<QPoint> &samplePointPixels,
                  int width,
                  int height,
                  double** sample,
                  fftw_complex** samplePrime,
                  int* sampleXCenter,
                  int* sampleYCenter,
                  int* sampleXExtent,
                  int* sampleYExtent);

  // Multiply corresponding elements of two matrices into a third matrix
  void multiplyMatrices(int width,
                        int height,
                        fftw_complex* in1,
                        fftw_complex* in2,
                        fftw_complex* out);
    
  // Given an original array length, this method returns an array length that includes enough padding so that the
  // array length equals 2^a * 3^b * 5^c * 7^d, which optimizes the fft performance. Typical memory penalties are
  // less than 6% to get a cpu performance increase of 0% to roughly 100% or 200%
  int optimizeLengthForFft(int originalLength);

  // Populate sample array with sample image
  void populateSampleArray(const QList<QPoint> &samplePointPixels,
                           int width,
                           int height,
                           double** sample,
                           int* sampleXCenter,
                           int* sampleYCenter,
                           int* sampleXExtent,
                           int* sampleYExtent);

  // Populate image array with processed image
  void populateImageArray(const QImage &imageProcessed,
                          int width, int height,
                          double** image);

  // Release memory for one array after finishing calculations
  void releaseImageArray(double* array);
  void releasePhaseArray(fftw_complex* array);

  // Prevent duplication of existing points. this function returns the number of pixels removed
  void removePixelsNearExistingPoints(double* image,
                                      int imageWidth,
                                      int imageHeight,
                                      const QList<QPoint> &pointsExisting,
                                      int pointSeparation);

  // Correlate the sample point with the image, returning points in list that is sorted by correlation
  void scanImage(bool* sampleMaskArray,
                 int sampleMaskWidth,
                 int sampleMaskHeight,
                 int sampleXCenter,
                 int sampleYCenter,
                 const DocumentModelPointMatch &modelPointMatch,
                 int* imageArray,
                 int imageWidth,
                 int imageHeight,
                 PointMatchList* pointsCreated);

  // Bounds of region around sample center
  int xDeltaMax(int sampleXCenter, int sampleXExtent);
  int xDeltaMin(int sampleXCenter);
  int yDeltaMax(int sampleYCenter, int sampleYExtent);
  int yDeltaMin(int sampleYCenter);
};

#endif // POINT_MATCH_ALGORITHM_H