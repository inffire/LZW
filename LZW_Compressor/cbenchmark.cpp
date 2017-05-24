#include "cbenchmark.h"
#include "ui_cbenchmark.h"

CBenchmark::CBenchmark(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CBenchmark)
{
    ui->setupUi(this);

    ui->pushButton_start->setEnabled(false);
    _input = new std::vector<uchar>;
    connect(ui->pushButton_openfile, SIGNAL(clicked(bool)), this, SLOT(onOpenFileButtonClicked(bool)));
    connect(ui->pushButton_generatedummy, SIGNAL(clicked(bool)), this, SLOT(onGenerateSequenceButtonClicked(bool)));
    connect(ui->pushButton_start, SIGNAL(clicked(bool)), this, SLOT(onStartBenchmarkButtonClicked(bool)));
}

CBenchmark::~CBenchmark()
{
    delete _input;
    delete ui;
}

void CBenchmark::log(QString line)
{
    QString time = QTime::currentTime().toString(QString("hh:mm:ss.zzz"));
    time.append(" ").append(line).append("\n");
    ui->plainTextEdit_logshow->setPlainText(ui->plainTextEdit_logshow->toPlainText() + time);
}

void CBenchmark::onOpenFileButtonClicked(bool)
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Open"), "", "");
    if(file_path.isEmpty()) return;

    QStringList list_file_name = file_path.split(QString("/"));
    QString file_name = list_file_name[list_file_name.length() - 1];

    QFile file;
    file.setFileName(file_path);
    if(file.open(QIODevice::ReadOnly))
    {
        ui->label_file->setText(file_name);
        QByteArray d = file.readAll();
        file.close();
        _input->clear();
        char* begin = reinterpret_cast<char*>(d.data());
        char* end = begin + d.length();
        _input->assign(begin, end);
        ui->label_filesize->setText(QString("File ") + QString::number(_input->size()/1024) + QString(" KB"));
    }
    ui->pushButton_start->setEnabled(true);
    log(QString("\"").append(file_path).append(QString("\"")).append(" loaded."));
}

void CBenchmark::onGenerateSequenceButtonClicked(bool)
{
    int idx = ui->comboBox_dummylength->currentIndex();
    if(idx < 0 || idx > 8)
        idx = 0;

    size_t dummy_length = 512 * 1024 * pow(2, idx);

    float dummy_chance = ui->spinBox_dummyonechance->value();
    if(dummy_chance < 0 || dummy_chance > 100)
        dummy_chance = 0;

    log(QString("Generating dummy sequence %1 bytes %2% chance..").arg(QString::number(dummy_length), QString::number(dummy_chance)));

    _input->clear();
    srand(QTime::currentTime().msecsSinceStartOfDay());
    for(size_t byte = 0; byte < dummy_length; ++byte)
    {
        char dummy_byte = '\0';
        for(int bit = 0; bit < 8; ++bit)
            dummy_byte ^= (-( rand() % 100 < dummy_chance) ^ dummy_byte) & (1 << bit);
        _input->push_back(dummy_byte);
    }
    ui->label_filesize->setText(QString("Dummy ") + QString::number(_input->size()/1024) + QString(" KB"));
    ui->pushButton_start->setEnabled(true);
    log(QString("Done."));
}

void CBenchmark::onStartBenchmarkButtonClicked(bool)
{
    if(!_input->size())
        return;

    int benchmark_type, benchmark_passes;
    benchmark_type = ui->comboBox_benchmarktype->currentIndex();
    benchmark_passes = ui->comboBox_passescount->currentIndex() + 1;

    std::vector<uchar> output;
    time_t t(0), ct(0), dt(0);

    CLZWCompressor compressor;
    CLZWDecompressor decompressor;

    int libraryaddresslength = ui->spinBox_libraryaddresslength->text().toInt();
    if(libraryaddresslength < 1 || libraryaddresslength > 3)
        libraryaddresslength = 1;

    compressor.set_library_address_length(libraryaddresslength);
    decompressor.set_library_address_length(libraryaddresslength);

    connect(&compressor, SIGNAL(passed_process(int)), ui->progressBar_local, SLOT(setValue(int)));
    connect(&decompressor, SIGNAL(passed_process(int)), ui->progressBar_local, SLOT(setValue(int)));

    int global_progress = 0;
    ui->progressBar_global->setValue(global_progress);
    ui->label_result_compression_rate_r->setText(QString("1.0x"));
    ui->label_result_compression_r->setText(QString("0ms"));
    ui->label_result_decompression_r->setText(QString("0ms"));

    switch (benchmark_type) {
    case 0:
        for(int c = 0; c < benchmark_passes; ++c)
        {
            t = clock();
            compressor.compress(_input, &output);
            ct += clock() - t;

            t = clock();
            decompressor.decompress(&output, _input);
            dt += clock() - t;

            global_progress = (int)(100 * (c+1) / (float)benchmark_passes);
            ui->progressBar_global->setValue(global_progress);
        }
        break;
    case 1:
        for(int c = 0; c < benchmark_passes; ++c)
        {
            t = clock();
            compressor.compress(_input, &output);
            ct += clock() - t;

            global_progress = (int)(100 * (c+1) / (float)benchmark_passes);
            ui->progressBar_global->setValue(global_progress);
        }
        break;
    case 2:
        compressor.compress(_input, &output);
        for(int c = 0; c < benchmark_passes; ++c)
        {
            t = clock();
            decompressor.decompress(&output, _input);
            dt += clock() - t;

            global_progress = (int)(100 * (c+1) / (float)benchmark_passes);
            ui->progressBar_global->setValue(global_progress);
        }
        break;
    default:
        break;
    }

    float compression_rate = 1.0;
    if(_input->size() && output.size())
        compression_rate = _input->size() / (float)output.size();

    float total = _input->size() * compression_rate / (dt + ct + 1) * benchmark_passes * 0.01;
    ui->label_result_total_r->setText(QString::number(total));
    ui->label_result_compression_rate_r->setText(QString("%1x").arg(QString::number(compression_rate)));
    ui->label_result_compression_r->setText(QString("%1ms").arg(QString::number(ct/benchmark_passes)));
    ui->label_result_decompression_r->setText(QString("%1ms").arg(QString::number(dt/benchmark_passes)));
    log(QString("Benchmark complete. Compression %1ms, decompression %2ms. Total %3.").arg(QString::number(ct/benchmark_passes), QString::number(dt/benchmark_passes), QString::number(total)));
}
