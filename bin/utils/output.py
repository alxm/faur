import sys

class Color:
    Black = '0;30'
    DarkGray = '1;30'
    Red = '0;31'
    LightRed = '1;31'
    Green = '0;32'
    LightGreen = '1;32'
    Brown = '0;33'
    Yellow = '1;33'
    Blue = '0;34'
    LightBlue = '1;34'
    Purple = '0;35'
    LightPurple = '1;35'
    Cyan = '0;36'
    LightCyan = '1;36'
    LightGray = '0;37'
    White = '1;37'

class Output:
    @staticmethod
    def colored(text, color):
        print('\033[{}m{}\033[0m'.format(color, text), end = '')

    @staticmethod
    def title(text, args):
        arguments = ' '.join(args[1 : ]) + ' ' if len(args) > 1 else ''
        whole_text = ' a2x_{} {}'.format(text, arguments)
        border = '-' * len(whole_text) + '\n'

        Output.colored(border, Color.DarkGray)
        Output.colored(' a', Color.LightBlue)
        Output.colored('2', Color.LightGreen)
        Output.colored('x', Color.Yellow)
        Output.colored('_{} '.format(text), Color.White)
        print(arguments)
        Output.colored(border, Color.DarkGray)

    @staticmethod
    def note(text):
        Output.colored('[ Note ] ', Color.LightGreen)
        print(text)

    @staticmethod
    def info(text):
        Output.colored('[ Info ] ', Color.LightBlue)
        print(text)

    @staticmethod
    def error(text):
        Output.colored('[ Error ] ', Color.LightRed)
        print(text)
        sys.exit(1)

    @staticmethod
    def shell(text):
        Output.colored('[ Shell ] ', Color.LightPurple)
        print(text)
