import click
from supplr import api_supplr


@click.group()
def supplr():
    pass


@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--channel", required=True, type=int, help="Number of channel (1-128)")
@click.option("--voltage", required=True, type=float, help="Settable voltage")
def set_channel(board, channel, voltage):
    api_supplr.set_channel(board, channel, voltage)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--voltage", required=True, type=float, help="Settable voltage")
def set_channels(board, voltage):
    api_supplr.set_channels(board, voltage)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--channel", required=True, type=int, help="Number of channel (1-128)")
def read_channel(board, channel):
    api_supplr.print_read_channel(board, channel)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def read_channels(board):
    api_supplr.read_channels(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def ref_voltage(board):
    api_supplr.print_ref_voltage(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def hv_supply_voltage(board):
    api_supplr.print_hv_supply_voltage(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def mez_temps(board):
    api_supplr.print_mez_temp(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def reset(board):
    api_supplr.reset(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--file", required=True, type=str, help="Path to file")
def set_channel_file(board, file):
    api_supplr.set_channel_file(board, file)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--file", required=True, type=str, help="Path to file")
def read_channel_file(board, file):
    api_supplr.read_channel_file(board, file)

