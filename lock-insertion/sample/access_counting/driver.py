#!/usr/local/bin/python3
import sys
import argparse
from argparse import RawTextHelpFormatter
import os
from Dylinx import NaiveSubject

def main(args):
    subject = NaiveSubject(args.config_path)
    for ret_code in subject.step():
        print(ret_code)
        sys.exit()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=RawTextHelpFormatter)
    parser.add_argument(
        "-m",
        "--mode",
        type=str,
        choices=["reverse", "optimize_locks", "fix"],
        required=True,
        help="Configuring the operating mode."
    )
    parser.add_argument(
        "-c",
        "--config_path",
        type=str,
        default=os.getcwd() + "/dylinx-config.yaml",
        help=
        "Specify the path of subject's config file. It should contain following component"
        "\n1. compile_commands_path"
        "\n2. output_directory_path"
        "\n3. instructions"
        "\n   - build_commands"
        "\n   - clean_commands"
    )
    args = parser.parse_args()
    main(args)
