#include <QDialog>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QList>
#include <QMimeData>
#include <QTranslator>
#include <string.h>
#include "file_archive_window.h"
#include "ui_file_archive_window.h"
#include "utils.h"
#include "text_dialog.h"

FILE_ARCHIVE_WINDOW::FILE_ARCHIVE_WINDOW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::file_archive_window)
{
    QTranslator translator;
    translator.load("file_archive_JP.qm", qApp->applicationDirPath());
    qApp->installTranslator(&translator);

    ui->setupUi(this);
    (void)memset(&archive, 0, sizeof(archive));

    ui->tab_widget->setTabText(0, tr("Make Archive"));
    ui->input_directory_button->setText(tr("Input Directory"));
    ui->output_file_button->setText(tr("Output File"));
    ui->execute_output_button->setText(tr("Output"));

    ui->tab_widget->setTabText(1, tr("Edit"));
    ui->archive_select_button->setText(tr("Open Archive"));
}

FILE_ARCHIVE_WINDOW::~FILE_ARCHIVE_WINDOW()
{
    delete ui;
}

void FILE_ARCHIVE_WINDOW::on_input_directory_button_clicked()
{
    QString directory_name = QFileDialog::getExistingDirectory(this,
        "Input Directory");
    ui->input_directory_path_edit->setText(directory_name);
}

void FILE_ARCHIVE_WINDOW::on_output_file_button_clicked()
{
    QString output_file_name = QFileDialog::getSaveFileName(this, "Output");
    ui->output_file_path_edit->setText(output_file_name);
}

static void AddDirectoryFiles(
    QDir& directory,
    QString path,
    QVector<FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM>& items
)
{
    FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM item;
    QFileInfoList list = directory.entryInfoList();
    QListIterator<QFileInfo> iterator_list(list);

    while(iterator_list.hasNext() != false)
    {
        QFileInfo file_info = iterator_list.next();
        if(file_info.fileName() != ".." && file_info.fileName() != ".")
        {
            if(file_info.isDir() != false)
            {
                QString sub_directory_path = path + file_info.fileName();
                QDir sub_directory(file_info.filePath());
                AddDirectoryFiles(sub_directory,
                                  sub_directory_path, items);
            }
            else if(file_info.isFile() != false)
            {
                item.file_name = path + file_info.fileName();
                item.hash_value = GetStringHashValue((const char*)item.file_name.toLower().toStdString().c_str());
                FILE *fp = fopen(file_info.filePath().toLocal8Bit().toStdString().c_str(), "rb");
                if(fp != NULL)
                {
                    (void)fseek(fp, 0, SEEK_END);
                    item.data_size = (quint32)ftell(fp);
                    item.data_position = 0;
                    items.append(item);
                    (void)fclose(fp);
                }
            }
        }
    }
}

static bool CompareArchiveItem(const FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM& item1, const FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM& item2)
{
    return item1.hash_value < item2.hash_value;
}

static void WriteArchiveData(
    FILE* fp,
    QVector<FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM>& items,
    const QString& target_directory_path
)
{
    quint32 header_size;
    unsigned char data[16];
    quint32 data32;
    quint32 sum_file_size = 0;
    QVectorIterator<FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM> iterator_vector(items);

    // ファイルの数を書き出す
    data32 = (quint32)items.count();
    data[0] = (data32 & 0xFF000000) >> 24;
    data[1] = (data32 & 0x00FF0000) >> 16;
    data[2] = (data32 & 0x0000FF00) >> 8;
    data[3] = (data32 & 0x000000FF);
    (void)fwrite(data, 1, 4, fp);

    header_size = 4;

    // ヘッダのサイズとデータの位置を確定する
    while(iterator_vector.hasNext() != false)
    {
        FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM item = iterator_vector.next();

        // ハッシュ値
        header_size += 4;

        // ファイル名
        header_size += (quint32)(strlen((const char*)item.file_name.toUtf8().toStdString().c_str()) + 1) + 4;

        // ファイルの位置
        header_size += 4;

        // ファイルサイズ
        header_size += 4;
    }

    // 先頭に戻る
    while(iterator_vector.hasPrevious() != false)
    {
        (void)iterator_vector.previous();
    }

    // それぞれのファイル名とハッシュ値を書き出す
    while(iterator_vector.hasNext() != false)
    {
        FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM item = iterator_vector.next();

        // ハッシュ値
        data32 = item.hash_value;
        data[0] = (data32 & 0xFF000000) >> 24;
        data[1] = (data32 & 0x00FF0000) >> 16;
        data[2] = (data32 & 0x0000FF00) >> 8;
        data[3] = (data32 & 0x000000FF);
        (void)fwrite(data, 1, 4, fp);

        // ファイル名の長さ
        data32 = (quint32)(strlen(item.file_name.toLower().toUtf8().toStdString().c_str()) + 1);
        data[0] = (data32 & 0xFF000000) >> 24;
        data[1] = (data32 & 0x00FF0000) >> 16;
        data[2] = (data32 & 0x0000FF00) >> 8;
        data[3] = (data32 & 0x000000FF);
        (void)fwrite(data, 1, 4, fp);
        // ファイル名
        (void)fwrite((const char*)item.file_name.toLower().toUtf8().toStdString().c_str(),
               1, data32, fp);

        // ファイルの位置
        data32 = header_size + sum_file_size;
        data[0] = (data32 & 0xFF000000) >> 24;
        data[1] = (data32 & 0x00FF0000) >> 16;
        data[2] = (data32 & 0x0000FF00) >> 8;
        data[3] = (data32 & 0x000000FF);
        (void)fwrite(data, 1, 4, fp);
        sum_file_size += item.data_size;

        // ファイルサイズ
        data32 = item.data_size;
        data[0] = (data32 & 0xFF000000) >> 24;
        data[1] = (data32 & 0x00FF0000) >> 16;
        data[2] = (data32 & 0x0000FF00) >> 8;
        data[3] = (data32 & 0x000000FF);
        (void)fwrite(data, 1, 4, fp);
    }

    // 先頭に戻る
    while(iterator_vector.hasPrevious() != false)
    {
        (void)iterator_vector.previous();
    }

    // ファイルの生データを書き出す
    while(iterator_vector.hasNext() != false)
    {
        FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM item = iterator_vector.next();
        FILE *reader = fopen((target_directory_path + "/" + item.file_name).toLocal8Bit().toStdString().c_str(), "rb");

        if(reader != NULL)
        {
            char *data = new char[item.data_size];

            (void)fread(data, 1, item.data_size, reader);
            (void)fwrite(data, 1, item.data_size, fp);

            delete data;
            (void)fclose(reader);
        }
    }
}

bool FILE_ARCHIVE_WINDOW::WriteArchive(const QString& directory_path, const QString& output_path)
{
    QDir directory(directory_path);
    QVector<FILE_ARCHIVE_WINDOW::ARCHIVE_ITEM> items;

    if(directory_path == ""
        || directory.dirName() == ".")
    {
        QMessageBox message_box(this);
        message_box.setIcon(QMessageBox::Warning);
        message_box.setStandardButtons(QMessageBox::Ok);
        message_box.setText("Failded to open target directory.");
        return false;
    }

    AddDirectoryFiles(directory, "", items);

    qSort(items.begin(), items.end(), CompareArchiveItem);

    FILE *fp = fopen(output_path.toLocal8Bit(), "wb");
    if(fp == NULL || output_path == "")
    {
        QMessageBox message_box(this);
        message_box.setIcon(QMessageBox::Warning);
        message_box.setStandardButtons(QMessageBox::Ok);
        message_box.setText("Failded to open archive file.");
        return false;
    }
    WriteArchiveData(fp, items, directory_path);
    (void)fclose(fp);

    return true;
}

void FILE_ARCHIVE_WINDOW::on_pushButton_clicked()
{
    if(WriteArchive(ui->input_directory_path_edit->text(),
        ui->output_file_path_edit->text()) != false)
    {
        QMessageBox message_box(this);
        message_box.setStandardButtons(QMessageBox::Ok);
        message_box.setText("Success to write archive.");
        message_box.exec();
    }
}

void FILE_ARCHIVE_WINDOW::ResetFileArchiveView(const QString& file_path)
{
    if(file_path != "")
    {
        FILE *fp = fopen(file_path.toStdString().c_str(), "rb");
        ReleaseFileArchive(&archive);
        if(InitializeFileArchive(&archive,
            (const char*)file_path.toStdString().c_str(),
            (void*)fp,
            (size_t (*)(void*, size_t, size_t, void*))fread,
            (int (*)(void*, long, int))fseek,
            (long (*)(void*))ftell
            ) == FALSE
        )
        {
            return;
        }

        // 表示内容をリセット
        ui->archive_item_view->clear();

        for(int i=0; i<archive.num_files; i++)
        {
            QTreeWidgetItem *item;
            item = new QTreeWidgetItem;
            item->setText(0, archive.files[i].file_name);
            ui->archive_item_view->insertTopLevelItem(i, item);
        }
    }
}

void FILE_ARCHIVE_WINDOW::on_archive_select_button_clicked()
{
    QString selected_file_name = QFileDialog::getOpenFileName(this, "Open Archive");
    ResetFileArchiveView(selected_file_name);
}

void FILE_ARCHIVE_WINDOW::dragEnterEvent(QDragEnterEvent* event_info)
{
    if(event_info->mimeData()->hasUrls() != false)
    {   // URIなら受け入れOK
        event_info->acceptProposedAction();
    }
}

void FILE_ARCHIVE_WINDOW::dropEvent(QDropEvent* event_info)
{
    QList<QUrl> urls = event_info->mimeData()->urls();
    if(urls.size() == 0)
    {
        return;
    }

    if(ui->tab_widget->currentIndex() == 0)
    {
        QDir directory(urls[0].toLocalFile());
        if(directory.exists() != false)
        {
            ui->input_directory_path_edit->setText(urls[0].toLocalFile());
        }
    }
    else if(ui->tab_widget->currentIndex() == 1)
    {
        FILE *fp = fopen(urls[0].toLocalFile().toStdString().c_str(), "rb");
        if(fp != NULL)
        {
            ResetFileArchiveView(urls[0].toLocalFile());
        }
        (void)fclose(fp);
    }
}

static bool IsBinaryData(const char* data, size_t data_size)
{
    for(size_t i = 0; i<data_size; i++)
    {
        if(data[i] == '\0')
        {
            return true;
        }
    }

    return false;
}

void FILE_ARCHIVE_WINDOW::on_archive_item_view_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    FILE_ARCHIVE_READ *reader;
    if((reader = FileArchiveReadNew(item->text(0).toLower().toStdString().c_str(), "rb", &archive))
        != NULL)
    {
        FileArchiveSeek(reader, 0, SEEK_END);
        long data_size = FileArchiveTell(reader);
        char *data = new char[data_size+1];
        FileArchiveSeek(reader, 0, SEEK_SET);
        FileArchiveRead(data, 1, data_size, reader);
        data[data_size] = '\0';
        if(IsBinaryData(data, data_size) == false)
        {
            TEXT_DIALOG dialog(this, data);
            dialog.show();
            dialog.exec();
        }
        delete data;
    }
}
