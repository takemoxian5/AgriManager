/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "Section.h"
#include "SimpleMissionItem.h"
#include "FirmwarePlugin.h"

QGC_LOGGING_CATEGORY(AgriSectionLog, "AgriSectionLog")

const char* AgriSection::_gimbalPitchName =                   "GimbalPitch";
const char* AgriSection::_gimbalYawName =                     "GimbalYaw";
const char* AgriSection::_agriActionName =                  "AgriAction";
const char* AgriSection::_agriPhotoIntervalDistanceName =   "AgriPhotoIntervalDistance";
const char* AgriSection::_agriPhotoIntervalTimeName =       "AgriPhotoIntervalTime";
const char* AgriSection::_agriModeName =                    "AgriMode";

QMap<QString, FactMetaData*> AgriSection::_metaDataMap;

AgriSection::AgriSection(Vehicle* vehicle, QObject* parent)
    : Section(vehicle, parent)
    , _available(false)
    , _settingsSpecified(false)
    , _specifyGimbal(false)
    , _specifyAgriMode(false)
    , _gimbalYawFact                    (0, _gimbalYawName,                     FactMetaData::valueTypeDouble)
    , _gimbalPitchFact                  (0, _gimbalPitchName,                   FactMetaData::valueTypeDouble)
    , _agriActionFact                 (0, _agriActionName,                  FactMetaData::valueTypeDouble)
    , _agriPhotoIntervalDistanceFact  (0, _agriPhotoIntervalDistanceName,   FactMetaData::valueTypeDouble)
    , _agriPhotoIntervalTimeFact      (0, _agriPhotoIntervalTimeName,       FactMetaData::valueTypeUint32)
    , _agriModeFact                   (0, _agriModeName,                    FactMetaData::valueTypeUint32)
    , _dirty(false)
{
    if (_metaDataMap.isEmpty()) {
        _metaDataMap = FactMetaData::createMapFromJsonFile(QStringLiteral(":/json/AgriSection.FactMetaData.json"), NULL /* metaDataParent */);
    }

    _gimbalPitchFact.setMetaData                    (_metaDataMap[_gimbalPitchName]);
    _gimbalYawFact.setMetaData                      (_metaDataMap[_gimbalYawName]);
    _agriActionFact.setMetaData                   (_metaDataMap[_agriActionName]);
    _agriPhotoIntervalDistanceFact.setMetaData    (_metaDataMap[_agriPhotoIntervalDistanceName]);
    _agriPhotoIntervalTimeFact.setMetaData        (_metaDataMap[_agriPhotoIntervalTimeName]);
    _agriModeFact.setMetaData                     (_metaDataMap[_agriModeName]);

    _gimbalPitchFact.setRawValue                    (_gimbalPitchFact.rawDefaultValue());
    _gimbalYawFact.setRawValue                      (_gimbalYawFact.rawDefaultValue());
    _agriActionFact.setRawValue                   (_agriActionFact.rawDefaultValue());
    _agriPhotoIntervalDistanceFact.setRawValue    (_agriPhotoIntervalDistanceFact.rawDefaultValue());
    _agriPhotoIntervalTimeFact.setRawValue        (_agriPhotoIntervalTimeFact.rawDefaultValue());
    _agriModeFact.setRawValue                     (_agriModeFact.rawDefaultValue());

    connect(this,                               &AgriSection::specifyGimbalChanged,       this, &AgriSection::_specifyChanged);
    connect(this,                               &AgriSection::specifyAgriModeChanged,   this, &AgriSection::_specifyChanged);

    connect(&_agriActionFact,                 &Fact::valueChanged,                        this, &AgriSection::_agriActionChanged);

    connect(&_gimbalPitchFact,                  &Fact::valueChanged,                        this, &AgriSection::_setDirty);
    connect(&_gimbalYawFact,                    &Fact::valueChanged,                        this, &AgriSection::_setDirty);
    connect(&_agriPhotoIntervalDistanceFact,  &Fact::valueChanged,                        this, &AgriSection::_setDirty);
    connect(&_agriPhotoIntervalTimeFact,      &Fact::valueChanged,                        this, &AgriSection::_setDirty);
    connect(&_agriModeFact,                   &Fact::valueChanged,                        this, &AgriSection::_setDirty);
    connect(this,                               &AgriSection::specifyGimbalChanged,       this, &AgriSection::_setDirty);
    connect(this,                               &AgriSection::specifyAgriModeChanged,   this, &AgriSection::_setDirty);

    connect(this,                               &AgriSection::specifyGimbalChanged,       this, &AgriSection::_updateSpecifiedGimbalYaw);
    connect(&_gimbalYawFact,                    &Fact::valueChanged,                        this, &AgriSection::_updateSpecifiedGimbalYaw);
}

void AgriSection::setSpecifyGimbal(bool specifyGimbal)
{
    if (specifyGimbal != _specifyGimbal) {
        _specifyGimbal = specifyGimbal;
        emit specifyGimbalChanged(specifyGimbal);
    }
}

void AgriSection::setSpecifyAgriMode(bool specifyAgriMode)
{
    if (specifyAgriMode != _specifyAgriMode) {
        _specifyAgriMode = specifyAgriMode;
        emit specifyAgriModeChanged(specifyAgriMode);
    }
}

int AgriSection::itemCount(void) const
{
    int itemCount = 0;

    if (_specifyGimbal) {
        itemCount++;
    }
    if (_specifyAgriMode) {
        itemCount++;
    }
    if (_agriActionFact.rawValue().toInt() != AgriActionNone) {
        itemCount++;
    }

    return itemCount;
}

void AgriSection::setDirty(bool dirty)
{
    if (_dirty != dirty) {
        _dirty = dirty;
        emit dirtyChanged(_dirty);
    }
}

void AgriSection::appendSectionItems(QList<MissionItem*>& items, QObject* missionItemParent, int& nextSequenceNumber)
{
    // IMPORTANT NOTE: If anything changes here you must also change AgriSection::scanForSection

    if (_specifyAgriMode) {
        MissionItem* item = new MissionItem(nextSequenceNumber++,
                                            MAV_CMD_SET_CAMERA_MODE,
                                            MAV_FRAME_MISSION,
                                            0,                                      // agri id, all agris
                                            _agriModeFact.rawValue().toDouble(),
                                            NAN,                                    // Audio off/on
                                            NAN, NAN, NAN, NAN,                     // param 4-7 reserved
                                            true,                                   // autoContinue
                                            false,                                  // isCurrentItem
                                            missionItemParent);
        items.append(item);
    }

    if (_specifyGimbal) {
        MissionItem* item = new MissionItem(nextSequenceNumber++,
                                            MAV_CMD_DO_MOUNT_CONTROL,
                                            MAV_FRAME_MISSION,
                                            _gimbalPitchFact.rawValue().toDouble(),
                                            0,                                      // Gimbal roll
                                            _gimbalYawFact.rawValue().toDouble(),
                                            0, 0, 0,                                // param 4-6 not used
                                            MAV_MOUNT_MODE_MAVLINK_TARGETING,
                                            true,                                   // autoContinue
                                            false,                                  // isCurrentItem
                                            missionItemParent);
        items.append(item);
    }

    if (_agriActionFact.rawValue().toInt() != AgriActionNone) {
        MissionItem* item = NULL;

        switch (_agriActionFact.rawValue().toInt()) {
        case TakePhotosIntervalTime:
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_IMAGE_START_CAPTURE,
                                   MAV_FRAME_MISSION,
                                   0,                                               // Agri ID, all agris
                                   _agriPhotoIntervalTimeFact.rawValue().toInt(), // Interval
                                   0,                                               // Unlimited photo count
                                   NAN, NAN, NAN, NAN,                              // param 4-7 reserved
                                   true,                                            // autoContinue
                                   false,                                           // isCurrentItem
                                   missionItemParent);
            break;

        case TakePhotoIntervalDistance:
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_DO_SET_CAM_TRIGG_DIST,
                                   MAV_FRAME_MISSION,
                                   _agriPhotoIntervalDistanceFact.rawValue().toDouble(),  // Trigger distance
                                   0,                                                       // No shutter integartion
                                   1,                                                       // Trigger immediately
                                   0, 0, 0, 0,                                              // param 4-7 not used
                                   true,                                                    // autoContinue
                                   false,                                                   // isCurrentItem
                                   missionItemParent);
            break;

        case TakeVideo:
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_VIDEO_START_CAPTURE,
                                   MAV_FRAME_MISSION,
                                   0,                           // agri id = 0, all agris
                                   0,                           // No CAMERA_CAPTURE_STATUS streaming
                                   NAN, NAN, NAN, NAN, NAN,     // param 3-7 reserved
                                   true,                        // autoContinue
                                   false,                       // isCurrentItem
                                   missionItemParent);
            break;

        case StopTakingVideo:
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_VIDEO_STOP_CAPTURE,
                                   MAV_FRAME_MISSION,
                                   0,                               // Agri ID, all agris
                                   NAN, NAN, NAN, NAN, NAN, NAN,    // param 2-7 reserved
                                   true,                            // autoContinue
                                   false,                           // isCurrentItem
                                   missionItemParent);
            break;

        case StopTakingPhotos:
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_DO_SET_CAM_TRIGG_DIST,
                                   MAV_FRAME_MISSION,
                                   0,                               // Trigger distance = 0 means stop
                                   0, 0, 0, 0, 0, 0,                // param 2-7 not used
                                   true,                            // autoContinue
                                   false,                           // isCurrentItem
                                   missionItemParent);
            items.append(item);
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_IMAGE_STOP_CAPTURE,
                                   MAV_FRAME_MISSION,
                                   0,                               // agri id, all agris
                                   NAN, NAN, NAN, NAN, NAN, NAN,    // param 2-7 reserved
                                   true,                            // autoContinue
                                   false,                           // isCurrentItem
                                   missionItemParent);
            break;

        case TakePhoto:
            item = new MissionItem(nextSequenceNumber++,
                                   MAV_CMD_IMAGE_START_CAPTURE,
                                   MAV_FRAME_MISSION,
                                   0,                           // agri id = 0, all agris
                                   0,                           // Interval (none)
                                   1,                           // Take 1 photo
                                   NAN, NAN, NAN, NAN,          // param 4-7 reserved
                                   true,                        // autoContinue
                                   false,                       // isCurrentItem
                                   missionItemParent);
            break;
        }
        if (item) {
            items.append(item);
        }
    }
}

bool AgriSection::_scanGimbal(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_DO_MOUNT_CONTROL) {
            if (missionItem.param2() == 0 && missionItem.param4() == 0 && missionItem.param5() == 0 && missionItem.param6() == 0 && missionItem.param7() == MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                setSpecifyGimbal(true);
                gimbalPitch()->setRawValue(missionItem.param1());
                gimbalYaw()->setRawValue(missionItem.param3());
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanTakePhoto(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_IMAGE_START_CAPTURE) {
            if (missionItem.param1() == 0 && missionItem.param2() == 0 && missionItem.param3() == 1) {
                agriAction()->setRawValue(TakePhoto);
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanTakePhotosIntervalTime(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_IMAGE_START_CAPTURE) {
            if (missionItem.param1() == 0 && missionItem.param2() >= 1 && missionItem.param3() == 0) {
                agriAction()->setRawValue(TakePhotosIntervalTime);
                agriPhotoIntervalTime()->setRawValue(missionItem.param2());
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanStopTakingPhotos(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_DO_SET_CAM_TRIGG_DIST) {
            if (missionItem.param1() == 0 && missionItem.param2() == 0 && missionItem.param3() == 0 && missionItem.param4() == 0 && missionItem.param5() == 0 && missionItem.param6() == 0 && missionItem.param7() == 0) {
                if (scanIndex < visualItems->count() - 1) {
                    SimpleMissionItem* nextItem = visualItems->value<SimpleMissionItem*>(scanIndex + 1);
                    if (nextItem) {
                        MissionItem& nextMissionItem = nextItem->missionItem();
                        if (nextMissionItem.command() == MAV_CMD_IMAGE_STOP_CAPTURE && nextMissionItem.param1() == 0) {
                            agriAction()->setRawValue(StopTakingPhotos);
                            visualItems->removeAt(scanIndex)->deleteLater();
                            visualItems->removeAt(scanIndex)->deleteLater();
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool AgriSection::_scanTriggerStartDistance(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_DO_SET_CAM_TRIGG_DIST) {
            if (missionItem.param1() > 0 && missionItem.param2() == 0 && missionItem.param3() == 1 && missionItem.param4() == 0 && missionItem.param5() == 0 && missionItem.param6() == 0 && missionItem.param7() == 0) {
                agriAction()->setRawValue(TakePhotoIntervalDistance);
                agriPhotoIntervalDistance()->setRawValue(missionItem.param1());
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanTriggerStopDistance(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_DO_SET_CAM_TRIGG_DIST) {
            if (missionItem.param1() == 0 && missionItem.param2() == 0 && missionItem.param3() == 0 && missionItem.param4() == 0 && missionItem.param5() == 0 && missionItem.param6() == 0 && missionItem.param7() == 0) {
                agriAction()->setRawValue(TakePhotoIntervalDistance);
                agriPhotoIntervalDistance()->setRawValue(missionItem.param1());
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanTakeVideo(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_VIDEO_START_CAPTURE) {
            if (missionItem.param1() == 0 && missionItem.param2() == 0) {
                agriAction()->setRawValue(TakeVideo);
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanStopTakingVideo(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_VIDEO_STOP_CAPTURE) {
            if (missionItem.param1() == 0) {
                agriAction()->setRawValue(StopTakingVideo);
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::_scanSetAgriMode(QmlObjectListModel* visualItems, int scanIndex)
{
    SimpleMissionItem* item = visualItems->value<SimpleMissionItem*>(scanIndex);
    if (item) {
        MissionItem& missionItem = item->missionItem();
        if ((MAV_CMD)item->command() == MAV_CMD_SET_CAMERA_MODE) {
            // We specifically don't test param 5/6/7 since we don't have NaN persistence for those fields
            if (missionItem.param1() == 0 && (missionItem.param2() == 0 || missionItem.param2() == 1) && qIsNaN(missionItem.param3())) {
                setSpecifyAgriMode(true);
                agriMode()->setRawValue(missionItem.param2());
                visualItems->removeAt(scanIndex)->deleteLater();
                return true;
            }
        }
    }

    return false;
}

bool AgriSection::scanForSection(QmlObjectListModel* visualItems, int scanIndex)
{
    bool foundGimbal = false;
    bool foundAgriAction = false;
    bool foundAgriMode = false;

    qCDebug(AgriSectionLog) << "AgriSection::scanForAgriSection visualItems->count():scanIndex;" << visualItems->count() << scanIndex;

    if (!_available || scanIndex >= visualItems->count()) {
        return false;
    }

    // Scan through the initial mission items for possible mission settings

    while (visualItems->count() > scanIndex) {
        if (!foundGimbal && _scanGimbal(visualItems, scanIndex)) {
            foundGimbal = true;
            continue;
        }
        if (!foundAgriAction && _scanTakePhoto(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriAction && _scanTakePhotosIntervalTime(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriAction && _scanStopTakingPhotos(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriAction && _scanTriggerStartDistance(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriAction && _scanTriggerStopDistance(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriAction && _scanTakeVideo(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriAction && _scanStopTakingVideo(visualItems, scanIndex)) {
            foundAgriAction = true;
            continue;
        }
        if (!foundAgriMode && _scanSetAgriMode(visualItems, scanIndex)) {
            foundAgriMode = true;
            continue;
        }
        break;
    }

    qCDebug(AgriSectionLog) << "AgriSection::scanForAgriSection foundGimbal:foundAgriAction:foundAgriMode;" << foundGimbal << foundAgriAction << foundAgriMode;

    _settingsSpecified = foundGimbal || foundAgriAction || foundAgriMode;
    emit settingsSpecifiedChanged(_settingsSpecified);

    return _settingsSpecified;
}

void AgriSection::_setDirty(void)
{
    setDirty(true);
}

void AgriSection::_setDirtyAndUpdateItemCount(void)
{
    emit itemCountChanged(itemCount());
    setDirty(true);
}

void AgriSection::setAvailable(bool available)
{
    if (_available != available) {
        _available = available;
        emit availableChanged(available);
    }
}

double AgriSection::specifiedGimbalYaw(void) const
{
    return _specifyGimbal ? _gimbalYawFact.rawValue().toDouble() : std::numeric_limits<double>::quiet_NaN();
}

void AgriSection::_updateSpecifiedGimbalYaw(void)
{
    emit specifiedGimbalYawChanged(specifiedGimbalYaw());
}

void AgriSection::_updateSettingsSpecified(void)
{
    bool newSettingsSpecified = _specifyGimbal || _specifyAgriMode || _agriActionFact.rawValue().toInt() != AgriActionNone;
    if (newSettingsSpecified != _settingsSpecified) {
        _settingsSpecified = newSettingsSpecified;
        emit settingsSpecifiedChanged(newSettingsSpecified);
    }
}

void AgriSection::_specifyChanged(void)
{
    _setDirtyAndUpdateItemCount();
    _updateSettingsSpecified();
}

void AgriSection::_agriActionChanged(void)
{
    _setDirtyAndUpdateItemCount();
    _updateSettingsSpecified();
}

bool AgriSection::agriModeSupported(void) const
{
    return _vehicle->firmwarePlugin()->supportedMissionCommands().contains(MAV_CMD_SET_CAMERA_MODE);
}
