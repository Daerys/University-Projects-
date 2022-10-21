#include "Image.h"

Image::Image(std::vector<std::vector<Image::Pixel>> table)
    : m_table(std::move(table))
{
}

Image::Pixel::Pixel(int red, int green, int blue)
    : m_red(red)
    , m_green(green)
    , m_blue(blue)
{
}
Image::Pixel Image::Pixel::operator-(Image::Pixel p) const
{
    return Image::Pixel(m_red - p.m_red, m_green - p.m_green, m_blue - p.m_blue);
}

Image::Pixel Image::GetPixel(size_t columnId, size_t rowId) const
{
    return m_table[columnId][rowId];
}
