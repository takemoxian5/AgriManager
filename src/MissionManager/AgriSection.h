/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "Section.h"
#include "ComplexMissionItem.h"
#include "MissionItem.h"
#include "Fact.h"

class AgriSection : public Section
{
    Q_OBJECT

public:
    AgriSection(Vehicle* vehicle, QObject* parent = NULL);

    // These enum values must match the json meta data

    enum AgriAction {
        AgriActionNone,
        TakePhotosIntervalTime,
        TakePhotoIntervalDistance,
        StopTakingPhotos,
        TakeVideo,
        StopTakingVideo,
        TakePhoto
    };    
    Q_ENUMS(AgriAction)

    enum AgriMode {
        AgriModePhoto,
        AgriModeVideo
    };
    Q_ENUMS(AgriMode)

    Q_PROPERTY(bool     specifyGimbal                   READ specifyGimbal                  WRITE setSpecifyGimbal              NOTIFY specifyGimbalChanged)
    Q_PROPERTY(Fact*    gimbalPitch                     READ gimbalPitch                                                        CONSTANT)
    Q_PROPERTY(Fact*    gimbalYaw                       READ gimbalYaw                                                          CONSTANT)
    Q_PROPERTY(Fact*    agriAction                    READ agriAction                                                       CONSTANT)
    Q_PROPERTY(Fact*    agriPhotoIntervalTime         READ agriPhotoIntervalTime                                            CONSTANT)
    Q_PROPERTY(Fact*    agriPhotoIntervalDistance     READ agriPhotoIntervalDistance                                        CONSTANT)
    Q_PROPERTY(bool     agriModeSupported             READ agriModeSupported                                                CONSTANT)   ///< true: agriMode is supported by this vehicle
    Q_PROPERTY(bool     specifyAgriMode               READ specifyAgriMode              WRITE setSpecifyAgriMode          NOTIFY specifyAgriModeChanged)
    Q_PROPERTY(Fact*    agriMode                      READ agriMode                                                         CONSTANT)   ///< MAV_CMD_SET_CAMERA_MODE.param2

    bool    specifyGimbal               (void) const { return _specifyGimbal; }
    Fact*   gimbalYaw                   (void) { return &_gimbalYawFact; }
    Fact*   gimbalPitch                 (void) { return &_gimbalPitchFact; }
    Fact*   agriAction                (void) { return &_agriActionFact; }
    Fact*   agriPhotoIntervalTime     (void) { return &_agriPhotoIntervalTimeFact; }
    Fact*   agriPhotoIntervalDistance (void) { return &_agriPhotoIntervalDistanceFact; }
    bool    agriModeSupported         (void) const;
    bool    specifyAgriMode           (void) const { return _specifyAgriMode; }
    Fact*   agriMode                  (void) { return &_agriModeFact; }

    void setSpecifyGimbal       (bool specifyGimbal);
    void setSpecifyAgriMode   (bool specifyAgriMode);

    ///< Signals specifiedGimbalYawChanged
    ///< @return The gimbal yaw specified by this item, NaN if not specified
    double specifiedGimbalYaw(void) const;

    // Overrides from Section
    bool available          (void) const override { return _available; }
    bool dirty              (void) const override { return _dirty; }
    void setAvailable       (bool available) override;
    void setDirty           (bool dirty) override;
    bool scanForSection     (QmlObjectListModel* visualItems, int scanIndex) override;
    void appendSectionItems (QList<MissionItem*>& items, QObject* missionItemParent, int& seqNum) override;
    int  itemCount          (void) const override;
    bool settingsSpecified  (void) const override {return _settingsSpecified; }

signals:
    bool specifyGimbalChanged       (bool specifyGimbal);
    bool specifyAgriModeChanged   (bool specifyAgriMode);
    void specifiedGimbalYawChanged  (double gimbalYaw);

private slots:
    void _setDirty(void);
    void _setDirtyAndUpdateItemCount(void);
    void _updateSpecifiedGimbalYaw(void);
    void _specifyChanged(void);
    void _updateSettingsSpecified(void);
    void _agriActionChanged(void);

private:
    bool _scanGimbal(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanTakePhoto(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanTakePhotosIntervalTime(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanStopTakingPhotos(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanTriggerStartDistance(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanTriggerStopDistance(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanTakeVideo(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanStopTakingVideo(QmlObjectListModel* visualItems, int scanIndex);
    bool _scanSetAgriMode(QmlObjectListModel* visualItems, int scanIndex);

    bool    _available;
    bool    _settingsSpecified;
    bool    _specifyGimbal;
    bool    _specifyAgriMode;
    Fact    _gimbalYawFact;
    Fact    _gimbalPitchFact;
    Fact    _agriActionFact;
    Fact    _agriPhotoIntervalDistanceFact;
    Fact    _agriPhotoIntervalTimeFact;
    Fact    _agriModeFact;
    bool    _dirty;

    static QMap<QString, FactMetaData*> _metaDataMap;

    static const char* _gimbalPitchName;
    static const char* _gimbalYawName;
    static const char* _agriActionName;
    static const char* _agriPhotoIntervalDistanceName;
    static const char* _agriPhotoIntervalTimeName;
    static const char* _agriModeName;
};
