DIR = $(shell pwd)
IDENTIFIER = com.totetero.gururin2

default: web

# --------------------------------

clean: web-clean and-clean ios-clean

debug: web-debug and-debug ios-debug

# --------------------------------

copy: copy-web copy-and copy-ios

copy-web:
	rsync -av --delete contents/ src_client/platform_web/bin/ --exclude='game.js' --exclude='game.js.mem' --exclude='game.wasm' --exclude='game.html' --exclude='frame.html'

copy-and:
	rsync -av --delete contents/ src_client/platform_android/assets/ --exclude='*.ogg'

copy-ios:
	rsync -av --delete contents/ src_client/platform_ios/assets/ --exclude='*.ogg'

# --------------------------------

web: copy-web web-debug web-node

web-node:
	node src_server/node/main.js

web-debug:
	$(MAKE) -C src_client/platform_web debug

web-release: copy-web
	$(MAKE) -C src_client/platform_web release

web-clean:
	$(MAKE) -C src_client/platform_web clean

# --------------------------------

and: copy-and and-debug and-install

and-install:
	adb install -r src_client/platform_android/build/outputs/apk/platform_android-all-debug.apk
	adb logcat

and-check-install:
	adb shell pm list package | grep $(IDENTIFIER)

and-debug:
	cd src_client/platform_android; ./gradlew assembleDebug
	ls src_client/platform_android/build/outputs/apk/platform_android-all-debug.apk

and-release: copy-and
	cd src_client/platform_android; ./gradlew assembleRelease
	ls src_client/platform_android/build/outputs/apk/platform_android-all-release.apk

and-clean:
	cd src_client/platform_android; ./gradlew clean

# --------------------------------
 
ios-debug:
	xcodebuild build -project src_client/platform_ios/fuhaha.xcodeproj -scheme fuhaha -sdk iphonesimulator -configuration Debug

ios-clean:
	xcodebuild clean -project src_client/platform_ios/fuhaha.xcodeproj -scheme fuhaha -sdk iphonesimulator -configuration Debug

# --------------------------------

secret: src_client/main/core/plugin/pluginSecretCoded.h

src_client/main/core/plugin/pluginSecretCoded.h: src_data/secret/createSecret.c src_data/secret/createSecret.h
	clang -o src_data/bin/createSecret.out src_data/secret/createSecret.c
	./src_data/bin/createSecret.out src_client/main/core/plugin/pluginSecretCoded.h

# --------------------------------

go: go-debug

go-copy:
	rsync -av --delete src_client/platform_web/bin/ src_server/go/statics/play/

go-debug: web-debug go-copy
	goapp serve src_server/go/

go-release: web-release go-copy
	gcloud auth login
	goapp deploy src_server/go/

go-clean:
	dev_appserver.py --clear_datastore=yes src_server/go/

# --------------------------------

DIFF_WEB_BIN = bin
DIFF_WEB_PLAY = play
DIFF_WEB_COMMAND = release

diff-web-release:
	rsync -av --delete contents/ src_client/platform_web/$(DIFF_WEB_BIN)/ --exclude='game.js' --exclude='game.js.mem' --exclude='game.wasm' --exclude='game.html' --exclude='frame.html'
	$(MAKE) -C src_client/platform_web $(DIFF_WEB_COMMAND)
	rsync -av --delete src_client/platform_web/$(DIFF_WEB_BIN)/ src_server/go/statics/$(DIFF_WEB_PLAY)/

diff-web-release11: DIFF_WEB_BIN = bin11
diff-web-release11: DIFF_WEB_PLAY = play11
diff-web-release11: DIFF_WEB_COMMAND = release11
diff-web-release11: diff-web-release

diff-web-release12: DIFF_WEB_BIN = bin12
diff-web-release12: DIFF_WEB_PLAY = play12
diff-web-release12: DIFF_WEB_COMMAND = release12
diff-web-release12: diff-web-release

diff-web-release21: DIFF_WEB_BIN = bin21
diff-web-release21: DIFF_WEB_PLAY = play21
diff-web-release21: DIFF_WEB_COMMAND = release21
diff-web-release21: diff-web-release

diff-web-release22: DIFF_WEB_BIN = bin22
diff-web-release22: DIFF_WEB_PLAY = play22
diff-web-release22: DIFF_WEB_COMMAND = release22
diff-web-release22: diff-web-release

diff-web-release31: DIFF_WEB_BIN = bin31
diff-web-release31: DIFF_WEB_PLAY = play31
diff-web-release31: DIFF_WEB_COMMAND = release31
diff-web-release31: diff-web-release

diff-web-release32: DIFF_WEB_BIN = bin32
diff-web-release32: DIFF_WEB_PLAY = play32
diff-web-release32: DIFF_WEB_COMMAND = release32
diff-web-release32: diff-web-release

diff-web-release41: DIFF_WEB_BIN = bin41
diff-web-release41: DIFF_WEB_PLAY = play41
diff-web-release41: DIFF_WEB_COMMAND = release41
diff-web-release41: diff-web-release

diff-web-release42: DIFF_WEB_BIN = bin42
diff-web-release42: DIFF_WEB_PLAY = play42
diff-web-release42: DIFF_WEB_COMMAND = release42
diff-web-release42: diff-web-release

# --------------------------------

