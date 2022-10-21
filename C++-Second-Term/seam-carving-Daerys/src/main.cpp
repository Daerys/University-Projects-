#include "Image.h"
#include "SeamCarver.h"

#include <fstream>
#include <iostream>

namespace {
std::vector<std::vector<Image::Pixel>> ReadImageFromCSV(std::ifstream & input)
{
    size_t width, height;
    input >> width >> height;
    std::vector<std::vector<Image::Pixel>> table;
    for (size_t columnId = 0; columnId < width; ++columnId) {
        std::vector<Image::Pixel> column;
        for (size_t rowId = 0; rowId < height; ++rowId) {
            size_t red, green, blue;
            input >> red >> green >> blue;
            column.emplace_back(red, green, blue);
        }
        table.emplace_back(std::move(column));
    }
    return table;
}

void WriteImageToCSV(const SeamCarver & carver, std::ofstream & output)
{
    const size_t width = carver.GetImageWidth();
    const size_t height = carver.GetImageHeight();
    output << width << " " << height << "\n";
    const Image & image = carver.GetImage();
    for (size_t columnId = 0; columnId < width; ++columnId) {
        for (size_t rowId = 0; rowId < height; ++rowId) {
            const Image::Pixel & pixel = image.GetPixel(columnId, rowId);
            output << pixel.m_red << " " << pixel.m_green << " " << pixel.m_blue << std::endl;
        }
    }
}
} // namespace

int main(int argc, char * argv[])
{
    // Check command line arguments
    const size_t expectedAmountOfArgs = 3;
    if (argc != expectedAmountOfArgs) {
        std::cout << "Wrong amount of arguments. Provide filenames as arguments. See example below:\n";
        std::cout << "seam-carving data/tower.csv data/tower_updated.csv" << std::endl;
        return 0;
    }
    // Check csv file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.good()) {
        std::cout << "Can't open source file " << argv[1] << ". Verify that the file exists." << std::endl;
    }
    else {
        const std::vector<Image::Pixel> col0 = {Image::Pixel(0, 0, 0), Image::Pixel(255, 255, 255), Image::Pixel(255, 255, 255), Image::Pixel(255, 255, 255), Image::Pixel(0, 0, 0)};
        const std::vector<Image::Pixel> col1 = {Image::Pixel(0, 0, 0), Image::Pixel(0, 0, 0), Image::Pixel(255, 255, 255), Image::Pixel(0, 0, 0), Image::Pixel(0, 0, 0)};
        const std::vector<Image::Pixel> col2 = {Image::Pixel(0, 0, 0), Image::Pixel(0, 0, 0), Image::Pixel(0, 0, 0), Image::Pixel(0, 0, 0), Image::Pixel(0, 0, 0)};
        const Image image({col2, col1, col2, col0, col2, col1, col0});
        //        const Image image({col2, col2,  col2, col2});
        SeamCarver carver(image);

        for (int i = 0; i < 4; i++) {
            std::vector<size_t> seam = carver.FindVerticalSeam();
            carver.RemoveVerticalSeam(seam);
        }
        for (size_t i = 0; i < carver.GetImageHeight(); i++) {
            for (size_t j = 0; j < carver.GetImageWidth(); j++) {
                std::cout << carver.GetPixelEnergy(j, i) << " (column: " << j << "; " << i << ")" << std::endl;
            }
        }
        auto imageSource = ReadImageFromCSV(inputFile);
        //        //        Image image({});
        //        SeamCarver carver(/*image*/ std::move(imageSource));
        //        std::cout << "Image: " << carver.GetImageWidth() << "x" << carver.GetImageHeight() << std::endl;
        //        const size_t pixelsToDelete = 150;
        //        for (size_t i = 0; i < pixelsToDelete; ++i) {
        //            std::vector<size_t> seam = carver.FindVerticalSeam();
        //            carver.RemoveVerticalSeam(seam);
        //            //            std::vector<size_t> seam = carver.FindHorizontalSeam();
        //            //            carver.RemoveHorizontalSeam(seam);
        //            std::cout << "width = " << carver.GetImageWidth() << ", height = " << carver.GetImageHeight() << std::endl;
        //        }
        std::ofstream outputFile(argv[2]);
        WriteImageToCSV(carver, outputFile);
        std::cout << "Updated image is written to " << argv[2] << "." << std::endl;
    }
    return 0;
}
/**
TEST(HandMadeTest, ImageHorizontalSeam1xk)
{
    for (int i = 1; i < 1000; i++) {
        std::vector<std::vector<Image::Pixel>>colums;
        colums.reserve(i);
        for (int j = 0; j < i; j ++) {
            colums.push_back(std::vector<Image::Pixel>{Image::Pixel(4, 9, 1)});
        }
        Image image(colums);
        SeamCarver carver(image);

        std::vector<size_t> seam = carver.FindHorizontalSeam();
        ASSERT_EQ(i, seam.size());
        EXPECT_EQ(0, seam.at(0));

        carver.RemoveHorizontalSeam(seam);
        ASSERT_EQ(0, carver.GetImageWidth());
        ASSERT_EQ(0, carver.GetImageHeight());
    }
}
TEST(HandMadeTest, Pic0x0)
{
    const Image image({});
    SeamCarver carver(image);

    std::vector<size_t> seam = carver.FindVerticalSeam();
    ASSERT_EQ(0, seam.size());
    carver.RemoveVerticalSeam(seam);
    ASSERT_EQ(0, carver.GetImageWidth());

    seam = carver.FindHorizontalSeam();
    ASSERT_EQ(0, seam.size());
    carver.RemoveHorizontalSeam(seam);
    ASSERT_EQ(0, carver.GetImageHeight());
}

TEST(HandMadeTest, EraseImage)
{
    const std::vector<Image::Pixel> col0 = { Image::Pixel(0, 0,  0), Image::Pixel(255, 255, 255), Image::Pixel(255, 255, 255), Image::Pixel(255, 255, 255), Image::Pixel(0, 0,  0)};
    const std::vector<Image::Pixel> col1 = { Image::Pixel(0, 0,  0), Image::Pixel(0, 0,  0), Image::Pixel(255, 255, 255), Image::Pixel(0, 0,  0), Image::Pixel(0, 0,  0)};
    const std::vector<Image::Pixel> col2 = { Image::Pixel(0, 0,  0), Image::Pixel(0, 0,  0), Image::Pixel(0,0,0), Image::Pixel(0, 0,  0), Image::Pixel(0, 0,  0)};
    const Image image({col2, col1, col2, col0, col2, col1, col0});
    SeamCarver carver(image);
    for (int i = 0; i < 7; i++) {
        std::vector<size_t> seam = carver.FindVerticalSeam();
        carver.RemoveVerticalSeam(seam);
    }
    ASSERT_EQ(0, carver.GetImageWidth());
    ASSERT_EQ(0, carver.GetImageHeight());
}
*/