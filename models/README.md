# Models
These are models to be 3D-printed for the robot. Basically you need to print each `.3mf` file once.
I use [PrusaSlicer](https://www.prusa3d.com/) for slicing and generating gcode.

## Editing
Probably (unless you happen to collect exactly the same [components](../README.md#shopping-list) as me) you'll need to edit
some parts. Currently I suggest using [toponaming](https://github.com/FreeCAD/FreeCAD/tree/development/toponaming) branch of
FreeCAD and (to prevent headaches) **not mapping anything on 3D features**, i.e. project only geometry from sketches and
use DatumPlanes and parametric expressions to achieve proper structure, otherwise recomputing hell will come upon you
with refernces jumping from one feature to another etc.

After editing and exporting the models, there is a `Reload from disk` feature in PrusaSlicer, but first you'll need to
set proper paths of your `.stl` files. Open `.3mf` files via an archive manager and edit `Metadata/Slic3r_PE_model.config`.
You will need to edit paths of these records
```
   <metadata type="volume" key="source_file" value="/home/matej/Projects/plant_guard/water_carrier/models/water_carrier-Wheel.stl"/>
```
and also filenames of these records (if you change the filename)
```
   <metadata type="volume" key="name" value="water_carrier-Wheel.stl"/>
```

After reloading from disk you will want to add supports to all overhangs under 40˚ and to disable supports at specific places where they are not actually needed and are
hard to tear off after parts have been printed
![Supports to be disabled](../doc/disabled-supports.jpg)
 * disabled surfaces are marked by red
 * some less visible surfaces are highlighted by red arrows
 * green arrows show areas that need to be also disabled to prevent support dilatation into problematic neighbouring pockets