#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QXmlStreamReader>
#include <QHeaderView>

#include "DeviceTableView.h"

DeviceTableView::DeviceTableView(QWidget *parent)
    : QTableView(parent)
    , m_model(this)
{
}

const QStandardItemModel &DeviceTableView::getModel() const
{
    return m_model;
}

QList<Device> DeviceTableView::getIOSDevices(const IOSDeviceProperties *prop) const
{
    QList<Device> devices;
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        if(isDeviceChecked(row))
        {
            QModelIndex si = m_model.index(row, 0);
            Device device = si.data(MyClassRole).value<Device>();

            if(prop)
            {
                if(prop->m_referenceImageSize > -1)
                {
                    device.m_referenceImageSize = prop->m_referenceImageSize;
                }
                if(prop->m_scaling != IMG_SCALING_NONE)
                {
                    device.m_scaling = prop->m_scaling;
                }
                device.m_trueColor = prop->m_trueColor;
                device.m_dither  = prop->m_dither;
            }
            devices.append(device);
        }
    }
    return devices;
}

int DeviceTableView::getCheckedDevicesCount() const
{
    int numDevices = 0;
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        if(m_model.item(row, 0)->checkState() == Qt::Checked)
        {
            numDevices++;
        }
    }
    return numDevices;
}

void DeviceTableView::enableListItems(bool enable)
{
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        m_model.item(row, 0)->setEnabled(enable);
    }
}

int DeviceTableView::getFileCount() const
{
    return m_model.rowCount();
}

QModelIndex DeviceTableView::getCurrentIndex() const
{
    return selectedIndexes().size() > 0 ? selectedIndexes().first() : QModelIndex();
}

void DeviceTableView::removeDuplicateDevices(const QString &deviceNamePlusSuffix)
{
    for(int row = 0; row < m_model.rowCount(); )
    {
        QModelIndex si = m_model.index(row, 0);
        Device device = si.data(MyClassRole).value<Device>();
        QString name_ = device.m_name.append(device.m_suffix);

        if(name_ == deviceNamePlusSuffix)
        {
            m_model.removeRow( row );
        }
        else
        {
            row ++;
        }
    }
}
