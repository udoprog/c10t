# -*- coding: utf-8 -*-
# vi:ts=4 sw=4 et

# Let me warn you... Tkinter sucks!
# It's so... crude... and ugly...
# But the *only* reason I'm using it is that
# it is available on the standard Python library.
#
# Also, be aware that most of this code is ugly because all it does is
# setting up the widgets at the window.

from Tkinter import *
from tooltip import ToolTip

try:
    import PIL.Image
    import PIL.ImageTk
    PIL_NOT_AVAILABLE = False
except ImportError as e:
    PIL_NOT_AVAILABLE = True
    PIL_NOT_AVAILABLE_MESSAGE = repr(e)


def add_tooltip(text, widgets):
    """Adds a tooltip to multiple elements at once. Useful for setting the
    same tooltip for the label/checkbox and the associated entry/spinbox.
    
    The tooltip widget is also stored at the "tooltip" attribute."""

    for w in widgets:
        w.tooltip = ToolTip(w, text=text)


def cross_platform_mouse_wheel(event):
    """Abstracts all Windows/Linux/Mac differences when handling the
    mouse wheel. Receives an 'event' object, and returns:
     negative value ==> scrolling down
     positive value ==> scrolling up
     zero           ==> something went wrong...

    This function should be called from within "<MouseWheel>",
    "<Button-4>" and "<Button-5>" event handlers.
    """
    # http://infohost.nmt.edu/tcc/help/pubs/tkinter/events.html#event-handlers

    # Linux maps scrolling to mouse buttons 4 and 5
    if event.num == 4:  # scroll up
        return 1
    elif event.num == 5:  # scroll down
        return -1
    # Windows and MacOs have a MouseWheel event
    elif event.delta:
        # In Windows, delta is a multiple of 120
        if abs(event.delta) >= 120:
            return event.delta // 120
        # In MacOS, delta is a multiples of 1
        else:
            return event.delta

    return 0


class XCheckbutton(Checkbutton):
    """Tkinter requires a Tk variable for Checkbutton. This class
    automatically creates such variable (as IntVar), stores it at the
    widget's "var" attribute, and adds get/set methods to access that var.
    """

    def __init__(self, *args, **kwargs):
        self.var = IntVar()
        Checkbutton.__init__(self, variable=self.var, *args, **kwargs)

    def get(self):
        return self.var.get()

    def set(self, value):
        self.var.set(value)


class XEntry(Entry):
    """For some reason, Entry supplies .get() but doesn't have a .set()
    method. This class adds such simple and useful method."""

    def set(self, value):
        # When the state is disabled or readonly, then .delete() and
        # .insert() methods do nothing. So, I change the state to NORMAL
        # before calling them, and restore the state afterwards.
        state = self["state"]
        self["state"] = NORMAL
        self.delete(0, END)
        self.insert(0, value)
        self["state"] = state


class XSpinbox(Spinbox):
    """As the Entry widget, Tkinter doesn't provide a .set() method to
    Spinbox. This class adds that method.
    
    In addition, it adds support for mouse-wheel."""

    def __init__(self, *args, **kwargs):
        Spinbox.__init__(self, *args, **kwargs)

        self.bind("<MouseWheel>", self.mouse_wheel_handler)
        self.bind("<Button-4>", self.mouse_wheel_handler)
        self.bind("<Button-5>", self.mouse_wheel_handler)

    def set(self, value):
        # Adding a simple ".set()" method to spinboxes...
        # http://stackoverflow.com/questions/3019800/tkinter-spinbox-widget/3024931#3024931
        # So simple solution... WHY doesn't Tkinter have it already?!
        self.delete(0, END)
        self.insert(0, value)

    def mouse_wheel_handler(self, event):
        # Mouse wheel in spinboxes...
        # http://www.daniweb.com/forums/post1158775.html#post1158775
        dir = cross_platform_mouse_wheel(event)

        while dir > 0:
            self.invoke("buttonup")
            dir -= 1
        while dir < 0:
            self.invoke("buttondown")
            dir += 1


class FilesFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master,
            text = u"Files",
            padx = 5,
            pady = 5
        )

        self.exepath_label = Label(self, text=u"c10t path", anchor=W, justify=LEFT)
        self.exepath_label.grid(column=0, row=0, columnspan=2, sticky=EW)
        self.exepath_entry = XEntry(self, name="exepath")
        self.exepath_entry.grid(column=2, row=0, sticky=EW)
        add_tooltip(u"Path to the c10t executable", (
            self.exepath_label,
            self.exepath_entry,
        ))

        self.world_label = Label(self, text=u"Input world", anchor=W, justify=LEFT)
        self.world_label.grid(column=0, row=1, columnspan=2, sticky=EW)
        self.world_entry = XEntry(self, name="world")
        self.world_entry.grid(column=2, row=1, sticky=EW)
        add_tooltip(u"Path to the World directory (the one that contains level.dat)", (
            self.world_label,
            self.world_entry,
        ))

        self.output_label = Label(self, text=u"Output image", anchor=W, justify=LEFT)
        self.output_label.grid(column=0, row=2, sticky=EW)
        self.load_button = Button(self, padx=0, pady=0, text=u"Load")
        self.load_button.grid(column=1, row=2)
        self.output_entry = XEntry(self, name="output")
        self.output_entry.grid(column=2, row=2, sticky=EW)
        add_tooltip(u"Destination file for the generated PNG", (
            self.output_label,
            self.output_entry,
        ))
        add_tooltip(u"(re)Loads this image.", (
            self.load_button,
        ))

        # Setting columns and rows to auto-expand
        for i in (0,2):
            self.columnconfigure(i, weight=1)
        for i in xrange(3):
            self.rowconfigure(i, weight=1)


class FilteringFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master,
            text = u"Filtering",
            padx = 5,
            pady = 5
        )

        self.top_checkbutton = XCheckbutton(self, name="topcheck", text=u"Top", anchor=W, justify=LEFT)
        self.top_checkbutton.grid(column=0, row=0, sticky=EW)
        self.top_spinbox = XSpinbox(self, name="top", from_=0, to=127, width=3)
        self.top_spinbox.set(127)
        self.top_spinbox.grid(column=1, row=0, sticky=EW)
        add_tooltip(u"Splice from the top, must be <= 127", (
            self.top_checkbutton,
            self.top_spinbox,
        ))

        self.bottom_checkbutton = XCheckbutton(self, name="bottomcheck", text=u"Bottom", anchor=W, justify=LEFT)
        self.bottom_checkbutton.grid(column=0, row=1, sticky=EW)
        self.bottom_spinbox = XSpinbox(self, name="bottom", from_=0, to=127, width=3)
        self.bottom_spinbox.set(0)
        self.bottom_spinbox.grid(column=1, row=1, sticky=EW)
        add_tooltip(u"Splice from the bottom, must be >= 0", (
            self.bottom_checkbutton,
            self.bottom_spinbox,
        ))

        # Begin "Limits" row...
        self.limits_checkbutton = XCheckbutton(self, name="limitscheck", text=u"Limits", anchor=W, justify=LEFT)
        self.limits_checkbutton.grid(column=0, row=2, sticky=EW)
        add_tooltip(u"Limit render to certain area (North, South, East, West)", (
            self.limits_checkbutton,
        ))

        self.limits_frame = Frame(self)
        self.limits_frame.grid(column=1, row=2, sticky=EW)

        # This loop sets the following vars:
        # self.limitsnorth_spinbox
        # self.limitssouth_spinbox
        # self.limitseast_spinbox
        # self.limitswest_spinbox
        for index, dir in enumerate(("north", "south", "east", "west")):
            name="limits"+dir
            widget = XSpinbox(self.limits_frame, name=name, from_=-999, to=999, width=4)
            widget.set(0)
            widget.grid(column=index, row=0, sticky=EW)
            self.limits_frame.columnconfigure(index, weight=1)

            setattr(self, name+"_spinbox", widget)

            signal = u"negative" if index % 2 == 0 else u"positive"
            tooltip = u"%s limit (%s)" % (dir.capitalize(), signal)
            add_tooltip(tooltip, (widget,))
        # End "Limits" row...

        self.cavemode_checkbutton = XCheckbutton(self, name="cavemodecheck", text=u"Cave mode", anchor=W, justify=LEFT)
        self.cavemode_checkbutton.grid(column=0, row=3, sticky=EW)
        add_tooltip(u"Cave mode - top down until solid block found, then render bottom outlines only", (
            self.cavemode_checkbutton,
        ))

        self.exclude_checkbutton = XCheckbutton(self, name="excludecheck", text=u"Exclude", anchor=W, justify=LEFT)
        self.exclude_checkbutton.grid(column=0, row=4, sticky=EW)
        self.exclude_entry = XEntry(self, name="exclude")
        self.exclude_entry.grid(column=1, row=4, sticky=EW)
        add_tooltip(u"Exclude block-ids from render", (
            self.exclude_checkbutton,
            self.exclude_entry,
        ))

        self.include_checkbutton = XCheckbutton(self, name="includecheck", text=u"Include", anchor=W, justify=LEFT)
        self.include_checkbutton.grid(column=0, row=5, sticky=EW)
        self.include_entry = XEntry(self, name="include")
        self.include_entry.grid(column=1, row=5, sticky=EW)
        add_tooltip(u"Include block-ids in render (and automatically exclude all non-listed blocks)", (
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
        LabelFrame.__init__(self, master,
            text = u"Rendering",
            padx = 5,
            pady = 5
        )

        self.oblique_checkbutton = XCheckbutton(self, name="obliquecheck", text=u"Oblique", anchor=W, justify=LEFT)
        self.oblique_checkbutton.grid(column=0, row=0, sticky=EW)
        add_tooltip(u"Oblique rendering", (
            self.oblique_checkbutton,
        ))

        self.obliqueangle_checkbutton = XCheckbutton(self, name="obliqueanglecheck", text=u"Oblique angle", anchor=W, justify=LEFT)
        self.obliqueangle_checkbutton.grid(column=0, row=1, sticky=EW)
        add_tooltip(u"Oblique angle rendering", (
            self.obliqueangle_checkbutton,
        ))

        self.isometric_checkbutton = XCheckbutton(self, name="isometriccheck", text=u"Isometric", anchor=W, justify=LEFT)
        self.isometric_checkbutton.grid(column=0, row=2, sticky=EW)
        add_tooltip(u"Isometric rendering", (
            self.isometric_checkbutton,
        ))

        self.night_checkbutton = XCheckbutton(self, name="nightcheck", text=u"Night-time", anchor=W, justify=LEFT)
        self.night_checkbutton.grid(column=0, row=3, sticky=EW)
        add_tooltip(u"Night-time rendering", (
            self.night_checkbutton,
        ))

        self.heightmap_checkbutton = XCheckbutton(self, name="heightmapcheck", text=u"Heightmap", anchor=W, justify=LEFT)
        self.heightmap_checkbutton.grid(column=0, row=4, sticky=EW)
        add_tooltip(u"Heightmap rendering", (
            self.heightmap_checkbutton,
        ))

        self.rotate_label = Label(self, text=u"Rotate", anchor=W, justify=LEFT)
        self.rotate_label.grid(column=0, row=5, sticky=EW)
        add_tooltip(u"Rotate the rendering clockwise", (
            self.rotate_label,
        ))
        self.rotate_frame = Frame(self)
        self.rotate_frame.grid(column=1, row=5, sticky=EW)
        self.rotate_var = IntVar(name="rotate")
        # This loop sets the following vars:
        # self.rotate0_radiobutton
        # self.rotate90_radiobutton
        # self.rotate180_radiobutton
        # self.rotate270_radiobutton
        for index, angle in enumerate(("0", "90", "180", "270")):
            name = "rotate"+angle
            widget = Radiobutton(self.rotate_frame, text=angle, variable=self.rotate_var, value=int(angle))
            widget.var = self.rotate_var
            widget.grid(column=index, row=0, sticky=EW)
            self.rotate_frame.columnconfigure(index, weight=1)
            setattr(self, name+"_radiobutton", widget)
        self.rotate_var.set(0)

        self.threads_label = Label(self, text=u"Threads", anchor=W, justify=LEFT)
        self.threads_label.grid(column=0, row=6, sticky=EW)
        self.threads_spinbox = XSpinbox(self, name="threads", from_=0, to=999, width=3)
        self.threads_spinbox.set(0)
        self.threads_spinbox.grid(column=1, row=6, sticky=EW)
        add_tooltip(u"Specify the amount of threads to use, for maximum efficency, this should match the amount of cores on your machine. Use zero to auto-detect the number of cores.", (
            self.threads_label,
            self.threads_spinbox,
        ))

        # Setting columns to auto-expand
        for i in xrange(2):
            self.columnconfigure(i, weight=1)
        for i in xrange(7):
            self.rowconfigure(i, weight=1)


class FontFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master,
            text = u"Text and fonts",
            padx = 5,
            pady = 5
        )

        self.ttfsize_label = Label(self, text=u"Font size", anchor=W, justify=LEFT)
        self.ttfsize_label.grid(column=0, row=0, columnspan=2, sticky=EW)
        self.ttfsize_spinbox = XSpinbox(self, name="ttfsize", from_=1, to=999, width=3)
        self.ttfsize_spinbox.set(12)
        self.ttfsize_spinbox.grid(column=2, row=0, sticky=EW)
        add_tooltip(u"Use the specified font size when drawing text", (
            self.ttfsize_label,
            self.ttfsize_spinbox,
        ))

        self.ttfcolor_label = Label(self, text=u"Default color", anchor=W, justify=LEFT)
        self.ttfcolor_label.grid(column=0, row=1, columnspan=2, sticky=EW)
        add_tooltip(u"Use the specified color when drawing text", (
            self.ttfcolor_label,
        ))
        self.ttfcolor_entry = XEntry(self, name="ttfcolor", width=15)
        self.ttfcolor_entry.set(u"0,0,0,255")
        self.ttfcolor_entry.grid(column=2, row=1, sticky=EW)

        self.showplayers_checkbutton = XCheckbutton(self, name="showplayerscheck", text=u"Show players", anchor=W, justify=LEFT)
        self.showplayers_checkbutton.grid(column=0, row=2, sticky=EW)
        add_tooltip(u"Will draw out player position and names from the players database in <world>/players", (
            self.showplayers_checkbutton,
        ))
        self.playercolor_checkbutton = XCheckbutton(self, name="playercolorcheck", text=u"Color:", anchor=W, justify=LEFT)
        self.playercolor_checkbutton.grid(column=1, row=2, sticky=EW)
        add_tooltip(u"Use a custom color for players", (
            self.playercolor_checkbutton,
        ))
        self.playercolor_entry = XEntry(self, name="playercolor", width=15)
        self.playercolor_entry.set(u"0,0,0,255")
        self.playercolor_entry.grid(column=2, row=2, sticky=EW)

        self.showsigns_checkbutton = XCheckbutton(self, name="showsignscheck", text=u"Show signs", anchor=W, justify=LEFT)
        self.showsigns_checkbutton.grid(column=0, row=3, sticky=EW)
        add_tooltip(u"Will draw out signs from all chunks", (
            self.showsigns_checkbutton,
        ))
        self.signcolor_checkbutton = XCheckbutton(self, name="signcolorcheck", text=u"Color:", anchor=W, justify=LEFT)
        self.signcolor_checkbutton.grid(column=1, row=3, sticky=EW)
        add_tooltip(u"Use a custom color for signs", (
            self.signcolor_checkbutton,
        ))
        self.signcolor_entry = XEntry(self, name="signcolor", width=15)
        self.signcolor_entry.set(u"0,0,0,255")
        self.signcolor_entry.grid(column=2, row=3, sticky=EW)

        self.showcoords_checkbutton = XCheckbutton(self, name="showcoordscheck", text=u"Show coordinates", anchor=W, justify=LEFT)
        self.showcoords_checkbutton.grid(column=0, row=4, sticky=EW)
        add_tooltip(u"Will draw out each chunks expected coordinates", (
            self.showcoords_checkbutton,
        ))
        self.coordcolor_checkbutton = XCheckbutton(self, name="coordcolorcheck", text=u"Color:", anchor=W, justify=LEFT)
        self.coordcolor_checkbutton.grid(column=1, row=4, sticky=EW)
        add_tooltip(u"Use a custom color for coordinates", (
            self.coordcolor_checkbutton,
        ))
        self.coordcolor_entry = XEntry(self, name="coordcolor", width=15)
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

        self.run_button = Button(self, text=u"Run!")
        self.run_button.pack(side=LEFT)

        self.update_button = Button(self, text=u"Update")
        self.update_button.pack(side=LEFT)
        add_tooltip(u"Update the command-line, without running the command. Easy for copy-and-paste into scripts.", (
            self.update_button,
        ))

        self.command_entry = XEntry(self, name="command", state="readonly")
        self.command_entry.pack(side=LEFT, fill=X, expand=1)
        add_tooltip(u"The full command-line that will be invoked", (
            self.command_entry,
        ))


class ImageFrame(Frame):
    """This is just a Frame containing one Canvas, two scrollbars and some
    logic for scrolling the image."""

    # This class is inspired by:
    # http://effbot.org/tkinterbook/photoimage.htm
    # http://www.swharden.com/blog/2010-03-03-viewing-large-images-with-scrollbars-using-python-tk-and-pil/

    def __init__(self, master=None):
        Frame.__init__(self, master)

        # Adding the widgets
        self.vertical_scrollbar = Scrollbar(self, orient=VERTICAL)
        self.vertical_scrollbar.pack(side=RIGHT, fill=Y)
        self.horizontal_scrollbar = Scrollbar(self, orient=HORIZONTAL)
        self.horizontal_scrollbar.pack(side=BOTTOM, fill=X)

        self.canvas = Canvas(self)
        self.canvas.pack(side=LEFT, expand=YES, fill=BOTH)

        # Add an error message if needed
        if PIL_NOT_AVAILABLE:
            self.canvas.create_text(10, 10, anchor=NW, text=u"Python Image Library (PIL) could not be loaded.\n\nNo image will be displayed here, but\nthey will still be rendered to the disk.\n\n%s" % (PIL_NOT_AVAILABLE_MESSAGE,))

        # Connecting canvas and scrollbars
        self.horizontal_scrollbar["command"] = self.canvas.xview
        self.vertical_scrollbar["command"] = self.canvas.yview
        self.canvas["xscrollcommand"] = self.horizontal_scrollbar.set
        self.canvas["yscrollcommand"] = self.vertical_scrollbar.set

        # Adding the event handlers to the canvas
        self.canvas.bind("<Button-1>", self.button_press_1_handler)
        self.canvas.bind("<B1-Motion>", self.button_motion_1_handler)
        self.canvas.bind("<ButtonRelease-1>", self.button_release_1_handler)

        self.canvas.bind("<Button-2>", self.mouse_wheel_handler)  # DEBUG
        self.canvas.bind("<Button-3>", self.mouse_wheel_handler)  # DEBUG
        self.canvas.bind("<MouseWheel>", self.mouse_wheel_handler)
        self.canvas.bind("<Button-4>", self.mouse_wheel_handler)
        self.canvas.bind("<Button-5>", self.mouse_wheel_handler)

        # Adding mouse wheel support to scrollbars
        self.horizontal_scrollbar.bind("<MouseWheel>", self.hscroll_mouse_wheel_handler)
        self.horizontal_scrollbar.bind("<Button-4>", self.hscroll_mouse_wheel_handler)
        self.horizontal_scrollbar.bind("<Button-5>", self.hscroll_mouse_wheel_handler)
        self.vertical_scrollbar.bind("<MouseWheel>", self.vscroll_mouse_wheel_handler)
        self.vertical_scrollbar.bind("<Button-4>", self.vscroll_mouse_wheel_handler)
        self.vertical_scrollbar.bind("<Button-5>", self.vscroll_mouse_wheel_handler)

        # Internal vars for handling the image
        self.pil_image = None
        self.tk_resized_images = {}
        self.canvas_image = None
        self.zoom = 0

    def hscroll_mouse_wheel_handler(self, event):
        dir = cross_platform_mouse_wheel(event)
        self.canvas.xview_scroll(-dir, UNITS)

    def vscroll_mouse_wheel_handler(self, event):
        dir = cross_platform_mouse_wheel(event)
        self.canvas.yview_scroll(-dir, UNITS)

    def button_press_1_handler(self, event):
        self.canvas.scan_mark(event.x, event.y)
        self.canvas["cursor"] = "fleur"

    def button_motion_1_handler(self, event):
        self.canvas.scan_dragto(event.x, event.y, gain=1)

    def button_release_1_handler(self, event):
        self.canvas["cursor"] = ""

    def mouse_wheel_handler(self, event):
        dir = cross_platform_mouse_wheel(event)
        #if dir == 0:
        #    return

        # Code for zooming the image (like Google Maps)
        self.resize_image_to_zoom(delta=dir, center=(event.x, event.y))

    def resize_image_to_zoom(self, delta=None, zoom=None, center=None, forcereload=False):
        """Parameters:
         delta      : How much to increase/decrease the current zoom?
         zoom       : Set zoom to this absolute value.
         center     : Uses these coordinates (x,y) as the zoom center,
                      scrolling the canvas as needed. These are "window
                      coordinates", relative to the widget's top-left
                      corner.
         forcereload: Clears the zoom cache, forces reloading the canvas
                      from self.pil_image.
        """

        # Sanity check
        if PIL_NOT_AVAILABLE:
            return
        if self.pil_image is None:
            return

        # Calculating the new zoom value
        prevzoom = self.zoom
        if zoom is not None:
            self.zoom = zoom
        if delta is not None:
            self.zoom += delta

        # Clamping maximum zoom
        if self.zoom > 2:
            self.zoom = 2
        # Clamping minimum zoom
        if self.zoom < -4:
            self.zoom = -4

        if forcereload:
            # Clearing the image cache
            self.tk_resized_images = {}

        # Zoom size is not cached...
        if not self.tk_resized_images.has_key(self.zoom):
            mult = 2 ** self.zoom
            ow, oh = self.pil_image.size
            w, h = int(mult * ow), int(mult * oh)

            # Let's apply a nice filter when scaling down,
            # but keep those lovely pixels when scaling up!
            filter = PIL.Image.NEAREST if mult >=1 else PIL.Image.BICUBIC

            # Resizing
            pil_resized_image = self.pil_image.resize((w,h), filter)
            # Converting to Tk
            tk_photoimage = PIL.ImageTk.PhotoImage(pil_resized_image)
            # Saving the Tk image to cache
            self.tk_resized_images[self.zoom] = tk_photoimage

        # Keeping the position centered
        if center:
            # Window coordinates, relative to widget's top-left corner
            wx, wy = center

            # Old canvas coordinates
            ocx = self.canvas.canvasx(wx)
            ocy = self.canvas.canvasy(wy)

            tx = self.canvas.canvasx(0)
            ty = self.canvas.canvasy(0)

            # Old scroll position
            osx = ocx - wx
            osy = ocy - wy

            # Multiplication...
            deltazoom = self.zoom - prevzoom
            deltamult = 2 ** deltazoom

            # New canvas coordinates
            ncx = ocx * deltamult
            ncy = ocy * deltamult

            # New scroll position
            nsx = ncx - wx
            nsy = ncy - wy

            # Debug
            #print (
            #    "w=({wx},{wy})\n"
            #    "old canvas=({ocx},{ocy})\n"
            #    "old scroll=({osx},{osy})\n"
            #    "old scroll=({tx},{ty})\n"
            #    "new canvas=({ncx},{ncy})\n"
            #    "new scroll=({nsx},{nsy})\n"
            #    "deltazoom={deltazoom}; deltamult={deltamult}"
            #    .format(**locals())
            #)

        # Getting the image from cache
        tk_img = self.tk_resized_images[self.zoom]
        # Setting it to canvas
        self.canvas.itemconfigure(self.canvas_image, image=tk_img)
        # Setting the size
        new_width = tk_img.width()
        new_height = tk_img.height()
        self.canvas["scrollregion"] = (0, 0, new_width, new_height)

        # Finally, scrolling in order to keep the position centered
        if center:
            # It was difficult to find the right formula to make
            # tkinter flawlessly...
            # http://stackoverflow.com/questions/3950773/how-to-scroll-a-tkinter-canvas-to-an-absolute-position
            offset_x = +1 if nsx >= 0 else 0
            offset_y = +1 if nsy >= 0 else 0
            self.canvas.xview_moveto(float(nsx + offset_x)/new_width)
            self.canvas.yview_moveto(float(nsy + offset_y)/new_height)

    def load_image_from_file(self, imagepath):
        if PIL_NOT_AVAILABLE:
            return

        # Loading the image from disk
        self.pil_image = PIL.Image.open(imagepath)

        # Creating the image inside the canvas
        if self.canvas_image is None:
            self.canvas_image = self.canvas.create_image(0, 0, anchor=NW)

        # Updating the canvas image
        self.resize_image_to_zoom(forcereload=True)


class ApplicationFrame(Frame):
    """The main application frame is divided like this:

    +--------------+--------------------+
    |Configuration | Run Frame here     |
    |Frame here,   +--------------------+
    |which contains|                   ^|
    |sub-frames for| Image Frame here   |
    |each section. |                    |
    |              |                    |
    |              |<                 >v|
    +--------------+--------------------+
    """

    def __init__(self, master=None):
        Frame.__init__(self, master,
            padx=5,
            pady=5
        )

        self.configuration_frame = ConfigurationFrame(self)
        self.configuration_frame.grid(column=0, row=0, rowspan=2, sticky=NSEW)

        self.run_frame = RunFrame(self)
        self.run_frame.grid(column=1, row=0, sticky=NSEW)

        self.image_frame = ImageFrame(self)
        self.image_frame.grid(column=1, row=1, sticky=NSEW)

        self.columnconfigure(1, weight=1)
        self.rowconfigure(1, weight=1)


def find_named_widgets(widget):
    """Starting at the 'widget', scans down recursively and returns a dict
    with {'name': widget_instance}, but only if the name has been set by
    the user.
    
    There is a special treatment for Radiobutton: it returns the associated
    variable instead of the widget, but only if the user has set a name to
    the variable and stored it at the widget's "var" attribute."""

    d = {}
    for name, w in widget.children.iteritems():
        # If the user has set a name for this widget, and it is a valid name.
        # Unnamed widgets get numbers in their name.
        if name[0].isalpha() or name[0] == "_":
            d[name] = w

        # Special case for Radiobutton
        if (
            isinstance(w, Radiobutton) and
            hasattr(w, "var") and
            not w.var._name.startswith("PY_VAR")
        ):
            d[w.var._name] = w.var

        # Recursive call
        d.update(find_named_widgets(w))
    return d


class UiShortcuts(object):
    """This object is a quick and handy way of accessing widget values.
    See the following small example to get an idea of how it works:

    ui = UiShortcuts(root_window)

    # This will call .set("bar") on a widget named "foo"
    ui.foo = "bar"
    # This will call .get() on a widget named "foo"
    print ui.foo
    """

    def __init__(self, root):
        object.__setattr__(self, "widgets", find_named_widgets(root))

    def __dir__(self):
        # Simple method to support tab-completion
        return self.__dict__.keys() + self.widgets.keys()

    def __getattr__(self, name):
        return self.widgets[name].get()

    def __setattr__(self, name, value):
        self.widgets[name].set(value)


class MainWindow(Tk):
    """Root window. Contains one ApplicationFrame at self.app, and has
    shortcuts to all important widgets in self.ui.
    
    If you want to interact with the interface, using self.ui should be
    enough, and it hides all the messy toolkit-related code."""

    def __init__(self):
        Tk.__init__(self)

        self.title("c10t GUI")

        self.app = ApplicationFrame(self)
        self.app.pack(fill=BOTH, expand=1)

        self.ui = UiShortcuts(self)

        # Global quit handlers
        self.bind_all("<Control-q>", self.quit_handler)
        self.protocol("WM_DELETE_WINDOW", self.quit_handler)

        #self.on_change_callback = None
        #self.app.configuration_frame.bind("")

        # Adding an "update" button, just because I don't know how to bind
        # to "on change"-like events in Tkinter.
        # http://stackoverflow.com/questions/3876229/how-to-run-a-code-whenever-a-tkinter-widget-value-changes
        self.update_button_callback = None
        self.app.run_frame.update_button["command"] = self.update_button_handler

        self.run_button_callback = None
        self.app.run_frame.run_button["command"] = self.run_button_handler

        self.load_button_callback = None
        self.app.configuration_frame.files_frame.load_button["command"] = self.load_button_handler

    def quit_handler(self, event=None):
        self.destroy()

    #def on_change_handler(self, event=None):
    #    if self.on_change_callback:
    #        self.on_change_callback()

    def update_button_handler(self, event=None):
        if self.update_button_callback:
            self.update_button_callback()

    def run_button_handler(self, event=None):
        if self.run_button_callback:
            self.run_button_callback()

    def load_button_handler(self):
        if self.load_button_callback:
            self.load_button_callback()

    def load_image_from_file(self, imagepath):
        self.app.image_frame.load_image_from_file(imagepath)

