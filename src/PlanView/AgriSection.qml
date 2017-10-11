import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts  1.2

import QGroundControl                   1.0
import QGroundControl.ScreenTools       1.0
import QGroundControl.Controls          1.0
import QGroundControl.FactControls      1.0
import QGroundControl.Palette           1.0

// Agri section for mission item editors
Column {
    anchors.left:   parent.left
    anchors.right:  parent.right
    spacing:        _margin

    property alias exclusiveGroup:  agriSectionHeader.exclusiveGroup
    property alias showSpacer:      agriSectionHeader.showSpacer
    property alias checked:         agriSectionHeader.checked

    property var    _agri:        missionItem.agriSection
    property real   _fieldWidth:    ScreenTools.defaultFontPixelWidth * 16
    property real   _margin:        ScreenTools.defaultFontPixelWidth / 2

    SectionHeader {
        id:             agriSectionHeader
        text:           qsTr("Agri")
        checked:        false
    }

    Column {
        anchors.left:   parent.left
        anchors.right:  parent.right
        spacing:        _margin
        visible:        agriSectionHeader.checked

        FactComboBox {
            id:             agriActionCombo
            anchors.left:   parent.left
            anchors.right:  parent.right
            fact:           _agri.agriAction
            indexModel:     false
        }

        RowLayout {
            anchors.left:   parent.left
            anchors.right:  parent.right
            spacing:        ScreenTools.defaultFontPixelWidth
            visible:        _agri.agriAction.rawValue == 1

            QGCLabel {
                text:               qsTr("Time")
                Layout.fillWidth:   true
            }
            FactTextField {
                fact:                   _agri.agriPhotoIntervalTime
                Layout.preferredWidth:  _fieldWidth
            }
        }

        RowLayout {
            anchors.left:   parent.left
            anchors.right:  parent.right
            spacing:        ScreenTools.defaultFontPixelWidth
            visible:        _agri.agriAction.rawValue == 2

            QGCLabel {
                text:               qsTr("Distance")
                Layout.fillWidth:   true
            }
            FactTextField {
                fact:                   _agri.agriPhotoIntervalDistance
                Layout.preferredWidth:  _fieldWidth
            }
        }

        GridLayout {
            anchors.left:   parent.left
            anchors.right:  parent.right
            columnSpacing:  ScreenTools.defaultFontPixelWidth / 2
            rowSpacing:     0
            columns:        3

            Item { width: 1; height: 1 }
            QGCLabel { text: qsTr("Pitch") }
            QGCLabel { text: qsTr("Yaw") }

            QGCCheckBox {
                id:                 gimbalCheckBox
                text:               qsTr("Gimbal")
                checked:            _agri.specifyGimbal
                onClicked:          _agri.specifyGimbal = checked
                Layout.fillWidth:   true
            }
            FactTextField {
                fact:           _agri.gimbalPitch
                implicitWidth:  ScreenTools.defaultFontPixelWidth * 9
                enabled:        gimbalCheckBox.checked
            }

            FactTextField {
                fact:           _agri.gimbalYaw
                implicitWidth:  ScreenTools.defaultFontPixelWidth * 9
                enabled:        gimbalCheckBox.checked
            }
        }

        RowLayout {
            anchors.left:   parent.left
            anchors.right:  parent.right
            spacing:        ScreenTools.defaultFontPixelWidth
            visible:        _agri.agriModeSupported

            QGCCheckBox {
                id:                 modeCheckBox
                text:               qsTr("Mode")
                checked:            _agri.specifyAgriMode
                onClicked:          _agri.specifyAgriMode = checked
            }
            FactComboBox {
                fact:               _agri.agriMode
                indexModel:         false
                enabled:            modeCheckBox.checked
                Layout.fillWidth:   true
            }
        }
    }
}
