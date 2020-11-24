#ifndef ReferenceImageSizeHelp_H
#define ReferenceImageSizeHelp_H

#include <QWidget>

namespace Ui {
class ReferenceImageSizeHelp;
}

class ReferenceImageSizeHelp : public QWidget
{
    Q_OBJECT

public:
    explicit ReferenceImageSizeHelp(QWidget *parent = 0);
    ~ReferenceImageSizeHelp();

private:
    Ui::ReferenceImageSizeHelp *ui;
};

#endif // ReferenceImageSizeHelp_H
