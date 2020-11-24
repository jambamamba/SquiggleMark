#ifndef IosDeviceIcons_H
#define IosDeviceIcons_H

#include <gd.h>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTableView>

#include "MainWindowInterface.h"
#include "WatermarkPlacement.h"
#include "DeviceTableView.h"

class QXmlStreamWriter;
class QXmlStreamReader;


class IosDeviceIcons : public DeviceTableView
{
    Q_OBJECT
public:
    explicit IosDeviceIcons(QWidget *parent = 0);
    virtual void loadIOSDevices(QXmlStreamReader &xml);
    virtual void saveIOSDevices(QXmlStreamWriter &writer) const;
protected:
signals:

public slots:

private:
    void loadDefaultDevices();
    bool isDeviceChecked(int row) const;
    virtual QString makeFileNameSuffix(const QString &iosVersion, int width, int height, int pixelsPerPoint) const;
    void addItemToModel(const QString &deviceName, const QString &iosVersion,
                        int pixelsPerPoint, int width, int height, bool checked);
};

#endif // IosDeviceIcons_H
