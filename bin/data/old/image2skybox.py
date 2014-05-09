import Image
import sys, os

list = {0:"f", 1:"l", 2:"r", 3:"b", 4:"d", 5:"t"}
filenames=[]

for infile in (sys.argv[1:]):
	for a in range(0, 6):
		filenames.append(infile.replace('%c', list[a]))

#open images
frontimage = Image.open(filenames[0])
leftimage  = Image.open(filenames[1])
rightimage = Image.open(filenames[2])
backimage  = Image.open(filenames[3])
downimage  = Image.open(filenames[4])
topimage   = Image.open(filenames[5])

#get dimensions of image
dimensions = frontimage.size

#Create new image + past old images into it
newimage = Image.new(frontimage.mode,(dimensions[0]*4, dimensions[1]*3), "white")
newimage.paste(frontimage, frontimage.size)
newimage.paste(leftimage , (0, frontimage.size[1]))
newimage.paste(topimage  , (frontimage.size[0], 0))
newimage.paste(downimage , (frontimage.size[0], frontimage.size[1]*2))
newimage.paste(rightimage, (frontimage.size[0]*2, frontimage.size[1]))
newimage.paste(backimage , (frontimage.size[0]*3, frontimage.size[1]))
newimage.show() 
