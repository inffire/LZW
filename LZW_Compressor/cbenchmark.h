#ifndef CBENCHMARK_H
#define CBENCHMARK_H

#include <math.h>
#include <vector>
#include <QTime>
#include <QFile>
#include <QDebug>
#include <QDialog>
#include <QByteArray>
#include <QFileDialog>

#include "clzwcompressor.h"
#include "clzwdecompressor.h"

namespace Ui {
class CBenchmark;
}

class CBenchmark : public QDialog
{
    Q_OBJECT

public:
    explicit CBenchmark(QWidget *parent = 0);
    ~CBenchmark();

private:
    Ui::CBenchmark*         ui;
    std::vector<uchar>*     _input;

    void log(QString line);


private slots:
    void onOpenFileButtonClicked(bool);
    void onGenerateSequenceButtonClicked(bool);
    void onStartBenchmarkButtonClicked(bool);
};

#endif // CBENCHMARK_H
