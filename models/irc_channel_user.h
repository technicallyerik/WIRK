#ifndef IRC_CHANNEL_USER_H
#define IRC_CHANNEL_USER_H

#include <QObject>

class irc_channel_user : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)

public:
    explicit irc_channel_user(QObject *parent = 0);

    QString getName();
    void setName(QString name);

signals:
    
public slots:
    
private:
    QString m_name;

};

#endif // IRC_CHANNEL_USER_H
