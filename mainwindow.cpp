#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QCoreApplication>
#include <QFont>
#include <QBrush>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QSettings>
#include <QSystemTrayIcon>
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{ // <--- ЗДЕСЬ конструктор открывается!

    ui->setupUi(this);

 if (!trayIcon) {
    trayIcon = new QSystemTrayIcon(this);
}

// 1. Устанавливаем системную иконку (убрали лишний слэш в начале!)
trayIcon->setIcon(QIcon::fromTheme("network-wire", windowIcon())); 

// 2. Создаем контекстное меню
QMenu *trayMenu = new QMenu(this);

// 3. Пункт меню "Развернуть"
QAction *restoreAction = new QAction("Развернуть", this);
connect(restoreAction, &QAction::triggered, this, &MainWindow::showNormal);
trayMenu->addAction(restoreAction);

// Разделитель
trayMenu->addSeparator();

// 4. Пункт меню "Выход"
QAction *quitAction = new QAction("Выход", this);
connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
trayMenu->addAction(quitAction);

// 5. Привязываем меню к трею
trayIcon->setContextMenu(trayMenu);

// 6. Клик ЛКМ по иконке: скрыть / развернуть
connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason){
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            showNormal();
            activateWindow();
        }
    }
});

// Показываем иконку
trayIcon->show();

    // 2. Твои таймеры статистики
    statsTimer = new QTimer(this);
    connect(statsTimer, &QTimer::timeout, this, &MainWindow::onStatsTimerTick);
    statsTimer->start(1000); 

    // 3. Твои вызовы функций инициализации данных
    checkAndSetSudoers();
    initTunnelsMockData();

    // 4. Твоя модель списка туннелей
    tunnelModel = new QStandardItemModel(this);
    for (const QString &tunnelName : tunnelsData.keys()) {
        QStandardItem *item = new QStandardItem(tunnelName);
        tunnelModel->appendRow(item);
    }
    ui->listView->setModel(tunnelModel);

    // 5. Все твои коннекты для кнопок и кликов
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onTunnelSelected);
    connect(ui->listView, &QListView::doubleClicked, this, &MainWindow::onTunnelDoubleClicked);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onAddTunnelClicked);       // Добавить
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onDeleteTunnelClicked); // Удалить
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onExportClicked);       // Экспорт

    // 6. Выбор дефолтного элемента
    if (tunnelModel->rowCount() > 0) {
        ui->listView->setCurrentIndex(tunnelModel->index(0, 0));
    }

    // 7. Фиксированный размер твоего главного окна
    this->setFixedSize(813, 624);

} 



MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::initTunnelsMockData() {
    tunnelsData.clear();

    // Автоматически получаем путь к домашней папке текущего пользователя
    // Например: /home/имя_пользователя/WireGuardConfigs
    QString path = QDir::homePath() + "/WireGuardConfigs";
    QDir wgDir(path);
    
    // Если папки еще нет, Qt сам её создаст со всеми правами текущего пользователя!
    if (!wgDir.exists()) {
        wgDir.mkpath(path);
    }

    // Фильтруем файлы с расширением *.conf
    QStringList filters;
    filters << "*.conf";
    wgDir.setNameFilters(filters);

    // Читаем список файлов
    QFileInfoList fileList = wgDir.entryInfoList(QDir::Files);

    // Если папка пустая
    if (fileList.isEmpty()) {


if (fileList.isEmpty()) {
    TunnelState noTunnels;
    noTunnels.name = "нет туннелей";
    noTunnels.isConnected = false;
    noTunnels.speed = 0.0;
    noTunnels.accept = 0.0;
    noTunnels.acceptUnit = "Б";
    noTunnels.sent = 0.0;
    noTunnels.sentUnit = "Б";
    noTunnels.lastTransferBytes = 0;
    noTunnels.lastHandshakeTime = 0;
    noTunnels.pubKey = "—";
    noTunnels.port = "—";
    noTunnels.addresses = "—";
    noTunnels.peerPubKey = "—";
    noTunnels.preKey = "—";
    noTunnels.ips = "—";
    noTunnels.endpoint = "—";
    noTunnels.keepalive = "—";
    noTunnels.handshake = "—";

    tunnelsData["нет туннелей"] = noTunnels;
    return;
}
        return;
    }

    // Заполняем карту туннелей реальными именами
    for (const QFileInfo &fileInfo : fileList) {
        QString tunnelName = fileInfo.baseName(); // Имя без ".conf"

        TunnelState realTunnel;
        realTunnel.name = tunnelName;
        realTunnel.isConnected = false;
        realTunnel.speed = 0.0;
        realTunnel.accept = 0.0;
        realTunnel.acceptUnit = "Б";
        realTunnel.sent = 0.0;
        realTunnel.sentUnit = "Б";
        
        realTunnel.pubKey = "Нажмите подключить";
        realTunnel.port = "—";
        realTunnel.addresses = "—";
        realTunnel.peerPubKey = "—";
        realTunnel.preKey = "—";
        realTunnel.ips = "—";
        realTunnel.endpoint = "—";
        realTunnel.keepalive = "—";
        realTunnel.handshake = "—";

        tunnelsData[tunnelName] = realTunnel;
    }
}

void MainWindow::onTunnelSelected(const QModelIndex &current, const QModelIndex &previous) {
    currentTunnelName = current.data().toString();
    updateTunnelUI(currentTunnelName);
}

void MainWindow::onTunnelDoubleClicked(const QModelIndex &index) {
    currentTunnelName = index.data().toString();
    updateTunnelUI(currentTunnelName);
    
    if (!tunnelsData[currentTunnelName].isConnected) {
        onConnectClicked();
    }
}

void MainWindow::updateTunnelUI(const QString &name) {
    if (!tunnelsData.contains(name)) return;
    const TunnelState &tunnel = tunnelsData[name];

    // Заполнение текстовых полей конфигурации
    ui->label_5->setText(tunnel.name);
    ui->label_7->setText(tunnel.pubKey);
    ui->label_6->setText(tunnel.port);
    ui->label_8->setText(tunnel.addresses);
    ui->label_16->setText(tunnel.peerPubKey);
    ui->label_17->setText(tunnel.preKey);
    ui->label_21->setText(tunnel.ips);
    ui->label_22->setText(tunnel.endpoint);
    ui->label_23->setText(tunnel.keepalive);
    ui->label_24->setText(tunnel.handshake);

    // Вывод скорости и трафика (если это QLabel)
  ui->lcdNumber_speed->display(tunnel.speed);
    ui->label_rx->display(tunnel.accept);
    ui->label_tx->display(tunnel.sent);
    
    // Если скорость выводится в QLCDNumber:
    ui->lcdNumber_speed->display(tunnel.speed);

    // Динамическое обновление единиц измерения (Б, КБ, МБ, ГБ) на созданных вами метках
    ui->label_25->setText(tunnel.acceptUnit);
    ui->label_26->setText(tunnel.sentUnit);

    // Стайлинг кнопки подключения
    if (tunnel.isConnected) {
        ui->pushButton_4->setText(tr("Disconnect"));
        ui->pushButton_4->setStyleSheet("background-color: #d32f2f; color: white; font-weight: bold; border-radius: 4px;");
    } else {
        ui->pushButton_4->setText(tr("Подключить"));
        ui->pushButton_4->setStyleSheet(""); 
    }
    
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            QSettings settings("K5ilnik", "LinuxWGGUI");
            // Проверяем, хочет ли пользователь уходить в трей
            bool shrinkToTray = settings.value("minimizeToTray", true).toBool();

            if (shrinkToTray) {
                QTimer::singleShot(0, this, &QWidget::hide); // Прячем с панели задач
                if (trayIcon) {
                    trayIcon->showMessage("LinuxWGGUI", "Приложение свернуто в трей", QSystemTrayIcon::Information, 1500);
                }
            }
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::onConnectClicked() {
    if (currentTunnelName.isEmpty() || currentTunnelName == "нет туннелей" || !tunnelsData.contains(currentTunnelName)) {
        return;
    }

    TunnelState &tunnel = tunnelsData[currentTunnelName];
    // Формируем полный путь к файлу конфигурации: /home/юзер/WireGuardConfigs/имя_туннеля.conf
    QString configPath = QDir::homePath() + "/WireGuardConfigs/" + tunnel.name + ".conf";

    QProcess process;

    if (!tunnel.isConnected) {
        // 1. Перед включением нового туннеля гасим все остальные активные (чтобы не было конфликтов сетей)
        for (auto &key : tunnelsData.keys()) {
            if (tunnelsData[key].isConnected) {
                QProcess disableProc;
                QString oldConfigPath = QDir::homePath() + "/WireGuardConfigs/" + tunnelsData[key].name + ".conf";
                disableProc.start("sudo", QStringList() << "wg-quick" << "down" << oldConfigPath);
                disableProc.waitForFinished();
                
                tunnelsData[key].isConnected = false;
                tunnelsData[key].speed = 0.0;
                tunnelsData[key].accept = 0.0;
                tunnelsData[key].sent = 0.0;
            }
        }

        // 2. Запускаем выбранный туннель: "sudo wg-quick up /путь/к/файлу.conf"
        // Благодаря нашему правилу в sudoers, это сработает без запроса пароля!
        process.start("sudo", QStringList() << "wg-quick" << "up" << configPath);
        process.waitForFinished();

        // Проверяем, успешно ли поднялся туннель (проверяем код возврата ОС)
        if (process.exitCode() == 0) {
    tunnel.isConnected = true;
    // Сбрасываем старые счетчики, чтобы расчет скорости начался с чистого листа
    tunnel.lastTransferBytes = 0;
    tunnel.lastHandshakeTime = 0;
    tunnel.speed = 0.0;
    
    // Мгновенно запрашиваем реальные данные, не дожидаясь тика таймера
    parseWireguardStats(tunnel.name); 
}
    
    } else {
        // 3. Если туннель уже был активен — выключаем его: "sudo wg-quick down /путь/к/файлу.conf"
        process.start("sudo", QStringList() << "wg-quick" << "down" << configPath);
        process.waitForFinished();

        if (process.exitCode() == 0) {
            tunnel.isConnected = false;
            tunnel.speed = 0.0;
            tunnel.accept = 0.0;
            tunnel.sent = 0.0;
        }
    }

    updateTunnelUI(currentTunnelName);
    updateListFonts();
}

void MainWindow::updateListFonts() {
    for (int i = 0; i < tunnelModel->rowCount(); ++i) {
        QStandardItem *item = tunnelModel->item(i);
        QString name = item->text();
        
        QFont font = item->font();
        if (tunnelsData.contains(name) && tunnelsData[name].isConnected) {
            font.setBold(true);
            item->setFont(font);
            item->setForeground(QBrush(QColor("#4caf50"))); // Жирный зеленый для активного
        } else {
            font.setBold(false);
            item->setFont(font);
            item->setForeground(QBrush(QColor("#cccccc"))); // Дефолтный серый для остальных
        }
    }
} // <--- Вот эта скобка закрывает updateListFonts()! Она должна стоять ТУТ.

// А теперь, СНАРУЖИ, идет отдельная функция прав доступа:
void MainWindow::checkAndSetSudoers() {
    QString ruleFilePath = "/etc/sudoers.d/linux_wg_gui";
    
    // 1. Если файл правила уже на месте, ничего не делаем
    if (QFile::exists(ruleFilePath)) {
        return; 
    }

    // 2. Создаем временный файл во временной папке ОС
    QString tempFilePath = QDir::tempPath() + "/wg_rule.tmp";
    QFile tempFile(tempFilePath);
    
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tempFile);
        out << "%sudo ALL=(ALL) NOPASSWD: /usr/bin/wg-quick *, /usr/bin/wg *\n";
        tempFile.close();
        
        // 3. Вызываем pkexec напрямую для копирования и выставления прав
        QProcess::execute("pkexec", QStringList() << "cp" << tempFilePath << ruleFilePath);
        QProcess::execute("pkexec", QStringList() << "chmod" << "0440" << ruleFilePath);
        
        // Удаляем временный файл
        QFile::remove(tempFilePath);
    }
}
void MainWindow::onAddTunnelClicked() {
    QString selectedFile = QFileDialog::getOpenFileName(this,
        tr("Выберите файл конфигурации WireGuard"),
        QDir::homePath(),
        tr("Конфигурация WireGuard (*.conf)"));

    if (selectedFile.isEmpty()) return;

    QFileInfo fileInfo(selectedFile);
    QString destPath = QDir::homePath() + "/WireGuardConfigs/" + fileInfo.fileName();

    if (QFile::exists(destPath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Файл уже существует"),
                                      tr("Туннель с таким именем уже есть. Перезаписать?"),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) return;
        QFile::remove(destPath);
    }

    if (QFile::copy(selectedFile, destPath)) {
        initTunnelsMockData();
        tunnelModel->clear();
        for (const QString &tunnelName : tunnelsData.keys()) {
            QStandardItem *item = new QStandardItem(tunnelName);
            item->setForeground(QBrush(QColor("#cccccc")));
            tunnelModel->appendRow(item);
        }
        updateListFonts();
    } else {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось скопировать файл."));
    }
}

void MainWindow::onDeleteTunnelClicked() {
    if (currentTunnelName.isEmpty() || currentTunnelName == "нет туннелей") return;

    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Удаление туннеля"),
                                  QString(tr("Вы уверены, что хотите удалить туннель '%1'?")).arg(currentTunnelName),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) return;

    if (tunnelsData[currentTunnelName].isConnected) {
        QProcess disableProc;
        QString configPath = QDir::homePath() + "/WireGuardConfigs/" + currentTunnelName + ".conf";
        disableProc.start("sudo", QStringList() << "wg-quick" << "down" << configPath);
        disableProc.waitForFinished();
    }

    QString fileToDelete = QDir::homePath() + "/WireGuardConfigs/" + currentTunnelName + ".conf";
    if (QFile::remove(fileToDelete)) {
        initTunnelsMockData();
        tunnelModel->clear();
        for (const QString &tunnelName : tunnelsData.keys()) {
            QStandardItem *item = new QStandardItem(tunnelName);
            item->setForeground(QBrush(QColor("#cccccc")));
            tunnelModel->appendRow(item);
        }
        if (tunnelModel->rowCount() > 0) {
            ui->listView->setCurrentIndex(tunnelModel->index(0, 0));
        } else {
            currentTunnelName = "";
        }
        updateListFonts();
    }
}

void MainWindow::onExportClicked() {
    QString srcDir = QDir::homePath() + "/WireGuardConfigs";
    QDir dir(srcDir);
    if (!dir.exists() || dir.entryList(QStringList() << "*.conf", QDir::Files).isEmpty()) {
        QMessageBox::warning(this, tr("Экспорт"), tr("Нет файлов для экспорта."));
        return;
    }

    QString saveZipPath = QFileDialog::getSaveFileName(this,
        tr("Сохранить резервную копию"),
        QDir::homePath() + "/wireguard_backup.zip",
        tr("ZIP Архив (*.zip)"));

    if (saveZipPath.isEmpty()) return;

    if (!saveZipPath.endsWith(".zip", Qt::CaseInsensitive)) {
        saveZipPath += ".zip";
    }

    QProcess zipProcess;
    zipProcess.start("sh", QStringList() << "-c" << QString("zip -j -r \"%1\" \"%2\"/*.conf").arg(saveZipPath, srcDir));
    zipProcess.waitForFinished();

    if (zipProcess.exitCode() == 0) {
        QMessageBox::information(this, tr("Успех"), tr("Все конфигурации экспортированы в ZIP!"));
    } else {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось создать ZIP. Убедитесь, что утилита 'zip' установлена."));
    }
}

void MainWindow::onStatsTimerTick() {
    // 1. Опрашиваем WireGuard
    for (const QString &name : tunnelsData.keys()) {
        if (name == "нет туннелей" || !tunnelsData[name].isConnected) continue;
        parseWireguardStats(name);
    }

    // 2. ПРИНУДИТЕЛЬНО ОБНОВЛЯЕМ ИНТЕРФЕЙС для текущего туннеля!
    if (!currentTunnelName.isEmpty() && currentTunnelName != "нет туннелей") {
        updateTunnelUI(currentTunnelName);
    }
}

// Функция перевода байтов в удобные единицы (Б, КБ, МБ, ГБ)
void formatBytes(double bytes, double &outValue, QString &outUnit) {
    if (bytes < 1024) {
        outValue = bytes;
        outUnit = "Б";
    } else if (bytes < 1024 * 1024) {
        outValue = bytes / 1024.0;
        outUnit = "КБ";
    } else if (bytes < 1024 * 1024 * 1024) {
        outValue = bytes / (1024.0 * 1024.0);
        outUnit = "МБ";
    } else {
        outValue = bytes / (1024.0 * 1024.0 * 1024.0);
        outUnit = "ГБ";
    }
}

void MainWindow::parseWireguardStats(const QString &tunnelName) {
    if (!tunnelsData.contains(tunnelName)) return;
    TunnelState &tunnel = tunnelsData[tunnelName];
    
    // Если туннель не запущен, статистики нет
    if (!tunnel.isConnected) return;

    QProcess process;
    process.start("sudo", QStringList() << "wg" << "show" << tunnelName << "dump");
    process.waitForFinished(2000);

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    if (output.isEmpty()) return;

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    if (lines.isEmpty()) return;

    // --- 1. ПАРСИМ ДАННЫЕ ИНТЕРФЕЙСА (Строка 0) ---
    // Формат wg: имя \t private-key \t public-key \t listen-port \t fwmark
    QStringList ifaceFields = lines[0].split('\t');
    if (ifaceFields.size() >= 4) {
        tunnel.pubKey = ifaceFields[2]; // Публичный ключ интерфейса
        tunnel.port = ifaceFields[3];   // Порт
    }

    // --- 2. ПАРСИМ ДАННЫЕ ПИРА (Строка 1) ---
    if (lines.size() >= 2) {
        QStringList peerFields = lines[1].split('\t');
        // Формат wg: peer-pubkey \t psk \t endpoint \t allowed-ips \t handshake \t rx \t tx \t keepalive
        if (peerFields.size() >= 7) {
            tunnel.peerPubKey = peerFields[0];
            tunnel.preKey = (peerFields[1] == "(none)") ? "выкл" : "вкл";
            tunnel.endpoint = peerFields[2];
            tunnel.ips = peerFields[3];

            // Читаем байты (rx = входящие, tx = исходящие)
            unsigned long long rx = peerFields[5].toULongLong();
            unsigned long long tx = peerFields[6].toULongLong();
            unsigned long long totalBytes = rx + tx;

            // Считаем скорость (Мбит/с)
            if (tunnel.lastTransferBytes > 0 && totalBytes >= tunnel.lastTransferBytes) {
                tunnel.speed = ((totalBytes - tunnel.lastTransferBytes) * 8.0) / 1000000.0;
            } else {
                tunnel.speed = 0.0;
            }
            tunnel.lastTransferBytes = totalBytes;

            // Форматируем для вывода (используем твой метод formatBytes)
            formatBytes(rx, tunnel.accept, tunnel.acceptUnit);
            formatBytes(tx, tunnel.sent, tunnel.sentUnit);
        }
    }
}
#include <QCloseEvent>

void MainWindow::closeEvent(QCloseEvent *event) {
    // Проходимся по всем туннелям
    for (const QString &name : tunnelsData.keys()) {
        if (tunnelsData[name].isConnected) {
            // Если туннель активен — отключаем его перед закрытием
            QProcess process;
            QString configPath = QDir::homePath() + "/WireGuardConfigs/" + name + ".conf";
            
            // Используем sudo, так как права суперпользователя нужны для удаления интерфейса
            process.start("sudo", QStringList() << "wg-quick" << "down" << configPath);
            process.waitForFinished(3000); // Даем 3 секунды на корректное закрытие
        }
    }
    
    // Останавливаем таймер статистики, чтобы он не пытался дергать wg при закрытии
    if (statsTimer) {
        statsTimer->stop();
    }

    // Принимаем событие закрытия, программа завершится
    event->accept();
}
void MainWindow::on_set_clicked() {
    // Пробуем вызвать класс через пространство имен Ui::
    SettingsDialog dialog(this); 
    
    if (dialog.exec() == QDialog::Accepted) {
        // Код при нажатии ОК
    }
}