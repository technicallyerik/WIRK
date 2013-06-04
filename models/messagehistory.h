#ifndef MESSAGEHISTORY_H
#define MESSAGEHISTORY_H

#include <QStandardItemModel>

class MessageHistory : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit MessageHistory(QObject *parent = 0);
    virtual ~MessageHistory();

    void insertNewMessage(QString text);
    QString getLastSentMessage(bool cycleUp = true);

private:
    int messageIndex;
    QStringList sentMessages;

    void resetMessagePosition();
};

#endif // MESSAGEHISTORY_H
