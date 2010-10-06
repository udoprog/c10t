#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vi:ts=4 sw=4 et

import re
import os.path
import subprocess
from mainwindow import MainWindow


def default_c10t_executable():
    return "../c10t"

def default_minecraft_world():
    return "~/.minecraft/saves/World1"

def default_output_image():
    return os.path.abspath("out.png")


def quote_arg_if_needed(arg):
    """Add quotes if the argument has 'weird' characters.

    This function is extremely simple, and it is not fool-proof.
    Improvements are quite welcome!
    
    WARNING: single-quotes inside the argument will break this!"""

    if re.search(r'''[^-a-zA-Z0-9_.,/+=]''', arg):
        return "'%s'" % (arg,)
    else:
        return arg

def args_to_string(args):
    """Converts a list of arguments to one string that can be copy-pasted
    into a terminal and will work (hopefully)."""

    return " ".join(quote_arg_if_needed(arg) for arg in args)


class Program(object):
    def __init__(self):
        self.win = MainWindow()
        self.args = []

        # Files
        self.win.ui.exepath = default_c10t_executable()
        self.win.ui.world = default_minecraft_world()
        self.win.ui.output = default_output_image()

        self.update_ui_commandline()

        self.win.update_button_callback = self.update_ui_commandline
        self.win.run_button_callback = self.run_command

    def run_command(self):
        self.update_ui_commandline()
        proc = subprocess.Popen(self.args, shell=False)
        # TODO: Add a progress window/progress bar
        # Meanwhile... let's just block this program until c10t finishes...
        # Ugly, but better than nothing.
        proc.communicate() # TODO: Check process returncode

        self.win.load_image(self.win.ui.output)

    def update_ui_commandline(self):
        self.build_commandline()
        self.win.ui.command = args_to_string(self.args)

    def build_commandline(self):
        ui = self.win.ui

        args = [os.path.expanduser(ui.exepath)]

        # Filtering
        if ui.topcheck   : args.extend(["--top"   , str(ui.top   )])
        if ui.bottomcheck: args.extend(["--bottom", str(ui.bottom)])
        if ui.limitscheck:
            args.extend([
                "--limits",
                ",".join(str(x) for x in (
                    ui.limitsnorth,
                    ui.limitssouth,
                    ui.limitseast,
                    ui.limitswest,
                ))
            ])
        if ui.cavemodecheck: args.append("--cave-mode")
        if ui.excludecheck:
            for block in re.split("[ \t,;/]+", ui.exclude):
                args.extend(["-e", str(block)])
        if ui.includecheck:
            # TODO: Should I include --hide-all ?
            for block in re.split("[ \t,;/]+", ui.include):
                args.extend(["-i", str(block)])

        # Rendering
        if ui.obliquecheck     : args.append("--oblique")
        if ui.obliqueanglecheck: args.append("--oblique-angle")
        if ui.nightcheck       : args.append("--night")
        if ui.rotate: args.extend(["-r", str(ui.rotate)])
        args.extend(["--threads", str(ui.threads)])

        # Text and fonts
        args.extend(["--ttf-size" , str(ui.ttfsize)])
        args.extend(["--ttf-color", str(ui.ttfcolor)])
        if ui.showplayerscheck: args.append("--show-players")
        if ui.showsignscheck  : args.append("--show-signs")
        if ui.showcoordscheck : args.append("--show-coordinates")
        if ui.playercolorcheck: args.extend(["--player-color", str(ui.playercolor)])
        if ui.signcolorcheck  : args.extend(["--sign-color", str(ui.signcolor)])
        if ui.coordcolorcheck : args.extend(["--coordinate-color", str(ui.coordcolor)])

        # Adding the "Files" section to the end for readability reasons
        args.extend([
            "-w", os.path.expanduser(ui.world),
            "-o", os.path.expanduser(ui.output),
        ])

        self.args = args

    def main(self):
        self.win.mainloop()


if __name__ == "__main__":
    p = Program()
    p.main()
