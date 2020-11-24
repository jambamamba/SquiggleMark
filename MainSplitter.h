#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include <QSplitter>

class MainSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit MainSplitter(Qt::Orientation, QWidget *parent = 0);
    ~MainSplitter();

    void resizeEvent(QResizeEvent *event);
    void moveSplitter(int pos, int index);

signals:

public slots:
};

#endif // MAINSPLITTER_H
