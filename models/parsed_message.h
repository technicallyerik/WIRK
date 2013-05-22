#ifndef PARSED_MESSAGE_H
#define PARSED_MESSAGE_H

#include <QObject>

class parsed_message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString server READ getServer WRITE setServer)
    Q_PROPERTY(QString channel READ getChannel WRITE setServer)
    Q_PROPERTY(QString message READ getMessage WRITE setMessage)

public:
    explicit parsed_message(QObject *parent = 0);
    explicit parsed_message(QString server, QString channel, QString message, QObject *parent = 0);

    void setServer(QString server);
    QString getServer();

    void setChannel(QString channel);
    QString getChannel();

    void setMessage(QString message);
    QString getMessage();

signals:
    
public slots:
    
private:
    QString m_server;
    QString m_channel;
    QString m_message;

};

#endif // PARSED_MESSAGE_H
