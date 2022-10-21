#include "SeamCarver.h"

#include <algorithm>
#include <cmath>

SeamCarver::SeamCarver(Image image)
    : m_image(std::move(image))
{
}

const Image & SeamCarver::GetImage() const
{
    return m_image;
}

size_t SeamCarver::GetImageWidth() const
{
    return m_image.m_table.size();
}

size_t SeamCarver::GetImageHeight() const
{
    return (GetImageWidth()) ? m_image.m_table[0].size() : 0;
}

double SeamCarver::GetPixelEnergy(size_t x, size_t y) const
{
    return std::sqrt(GetDifference(m_image.GetPixel((x + GetImageWidth() - 1) % GetImageWidth(), y), m_image.GetPixel((x + 1) % GetImageWidth(), y)) +
                     GetDifference(m_image.GetPixel(x, (y + GetImageHeight() - 1) % GetImageHeight()), m_image.GetPixel(x, (y + 1) % GetImageHeight())));
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    return GenerateSeam(GenerateTable(true), true);
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    return GenerateSeam(GenerateTable(false), false);
}

void SeamCarver::RemoveHorizontalSeam(const Seam & seam)
{
    const std::size_t width = GetImageWidth();
    if (width == 0 || GetImageHeight() == 0) {
        return;
    }
    if (GetImageHeight() == 1) {
        m_image.m_table.clear();
        return;
    }
    for (std::size_t i = 0; i < width; i++) {
        m_image.m_table[i].erase(m_image.m_table[i].begin() + seam[i]);
    }
}

void SeamCarver::RemoveVerticalSeam(const Seam & seam)
{
    const std::size_t height = GetImageHeight();
    const std::size_t width = GetImageWidth();
    if (width == 0 || height == 0) {
        return;
    }
    if (width == 1) {
        m_image.m_table.clear();
        return;
    }
    for (std::size_t i = 0; i < height; i++) {
        for (std::size_t j = seam[i] + 1; j < width; j++) {
            m_image.m_table[j - 1][i] = m_image.m_table[j][i];
        }
    }
    m_image.m_table.pop_back();
}

double SeamCarver::GetDifference(Image::Pixel previous, Image::Pixel next) const
{
    Image::Pixel dx = previous - next;
    return std::pow(dx.m_red, 2) + std::pow(dx.m_green, 2) + std::pow(dx.m_blue, 2);
}

std::vector<std::vector<SeamCarver::ETPixel>> SeamCarver::GenerateTable(bool horizontal) const
{
    const std::size_t xRange = (horizontal ? GetImageWidth() : GetImageHeight());
    const std::size_t yRange = (horizontal ? GetImageHeight() : GetImageWidth());
    std::vector<std::vector<ETPixel>> result(GetImageWidth(), std::vector<ETPixel>(GetImageHeight()));
    for (std::size_t x = 0; x < xRange; x++) {
        for (std::size_t y = 0; y < yRange; y++) {
            ETPixel & current = (horizontal ? result[x][y] : result[y][x]);
            if (x != 0) {
                ETPixel & a = (horizontal ? result[x - 1][y] : result[y][x - 1]);
                ETPixel & b = (horizontal ? ((y > 0) ? result[x - 1][y - 1] : a) : ((y > 0) ? result[y - 1][x - 1] : a));
                ETPixel & c = (horizontal ? ((y < yRange - 1) ? result[x - 1][y + 1] : a)
                                          : ((y < yRange - 1) ? result[y + 1][x - 1] : a));
                current.energy = a.energy;
                current.previous = &a;
                if (current.energy >= b.energy) {
                    current.energy = b.energy;
                    current.previous = &b;
                }
                if (current.energy > c.energy) {
                    current.energy = c.energy;
                    current.previous = &c;
                }
            }
            current.energy += (horizontal ? GetPixelEnergy(x, y) : GetPixelEnergy(y, x));
            current.current_index = y;
        }
    }
    return result;
}

SeamCarver::Seam SeamCarver::GenerateSeam(std::vector<std::vector<ETPixel>> table, bool horizontal) const
{
    const std::size_t width = GetImageWidth();
    const std::size_t height = GetImageHeight();
    Seam result;
    if (!width || !height) {
        return result;
    }
    result.reserve(horizontal ? width : height);
    ETPixel minimal = (horizontal ? table[width - 1][0] : table[0][height - 1]);
    for (std::size_t i = 1; i < (horizontal ? height : width); i++) {
        ETPixel & current = (horizontal ? table[width - 1][i] : table[i][height - 1]);
        if (current.energy < minimal.energy) {
            minimal = current;
        }
    }
    ExtractSeam(minimal, result);
    std::reverse(result.begin(), result.end());
    return result;
}

void SeamCarver::ExtractSeam(ETPixel pixel, SeamCarver::Seam & result) const
{
    EnergyTablePixel * current = &pixel;
    while (current != nullptr) {
        result.push_back(current->current_index);
        current = current->previous;
    }
}