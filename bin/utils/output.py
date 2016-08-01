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
    def title(text):
        whole_text = ' a2x_{} '.format(text)
        border = '-' * len(whole_text)

        print(border)
        Output.colored(' a', Color.LightBlue)
        Output.colored('2', Color.LightGreen)
        Output.colored('x', Color.Yellow)
        Output.colored('_{}\n'.format(text), Color.White)
        print(border)

    @staticmethod
    def note(text):
        Output.colored('[ Note ] ', Color.LightGreen)
        print(text)

    @staticmethod
    def info(text):
        Output.colored('[ Info ] ', Color.Green)
        print(text)

    @staticmethod
    def error(text):
        Output.colored('[ Error ] ', Color.Red)
        print(text)

    @staticmethod
    def shell(text):
        Output.colored('[ Shell ] ', Color.Purple)
        print(text)
