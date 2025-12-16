# Qt Quick/QML Migration Guide

## Overview

The Network Sniffer application has been refactored from Qt Widgets (using `.ui` files) to Qt Quick/QML for a modern, declarative UI approach.

## What Changed

### Architecture Changes

#### Before (Qt Widgets):
```
main.cpp → QApplication → MainWindow (QMainWindow)
                          ↓
                      mainwindow.ui (XML UI definition)
                          ↓
                      QTableView + QTextEdit
```

#### After (Qt Quick/QML):
```
main.cpp → QGuiApplication → QQmlApplicationEngine
                              ↓
                          main.qml (Declarative UI)
                              ↓
                          MainWindow (QObject controller)
                              ↓
                          PacketModel (QAbstractListModel)
```

### File Changes

#### New Files:
- **main.qml** - Complete UI definition in QML
  - ApplicationWindow with MenuBar
  - ToolBar with capture controls
  - SplitView with packet list and details
  - Custom HeaderCell and TableCell components

#### Modified Files:

1. **sniffer.pro**
   ```diff
   - QT += core gui network
   - greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
   + QT += core gui widgets network qml quick quickcontrols2
   
   - FORMS += mainwindow.ui
   ```

2. **main.cpp**
   ```diff
   - #include <QApplication>
   + #include <QGuiApplication>
   + #include <QQmlApplicationEngine>
   + #include <QQmlContext>
   
   - QApplication a(argc, argv);
   - MainWindow w;
   - w.show();
   + QGuiApplication app(argc, argv);
   + QQmlApplicationEngine engine;
   + MainWindow controller;
   + engine.rootContext()->setContextProperty("controller", &controller);
   + engine.load(url);
   ```

3. **mainwindow.h**
   ```diff
   - class MainWindow : public QMainWindow
   + class MainWindow : public QObject
   
   + Q_PROPERTY(PacketModel* packetModel READ getPacketModel CONSTANT)
   + Q_PROPERTY(QVariantList networkInterfaces ...)
   + Q_PROPERTY(QString statusMessage ...)
   + Q_PROPERTY(QString packetDetails ...)
   + Q_PROPERTY(bool capturing ...)
   
   + Q_INVOKABLE void startCapture(const QString &interface);
   + Q_INVOKABLE void stopCapture();
   + Q_INVOKABLE void clearPackets();
   + Q_INVOKABLE void selectPacket(int index);
   + Q_INVOKABLE void saveToPcap();
   ```

4. **mainwindow.cpp**
   ```diff
   - Removed all ui-> interactions
   - Removed Qt Widgets includes
   + Exposed properties for QML binding
   + Implemented Q_INVOKABLE methods
   + Changed network interfaces to QVariantList
   ```

5. **packetmodel.h/cpp**
   ```diff
   - class PacketModel : public QAbstractTableModel
   + class PacketModel : public QAbstractListModel
   
   + enum PacketRoles { NumberRole, TimestampRole, ... }
   + QHash<int, QByteArray> roleNames() const override;
   
   - int columnCount() const
   - QVariant headerData() const
   ```

6. **resources.qrc**
   ```diff
   + <file>main.qml</file>
   ```

#### Removed Files:
- **mainwindow.ui** - No longer needed (replaced by main.qml)

## Key Differences

### UI Definition

**Qt Widgets (XML):**
```xml
<widget class="QPushButton" name="startButton">
  <property name="text">
    <string>Start</string>
  </property>
</widget>
```

**Qt Quick (QML):**
```qml
Button {
    id: startButton
    text: "Start"
    onClicked: {
        if (controller) {
            controller.startCapture(interfaceComboBox.currentValue)
        }
    }
}
```

### Data Binding

**Qt Widgets:**
```cpp
ui->statusBar->showMessage(message);
ui->startButton->setEnabled(false);
```

**Qt Quick:**
```qml
// Automatic binding to property
Label {
    text: controller ? controller.statusMessage : "Ready"
}

Button {
    enabled: !controller.capturing
}
```

### Model-View Integration

**Qt Widgets:**
```cpp
ui->packetTableView->setModel(packetModel);
connect(ui->packetTableView, &QTableView::clicked, ...);
```

**Qt Quick:**
```qml
ListView {
    model: controller ? controller.packetModel : null
    delegate: Rectangle {
        // Access model roles directly
        text: model.number
        text: model.timestamp
    }
}
```

## Benefits of Qt Quick

### 1. Declarative UI
- More readable and maintainable
- Clearer structure and hierarchy
- Less boilerplate code

### 2. Better Separation of Concerns
- UI logic in QML
- Business logic in C++
- Clear controller pattern

### 3. Modern Features
- Hardware acceleration
- Fluid animations
- Touch-friendly
- Better suited for mobile (Android)

### 4. Hot Reload
- QML can be reloaded without recompiling C++
- Faster UI iteration during development

### 5. Flexible Styling
- Easy theme customization
- Consistent across platforms
- Support for Material and Universal styles

## Building

### Requirements
The application now requires:
```bash
# Additional Qt modules
QT += qml quick quickcontrols2
```

### Build Commands
```bash
qmake sniffer.pro
make
sudo ./NetworkSniffer
```

## Running

The application behavior is the same as before:
- Same packet capture functionality
- Same PCAP export
- Same network interface detection
- Improved UI responsiveness

## Compatibility

- **Linux**: Fully supported
- **Android**: Better suited for Qt Quick
- **Cross-platform**: Improved consistency

## Future Enhancements

With Qt Quick, these features are now easier to implement:
- Animated transitions
- Touch gestures
- Mobile-optimized layouts
- Custom visual effects
- Theme switching
- Responsive design

## Migration Checklist

If you want to add new features:

### Adding UI Elements
1. Edit `main.qml` instead of `mainwindow.ui`
2. Use QML components (Button, TextField, etc.)
3. Bind to controller properties or call Q_INVOKABLE methods

### Adding Data Properties
1. Add Q_PROPERTY in `mainwindow.h`
2. Implement getter/setter with NOTIFY signal
3. Emit changed signal when value updates
4. Access in QML: `controller.propertyName`

### Adding Actions
1. Add Q_INVOKABLE method in `mainwindow.h`
2. Implement in `mainwindow.cpp`
3. Call from QML: `controller.methodName(args)`

### Updating Model
1. Keep using QAbstractListModel
2. Define custom roles in enum
3. Implement roleNames()
4. Access in QML delegates: `model.roleName`

## Troubleshooting

### QML Loading Errors
- Check qrc file includes main.qml
- Verify QML syntax with `qmllint main.qml`
- Check console for QML warnings

### Property Binding Issues
- Ensure Q_PROPERTY has NOTIFY signal
- Emit signal when property changes
- Check controller is not null in QML

### Model Not Updating
- Ensure beginInsertRows/endInsertRows calls
- Check roleNames() returns correct hash
- Verify model roles match QML access

## Resources

- [Qt Quick Documentation](https://doc.qt.io/qt-5/qtquick-index.html)
- [QML Reference](https://doc.qt.io/qt-5/qmlreference.html)
- [Qt Quick Controls](https://doc.qt.io/qt-5/qtquickcontrols-index.html)
- [Integrating QML and C++](https://doc.qt.io/qt-5/qtqml-cppintegration-topic.html)
