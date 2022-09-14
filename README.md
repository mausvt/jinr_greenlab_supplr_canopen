# Supplr Canopen

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
Set path to supplr_canopen in ~/.bashrc
```
export SUPPLR_CANOPEN_DIR=<path/to/supplr_canopen>
```
Specify the server address in the config.py file
```
cmd/supplr/config.py
IP = "server_address:port"
```
Add calibration files in folder
```
/cmd/calib_files/board<serial_number>_ref2.500/
```
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
Open the configuration file `config.yaml` provide information about the power units (NodeId and BoardSN) and the path to the calibration files
```
CanNetwork: 1
BitrateIndex: 2
CalibFolder: "<path/to/calib_files>"
Nodes:
- NodeID: <Node_number_(1-127)>
  DeviceType: 0x008C0191
  VendorID: 0x000000BE
  ProductCode: 0x00910001
  Revision: 0x00010001
  BoardSN: <Serial_number(example: 2022010015)>
```


# Modify permissions for usb devices
```
find /dev/bus/usb -type c | sudo xargs chown root:adm
```
