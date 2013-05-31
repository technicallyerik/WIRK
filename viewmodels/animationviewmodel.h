#ifndef ANIMATIONVIEWMODEL_H
#define ANIMATIONVIEWMODEL_H

#include <QObject>
#include <QBuffer>
#include <QUrl>
#include <QPixmap>
#include <QThread>

class QMovie;

class AnimationViewModel : public QThread
{
    Q_OBJECT

public:
    explicit AnimationViewModel(QByteArray bytes, QUrl url, QObject *parent = 0);

signals:
    void movieAnimated(QPixmap pixels, QUrl url);

private slots:
    void movieAnimated(int frame);

private:
    QBuffer buffer;
    QMovie *movie;
    QUrl movieUrl;

};

#endif // ANIMATIONVIEWMODEL_H
