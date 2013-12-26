#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class ChannelSettings;
}

class ChannelSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelSettings(QString channelName, QString serverName, QWidget *parent = 0);
    ~ChannelSettings();
    bool shouldHideJoinNotifications();
    bool shouldJoinOnConnect();

private slots:
    void accept();
    QString getGroupKey();

private:
    static const QString suppressJoinNotificationsKey;
    static const QString joinOnConnectKey;

    Ui::ChannelSettings *ui;
    QString channelName;
    QString serverName;
    QSettings *settings;
};

#endif // CHANNELSETTINGS_H
