#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QXmlStreamReader>
#include <QHeaderView>

#include "IosLaunchImages.h"

extern void mylog(const char *fmt, ...);

IosLaunchImages::IosLaunchImages(QWidget *parent)
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

void IosLaunchImages::swapWidthHeight(Device &device) const
{
    int tmp = device.m_heightPixels;
    device.m_heightPixels = device.m_widthPixels;
    device.m_widthPixels = tmp;
}

QList<Device> IosLaunchImages::getIOSDevices() const
{
    QList<Device> devices;
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        if(isDeviceChecked(row))
        {
            QModelIndex si = m_model.index(row, 0);
            Device device = si.data(MyClassRole).value<Device>();
            device.m_suffix = makeFileNameSuffix(
                    device.m_name,
                    device.m_widthPixels,
                    device.m_heightPixels,
                    device.m_pixelsPerPoint);
            devices.append(device);

            device = si.data(MyClassRole).value<Device>();
            swapWidthHeight(device);
            device.m_suffix = makeFileNameSuffix(
                    device.m_name,
                    device.m_widthPixels,
                    device.m_heightPixels,
                    device.m_pixelsPerPoint);
            device.m_rotationAngle = -90;
            devices.append(device);
        }
    }
    return devices;
}

bool IosLaunchImages::isDeviceChecked(int row) const
{
    QStandardItem *iz = m_model.itemFromIndex( m_model.index(row, 0) );
    return Qt::Checked == iz->checkState();
}

void IosLaunchImages::saveIOSDevices(QXmlStreamWriter &writer) const
{
    writer.writeStartElement("LaunchImages");
        for (int e = 0; e < m_model.rowCount(); ++e)
        {
            QModelIndex si = m_model.index(e, 0);
            Device device = si.data(MyClassRole).value<Device>();
                writer.writeStartElement("Device");
                writer.writeAttribute("Name", device.m_name);
                writer.writeAttribute("Suffix", device.m_suffix);
                writer.writeAttribute("PixelsPerPoint", QString("%1").arg(device.m_pixelsPerPoint));
                writer.writeAttribute("Width", QString("%1").arg(device.m_widthPixels));
                writer.writeAttribute("Height", QString("%1").arg(device.m_heightPixels));
                writer.writeAttribute("Checked", QString("%1").arg( isDeviceChecked(e) ? "1":"0" ));
            writer.writeEndElement();
        }
    writer.writeEndElement();
}

void IosLaunchImages::loadDefaultDevices()
{
    addItemToModel("iPhone", "7,8", 2, 960, 640, true, false);
    addItemToModel("iPhone Retina 4", "7,8", 2, 1136, 640, true, false);

    addItemToModel("iPad", "7,8", 1, 1024, 768, true, false);
    addItemToModel("iPad", "7,8", 2, 2048, 1536, true, false);

    addItemToModel("iPhone", "5,6", 1, 480, 320, true, false);
    addItemToModel("iPhone", "5,6", 2, 960, 640, true, false);
    addItemToModel("iPhone Retina 4", "5,6", 2, 1136, 640, true, false);

    addItemToModel("iPad", "5,6", 1, 1024, 768, true, false);
    addItemToModel("iPad", "5,6", 2, 2048, 1536, true, false);
}

void IosLaunchImages::loadIOSDevices(QXmlStreamReader &xml)
{
    if(xml.name() != "LaunchImages")
    {
        return;
    }
    if(!xml.readNextStartElement())
    {
        return;
    }
    while (!xml.atEnd() && xml.name() != "LaunchImages")
    {
        if(xml.isStartElement())
        {
            if(xml.name() == "Device")
            {
                int pixelsPerPoint = 1;
                int width = 1;
                int height = 1;
                QString deviceName;
                bool checked = false;
                foreach(const QXmlStreamAttribute &attribute, xml.attributes())
                {
                    if(attribute.name() == "Name")
                    {
                        deviceName = attribute.value().toString();
                    }
                    else if(attribute.name() == "PixelsPerPoint")
                    {
                        pixelsPerPoint = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Width")
                    {
                        width = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Height")
                    {
                        height = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Checked")
                    {
                        checked = attribute.value().toInt() == 0 ? false:true;
                    }
                }
                if(deviceName.size() > 0)
                {
                    QString suffix = makeFileNameSuffix(deviceName, width, height, pixelsPerPoint);
                    removeDuplicateDevices(QString(deviceName).append(suffix));
                    addItemToModel(deviceName, "", pixelsPerPoint, width, height, checked, false);
                }
            }
        }
        xml.readNext();
    }
}

QString IosLaunchImages::makeFileNameSuffix(const QString &deviceName, int width, int height, int pixelsPerPoint) const
{
    if(pixelsPerPoint < 1)
    {
        pixelsPerPoint = 1;
    }
    QString suffix = QString("-%1-%2x%3px").arg(deviceName).arg(width).arg(height);
    if(pixelsPerPoint > 1)
    {
        suffix.append(QString("@%1x").arg(pixelsPerPoint));
    }
    return suffix;
}

void IosLaunchImages::addItemToModel(const QString &deviceName, const QString &iosVersion,
                                     int pixelsPerPoint, int width, int height, bool checked, bool isReferenceDevice)
{
    QStandardItem *col0 = new QStandardItem(deviceName);
    col0->setCheckable(true);
    col0->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    QString suffix = makeFileNameSuffix(deviceName, width, height, pixelsPerPoint);
    Device device(IMG_SCALING_FIXED_WIDTH_AND_HEIGHT,
                  0,
                  width,
                  height,
                  pixelsPerPoint,
                  suffix, deviceName, false, 0,
                  IMG_ACTION_KEEP_TRANSPARENCY|IMG_ACTION_RESIZE_CANVAS);
    QVariant v;
    v.setValue(device);
    col0->setData(v, MyClassRole);

    int column = 0;
    int row = m_model.rowCount();
    m_model.setItem(row, column++, col0);
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(width)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(height)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(pixelsPerPoint)));

    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
