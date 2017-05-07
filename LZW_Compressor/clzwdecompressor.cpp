#include "clzwdecompressor.h"

CLZWDecompressor::CLZWDecompressor()
{
    _libraryAddressLength = 1;
    _libraryLength = pow(255, _libraryAddressLength);
}

CLZWDecompressor::CLZWDecompressor(int libraryAddressLength)
{
    _libraryAddressLength = 1;
    if(libraryAddressLength > 0 && libraryAddressLength < 4)
        _libraryAddressLength = libraryAddressLength;
    _libraryLength = pow(255, _libraryAddressLength);
}

size_t CLZWDecompressor::decompress(std::vector<uchar>& input, std::vector<uchar>& output)
{
    size_t file_size = input.size();
    if(!(file_size > 0))
        return 0;

    output.clear();

    QByteArray  address_to_find;
    QByteArray  new_word;
    uint        cursor_position = 0;

    byteArrayLibrary.clear();
    byteArrayLibrary.append(QByteArray(_libraryAddressLength, 0));

    time_t t = clock();

    do
    {
        address_to_find.clear();
        new_word.clear();
        for(int b = 0; b < _libraryAddressLength; ++b)
        {
            address_to_find.append(input.at(cursor_position + b));
        }
        int addr_int = 0;
        for(int i = 0; i < _libraryAddressLength; i++)
        {
            addr_int += (uchar)address_to_find.at(_libraryAddressLength - 1 - i) * (pow(256, i));
        }
        if(addr_int != 0)
            new_word.append(byteArrayLibrary.at(addr_int));
        if(!((cursor_position + _libraryAddressLength) == (file_size - 1) && (input.at(cursor_position + _libraryAddressLength) == 0)))
            new_word.append(input.at(cursor_position + _libraryAddressLength));

        if(byteArrayLibrary.count() < _libraryLength)
        {
            byteArrayLibrary.append(new_word);
            emit library_capacity_changed((int)(byteArrayLibrary.length() / _libraryLength * 100.0f));
        }

        for(int j = 0; j < new_word.count(); ++j)
            output.push_back((uchar)new_word.at(j));

        emit passed_process((int)(cursor_position / (float)file_size * 100.0f));
        cursor_position += (_libraryAddressLength + 1);
    }
    while(cursor_position < file_size);
    qDebug() << clock() - t << "ms" << output.size() << "bytes @" << (float)output.size()/file_size << "x";
    return output.size();
}
