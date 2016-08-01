import subprocess
import sys

from utils.output import Output

class Shell:
    @staticmethod
    def run(cmd):
        Output.shell(cmd)
        status, output = subprocess.getstatusoutput(cmd)

        for line in output.splitlines():
            print('    {}'.format(line))

        if status != 0:
            sys.exit(status)
