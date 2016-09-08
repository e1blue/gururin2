#!/bin/bash

cd $(dirname $0)

# ----------------------------------------------------------------

TARGET=system
#TESTMODE=1
RELEASEMODE=1
IMGW=1024
IMGH=1024

. ../util.sh

# ----------------------------------------------------------------

convert -size ${IMGW}x${IMGH} xc:'rgba(0, 0, 0, 0)' ${TEMPFILE}

convert -size 32x32 xc:'rgba(255, 255, 255, 1)' png:- | convert ${TEMPFILE} png:- -geometry +0+0 -composite ${TEMPFILE}

X0=0
Y0=0
putSprite BOXBASIC             box/basic.png              $((${X0})) $((${Y0} + 32 + ${SP})) 41 41
putSprite BUTTONBASICNORMAL    button/basic/normal.png    $((${X0} + (41 + ${SP}) * 1)) $((${Y0})) 41 49
putSprite BUTTONBASICACTIVE    button/basic/active.png    $((${X0} + (41 + ${SP}) * 2)) $((${Y0})) 41 49
putSprite BUTTONBASICSELECT    button/basic/select.png    $((${X0} + (41 + ${SP}) * 3)) $((${Y0})) 41 49
putSprite BUTTONBASICINACTIVE  button/basic/inactive.png  $((${X0} + (41 + ${SP}) * 4)) $((${Y0})) 41 49
putSprite BUTTONCTRLBTNNORMAL  button/ctrl/btn/normal.png $((${X0} + (41 + ${SP}) * 5)) $((${Y0})) 41 49
putSprite BUTTONCTRLBTNACTIVE  button/ctrl/btn/active.png $((${X0} + (41 + ${SP}) * 6)) $((${Y0})) 41 49
putSprite BUTTONPICKERNORMAL   button/picker/normal.png   $((${X0} + 41 + ${SP} + (56 + ${SP}) * 0)) $((${Y0} + 49 + ${SP})) 56 60
putSprite BUTTONPICKERACTIVE   button/picker/active.png   $((${X0} + 41 + ${SP} + (56 + ${SP}) * 1)) $((${Y0} + 49 + ${SP})) 56 60
putSprite BUTTONPICKERSELECT   button/picker/select.png   $((${X0} + 41 + ${SP} + (56 + ${SP}) * 2)) $((${Y0} + 49 + ${SP})) 56 60
putSprite BUTTONPICKERINACTIVE button/picker/inactive.png $((${X0} + 41 + ${SP} + (56 + ${SP}) * 3)) $((${Y0} + 49 + ${SP})) 56 60

X0=0
Y0=$((49 + ${SP} + 60 + ${SP}))
putSprite BUTTONCTRLUPNORMAL button/ctrl/up/normal.png $((${X0} +  0 +     0)) $((${Y0} +  0 +     0)) 68 96
putSprite BUTTONCTRLDNNORMAL button/ctrl/dn/normal.png $((${X0} + 92 + ${SP})) $((${Y0} + 72 + ${SP})) 68 96
putSprite BUTTONCTRLRTNORMAL button/ctrl/rt/normal.png $((${X0} + 68 + ${SP})) $((${Y0} +  0 +     0)) 92 72
putSprite BUTTONCTRLLTNORMAL button/ctrl/lt/normal.png $((${X0} +  0 +     0)) $((${Y0} + 96 + ${SP})) 92 72
X0=$((${X0} + 68 + ${SP} + 92 + ${SP}))
putSprite BUTTONCTRLUPACTIVE button/ctrl/up/active.png $((${X0} +  0 +     0)) $((${Y0} +  0 +     0)) 68 96
putSprite BUTTONCTRLDNACTIVE button/ctrl/dn/active.png $((${X0} + 92 + ${SP})) $((${Y0} + 72 + ${SP})) 68 96
putSprite BUTTONCTRLRTACTIVE button/ctrl/rt/active.png $((${X0} + 68 + ${SP})) $((${Y0} +  0 +     0)) 92 72
putSprite BUTTONCTRLLTACTIVE button/ctrl/lt/active.png $((${X0} +  0 +     0)) $((${Y0} + 96 + ${SP})) 92 72

X0=0
Y0=$((49 + ${SP} + 60 + ${SP} + 96 + ${SP} + 72 + ${SP}))
putSprite FONTABCD font/abcd.png $((${X0})) $((${Y0} + 180 * 0)) 320 180
putSprite FONTHIRA font/hira.png $((${X0})) $((${Y0} + 180 * 1)) 320 180
putSprite FONTKATA font/kata.png $((${X0})) $((${Y0} + 180 * 2)) 320 180
putSprite FONTNUMBER font/number.png $((${X0})) $((${Y0} + 180 * 3)) 200 30

X0=0
Y0=$((${IMGH} - (90 + ${SP}) - (34 + ${SP}) * 2))
putSprite WALLPAPER11 background/wallpaper11.png $((${X0} + (34 + ${SP}) * 0)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER12 background/wallpaper12.png $((${X0} + (34 + ${SP}) * 0)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER21 background/wallpaper21.png $((${X0} + (34 + ${SP}) * 1)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER22 background/wallpaper22.png $((${X0} + (34 + ${SP}) * 1)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER31 background/wallpaper31.png $((${X0} + (34 + ${SP}) * 2)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER32 background/wallpaper32.png $((${X0} + (34 + ${SP}) * 2)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER41 background/wallpaper41.png $((${X0} + (34 + ${SP}) * 3)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER42 background/wallpaper42.png $((${X0} + (34 + ${SP}) * 3)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER51 background/wallpaper51.png $((${X0} + (34 + ${SP}) * 4)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER52 background/wallpaper52.png $((${X0} + (34 + ${SP}) * 4)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER61 background/wallpaper61.png $((${X0} + (34 + ${SP}) * 5)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER62 background/wallpaper62.png $((${X0} + (34 + ${SP}) * 5)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER71 background/wallpaper71.png $((${X0} + (34 + ${SP}) * 6)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER72 background/wallpaper72.png $((${X0} + (34 + ${SP}) * 6)) $((${Y0} + (34 + ${SP}) * 1)) 34 34
putSprite WALLPAPER81 background/wallpaper81.png $((${X0} + (34 + ${SP}) * 7)) $((${Y0} + (34 + ${SP}) * 0)) 34 34
putSprite WALLPAPER82 background/wallpaper82.png $((${X0} + (34 + ${SP}) * 7)) $((${Y0} + (34 + ${SP}) * 1)) 34 34

X0=0
Y0=$((${IMGH} - (90 + ${SP})))
putSprite TITLE image/title.png $((${X0})) $((${Y0})) 255 90

X0=$(((34 + ${SP}) * 8))
Y0=$((${IMGH} - (32 + ${SP}) * 5))
putSprite ICONSTAGE01 stage/stage01.png $((${X0} + (32 + ${SP}) * 0)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTAGE02 stage/stage02.png $((${X0} + (32 + ${SP}) * 0)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTAGE03 stage/stage03.png $((${X0} + (32 + ${SP}) * 0)) $((${Y0} + (32 + ${SP}) * 2)) 32 32
putSprite ICONSTAGE04 stage/stage04.png $((${X0} + (32 + ${SP}) * 0)) $((${Y0} + (32 + ${SP}) * 3)) 32 32
putSprite ICONSTAGE05 stage/stage05.png $((${X0} + (32 + ${SP}) * 0)) $((${Y0} + (32 + ${SP}) * 4)) 32 32
putSprite ICONSTAGE06 stage/stage06.png $((${X0} + (32 + ${SP}) * 1)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTAGE07 stage/stage07.png $((${X0} + (32 + ${SP}) * 1)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTAGE08 stage/stage08.png $((${X0} + (32 + ${SP}) * 1)) $((${Y0} + (32 + ${SP}) * 2)) 32 32
putSprite ICONSTAGE09 stage/stage09.png $((${X0} + (32 + ${SP}) * 1)) $((${Y0} + (32 + ${SP}) * 3)) 32 32
putSprite ICONSTAGE10 stage/stage10.png $((${X0} + (32 + ${SP}) * 1)) $((${Y0} + (32 + ${SP}) * 4)) 32 32
putSprite ICONSTAGE11 stage/stage11.png $((${X0} + (32 + ${SP}) * 2)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTAGE12 stage/stage12.png $((${X0} + (32 + ${SP}) * 2)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTAGE13 stage/stage13.png $((${X0} + (32 + ${SP}) * 2)) $((${Y0} + (32 + ${SP}) * 2)) 32 32
putSprite ICONSTAGE14 stage/stage14.png $((${X0} + (32 + ${SP}) * 2)) $((${Y0} + (32 + ${SP}) * 3)) 32 32
putSprite ICONSTAGE15 stage/stage15.png $((${X0} + (32 + ${SP}) * 2)) $((${Y0} + (32 + ${SP}) * 4)) 32 32
putSprite ICONSTAGE16 stage/stage16.png $((${X0} + (32 + ${SP}) * 3)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTAGE17 stage/stage17.png $((${X0} + (32 + ${SP}) * 3)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTAGE18 stage/stage18.png $((${X0} + (32 + ${SP}) * 3)) $((${Y0} + (32 + ${SP}) * 2)) 32 32
putSprite ICONSTAGE19 stage/stage19.png $((${X0} + (32 + ${SP}) * 3)) $((${Y0} + (32 + ${SP}) * 3)) 32 32
putSprite ICONSTAGE20 stage/stage20.png $((${X0} + (32 + ${SP}) * 3)) $((${Y0} + (32 + ${SP}) * 4)) 32 32
putSprite ICONSTAGE21 stage/stage21.png $((${X0} + (32 + ${SP}) * 4)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTAGE22 stage/stage22.png $((${X0} + (32 + ${SP}) * 4)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTAGE23 stage/stage23.png $((${X0} + (32 + ${SP}) * 4)) $((${Y0} + (32 + ${SP}) * 2)) 32 32
putSprite ICONSTAGE24 stage/stage24.png $((${X0} + (32 + ${SP}) * 4)) $((${Y0} + (32 + ${SP}) * 3)) 32 32
putSprite ICONSTAGE25 stage/stage25.png $((${X0} + (32 + ${SP}) * 4)) $((${Y0} + (32 + ${SP}) * 4)) 32 32
putSprite ICONSTAGE26 stage/stage26.png $((${X0} + (32 + ${SP}) * 5)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTAGE27 stage/stage27.png $((${X0} + (32 + ${SP}) * 5)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTORY01 story/story01.png $((${X0} + (32 + ${SP}) * 6)) $((${Y0} + (32 + ${SP}) * 0)) 32 32
putSprite ICONSTORY02 story/story02.png $((${X0} + (32 + ${SP}) * 6)) $((${Y0} + (32 + ${SP}) * 1)) 32 32
putSprite ICONSTORY03 story/story03.png $((${X0} + (32 + ${SP}) * 6)) $((${Y0} + (32 + ${SP}) * 2)) 32 32
putSprite ICONSTORY04 story/story04.png $((${X0} + (32 + ${SP}) * 6)) $((${Y0} + (32 + ${SP}) * 3)) 32 32
putSprite ICONSTORY05 story/story05.png $((${X0} + (32 + ${SP}) * 6)) $((${Y0} + (32 + ${SP}) * 4)) 32 32

X0=$((326 + ${SP}))
Y0=0
putSprite TRAPNEEDLE1 trap/needle1.png $((${X0} + (120 + ${SP}) * 0)) $((${Y0} + (120 + ${SP}) * 0)) 120 120
putSprite TRAPNEEDLE2 trap/needle2.png $((${X0} + (120 + ${SP}) * 1)) $((${Y0} + (120 + ${SP}) * 0)) 120 120
putSprite TRAPNEEDLE3 trap/needle3.png $((${X0} + (120 + ${SP}) * 0)) $((${Y0} + (120 + ${SP}) * 1)) 120 120
putSprite TRAPNEEDLE4 trap/needle4.png $((${X0} + (120 + ${SP}) * 1)) $((${Y0} + (120 + ${SP}) * 1)) 120 120
X0=$((326 + ${SP} + (120 + ${SP}) * 2))
Y0=0
putSprite TRAPDASH1 trap/dash1.png $((${X0} + (80 + ${SP}) * 0)) $((${Y0} + (80 + ${SP}) * 0)) 80 80
putSprite TRAPDASH2 trap/dash2.png $((${X0} + (80 + ${SP}) * 1)) $((${Y0} + (80 + ${SP}) * 0)) 80 80
putSprite TRAPDASH3 trap/dash3.png $((${X0} + (80 + ${SP}) * 0)) $((${Y0} + (80 + ${SP}) * 1)) 80 80
putSprite TRAPDASH4 trap/dash4.png $((${X0} + (80 + ${SP}) * 1)) $((${Y0} + (80 + ${SP}) * 1)) 80 80
X0=$((326 + ${SP} + (120 + ${SP}) * 2))
Y0=$(((80 + ${SP}) * 2))
putSprite ITEMCOIN1 item/coin1.png $((${X0} + (50 + ${SP}) * 0)) $((${Y0} + (50 + ${SP}) * 0)) 50 50
putSprite ITEMCOIN2 item/coin2.png $((${X0} + (50 + ${SP}) * 1)) $((${Y0} + (50 + ${SP}) * 0)) 50 50
putSprite PARAMHEART param/heart.png $((${X0} + (50 + ${SP}) * 2)) $((${Y0} + (50 + ${SP}) * 0)) 50 50
X0=$((326 + ${SP} + (120 + ${SP}) * 2))
Y0=$(((80 + ${SP}) * 2 + (50 + ${SP}) * 1))
putSprite PARAMELEMCOIN1 param/elemcoin1.png $((${X0} + (20 + ${SP}) * 0)) $((${Y0} + (20 + ${SP}) * 0)) 20 20
putSprite PARAMELEMCOIN2 param/elemcoin2.png $((${X0} + (20 + ${SP}) * 1)) $((${Y0} + (20 + ${SP}) * 0)) 20 20
X0=$((326 + ${SP} + (120 + ${SP}) * 2 + (80 + ${SP}) * 2))
Y0=0
putSprite ITEMSTAR1 item/star1.png $((${X0} + (90 + ${SP}) * 0)) $((${Y0} + (90 + ${SP}) * 0)) 90 90
putSprite ITEMSTAR2 item/star2.png $((${X0} + (90 + ${SP}) * 0)) $((${Y0} + (90 + ${SP}) * 1)) 90 90
X0=$((326 + ${SP} + (120 + ${SP}) * 2 + (80 + ${SP}) * 2))
Y0=$(((90 + ${SP}) * 2))
putSprite PARAMELEMSTAR1 param/elemstar1.png $((${X0} + (40 + ${SP}) * 0)) $((${Y0} + (20 + ${SP}) * 0)) 40 40
putSprite PARAMELEMSTAR2 param/elemstar2.png $((${X0} + (40 + ${SP}) * 1)) $((${Y0} + (20 + ${SP}) * 0)) 40 40

X0=$((826 + ${SP}))
Y0=0
putSprite EMOTIONHAPPY       emotion/happy.png       $((${X0} + (80 + ${SP}) * 0)) $((${Y0} + (80 + ${SP}) * 0)) 80 80
putSprite EMOTIONEXCLAMATION emotion/exclamation.png $((${X0} + (80 + ${SP}) * 1)) $((${Y0} + (80 + ${SP}) * 0)) 80 80
putSprite EMOTIONQUESTION    emotion/question.png    $((${X0} + (80 + ${SP}) * 0)) $((${Y0} + (80 + ${SP}) * 1)) 80 80
putSprite EMOTIONSAD         emotion/sad.png         $((${X0} + (80 + ${SP}) * 1)) $((${Y0} + (80 + ${SP}) * 1)) 80 80
putSprite EMOTIONANGRY       emotion/angry.png       $((${X0} + (80 + ${SP}) * 0)) $((${Y0} + (80 + ${SP}) * 2)) 80 80

X0=$((326 + ${SP}))
Y0=$(((120 + ${SP}) * 2))
putSprite EFFECTSMORK1 effect/smork1.png $((${X0} + (32 + ${SP}) *  0)) $((${Y0})) 32 32
putSprite EFFECTSMORK2 effect/smork2.png $((${X0} + (32 + ${SP}) *  1)) $((${Y0})) 32 32
putSprite EFFECTSMORK3 effect/smork3.png $((${X0} + (32 + ${SP}) *  2)) $((${Y0})) 32 32
putSprite EFFECTSMORK4 effect/smork4.png $((${X0} + (32 + ${SP}) *  3)) $((${Y0})) 32 32
putSprite EFFECTSTAR11 effect/star11.png $((${X0} + (32 + ${SP}) *  4)) $((${Y0})) 32 32
putSprite EFFECTSTAR12 effect/star12.png $((${X0} + (32 + ${SP}) *  5)) $((${Y0})) 32 32
putSprite EFFECTSTAR21 effect/star21.png $((${X0} + (32 + ${SP}) *  6)) $((${Y0})) 32 32
putSprite EFFECTSTAR22 effect/star22.png $((${X0} + (32 + ${SP}) *  7)) $((${Y0})) 32 32
putSprite EFFECTSTAR31 effect/star31.png $((${X0} + (32 + ${SP}) *  8)) $((${Y0})) 32 32
putSprite EFFECTSTAR32 effect/star32.png $((${X0} + (32 + ${SP}) *  9)) $((${Y0})) 32 32
putSprite EFFECTSTAR41 effect/star41.png $((${X0} + (32 + ${SP}) * 10)) $((${Y0})) 32 32
putSprite EFFECTSTAR42 effect/star42.png $((${X0} + (32 + ${SP}) * 11)) $((${Y0})) 32 32

X0=$((320 + ${SP}))
Y0=285
putSprite PLAYERGURU character/guru.png $((${X0} + (128 + ${SP}) * 0)) $((${Y0} + 32 * 0 + ${SP} * 0)) 128 128
putSprite PLAYERKUMA character/kuma.png $((${X0} + (128 + ${SP}) * 2)) $((${Y0} + 32 * 0 + ${SP} * 0)) 128 128
putSprite PLAYERRIBB character/ribb.png $((${X0} + (128 + ${SP}) * 1)) $((${Y0} + 32 * 0 + ${SP} * 0)) 128 160
putSprite PLAYERPEBU character/pebu.png $((${X0} + (128 + ${SP}) * 3)) $((${Y0} + 32 * 0 + ${SP} * 0)) 128  96
putSprite PLAYERMAMI character/mami.png $((${X0} + (128 + ${SP}) * 0)) $((${Y0} + 32 * 4 + ${SP} * 1)) 128 160
putSprite PLAYERHAND character/hand.png $((${X0} + (128 + ${SP}) * 4)) $((${Y0} + 32 * 0 + ${SP} * 0)) 128  96
putSprite PLAYERBEAR character/bear.png $((${X0} + (128 + ${SP}) * 2)) $((${Y0} + 32 * 4 + ${SP} * 1)) 128  96
putSprite PLAYERGUDA character/guda.png $((${X0} + (128 + ${SP}) * 1)) $((${Y0} + 32 * 5 + ${SP} * 1)) 128 128
putSprite PLAYERROBO character/robo.png $((${X0} + (128 + ${SP}) * 3)) $((${Y0} + 32 * 3 + ${SP} * 1)) 128  96
putSprite PLAYERPIKA character/pika.png $((${X0} + (128 + ${SP}) * 4)) $((${Y0} + 32 * 3 + ${SP} * 1)) 128  96
putSprite ENEMYKORO character/koro.png $((${X0} + (128 + ${SP}) * 2)) $((${Y0} + 32 * 7 + ${SP} * 2)) 128 64
putSprite ENEMYBONE character/bone.png $((${X0} + (128 + ${SP}) * 3)) $((${Y0} + 32 * 6 + ${SP} * 2)) 128 96
putSprite ENEMYARMO character/armo.png $((${X0} + (128 + ${SP}) * 4)) $((${Y0} + 32 * 6 + ${SP} * 2)) 128 96

X0=$((320 + ${SP}))
Y0=577
putSprite IMAGEMAPCHIP image/mapchip.png $((${X0})) $((${Y0})) 256 256

X0=$((320 + ${SP} + 256 + ${SP}))
Y0=$((579 + 0))
putSprite ICONSELECT  icon/select.png  $((${X0} + (75 + ${SP}) * 0)) $((${Y0} + (75 + ${SP}) * 0)) 75 75
putSprite ICONPUPPET  icon/puppet.png  $((${X0} + (75 + ${SP}) * 1)) $((${Y0} + (75 + ${SP}) * 0)) 75 75
putSprite ICONSETTING icon/setting.png $((${X0} + (75 + ${SP}) * 2)) $((${Y0} + (75 + ${SP}) * 0)) 75 75
putSprite ICONNEXT    icon/next.png    $((${X0} + (75 + ${SP}) * 0)) $((${Y0} + (75 + ${SP}) * 1)) 75 75
putSprite ICONRELOAD  icon/reload.png  $((${X0} + (75 + ${SP}) * 1)) $((${Y0} + (75 + ${SP}) * 1)) 75 75
putSprite ICONTWIT1   icon/twit1.png   $((${X0} + (75 + ${SP}) * 2)) $((${Y0} + (75 + ${SP}) * 1)) 75 75
Y0=$((579 + (75 + ${SP}) * 2))
putSprite ICONPLAY  icon/play.png  $((${X0} + (52 + ${SP}) * 0)) $((${Y0})) 52 52
putSprite ICONSTOP  icon/stop.png  $((${X0} + (52 + ${SP}) * 1)) $((${Y0})) 52 52
putSprite ICONBACK  icon/back.png  $((${X0} + (52 + ${SP}) * 2)) $((${Y0})) 52 52
putSprite ICONRETRY icon/retry.png $((${X0} + (52 + ${SP}) * 3)) $((${Y0})) 52 52
Y0=$((579 + (75 + ${SP}) * 2 + (52 + ${SP})))
putSprite ICONLOCK     icon/lock.png     $((${X0} + (60 + ${SP}) * 0)) $((${Y0})) 60 60
putSprite ICONLOADING  icon/loading.png  $((${X0} + (60 + ${SP}) * 1)) $((${Y0})) 60 60
putSprite ICONLOADFAIL icon/loadfail.png $((${X0} + (60 + ${SP}) * 2)) $((${Y0})) 60 60
Y0=$((579 + (75 + ${SP}) * 2 + (52 + ${SP}) + (60 + ${SP})))
putSprite ICONREPLAY icon/replay.png $((${X0} + (38 + ${SP}) * 0)) $((${Y0})) 38 38
putSprite ICONTWIT2  icon/twit2.png  $((${X0} + (38 + ${SP}) * 1)) $((${Y0})) 38 38
Y0=$((579 + (75 + ${SP}) * 2 + (52 + ${SP}) + (60 + ${SP}) + (38 + ${SP})))
putSprite ICONBGM icon/bgm.png $((${X0} + (48 + ${SP}) * 0)) $((${Y0})) 48 48
putSprite ICONSE  icon/se.png  $((${X0} + (48 + ${SP}) * 1)) $((${Y0})) 48 48

X0=$((${IMGW} - (64 + ${SP})))
Y0=$((${IMGH} - (64 + ${SP})))
putSprite TEST image/test.png $((${X0})) $((${Y0})) 64 64

optimize ${IMGDIR}/${TARGET}.png

# ----------------------------------------------------------------

