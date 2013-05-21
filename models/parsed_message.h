#ifndef PARSED_MESSAGE_H
#define PARSED_MESSAGE_H

#include <QObject>

class parsed_message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString destination READ getDestination WRITE setDestination)
    Q_PROPERTY(QString message READ getMessage WRITE setMessage)

public:
    explicit parsed_message(QObject *parent = 0);
    explicit parsed_message(QString destination, QString message, QObject *parent = 0);

    void setDestination(QString m_destination);
    QString getDestination();

    void setMessage(QString message);
    QString getMessage();

signals:
    
public slots:
    
private:
    QString m_destination;
    QString m_message;

};

#endif // PARSED_MESSAGE_H
