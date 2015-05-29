#ifndef FILE_ARCHIVE_WINDOW_H
#define FILE_ARCHIVE_WINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "file_archive.h"

namespace Ui {
class file_archive_window;
}

class FILE_ARCHIVE_WINDOW : public QMainWindow
{
    Q_OBJECT

public:
    explicit FILE_ARCHIVE_WINDOW(QWidget *parent = 0);
    ~FILE_ARCHIVE_WINDOW();
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);

    struct ARCHIVE_ITEM
    {
        quint32 hash_value;
        QString file_name;
        quint32 data_position;
        quint32 data_size;
    };

private slots:
    void on_input_directory_button_clicked();

    void on_output_file_button_clicked();

    void on_archive_select_button_clicked();

    void on_archive_item_view_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_execute_output_button_clicked();

private:
    Ui::file_archive_window *ui;
    bool WriteArchive(const QString& directory_path, const QString& output_path);
    void ResetFileArchiveView(const QString& file_path);
    FILE_ARCHIVE archive;
};

#endif // FILE_ARCHIVE_WINDOW_H
