#!/bin/bash
basics=" -w /home/j/.minecraft/saves/World1"
prefix='./images/areaslice'
threads=2
for i in $(seq -150 -20)
do
imgfile="$prefix$i.png"
let j=i-2
echo "Rendering $i..."
./c10t $basics -o $imgfile -L $j,$i,-100,150 -q -s &
# Wait for threads to finish...
let mod=$i%$threads
if [ $mod = 0 ]; then
  wait
fi 
done
echo Trimming...
# Make all images the same size (assuming last image largest)
mogrify -bordercolor white -border 1x1 -trim $prefix*.png
newsize=$(identify -format '%wx130' $imgfile)
mogrify -gravity south -extent $newsize -background white +repage $prefix*.png
#Animate into a gif...
echo Animating...
sleep 1
convert -delay 20 -loop -1 $prefix*.png $prefix-animated.gif
