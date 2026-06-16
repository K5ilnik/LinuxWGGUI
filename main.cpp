#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QMap>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Указываем ТОЧНО ТЕ ЖЕ параметры, что и в settingsdialog.cpp
    QSettings settings("K5ilnik", "LinuxWGGUI"); 
    QString selectedLang = settings.value("lang", "Русский").toString(); 
    
    qDebug() << "ЛинуксWG-GUI -> Считанный язык из настроек:" << selectedLang;

    // 2. Карта языковых пакетов
    QMap<QString, QString> langFiles = {
        {"Українська", "LinuxWGGUI_uk.qm"},
        {"Беларускі",  "LinuxWGGUI_be.qm"},
        {"English",    "LinuxWGGUI_en.qm"},
        {"Deutsch",    "LinuxWGGUI_de.qm"},
        {"Français",   "LinuxWGGUI_fr.qm"},
        {"Italiano",   "LinuxWGGUI_it.qm"}
    };

    // 3. Загружаем и применяем переводчик
    QTranslator *translator = new QTranslator(&a);

    if (selectedLang != "Русский" && langFiles.contains(selectedLang)) {
        QString qmFile = langFiles.value(selectedLang);
        QString resourcePath = ":/i18n/" + qmFile;
        qDebug() << "ЛинуксWG-GUI -> Пробуем загрузить:" << resourcePath;

        if (translator->load(resourcePath)) {
            a.installTranslator(translator);
            qDebug() << "ЛинуксWG-GUI -> УСПЕХ: Перевод успешно применен!";
        } else {
            qDebug() << "ЛинуксWG-GUI -> ОШИБКА: Не удалось найти перевод по пути" << resourcePath;
        }
    } else {
        qDebug() << "ЛинуксWG-GUI -> Выбран дефолтный русский язык, переводчик не требуется.";
    }

    // 4. Окно создается СТРОГО ПОСЛЕ загрузки перевода!
    MainWindow w;
    w.show();
    
    return a.exec();
}