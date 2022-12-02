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

```
To view the commands use
```
supplr
```
## Supplr commands
- **can-status** - get CAN network status
- **ext-voltage** - get voltage value from high voltage power supply
- **mez-temps** - get mezzanine temperature
- **read-channel** - get voltage value from one channel
- **read-channels** - get voltage value from all channels
- **read-channel-file** - get the voltage value from the channels described in the file
- **ref-voltage** - get reference voltage value
- **reset** - board reset. Voltage is set to ~0.3V for all channels
- **sever-status** - get information about server status (available/unavailable)
- **set-channel** - set voltage value for one channel
- **set-channels** - set voltage value for all channels
- **set-channel-file** - set the voltage value for the channels described in the file

To view command options use
```
--help
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
Restart=always

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
```
## Supplr commands usage examples

Set voltage to selected channel
```
supplr set-channel --board 10 --channel 1 --voltage 55.5
```
Set voltage to all channels
```
supplr set-channels --board 10 --voltage 55.5
```
Read voltage from selected channel
```
supplr read-channel --board 10 --channel 1
```
Read voltage from all channels
```
supplr read-channels --board 10
```
Reset the voltage value on the board
```
supplr reset --board 10
```
Get the voltage value of the high voltage power supply
```
supplr ext-voltage --board 10
```
