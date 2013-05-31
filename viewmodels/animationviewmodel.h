#ifndef ANIMATIONVIEWMODEL_H
#define ANIMATIONVIEWMODEL_H

#include <QObject>
#include <QBuffer>
#include <QUrl>
#include <QPixmap>
#include <QThread>
#include <QTextDocument>

class QMovie;

class AnimationViewModel : public QThread
{
    Q_OBJECT

public:
    explicit AnimationViewModel(QByteArray bytes, QUrl url, QTextDocument *document, QObject *parent = 0);

signals:
    void movieAnimated();

private slots:
    void movieAnimated(int frame);

private:
    QBuffer buffer;
    QMovie *movie;
    QUrl movieUrl;
    QTextDocument *textDocument;
};

#endif // ANIMATIONVIEWMODEL_H
