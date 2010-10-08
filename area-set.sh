#!/bin/bash
basics=" -w /home/j/.minecraft/saves/World1"
prefix='./images/area'
threads=2
for i in $(seq -150 -20)
do
imgfile="$prefix$i.png"
echo "Rendering $i..."
./c10t $basics -o $imgfile -L -150,$i,-100,150 -q -s &
# Wait for threads to finish...
let mod=$i%$threads
if [ $mod = 0 ]; then
  wait
fi 
done

echo Trimming...
# Make all images the same size (assuming last image largest)
mogrify -bordercolor white -border 1x1 -trim $prefix*.png
newsize=$(identify -format '%wx%h' $imgfile)
mogrify -extent $newsize -background white +repage $prefix*.png
#Animate into a gif...
echo Animating...
convert -delay 20 -loop -1 $prefix*.png $prefix-animated.gif

