#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QXmlStreamReader>
#include <QHeaderView>

#include "MacOSAppIcons.h"

MacOSAppIcons::MacOSAppIcons(QWidget *parent)
    : DeviceTableView(parent)
{
    int column = 0;
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("File"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Size(px)"));
    m_model.setHorizontalHeaderItem(column++, new QStandardItem("Pixels/Point"));
    setModel(&m_model);
    loadDefaultDevices();
}

bool MacOSAppIcons::isDeviceChecked(int row) const
{
    QStandardItem *iz = m_model.itemFromIndex( m_model.index(row, 0) );
    return Qt::Checked == iz->checkState();
}

void MacOSAppIcons::saveIOSDevices(QXmlStreamWriter &writer) const
{
    writer.writeStartElement("MacIcons");
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

void MacOSAppIcons::loadDefaultDevices()
{
    addItemToModel("icon", "-16x16", 1, 16, 16, true);
    addItemToModel("icon", "-16x16", 2, 32, 32, true);

    addItemToModel("icon", "-32x32", 1, 32, 32, true);
    addItemToModel("icon", "-32x32", 2, 64, 64, true);

    addItemToModel("icon", "-128x128", 1, 128, 128, true);
    addItemToModel("icon", "-128x128", 2, 256, 256, true);

    addItemToModel("icon", "-256x256", 1, 256, 256, true);
    addItemToModel("icon", "-256x256", 2, 512, 512, true);

    addItemToModel("icon", "-512x512", 1, 512, 512, true);
    addItemToModel("icon", "-512x512", 2, 1024, 1024, true);

}

void MacOSAppIcons::loadIOSDevices(QXmlStreamReader &xml)
{
    if(xml.name() != "MacIcons")
    {
        return;
    }
    if(!xml.readNextStartElement())
    {
        return;
    }
    while (!xml.atEnd() && xml.name() != "MacIcons")
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

QString MacOSAppIcons::makeFileNameSuffix(const QString &iosVersion, int width, int height, int pixelsPerPoint) const
{
    if(pixelsPerPoint < 1)
    {
        pixelsPerPoint = 1;
    }
    QString suffix = QString("icon_%1x%2").arg(width/pixelsPerPoint).arg(height/pixelsPerPoint);
    if(pixelsPerPoint > 1)
    {
        suffix.append(QString("@%1x").arg(pixelsPerPoint));
    }
    return suffix;
}

void MacOSAppIcons::addItemToModel(const QString &deviceName, const QString &iosVersion,
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
                  suffix, deviceName,
                  IMG_ACTION_KEEP_TRANSPARENCY|IMG_ACTION_DONT_PREPEND_FILENAME);
    QVariant v;
    v.setValue(device);
    col0->setData(v, MyClassRole);

    int column = 0;
    int row = m_model.rowCount();
    m_model.setItem(row, column++, col0);
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(width)));
    m_model.setItem(row, column++, new QStandardItem(QString("%1").arg(pixelsPerPoint)));

    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
