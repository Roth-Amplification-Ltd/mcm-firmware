# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0

.PHONY: setup build test upload clean

setup:
	./setup.sh

build:
	./build.sh

test:
	python3 tools/check_license_headers.py
	python3 tools/check_misra_like.py
	bash tests/host/run.sh

upload:
	@test -n "$(PORT)" || (echo "Use: make upload PORT=/dev/ttyACM0" >&2; exit 2)
	./upload.sh "$(PORT)"

clean:
	rm -rf build .tools
