# -*- coding: utf-8 -*-
# vi:ts=4 sw=4 et

# Let me warn you... Tkinter sucks!
# It's so... crude... and ugly...
# But the *only* reason I'm using it is that
# it is available on the standard Python library.

from Tkinter import *
from tooltip import ToolTip


def add_tooltip(text, widgets):
    for w in widgets:
        # Adding a new attribute on-the-fly.
        # Dynamic languages rock! :P
        w.tooltip = ToolTip(w, text=text)


class XEntry(Entry):
    def set(self, value):
        # When the state is disabled or readonly,
        # we can't use .delete() and .insert() methods
        state = self["state"]
        self["state"] = NORMAL
        self.delete(0, END)
        self.insert(0, value)
        self["state"] = state


class XSpinbox(Spinbox):
    def __init__(self, *args, **kwargs):
        Spinbox.__init__(self, *args, **kwargs)

        # Windows
        self.bind("<MouseWheel>", self.mouse_wheel)
        # Linux
        self.bind("<Button-4>", self.mouse_wheel)
        self.bind("<Button-5>", self.mouse_wheel)
        # How about Mac... ???
        # I have no idea!

    def set(self, value):
        # Adding a simple ".set()" method to spinboxes...
        # http://stackoverflow.com/questions/3019800/tkinter-spinbox-widget/3024931#3024931
        # So simple solution... WHY doesn't Tkinter have it already?!
        self.delete(0, END)
        self.insert(0, value)

    def mouse_wheel(self, event):
        # Things we get for granted...
        # like built-in support for mouse wheel in spinboxes...
        # http://www.daniweb.com/forums/post1158775.html#post1158775
        if event.num == 5:
            dir = -1
        elif event.num == 4:
            dir = 1
        elif abs(event.delta) >= 120:
            dir = event.delta // 120

        while dir > 0:
            self.invoke("buttonup")
            dir -= 1
        while dir < 0:
            self.invoke("buttondown")
            dir += 1


class FilesFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master)
        self.config(
            text = u"Files",
            padx = 5,
            pady = 5
        )
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.exepath_label = Label(master, text=u"c10t path", anchor=W, justify=LEFT)
        self.exepath_label.grid(column=0, row=0, sticky=EW)
        self.exepath_entry = XEntry(master)
        self.exepath_entry.grid(column=1, row=0, sticky=EW)
        add_tooltip(u"Path to the c10t executable", (
            self.exepath_label,
            self.exepath_entry,
        ))

        self.world_label = Label(master, text=u"Input world", anchor=W, justify=LEFT)
        self.world_label.grid(column=0, row=1, sticky=EW)
        self.world_entry = XEntry(master)
        self.world_entry.grid(column=1, row=1, sticky=EW)
        add_tooltip(u"Path to the World directory (the one that contains level.dat)", (
            self.world_label,
            self.world_entry,
        ))

        self.output_label = Label(master, text=u"Output image", anchor=W, justify=LEFT)
        self.output_label.grid(column=0, row=2, sticky=EW)
        self.output_entry = XEntry(master)
        self.output_entry.grid(column=1, row=2, sticky=EW)
        add_tooltip(u"Destination file for the generated PNG", (
            self.output_label,
            self.output_entry,
        ))

        # Setting columns and rows to auto-expand
        for i in xrange(2):
            self.columnconfigure(i, weight=1)
        for i in xrange(3):
            self.rowconfigure(i, weight=1)


class FilteringFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master)
        self.config(
            text = u"Filtering",
            padx = 5,
            pady = 5
        )
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.top_checkbutton = Checkbutton(master, text=u"Top", anchor=W, justify=LEFT)
        self.top_checkbutton.grid(column=0, row=0, sticky=EW)
        self.top_spinbox = XSpinbox(master, from_=0, to=127, width=3)
        self.top_spinbox.set(127)
        self.top_spinbox.grid(column=1, row=0, sticky=EW)
        add_tooltip(u"Splice from the top, must be <= 127", (
            self.top_checkbutton,
            self.top_spinbox,
        ))

        self.bottom_checkbutton = Checkbutton(master, text=u"Bottom", anchor=W, justify=LEFT)
        self.bottom_checkbutton.grid(column=0, row=1, sticky=EW)
        self.bottom_spinbox = XSpinbox(master, from_=0, to=127, width=3)
        self.bottom_spinbox.set(0)
        self.bottom_spinbox.grid(column=1, row=1, sticky=EW)
        add_tooltip(u"Splice from the bottom, must be >= 0", (
            self.bottom_checkbutton,
            self.bottom_spinbox,
        ))

        # Begin "Limits" row...
        self.limits_checkbutton = Checkbutton(master, text=u"Limits", anchor=W, justify=LEFT)
        self.limits_checkbutton.grid(column=0, row=2, sticky=EW)
        add_tooltip(u"Limit render to certain area (North, South, East, West)", (
            self.limits_checkbutton,
        ))

        self.limits_frame = Frame(master)
        self.limits_frame.grid(column=1, row=2, sticky=EW)

        # This loop sets the following vars:
        # self.limits_north
        # self.limits_south
        # self.limits_east
        # self.limits_west
        for index, dir in enumerate(("north", "south", "east", "west")):
            widget = XSpinbox(self.limits_frame, from_=-999, to=999, width=4)
            widget.set(0)
            widget.grid(column=index, row=0, sticky=EW)
            self.limits_frame.columnconfigure(index, weight=1)

            name = "limits_"+dir
            setattr(self, name, widget)

            signal = u"negative" if index % 2 == 0 else u"positive"
            tooltip = u"%s limit (%s)" % (dir.capitalize(), signal)
            add_tooltip(tooltip, (widget,))
        # End "Limits" row...

        self.cavemode_checkbutton = Checkbutton(master, text=u"Cave mode", anchor=W, justify=LEFT)
        self.cavemode_checkbutton.grid(column=0, row=3, sticky=EW)
        add_tooltip(u"Cave mode - top down until solid block found, then render bottom outlines only", (
            self.cavemode_checkbutton,
        ))

        self.exclude_checkbutton = Checkbutton(master, text=u"Exclude", anchor=W, justify=LEFT)
        self.exclude_checkbutton.grid(column=0, row=4, sticky=EW)
        self.exclude_entry = XEntry(master)
        self.exclude_entry.grid(column=1, row=4, sticky=EW)
        add_tooltip(u"Exclude block-ids from render", (
            self.exclude_checkbutton,
            self.exclude_entry,
        ))

        self.include_checkbutton = Checkbutton(master, text=u"Include", anchor=W, justify=LEFT)
        self.include_checkbutton.grid(column=0, row=5, sticky=EW)
        self.include_entry = XEntry(master)
        self.include_entry.grid(column=1, row=5, sticky=EW)
        add_tooltip(u"Include block-ids in render", (
            self.include_checkbutton,
            self.include_entry,
        ))

        # Setting columns to auto-expand
        for i in xrange(2):
            self.columnconfigure(i, weight=1)
        for i in xrange(6):
            self.rowconfigure(i, weight=1)


class RenderingFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master)
        self.config(
            text = u"Rendering",
            padx = 5,
            pady = 5
        )
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.oblique_checkbutton = Checkbutton(master, text=u"Oblique", anchor=W, justify=LEFT)
        self.oblique_checkbutton.grid(column=0, row=0, sticky=EW)
        add_tooltip(u"Oblique rendering", (
            self.oblique_checkbutton,
        ))

        self.obliqueangle_checkbutton = Checkbutton(master, text=u"Oblique angle", anchor=W, justify=LEFT)
        self.obliqueangle_checkbutton.grid(column=0, row=1, sticky=EW)
        add_tooltip(u"Oblique angle rendering", (
            self.obliqueangle_checkbutton,
        ))

        self.night_checkbutton = Checkbutton(master, text=u"Night", anchor=W, justify=LEFT)
        self.night_checkbutton.grid(column=0, row=2, sticky=EW)
        add_tooltip(u"Night-time rendering", (
            self.night_checkbutton,
        ))

        self.rotate_label = Label(master, text=u"Rotate", anchor=W, justify=LEFT)
        self.rotate_label.grid(column=0, row=3, sticky=EW)
        add_tooltip(u"Rotate the rendering clockwise", (
            self.rotate_label,
        ))
        self.rotate_frame = Frame(master)
        self.rotate_frame.grid(column=1, row=3, sticky=EW)
        self.rotate_var = IntVar()
        # This loop sets the following vars:
        # self.rotate_0
        # self.rotate_90
        # self.rotate_180
        # self.rotate_270
        for index, angle in enumerate(("0", "90", "180", "270")):
            widget = Radiobutton(self.rotate_frame, text=angle, variable=self.rotate_var, value=int(angle))
            widget.grid(column=index, row=0, sticky=EW)
            self.rotate_frame.columnconfigure(index, weight=1)
            name = "rotate_"+angle
            setattr(self, name, widget)

        self.threads_label = Label(master, text=u"Threads", anchor=W, justify=LEFT)
        self.threads_label.grid(column=0, row=4, sticky=EW)
        self.threads_spinbox = XSpinbox(master, from_=1, to=999, width=3)
        self.threads_spinbox.set(1)
        self.threads_spinbox.grid(column=1, row=4, sticky=EW)
        add_tooltip(u"Specify the amount of threads to use, for maximum efficency, this should match the amount of cores on your machine", (
            self.threads_label,
            self.threads_spinbox,
        ))

        # Setting columns to auto-expand
        for i in xrange(2):
            self.columnconfigure(i, weight=1)
        for i in xrange(4):
            self.rowconfigure(i, weight=1)


class FontFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master)
        self.config(
            text = u"Text and fonts",
            padx = 5,
            pady = 5
        )
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.ttfsize_label = Label(master, text=u"Font size", anchor=W, justify=LEFT)
        self.ttfsize_label.grid(column=0, row=0, columnspan=2, sticky=EW)
        self.ttfsize_spinbox = XSpinbox(master, from_=1, to=999, width=3)
        self.ttfsize_spinbox.set(12)
        self.ttfsize_spinbox.grid(column=2, row=0, sticky=EW)
        add_tooltip(u"Use the specified font size when drawing text", (
            self.ttfsize_label,
            self.ttfsize_spinbox,
        ))

        self.ttfcolor_label = Label(master, text=u"Default color", anchor=W, justify=LEFT)
        self.ttfcolor_label.grid(column=0, row=1, columnspan=2, sticky=EW)
        add_tooltip(u"Use the specified color when drawing text", (
            self.ttfcolor_label,
        ))
        self.ttfcolor_entry = XEntry(master, width=15)
        self.ttfcolor_entry.set(u"0,0,0,255")
        self.ttfcolor_entry.grid(column=2, row=1, sticky=EW)

        self.showplayers_checkbutton = Checkbutton(master, text=u"Show players", anchor=W, justify=LEFT)
        self.showplayers_checkbutton.grid(column=0, row=2, sticky=EW)
        add_tooltip(u"Will draw out player position and names from the players database in <world>/players", (
            self.showplayers_checkbutton,
        ))
        self.playercolor_checkbutton = Checkbutton(master, text=u"Color:", anchor=W, justify=LEFT)
        self.playercolor_checkbutton.grid(column=1, row=2, sticky=EW)
        add_tooltip(u"Use a custom color for players", (
            self.playercolor_checkbutton,
        ))
        self.playercolor_entry = XEntry(master, width=15)
        self.playercolor_entry.set(u"0,0,0,255")
        self.playercolor_entry.grid(column=2, row=2, sticky=EW)

        self.showsigns_checkbutton = Checkbutton(master, text=u"Show signs", anchor=W, justify=LEFT)
        self.showsigns_checkbutton.grid(column=0, row=3, sticky=EW)
        add_tooltip(u"Will draw out signs from all chunks", (
            self.showsigns_checkbutton,
        ))
        self.signcolor_checkbutton = Checkbutton(master, text=u"Color:", anchor=W, justify=LEFT)
        self.signcolor_checkbutton.grid(column=1, row=3, sticky=EW)
        add_tooltip(u"Use a custom color for signs", (
            self.signcolor_checkbutton,
        ))
        self.signcolor_entry = XEntry(master, width=15)
        self.signcolor_entry.set(u"0,0,0,255")
        self.signcolor_entry.grid(column=2, row=3, sticky=EW)

        self.showcoords_checkbutton = Checkbutton(master, text=u"Show coordinates", anchor=W, justify=LEFT)
        self.showcoords_checkbutton.grid(column=0, row=4, sticky=EW)
        add_tooltip(u"Will draw out each chunks expected coordinates", (
            self.showcoords_checkbutton,
        ))
        self.coordcolor_checkbutton = Checkbutton(master, text=u"Color:", anchor=W, justify=LEFT)
        self.coordcolor_checkbutton.grid(column=1, row=4, sticky=EW)
        add_tooltip(u"Use a custom color for coordinates", (
            self.coordcolor_checkbutton,
        ))
        self.coordcolor_entry = XEntry(master, width=15)
        self.coordcolor_entry.set(u"0,0,0,255")
        self.coordcolor_entry.grid(column=2, row=4, sticky=EW)

        add_tooltip(u"Red,Green,Blue,Alpha\n0,0,0,255 means black", (
            self.playercolor_entry,
            self.signcolor_entry,
            self.coordcolor_entry,
            self.ttfcolor_entry,
        ))

        # Setting columns to auto-expand
        for i in xrange(3):
            self.columnconfigure(i, weight=1)
        for i in xrange(5):
            self.rowconfigure(i, weight=1)


class ConfigurationFrame(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.files_frame = FilesFrame(self)
        self.files_frame.pack(fill=BOTH, expand=1)

        self.filtering_frame = FilteringFrame(self)
        self.filtering_frame.pack(fill=BOTH, expand=1)

        self.rendering_frame = RenderingFrame(self)
        self.rendering_frame.pack(fill=BOTH, expand=1)

        self.font_frame = FontFrame(self)
        self.font_frame.pack(fill=BOTH, expand=1)


class RunFrame(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.run_button = Button(master, text=u"Run!")
        # TODO: Assign the run_button["command"]
        self.run_button.pack(side=LEFT)

        self.command_entry = XEntry(master, state="readonly")
        self.command_entry.set("AAAA")
        self.command_entry.pack(side=LEFT, fill=X, expand=1)
        add_tooltip(u"The full command-line that will be invoked", (
            self.command_entry,
        ))


class ImageFrame(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.vertical_scrollbar = Scrollbar(master, orient=VERTICAL)
        self.vertical_scrollbar.pack(side=RIGHT, fill=Y)
        self.horizontal_scrollbar = Scrollbar(master, orient=HORIZONTAL)
        self.horizontal_scrollbar.pack(side=BOTTOM, fill=X)

        self.canvas = Canvas(master)
        # TODO: Set the size with self.canvas["width"] and height
        # And set self.canvas["scrollregion"] = (0,0,width,height)
        self.canvas.pack(side=LEFT, expand=YES, fill=BOTH)

        self.vertical_scrollbar["command"] = self.canvas.yview
        self.vertical_scrollbar["command"] = self.canvas.yview
        self.canvas["xscrollcommand"] = self.horizontal_scrollbar.set
        self.canvas["yscrollcommand"] = self.vertical_scrollbar.set


class ApplicationFrame(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.create_widgets(self)

    def create_widgets(self, master=None):
        self.configuration_frame = ConfigurationFrame(self)
        self.configuration_frame.grid(column=0, row=0, rowspan=2, sticky=NSEW)

        self.run_frame = RunFrame(self)
        self.run_frame.grid(column=1, row=0, sticky=NSEW)

        self.image_frame = ImageFrame(self)
        self.image_frame.grid(column=1, row=1, sticky=NSEW)

        self.columnconfigure(1, weight=1)
        self.rowconfigure(1, weight=1)


class MainWindow(Tk):
    def __init__(self):
        Tk.__init__(self)

        self.title("c10t GUI")
        self.app = ApplicationFrame(self)
        self.app.config(padx=5, pady=5)
        self.app.quitHandler = self.quitHandler
        self.app.grid(sticky="NSEW")
        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)

        self.bind_all("<Control-q>", self.quitHandler)

        self.protocol("WM_DELETE_WINDOW", self.quitHandler)

    def quitHandler(self, event=None):
        self.destroy()
