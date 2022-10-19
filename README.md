# Supplr Canopen

# FTDI library
## Install
FTDI drivers allow direct access to the USB device through a DLL. Application software can access the USB device through a series of DLL function calls. You need to unzip and install the driver. Use the instruction described in `ReadMe.txt`. Path to driver
```
soft/libftd2xx-******.tgz
```

## http
Since the http server is using ulfius framework we need to clone it first and some other repos (ulfius dependencies).

```
git clone https://github.com/babelouest/orcania.git
git clone https://github.com/babelouest/yder.git
git clone https://github.com/babelouest/ulfius.git
```

## Command Line Argument Parser
Сargs is a command line argument parser library which can be used to parse argv and argc parameters passed to a main function.

Download the library
```
git clone -b stable git@github.com:likle/cargs.git
```
Use the following to compile Cargs
```
mkdir cargs/build
cd cargs/build
cmake ..
make
```

# Install and start application
To build the application use the command
```
make build
```
*if a required package is missing, use
```
apt-get install lib<package_name>-dev
```

## Start server
```
./build/supplr-server/supplr-server -c /path/to/config.yaml
```

## Start Log journal
```
journalctl -t supplr-log -f
```

## Command line interface
Install supplr
```
cd cmd
pip install -r requirements.txt
sudo python3 setup.py develop
supplr
```
To view command options use
```
--help
```
Command usage examples
```
supplr read-channel --help
supplr read-channel --board 2022010015 --channel 120
supplr set-channel --board 2022010015 --channel 1 --voltage 55
supplr ref-voltage --board 2022010014
```

## Setting up the configuration file
Open the configuration file `config.yaml` provide information about the power units (NodeId and BoardSN), server address and reference voltage. Example
```
# APP settings
ServerAddress: "159.93.78.999:5008"

# Board settings
RefVoltage: "2.500"  # Available values: "1.250", "2.048", "2.500", "4.096"

# CAN Open settings
CanNetwork: 1
BitrateIndex: 2
Nodes:
- NodeID: 12
  DeviceType: 0x008C0191
  VendorID: 0x000000BE
  ProductCode: 0x00910001
  Revision: 0x00010001
  BoardSN: 2022010015
- NodeID: 13
  DeviceType: 0x008C0191
  VendorID: 0x000000BE
  ProductCode: 0x00910001
  Revision: 0x00010001
  BoardSN: 2022010018
```

# Modify permissions for usb devices
```
find /dev/bus/usb -type c | sudo xargs chown root:adm
```

# Systemctl service
For the stable operation of the application, you need to create a service. Create a file called `supplr.service` in the `/etc/systemd/system` directory with the content:
```
[Unit]
Description=supplr
After=network.target remote-fs.target nss-lookup.target

[Service]
User=<Add user>
ExecStart=<path_to_supplr_app>/supplr_canopen/build/supplr-server/supplr-server -c <path_to_supplr_app>/supplr_canopen/config.yaml

[Install]
WantedBy=multi-user.target
```
Perform an update for the added service
```
systemctl daemon-reload
```
To start, update and stop the `supplr-server`, use
```
sudo systemctl start supplr.service
sudo systemctl restart supplr.service
sudo systemctl stop supplr.service
