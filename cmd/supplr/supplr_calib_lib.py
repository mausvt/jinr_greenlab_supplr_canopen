from time import sleep
import datetime
import subprocess
import csv
import os
import requests
import supplr.supplr_cli_lib as supplr_cli_lib


IP_MULTPLEX = "<IP_address_RPi_multiplexer>"  
CAL_DIR_RAW = "<Path_to_save_calibration_files>"
DELAY = 1.5 #sec

def set_channel_multPlex(channel, level):
    buf = {"channel": channel, "level": level}
    with requests.post("http://" + IP_MULTPLEX + "/api/channels", json=buf) as resp:
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"Error: {message}")
            return

def start_k2000(kport):
    command = 'sudo chown root:adm /dev/ttyUSB'+str(kport)
    subprocess.check_output(command,shell=True)
    command_root = 'find /dev/bus/usb -type c | sudo xargs chown root:adm'
    subprocess.check_output(command_root,shell=True)
    ttycmd = 'echo ":SYST:BEEP:STAT OFF">>/dev/ttyUSB'+str(kport)
    subprocess.check_output(ttycmd,shell=True)

def k2000_get_voltage(kport):
    ttycmd = 'echo ":MEAS?">>/dev/ttyUSB'+str(kport)+' && head -n1 /dev/ttyUSB'+str(kport)
    voltage_from_k2000 = float(subprocess.check_output(ttycmd,shell=True))
    return voltage_from_k2000

def multPlex_channel(channel):
    if 1<=channel<=128:
        mplex = channel
    elif channel == 0:
        return 0
    return mplex

# Calibration 1-128 ch 128 points
def multPlex_calibration(board_sn, start_channel, finish_channel, kport):
    start = datetime.datetime.today().replace(microsecond=0)
    global REF
    global ref_value
    ref_value = supplr_cli_lib.ref_voltage(board_sn)['value']
    if ref_value > 3.8:
        REF = "4.096"
    elif 2.3<ref_value<3:
        REF = "2.500"
    elif 1.9<ref_value<2.2:
        REF = "2.048"
    elif 1.1<ref_value<1.4:
        REF = "1.250"
    print("Ref.value: " + str(ref_value))
    for channel in range(start_channel,finish_channel+1):
        channel_multPlex = multPlex_channel(channel)
        set_channel_multPlex(0, 0)
        sleep(DELAY)
        set_channel_multPlex(channel_multPlex, 1)
        supplr_cli_lib.reset_network()
        calibration_channel_128(board_sn, channel, kport)
    set_channel_multPlex(0, 0)
    try:
        supplr_cli_lib.reset_network()
    except:
        print("ERROR RESET CAN NETWORK!!!")
        print("RESET CAN NETWORK!!!")
        # restart_can_network("supplr-server")
        sleep(DELAY)
        supplr_cli_lib.reset_network()
    finish = datetime.datetime.today().replace(microsecond=0)
    print(f"CALIBRATION FIISHED!")
    print(f"FULL CALIBRATION TIME: {finish - start}")

def calibration_channel_128(board_sn, channel, kport):
    start_k2000(kport)
    board_dir = CAL_DIR_RAW + "/board_" + str(board_sn) + "_ref" + REF
    if not os.path.exists(board_dir):
        os.makedirs(board_dir)
    file_name = board_dir + "/board_"+str(board_sn)+"_channel_"+str(channel)+"_points_128.txt"
    print(file_name)
    csv_file = open(file_name, mode='w')
    writer = csv.writer(csv_file)
    start = datetime.datetime.today().replace(microsecond=0)
    writer.writerow(["Timestamp: " + str(start)])
    writer.writerow(["Board SN: " + str(board_sn)])
    writer.writerow(["Ref.value: " + str(ref_value)])
    writer.writerow(["DAC,bit", "ADC,code", "K2000,V"])
    voltage_bit_values = [i-1 for i in range(1,16385) if i%128 == 0]
    voltage_bit_values.insert(0,0)
    for voltage_bit_value in voltage_bit_values:
        DAC_code = int(hex(voltage_bit_value), 16)
        supplr_cli_lib.set_channel_bit(board_sn, channel, DAC_code)
        sleep(DELAY)
        try:
            ADC_code = supplr_cli_lib.read_channel_adc_code(board_sn, channel)['value']
            while ADC_code < 0:
                ADC_code = supplr_cli_lib.read_channel_adc_code(board_sn, channel)['value']
                sleep(DELAY)
        except:
            print("EXCEPT BLOCK. SOME WRONG!?")
            ADC_code = supplr_cli_lib.read_channel_adc_code(board_sn, channel)['value']
        voltage_k2000 = k2000_get_voltage(kport)
        writer.writerow([voltage_bit_value, ADC_code, voltage_k2000])
        cur_time = datetime.datetime.today().replace(microsecond=0)
        print(f"{cur_time}  |   DAC code: {voltage_bit_value:6}     ADC code: {ADC_code:8}     K2000: {round(voltage_k2000,4):10}")
    csv_file.close()
    finish = datetime.datetime.today().replace(microsecond=0)
    print(f"Start time: {start}")
    print(f"Finish time: {finish}")
    print(f"Calibration time: {finish - start}")
