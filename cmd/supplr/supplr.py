import click
from supplr import supplr_cli_lib


@click.group()
def supplr():
    pass


@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
@click.option("--channel", required=True, type=int, help="Number of channel (1-128)")
@click.option("--voltage", required=True, type=float, help="Settable voltage")
def set_channel(board, channel, voltage):
    supplr_cli_lib.cli_set_channel(board, channel, voltage)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
@click.option("--voltage", required=True, type=float, help="Settable voltage")
def set_channels(board, voltage):
    supplr_cli_lib.cli_set_channels(board, voltage)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
@click.option("--channel", required=True, type=int, help="Number of channel (1-128)")
def read_channel(board, channel):
    supplr_cli_lib.cli_read_channel(board, channel)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
def read_channels(board):
    supplr_cli_lib.cli_read_channels(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
def ref_voltage(board):
    supplr_cli_lib.cli_ref_voltage(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
def ext_voltage(board):
    supplr_cli_lib.cli_ext_voltage(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
def mez_temps(board):
    supplr_cli_lib.cli_mez_temps(board)

@supplr.command()
@click.option("--board", required=True, type=int, help="Serial number of board (node or SN number)")
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
