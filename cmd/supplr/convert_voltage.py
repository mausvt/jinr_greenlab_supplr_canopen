SKIPROW = 6

def volt_to_bit(path, voltage):
    lines = []
    last_split_line = ''
    with open(path) as file:
        for i, line in enumerate(file):
            if i < SKIPROW:
                continue
            split_line = line.rstrip().split(',')
            if float(split_line[2])>voltage:
                lines.extend([last_split_line, split_line])
                break
            last_split_line = split_line
    if abs(voltage - float(lines[0][2])) < abs(voltage - float(lines[1][2])):
        return int(lines[0][0])
    else:
        return int(lines[1][0])

def ADC_code_to_volt(path, ADC_code):
    lines = []
    last_split_line = ''
    with open(path) as file:
        for i, line in enumerate(file):
            if i < SKIPROW:
                continue
            split_line = line.rstrip().split(',')
            if float(split_line[1])>ADC_code:
                lines.extend([last_split_line, split_line])
                break
            last_split_line = split_line
    per = (ADC_code-int(lines[0][1]))/(int(lines[1][1])-int(lines[0][1]))*100
    voltage = per*(float(lines[1][2])-float(lines[0][2]))/100 + float(lines[0][2])
    return voltage
    