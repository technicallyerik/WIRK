#ifndef ANIMATIONVIEWMODEL_H
#define ANIMATIONVIEWMODEL_H

#include <QObject>
#include <QBuffer>
#include <QUrl>
#include <QPixmap>
#include <QThread>
#include <QTextDocument>
#include <QMovie>

class AnimationViewModel : public QThread
{
    Q_OBJECT

public:
    explicit AnimationViewModel(QByteArray bytes, QUrl url, QTextDocument *document, QObject *parent = 0);

private slots:
    void movieAnimated(int frame);

private:
    void run();
    QByteArray movieBits;
    QUrl movieUrl;
    QTextDocument *textDocument;

    QMovie *movie;
};

#endif // ANIMATIONVIEWMODEL_H
