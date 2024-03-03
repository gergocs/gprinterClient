# gprinter client for gPrinterESP

This is a client for the gPrinterESP. It is written in c++ and uses QT for the GUI.

## Installation

To install the client, you need to have QT installed and postgres. Then you can simply run the following commands:

```bash
cmake . -Bbuild -DCMAKE_PREFIX_PATH=QTPATH -DPostgreSQL_ROOT=POSTGRESPATH -DDEFAULT_GCODE_PATH="DDEFAULT_GCODE_PATH" -G Ninja
ninja -C build
```

Replace QTPATH with the path to your QT installation, POSTGRESPATH with the path to your postgres installation and
DEFAULT_GCODE_PATH with the path to the default gcode folder.

## Usage
 TODO

## Features
- [x] Connect to gPrinterESP
- [x] Send gcode to gPrinterESP
- [x] Save gcode to database
- [x] Load gcode from database
- [x] Load and parse gcode from file
- [ ] Receive status updates from gPrinterESP
- [ ] Receive temperature updates from gPrinterESP
- [ ] Receive progress updates from gPrinterESP
- [ ] Receive error updates from gPrinterESP
- [ ] Receive print updates from gPrinterESP
- [ ] Receive print time updates from gPrinterESP
- [ ] Nice GUI

## License
