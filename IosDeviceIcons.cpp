#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QXmlStreamReader>
#include <QHeaderView>

#include "IosDeviceIcons.h"

IosDeviceIcons::IosDeviceIcons(QWidget *parent)
    : DeviceTableView(parent)
{
    int column = 0;
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Device"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("iOS Version"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Size(px)"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Pixels/Point"));
    setModel(&m_model);
    loadDefaultDevices();
}

bool IosDeviceIcons::isDeviceChecked(int row) const
{
    QStandardItem *iz = m_model.itemFromIndex( m_model.index(row, 0) );
    return Qt::Checked == iz->checkState();
}

void IosDeviceIcons::saveIOSDevices(QXmlStreamWriter &writer) const
{
    writer.writeStartElement("Icons");
        for (int e = 0; e < m_model.rowCount(); ++e)
        {
            QModelIndex si = m_model.index(e, 0);
            Device device = si.data(MyClassRole).value<Device>();
                writer.writeStartElement("Device");
                writer.writeAttribute("Name", device.m_name);
                writer.writeAttribute("Suffix", device.m_suffix);
                writer.writeAttribute("PixelsPerPoint", QString("%1").arg(device.m_pixelsPerPoint));
                writer.writeAttribute("Size", QString("%1").arg(device.m_widthPixels));
                writer.writeAttribute("Checked", QString("%1").arg( isDeviceChecked(e) ? "1":"0" ));
            writer.writeEndElement();
        }
    writer.writeEndElement();
}

void IosDeviceIcons::loadDefaultDevices()
{
    addItemToModel("iPhone", "7,8", 3, 180, 180, true);
    addItemToModel("iPhone", "7,8", 2, 120, 120, true);
    addItemToModel("iPhone", "5,6", 2, 114, 114, true);
    addItemToModel("iPhone", "5,6", 1, 57, 57, true);

    addItemToModel("iPad", "7,8", 2, 152, 152, true);
    addItemToModel("iPad", "7,8", 1, 76, 76, true);
    addItemToModel("iPad", "5,6", 2, 144, 144, true);
    addItemToModel("iPad", "5,6", 1, 72, 72, true);

    addItemToModel("StoreIcon", "", 1, 1024, 1024, true);
}

void IosDeviceIcons::loadIOSDevices(QXmlStreamReader &xml)
{
    if(xml.name() != "Icons")
    {
        return;
    }
    if(!xml.readNextStartElement())
    {
        return;
    }
    while (!xml.atEnd() && xml.name() != "Icons")
    {
        if(xml.isStartElement())
        {
            if(xml.name() == "Device")
            {
                int pixelsPerPoint = 1;
                int size = 1;
                QString iosVersion;
                QString deviceName;
                bool checked = false;
                foreach(const QXmlStreamAttribute &attribute, xml.attributes())
                {
                    if(attribute.name() == "Name")
                    {
                        deviceName = attribute.value().toString();
                    }
                    else if(attribute.name() == "Suffix")
                    {
                        iosVersion = attribute.value().toString();
                    }
                    else if(attribute.name() == "PixelsPerPoint")
                    {
                        pixelsPerPoint = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Size")
                    {
                        size = attribute.value().toInt();
                    }
                    else if(attribute.name() == "Checked")
                    {
                        checked = attribute.value().toInt() == 0 ? false:true;
                    }
                }
                if(deviceName.size() > 0)
                {
                    QString suffix = makeFileNameSuffix(iosVersion, size, size, pixelsPerPoint);
                    removeDuplicateDevices(QString(deviceName).append(suffix));
                    addItemToModel(deviceName, iosVersion, pixelsPerPoint, size, size, checked);
                }
            }
        }
        xml.readNext();
    }
}

QString IosDeviceIcons::makeFileNameSuffix(const QString &iosVersion, int width, int height, int pixelsPerPoint) const
{
    if(pixelsPerPoint < 1)
    {
        pixelsPerPoint = 1;
    }
    QString suffix = QString("-iOSv%1-%2pt").arg(iosVersion).arg(width/pixelsPerPoint);
    if(pixelsPerPoint > 1)
    {
        suffix.append(QString("@%1x").arg(pixelsPerPoint));
    }
    return suffix;
}

void IosDeviceIcons::addItemToModel(const QString &deviceName, const QString &iosVersion,
                                         int pixelsPerPoint, int width, int height, bool checked)
{
    QStandardItem *col0 = new QStandardItem(deviceName);
    col0->setCheckable(true);
    col0->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    QString suffix = makeFileNameSuffix(iosVersion, width, height, pixelsPerPoint);
    Device device(IMG_SCALING_FIXED_WIDTH_AND_HEIGHT,
                  0,
                  QRect(),
                  width,
                  height,
                  pixelsPerPoint,
                  suffix, deviceName, IMG_ACTION_KEEP_TRANSPARENCY);
    QVariant v;
    v.setValue(device);
    col0->setData(v, MyClassRole);

    int column = 0;
    int row = m_model.rowCount();
    m_model.setItem(row, column++, col0);
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(iosVersion)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(width)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(pixelsPerPoint)));

    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
