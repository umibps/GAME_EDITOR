#ifndef TEXT_DIALOG_H
#define TEXT_DIALOG_H

#include <QDialog>

class TEXT_DIALOG : public QDialog
{
    Q_OBJECT
public:
    TEXT_DIALOG(QWidget* parent = NULL, const char* text = NULL);
};

#endif // TEXT_DIALOG_H

