#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

// Этот namespace связывает C++ класс с формой из Qt Designer
namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void on_okButton_clicked();     // Слот для кнопки ОК
    void on_cancelButton_clicked(); // Слот для кнопки Отмена

private:
    Ui::SettingsDialog *ui;
    
    void loadSettings();
    void updateAutostart(bool enable);
};

#endif // SETTINGSDIALOG_H