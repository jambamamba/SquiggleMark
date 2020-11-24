#include <QDebug>

#include "MainSplitter.h"

MainSplitter::MainSplitter(Qt::Orientation, QWidget* parent)
    : QSplitter(parent)
{
}

MainSplitter::~MainSplitter()
{
}

void MainSplitter::resizeEvent(QResizeEvent *event)
{
    QList<int> previousSizes = sizes();
    QSplitter::resizeEvent(event);
    QList<int> newSizes = sizes();

    qDebug() << "splitter resized" << previousSizes << " -> " << newSizes;
}

void MainSplitter::moveSplitter(int pos, int index)
{
    qDebug() << "moveSplitter" << pos << index;
    QSplitter::moveSplitter(pos, index);
}
