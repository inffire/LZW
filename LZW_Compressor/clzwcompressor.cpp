#include "clzwcompressor.h"

CLZWCompressor::CLZWCompressor()
{
    _libraryAddressLength = 1;
    _libraryLength = pow(255, _libraryAddressLength);
}

CLZWCompressor::CLZWCompressor(int libraryAddressLength)
{
    _libraryAddressLength = 1;
    if(libraryAddressLength > 0 && libraryAddressLength < 4)
        _libraryAddressLength = libraryAddressLength;
    _libraryLength = pow(255, _libraryAddressLength);
}

size_t CLZWCompressor::compress(std::vector<uchar>* input, std::vector<uchar>* output)
{
    size_t file_size = input->size();
    if(!(file_size > 0))
        return 0;

    output->clear();

    uint        cursor_position = 0;
    char        cursor_word = 0;
    int         current_library_address = 0;
    int         library_address = 0;
    QByteArray  word_to_find;
    bool        address_found = false;

    byteArrayLibrary.clear();
    byteArrayLibrary.append(QByteArray(_libraryAddressLength, 0));

    time_t t = clock();

    do
    {
        cursor_word = 0;
        word_to_find.clear();
        current_library_address = 1;
        library_address = 0;
        address_found = false;
        do
        {
            cursor_word = input->at(cursor_position);
            word_to_find.append(cursor_word);
            current_library_address = byteArrayLibrary.indexOf(word_to_find, current_library_address);
            if(current_library_address > 0)
            {
                library_address = current_library_address;
                if(cursor_position == file_size - 1)
                {
                    word_to_find.clear();
                    cursor_word = 0;
                    address_found = false;
                }
                else
                {
                    cursor_position++;
                    address_found = true;
                }
            }
            else
            {
                address_found = false;
            }
        }
        while(address_found);
        for(char b = _libraryAddressLength - 1; b >= 0; --b)
            output->push_back((library_address >> (b * 8)) & 0xFF);
        output->push_back(cursor_word);

        if(byteArrayLibrary.count() < _libraryLength)
        {
            byteArrayLibrary.append(word_to_find);
            emit library_capacity_changed((int)(byteArrayLibrary.length() / _libraryLength * 100.0f));
        }

        emit passed_process((int)(1 + cursor_position / (float)file_size * 100.0f));
        cursor_position++;
    }
    while(cursor_position < file_size);
    qDebug() << clock() - t << "ms" << output->size() << "bytes @" << (float)file_size/output->size() << "x";
    return output->size();
}
