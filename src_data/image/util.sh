#!/bin/bash

# ----------------------------------------------------------------
# 定義の確認

[ -z "${TARGET+x}" ] && exit 1

echo creating image of ${TARGET}...

# ----------------------------------------------------------------

TEMPFILE=temp.png
IMGDIR=../../../contents/img
SP=2

HEADERFILE=../header/texinfo_${TARGET}.h
echo // texture infomation of ${TARGET} > ${HEADERFILE}

TARGETCAPITAL=`echo ${TARGET}  | tr "a-z" "A-Z"`
echo \#define TEXSIZ_${TARGETCAPITAL}_W ${IMGW} >> ${HEADERFILE}
echo \#define TEXSIZ_${TARGETCAPITAL}_H ${IMGH} >> ${HEADERFILE}
echo \#define TEXSIZ_${TARGETCAPITAL}_WH ${IMGW}, ${IMGH} >> ${HEADERFILE}
echoDefine(){
	if [ `eval echo \$\{OVERLAP_${1}:-0\}` -eq 0 ]; then
		eval OVERLAP_${1}=1
		echo \#define TEXPOS_${TARGETCAPITAL}_${1}_X ${2}
		echo \#define TEXPOS_${TARGETCAPITAL}_${1}_Y ${3}
		echo \#define TEXPOS_${TARGETCAPITAL}_${1}_W ${4}
		echo \#define TEXPOS_${TARGETCAPITAL}_${1}_H ${5}
		echo \#define TEXPOS_${TARGETCAPITAL}_${1}_XYWH ${2}, ${3}, ${4}, ${5}
	fi
}

putSprite(){
	[ $# -ge 3 ] && X=${3} || X=0
	[ $# -ge 3 ] && Y=${4} || Y=0
	[ $# -ge 5 ] && W=${5} || W=`identify -format "%w" ${2}`
	[ $# -ge 5 ] && H=${6} || H=`identify -format "%h" ${2}`

	echoDefine ${1} ${X} ${Y} ${W} ${H} >> ${HEADERFILE}

	if [ ${TESTMODE:-0} -eq 1 ]; then
		convert -size ${W}x${H} xc:'rgba(0, 0, 0, 1)' png:- |\
		convert ${TEMPFILE} png:- -geometry ${W}x${H}+${X}+${Y} -composite ${TEMPFILE}
	fi

	convert ${TEMPFILE} ${2} -geometry ${W}x${H}+${X}+${Y} -composite ${TEMPFILE}
}

optimize(){
	if [ ${RELEASEMODE:-0} -eq 1 ]; then
		mv ${TEMPFILE} ${1}
		if [ ${OPTIMMODE:-0} -eq 1 ]; then
			/Applications/ImageAlpha.app/Contents/Resources/pngquant --ext .png --force 256 ${1}
			open -a ImageOptim.app ${1}
		fi
	fi
}

# ----------------------------------------------------------------

