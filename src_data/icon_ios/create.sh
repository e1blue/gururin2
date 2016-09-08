#!/bin/bash

cd $(dirname $0)
mkdir -p dst

# ----------------------------------------------------------------

#sips -Z  29 icon1024.png --out dst/icon29x1.png
#sips -Z  58 icon1024.png --out dst/icon29x2.png
#sips -Z  87 icon1024.png --out dst/icon29x3.png
#sips -Z  40 icon1024.png --out dst/icon40x1.png
#sips -Z  80 icon1024.png --out dst/icon40x2.png
#sips -Z 120 icon1024.png --out dst/icon40x3.png
#sips -Z 120 icon1024.png --out dst/icon60x2.png
#sips -Z 180 icon1024.png --out dst/icon60x3.png
#sips -Z  76 icon1024.png --out dst/icon76x1.png
#sips -Z 152 icon1024.png --out dst/icon76x2.png
#sips -Z 167 icon1024.png --out dst/icon83x2.png

# ----------------------------------------------------------------

createSplash(){
	SRCIMGW=640
	SRCIMGH=960
	DSTIMGW=${1}
	DSTIMGH=${2}
	if [ $((${SRCIMGH} * ${DSTIMGW})) -gt $((${SRCIMGW} * ${DSTIMGH})) ]; then
		W=$((${DSTIMGH} * ${SRCIMGW} / ${SRCIMGH}))
		H=${DSTIMGH}
		X=$(((${DSTIMGW} - ${W}) / 2))
		Y=0
	else
		W=${DSTIMGW}
		H=$((${DSTIMGW} * ${SRCIMGH} / ${SRCIMGW}))
		X=0
		Y=$(((${DSTIMGH} - ${H}) / 2))
	fi
	SRCFILE=${3}
	DSTFILE=${4}
	convert -size ${DSTIMGW}x${DSTIMGH} xc:'rgba(255, 255, 215, 1)' png:- |\
	convert png:- ${SRCFILE} -geometry ${W}x${H}+${X}+${Y} -composite ${DSTFILE}
}

#createSplash 1242 2208 store1.png dst/iPhone5.5_img1.png
#createSplash 2048 2732 store1.png dst/iPadPro___img1.png
#createSplash 1242 2208 store2.png dst/iPhone5.5_img2.png
#createSplash 2048 2732 store2.png dst/iPadPro___img2.png
#createSplash 1242 2208 store3.png dst/iPhone5.5_img3.png
#createSplash 2048 2732 store3.png dst/iPadPro___img3.png
#createSplash 1242 2208 store4.png dst/iPhone5.5_img4.png
#createSplash 2048 2732 store4.png dst/iPadPro___img4.png
#createSplash 1242 2208 store5.png dst/iPhone5.5_img5.png
#createSplash 2048 2732 store5.png dst/iPadPro___img5.png

# ----------------------------------------------------------------

