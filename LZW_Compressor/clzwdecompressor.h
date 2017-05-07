#ifndef CLZWDECOMPRESSOR_H
#define CLZWDECOMPRESSOR_H

#include <time.h>
#include <vector>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QByteArray>

class CLZWDecompressor : public QObject
{
    Q_OBJECT

    int _libraryAddressLength;
    int _libraryLength;
    QList<QByteArray> byteArrayLibrary;

public:
    CLZWDecompressor();
    CLZWDecompressor(int libraryAddressLength);

    inline int  get_library_address_length() { return _libraryAddressLength; }
    inline void set_library_address_length(int v) { ((v > 0) && (v < 4)) ? _libraryAddressLength = v : _libraryAddressLength = 1;
                                                  _libraryLength = pow(255, _libraryAddressLength); }

    size_t decompress(std::vector<uchar>& input, std::vector<uchar>& output);

signals:
    void passed_process(int p);
    void library_capacity_changed(int c);
};

#endif // CLZWDECOMPRESSOR_H
