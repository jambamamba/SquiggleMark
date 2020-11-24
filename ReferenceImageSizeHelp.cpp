#include "ReferenceImageSizeHelp.h"
#include "ui_ReferenceImageSizeHelp.h"

ReferenceImageSizeHelp::ReferenceImageSizeHelp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReferenceImageSizeHelp)
{
    ui->setupUi(this);

    ui->label->setText(
        "If you have a set of images, then set this to the dimension of the largest image.\n"
        "When the images are resized, they will be scaled proportionlly to this width or height.\n\n"
        "True color images are 32 bits per pixel. When loaded into memory they will take\n"
        "(32 x width x height) number of bytes. To reduce memory footprint, uncheck this "
        "option.");
}

ReferenceImageSizeHelp::~ReferenceImageSizeHelp()
{
    delete ui;
}
