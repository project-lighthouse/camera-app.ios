#import "bridge.h"
#include <opencv2/opencv.hpp>

#include "lighthouse.hpp"

using namespace cv;

@implementation Bridge
- (UIImage *)contours:(UIImage *)input {
    Lighthouse lighthouse;

    cv::Mat outputMatrix;
    lighthouse.contours([self imageToMatrix:input], outputMatrix);

    return [self matrixToImage:outputMatrix andImageOrientation:[input imageOrientation]];
}

-(cv::Mat)imageToMatrix:(UIImage *) image {
    CGImageRef imageRef = image.CGImage;

    const int srcWidth = (int) CGImageGetWidth(imageRef);
    const int srcHeight = (int) CGImageGetHeight(imageRef);

    CGDataProviderRef provider = CGImageGetDataProvider(imageRef);
    CFDataRef data = CGDataProviderCopyData(provider);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    cv::Mat rgbaContainer(srcHeight, srcWidth, CV_8UC4);
    CGContextRef context = CGBitmapContextCreate(rgbaContainer.data,
            srcWidth,
            srcHeight,
            8,
            4 * srcWidth,
            colorSpace,
            kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big);

    CGContextDrawImage(context, CGRectMake(0, 0, srcWidth, srcHeight), imageRef);

    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
    CFRelease(data);

    cv::Mat matrix;
    cv::cvtColor(rgbaContainer, matrix, cv::COLOR_RGBA2BGRA);

    return matrix;
}


- (UIImage *)matrixToImage:(const cv::Mat &)matrix andImageOrientation:(UIImageOrientation)orientation {
    cv::Mat rgbaView;

    if (matrix.channels() == 3) {
        cv::cvtColor(matrix, rgbaView, cv::COLOR_BGR2RGBA);
    } else if (matrix.channels() == 4) {
        cv::cvtColor(matrix, rgbaView, cv::COLOR_BGRA2RGBA);
    } else if (matrix.channels() == 1) {
        cv::cvtColor(matrix, rgbaView, cv::COLOR_GRAY2RGBA);
    }

    NSData *data = [NSData dataWithBytes:rgbaView.data length:rgbaView.elemSize() * rgbaView.total()];

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef) data);

    // Creating CGImage from cv::Mat
    CGImageRef imageRef = CGImageCreate(
            // Image width.
            rgbaView.cols,
            // Image height.
            rgbaView.rows,
            // Bits per component.
            8,
            // Bits per pixel.
            8 * rgbaView.elemSize(),
            // Bytes per row.
            rgbaView.step.p[0],
            // Color space.
            colorSpace,
            // Bitmap info.
            kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big,
            // CGDataProvider reference.
            provider,
            // Decode.
            NULL,
            // Indicates whether we should interpolate.
            false,
            // Intent.
            kCGRenderingIntentDefault
    );

    // Getting UIImage from CGImage
    UIImage *finalImage = [UIImage imageWithCGImage:imageRef scale:1 orientation:orientation];

    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);

    return finalImage;
}
@end
