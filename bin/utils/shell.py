import subprocess

from utils.output import Output

class Shell:
    @staticmethod
    def run(cmd):
        Output.shell(cmd)
        status, output = subprocess.getstatusoutput(cmd)

        if status != 0:
            for line in output.splitlines():
                print('    {}'.format(line))
