#include <QVBoxLayout>
#include <QTextBrowser>
#include "text_dialog.h"

TEXT_DIALOG::TEXT_DIALOG(QWidget* parent, const char* text)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QString show_text = QString::fromStdString(text);
    QTextBrowser *viewer = new QTextBrowser(parent);
    show_text.replace("\t", "   ");
    viewer->setText(show_text);
    setLayout(layout);
    layout->addWidget(viewer);
    viewer->setReadOnly(true);
}
