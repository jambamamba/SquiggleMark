#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QXmlStreamReader>
#include <QHeaderView>

#include "IosDeviceImages.h"

extern "C" void mylog(const char *fmt, ...);

IosDeviceImages::IosDeviceImages(QWidget *parent)
    : DeviceTableView(parent)
{
    int column = 0;
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Device"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Width(px)"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Height(px)"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Pixels/Point"));
    setModel(&m_model);
    loadDefaultDevices();
}

bool IosDeviceImages::isDeviceChecked(int row) const
{
    QStandardItem *iz = m_model.itemFromIndex( m_model.index(row, 0) );
    return Qt::Checked == iz->checkState();
}

void IosDeviceImages::saveIOSDevices(QXmlStreamWriter &writer) const
{
    writer.writeStartElement("Devices");
        for (int e = 0; e < m_model.rowCount(); ++e)
        {
            QModelIndex si = m_model.index(e, 0);
            Device device = si.data(MyClassRole).value<Device>();
                writer.writeStartElement("Device");
                writer.writeAttribute("Name", device.m_name);
                writer.writeAttribute("Suffix", device.m_suffix);
                writer.writeAttribute("Width", QString("%1").arg(device.m_widthPixels));
                writer.writeAttribute("Height", QString("%1").arg(device.m_heightPixels));
                writer.writeAttribute("PixelsPerPoint", QString("%1").arg(device.m_pixelsPerPoint));
                writer.writeAttribute("Checked", QString("%1").arg( isDeviceChecked(e) ? "1":"0" ));
            writer.writeEndElement();
        }
    writer.writeEndElement();
}

void IosDeviceImages::loadDefaultDevices()
{
    addItemToModel("iPhone6+", "", 3, 2208, 1242, true);
    addItemToModel("iPhone6", "", 2, 1334, 750, true);
    addItemToModel("iPhone5", "", 2, 1136, 640, true);
    addItemToModel("iPadPro", "", 2, 1366*2, 1024*2, true);
    addItemToModel("iPad", "", 2, 2048, 1536, true);
    addItemToModel("iPad", "", 1, 1024, 768, true);
    addItemToModel("iPhone4", "", 2, 960, 640, true);
    addItemToModel("iPhone3", "", 1, 480, 320, true);
}

void IosDeviceImages::loadIOSDevices(QXmlStreamReader &xml)
{
    if(xml.name() != "Devices")
    {
        return;
    }
    if(!xml.readNextStartElement())
    {
        return;
    }
    while (!xml.atEnd() && xml.name() != "Devices")
    {
        if(xml.isStartElement())
        {
            if(xml.name() == "Device")
            {
                int width = 1024;
                int height = 1024;
                int pixelsPerPoint = 1;
                int iconSize = 1024;
                int storeIconSize = 1024;
                QString suffix;
                QString deviceName;
                QString iconName;
                bool checked = false;
                foreach(const QXmlStreamAttribute &attribute, xml.attributes())
                {
                    if(attribute.name() == "Name")
                    {
                        deviceName = attribute.value().toString();
                    }
                    else if(attribute.name() == "Suffix")
                    {
                        suffix = attribute.value().toString();
                    }
                    else if(attribute.name() == "Width")
                    {
                        width = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Height")
                    {
                        height = attribute.value().toInt();
                    }
                    else if(attribute.name() == "PixelsPerPoint")
                    {
                        pixelsPerPoint = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Checked")
                    {
                        checked = attribute.value().toInt() == 0 ? false:true;
                    }
                   else if(attribute.name() == "IconName")
                    {
                        iconName = attribute.value().toString();
                    }
                    else if(attribute.name() == "AppIcon")
                    {
                        iconSize = attribute.value().toInt();
                    }
                    else if(attribute.name() == "StoreIcon")
                    {
                        storeIconSize = attribute.value().toInt();
                    }
                }
                if(deviceName.size() > 0)
                {
                    QString suffix = makeFileNameSuffix(deviceName, width, height, pixelsPerPoint);
                    removeDuplicateDevices(QString(deviceName).append(suffix));
                    addItemToModel(deviceName, "", pixelsPerPoint, width, height, checked);
                }
            }
        }
        xml.readNext();
    }
}

QString IosDeviceImages::makeFileNameSuffix(const QString &iosVersion, int width, int height, int pixelsPerPoint) const
{
    if(pixelsPerPoint < 1)
    {
        pixelsPerPoint = 1;
    }
    QString suffix;
    //suffix.append(QString("-%1").arg(iosVersion));
    if(pixelsPerPoint > 1)
    {
        suffix.append(QString("@%1x").arg(pixelsPerPoint));
    }
    return suffix;
}

void IosDeviceImages::addItemToModel(const QString &deviceName, const QString &iosVersion,
                                     int pixelsPerPoint, int width, int height, bool checked)
{
    QString suffix = makeFileNameSuffix(deviceName, width, height, pixelsPerPoint);
    Device device(IMG_SCALE_WIDTH_AND_CROP_HEIGHT,
                  0,
                  QRect(),
                  width,
                  height,
                  pixelsPerPoint,
                  suffix,
                  deviceName,
                  IMG_ACTION_RESIZE_FOR_IOS_DEVICE|IMG_ACTION_KEEP_TRANSPARENCY
                  );

    QStandardItem *col1 = new QStandardItem(deviceName);
    col1->setCheckable(true);
    col1->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    QVariant v;
    v.setValue(device);
    col1->setData(v, MyClassRole);

    int column = 0;
    int row = m_model.rowCount();
    m_model.setItem(row, column++, col1);
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(width)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(height)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(pixelsPerPoint)));

    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
