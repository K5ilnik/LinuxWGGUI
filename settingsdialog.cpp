#include "settingsdialog.h"
#include "ui_settingsdialog.h" 

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSettings>
#include <QDir>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // 1. Сначала заполняем списки (чтобы было из чего выбирать)
    if (ui->langCombo->count() == 0) {
        ui->langCombo->addItems({
            "Русский", 
            "Українська", 
            "Беларускі", 
            "English", 
            "Deutsch", 
            "Français", 
            "Italiano"
        });
    }
    if (ui->themeCombo->count() == 0) ui->themeCombo->addItems({"Тёмная", "Светлая"});

    // 2. Только теперь загружаем сохраненные настройки и применяем их к интерфейсу
    loadSettings();

    // 3. Подключаем кнопки
    connect(ui->okButton, &QPushButton::clicked, this, &SettingsDialog::on_okButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsDialog::on_cancelButton_clicked);
}

void SettingsDialog::loadSettings() {
    // Единая точка чтения настроек
    QSettings settings("K5ilnik", "LinuxWGGUI");

    ui->autostartCheck->setChecked(settings.value("autostart", false).toBool());
    ui->trayCheck->setChecked(settings.value("minimizeToTray", true).toBool());
    ui->saveStateCheck->setChecked(settings.value("saveState", true).toBool());

    ui->langCombo->setCurrentText(settings.value("lang", "Русский").toString());
    ui->themeCombo->setCurrentText(settings.value("theme", "Тёмная").toString());
}

void SettingsDialog::on_okButton_clicked() {
    // Единая точка сохранения настроек
    QSettings settings("K5ilnik", "LinuxWGGUI");

    // Сохраняем все значения галочек и списков
    settings.setValue("autostart", ui->autostartCheck->isChecked());
    settings.setValue("minimizeToTray", ui->trayCheck->isChecked());
    settings.setValue("saveState", ui->saveStateCheck->isChecked());
    
    QString chosenLang = ui->langCombo->currentText();
    settings.setValue("lang", chosenLang); 
    settings.setValue("theme", ui->themeCombo->currentText());

    // Выводим в лог, что именно мы сохранили
    qDebug() << "ЛинуксWG-GUI [Диалог] -> Успешно записали в базу ключ lang со значением:" << chosenLang;

    // Обновляем файл автозапуска в системе
    updateAutostart(ui->autostartCheck->isChecked());

    // Закрываем окно ОДИН раз в самом конце
    accept(); 
}

void SettingsDialog::on_cancelButton_clicked() {
    reject(); 
}

void SettingsDialog::updateAutostart(bool enable) {
    QString autostartDir = QDir::homePath() + "/.config/autostart";
    QString filePath = autostartDir + "/linuxwggui.desktop";

    if (!enable) {
        if (QFile::exists(filePath)) {
            QFile::remove(filePath);
        }
        return;
    }

    QDir().mkpath(autostartDir);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "[Desktop Entry]\n"
            << "Type=Application\n"
            << "Exec=" << qApp->applicationFilePath() << " --autostart\n"
            << "Hidden=false\n"
            << "NoDisplay=false\n"
            << "X-GNOME-Autostart-enabled=true\n"
            << "Name=LinuxWGGUI\n"
            << "Comment=WireGuard GUI Client\n";
        file.close();
    }
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}