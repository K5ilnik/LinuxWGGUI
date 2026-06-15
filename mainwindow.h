#pragma once

#include <QMainWindow>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QMap>
#include <QTimer>
#include <QCloseEvent> // Обязательно добавь это

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct TunnelState {
    QString name;
    bool isConnected = false;
    double speed = 0.0;
    double accept = 0.0;
    QString acceptUnit = "Б"; 
    double sent = 0.0;
    QString sentUnit = "Б";   
    unsigned long long lastTransferBytes = 0; 
    unsigned long long lastHandshakeTime = 0; 
    QString pubKey, port, addresses, peerPubKey, preKey, ips, endpoint, keepalive, handshake;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override; // Вот этот метод здесь должен быть!

private slots:
    void onConnectClicked();
    void onTunnelSelected(const QModelIndex &current, const QModelIndex &previous);
    void onTunnelDoubleClicked(const QModelIndex &index);
    void onAddTunnelClicked();
    void onDeleteTunnelClicked();
    void onExportClicked();
    void onStatsTimerTick();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *tunnelModel;
    QMap<QString, TunnelState> tunnelsData;
    QString currentTunnelName;
    QTimer *statsTimer;

    void initTunnelsMockData();
    void updateTunnelUI(const QString &name);
    void updateListFonts();
    void checkAndSetSudoers();
    void parseWireguardStats(const QString &tunnelName);
};