#include <array>
#include <cstring>
#include <fstream>
#include <iostream>

namespace {
bool ExtractLine(std::istream & iStream, std::string & str)
{
    return static_cast<bool>(std::getline(iStream, str));
}

void PrintLines(const std::array<bool, 3> & supress, const std::array<std::size_t, 3> & offset, std::istream & iStream, std::string & str, bool & isOpen, const std::size_t column)
{
    if (!supress[column]) {
        for (std::size_t i = 0; i < offset[column]; i++) {
            std::cout << "\t";
        }
        std::cout << str << std::endl;
    }
    isOpen = ExtractLine(iStream, str);
}

void Sorter(std::istream & iStream1, std::istream & iStream2, const std::array<bool, 3> & supress, const std::array<std::size_t, 3> & offset)
{
    std::string streamStr1, streamStr2;
    bool firstOpen = ExtractLine(iStream1, streamStr1);
    bool secondOpen = ExtractLine(iStream2, streamStr2);
    do {
        while (streamStr1.compare(streamStr2) == 0 && firstOpen && secondOpen) {
            PrintLines(supress, offset, iStream1, streamStr1, firstOpen, 2);
            secondOpen = ExtractLine(iStream2, streamStr2);
        }
        while (firstOpen && (streamStr1.compare(streamStr2) < 0 || !secondOpen)) {
            PrintLines(supress, offset, iStream1, streamStr1, firstOpen, 0);
        }
        while (secondOpen && (streamStr1.compare(streamStr2) > 0 || !firstOpen)) {
            PrintLines(supress, offset, iStream2, streamStr2, secondOpen, 1);
        }
    } while (firstOpen || secondOpen);
}

void SetOffsetSupress(char * const * argv, std::array<std::size_t, 3> & offset, std::array<bool, 3> & supress)
{
    std::string_view option{argv[1]};
    for (char a : option) {
        if ('1' <= a && a <= '3') {
            supress[a - '1'] = true;
            for (std::size_t i = a - '1'; i < 3; i++) {
                offset[i]--;
            }
        }
    }
}
} // namespace

int main(int argc, char * argv[])
{
    std::array<bool, 3> supress{false, false, false};
    std::array<std::size_t, 3> offset{0, 1, 2};
    std::size_t index = 1;
    if (argc == 4) {
        index++;
        SetOffsetSupress(argv, offset, supress);
    }
    std::istream * iStream1 = &std::cin;
    std::istream * iStream2 = &std::cin;
    std::string_view fileName{argv[index]};
    if ("-" != fileName) {
        std::ifstream fileStream1(argv[index++]);
        fileName = {argv[index]};
        iStream1 = &fileStream1;
        if ("-" != fileName) {
            std::ifstream fileStream2(argv[index]);
            iStream2 = &fileStream2;
            Sorter(*iStream1, *iStream2, supress, offset);
            return 0;
        }
        Sorter(*iStream1, *iStream2, supress, offset);
        return 0;
    }
    std::ifstream fileStream2(argv[++index]);
    iStream2 = &fileStream2;
    Sorter(*iStream1, *iStream2, supress, offset);
}
