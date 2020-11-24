#ifndef DeviceTableView_H
#define DeviceTableView_H

#include <gd.h>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTableView>

#include "WatermarkPlacement.h"

class QXmlStreamWriter;
class QXmlStreamReader;


class DeviceTableView : public QTableView
{
    Q_OBJECT

public:
    explicit DeviceTableView(QWidget *parent = 0);
    const QStandardItemModel &getModel() const;
    int getCheckedDevicesCount() const;
    int getFileCount() const;
    QModelIndex getCurrentIndex() const;
    virtual void loadIOSDevices(QXmlStreamReader &xml) = 0;
    virtual void saveIOSDevices(QXmlStreamWriter &writer) const = 0;
    virtual QList<Device> getIOSDevices(const IOSDeviceProperties *prop = 0) const;
    void enableListItems(bool enable);

protected:
    enum MyRoles {
        MyClassRole = Qt::UserRole + 1
    };

signals:

public slots:

protected:
    QStandardItemModel m_model;

    virtual void removeDuplicateDevices(const QString &deviceNamePlusSuffix);
    virtual void loadDefaultDevices() = 0;
    virtual bool isDeviceChecked(int row) const = 0;
    virtual QString makeFileNameSuffix(const QString &iosVersion, int width, int height, int pixelsPerPoint) const = 0;
    virtual void addItemToModel(const QString &deviceName, const QString &iosVersion,
                        int pixelsPerPoint, int width, int height, bool checked) = 0;
};

#endif // DeviceTableView_H
