import click
from supplr import supplr_cli_lib


@click.group()
def supplr():
    pass


@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--channel", required=True, type=int, help="Number of channel (1-128)")
@click.option("--voltage", required=True, type=float, help="Settable voltage")
def set_channel(board, channel, voltage):
    supplr_cli_lib.set_channel(board, channel, voltage)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--voltage", required=True, type=float, help="Settable voltage")
def set_channels(board, voltage):
    supplr_cli_lib.set_channels(board, voltage)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--channel", required=True, type=int, help="Number of channel (1-128)")
def read_channel(board, channel):
    supplr_cli_lib.print_read_channel(board, channel)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def read_channels(board):
    supplr_cli_lib.read_channels(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def ref_voltage(board):
    supplr_cli_lib.print_ref_voltage(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def hv_supply_voltage(board):
    supplr_cli_lib.print_hv_supply_voltage(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def mez_temps(board):
    supplr_cli_lib.print_mez_temp(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
def reset(board):
    supplr_cli_lib.reset(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--file", required=True, type=str, help="Path to file")
def set_channel_file(board, file):
    supplr_cli_lib.set_channel_file(board, file)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board")
@click.option("--file", required=True, type=str, help="Path to file")
def read_channel_file(board, file):
    supplr_cli_lib.read_channel_file(board, file)

