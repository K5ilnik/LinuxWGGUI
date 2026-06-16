#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog; // <-- Сюда пишем то имя, которое нашли в файле build/ui_settingsdialog.h
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::SettingsDialog *ui; // <-- И сюда его же
    
    void loadSettings();
    void updateAutostart(bool enable);
};

#endif // SETTINGSDIALOG_H