#include <QVBoxLayout>
#include <QPlainTextEdit>
#include "text_dialog.h"

TEXT_DIALOG::TEXT_DIALOG(QWidget* parent, const char* text)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QString show_text = QString::fromStdString(text);
    QPlainTextEdit *viewer = new QPlainTextEdit(text);
    setLayout(layout);
    layout->addWidget(viewer);
    viewer->setEnabled(false);
}
