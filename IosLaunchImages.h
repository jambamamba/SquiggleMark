#ifndef IosLaunchImages_H
#define IosLaunchImages_H

#include <gd.h>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTableView>

#include "MainWindowInterface.h"
#include "WatermarkPlacement.h"
#include "DeviceTableView.h"

class QXmlStreamWriter;
class QXmlStreamReader;


class IosLaunchImages : public DeviceTableView
{
    Q_OBJECT
public:
    explicit IosLaunchImages(QWidget *parent = 0);
    virtual void loadIOSDevices(QXmlStreamReader &xml);
    virtual void saveIOSDevices(QXmlStreamWriter &writer) const;
    virtual QList<Device> getIOSDevices() const;
protected:
    void addItemToModel(const QString &deviceName, const QString &iosVersion,
                        int pixelsPerPoint, int width, int height, bool checked, bool isReferenceDevice);
signals:

public slots:

private:
    void swapWidthHeight(Device &device) const;
    void loadDefaultDevices();
    bool isDeviceChecked(int row) const;
    virtual QString makeFileNameSuffix(const QString &iosVersion, int width, int height, int scale) const;
};

#endif // IosLaunchImages_H
