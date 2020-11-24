#ifndef iTunesScreenShotImages_H
#define iTunesScreenShotImages_H

#include <gd.h>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTableView>

#include "MainWindowInterface.h"
#include "WatermarkPlacement.h"
#include "DeviceTableView.h"

class QXmlStreamWriter;
class QXmlStreamReader;


class ITunesScreenShots : public DeviceTableView
{
    Q_OBJECT
public:
    explicit ITunesScreenShots(QWidget *parent = 0);
    virtual void loadIOSDevices(QXmlStreamReader &xml);
    virtual void saveIOSDevices(QXmlStreamWriter &writer) const;
    virtual QList<Device> getIOSDevices(const IOSDeviceProperties *prop = 0) const;
protected:
    void addItemToModel(const QString &deviceName, const QString &iosVersion,
                        int pixelsPerPoint, int width, int height, bool checked);
signals:

public slots:

private:
    void swapWidthHeight(Device &device) const;
    void loadDefaultDevices();
    bool isDeviceChecked(int row) const;
    virtual QString makeFileNameSuffix(const QString &iosVersion, int width, int height, int scale) const;
};

#endif // iTunesScreenShotImages_H
