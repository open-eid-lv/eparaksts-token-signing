#
# Chrome Token Signing Native Host
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

EXTERNAL_PATH = /Library/Application\ Support/Google/Chrome/External\ Extensions
NATIVE_PATH = /Library/Google/Chrome/NativeMessagingHosts
ifeq ($(BUILD_NUMBER),)
BUILD_NUMBER = 0
endif
include ../VERSION.mk
SIGNCERT = Developer ID Application
INSTCERT = Developer ID Installer
PROJ = xcodebuild -project chrome-token-signing.xcodeproj VERSION=$(VERSION) BUILD_NUMBER=$(BUILD_NUMBER)

all: *.mm
	$(PROJ)

clean:
	$(PROJ) clean

stage:
	$(PROJ) DSTROOT=/Users/ivars/Development/projects/eparaksts-token-signing/generated/eparaksts-token-signing.dst install
	codesign --deep -f -s "$(SIGNCERT)" "/Users/ivars/Development/projects/eparaksts-token-signing/generated/eparaksts-token-signing.dst/$(NATIVE_PATH)/eparaksts-chrome-extension.app"
#	pkgbuild --root /Users/ivars/Development/projects/eparaksts-token-signing/generated/eparaksts-chrome-extension.dst --sign "$(INSTCERT)" eparaksts-chrome-extension.pkg
#	productbuild --distribution eparaksts-chrome-extension.dist.xml --sign "$(INSTCERT)" \
#		--package-path . --resources . eparaksts-chrome-extension-dist_$(VERSION).pkg
#	mv eparaksts-chrome-extension.pkg eparaksts-chrome-extension_$(VERSION).pkg
#	pkgbuild --component build/Release/eparaksts-chrome-extension.app.dSYM --sign "$(INSTCERT)" \
#		--identifier "lv.eparaksts.eparaksts-chrome-extension-dbg" --version "$(VERSION)" \
#		--install-location $(NATIVE_PATH)/eparaksts-chrome-extension.app/Contents/MacOS \
#		eparaksts-chrome-extension-dbg_$(VERSION).pkg

install:
	mkdir -p $(NATIVE_PATH)
	cp -rv build/Release/eparaksts-chrome-extension.app $(NATIVE_PATH)
	cp -v lv.eparaksts.esteid.json $(NATIVE_PATH)
	mkdir -p $(EXTERNAL_PATH)
	cp -v ../ckjefchnfjhjfedoccjbhjpbncimppeg.json $(EXTERNAL_PATH)

uninstall:
	rm -f $(EXTERNAL_PATH)/ckjefchnfjhjfedoccjbhjpbncimppeg.json
	rm -f $(NATIVE_PATH)/lv.eparaksts.esteid.json
	rm -rf $(NATIVE_PATH)/eparaksts-chrome-extension.app

test:
	python ../host-test/stateless-test.py build/Release/eparaksts-chrome-extension.app/Contents/MacOS/eparaksts-chrome-extension
