# Copyright (C) 2019 tribe29 GmbH - License: GNU General Public License v2
# This file is part of Checkmk (https://checkmk.com). It is subject to the terms and
# conditions defined in the file COPYING, which is part of this source code package.

SHELL := /bin/bash -e -o pipefail
CHECKMK_DIR = check_mk
WEBLATE_DIR = weblate_git
WEBLATE_USER = "Weblate Admin"
PATCH_FN = "weblate.patch"

help:
	@echo "setup      - Ubuntu/Debian für das Lokalisieren vorbereiten"
	@echo "update-pot - Updates and commits multisite.pot"
	@echo "update-pos - Pull changes in po-files from Weblate and apply them to Checkmk repository"
	# @echo "update-mos - Update mo-files from po-files and commit changes"

setup:
	sudo apt-get install gettext

update-pot:
	git pull
	xgettext -w 80 --sort-output --force-po -L python --from-code=utf-8 --omit-header -o scanned.pot \
	../web/app/index.wsgi \
	../livestatus/api/python/livestatus.py \
        $$(find -L ../cmk -type f -name "*.py") 2>&1 | \
	sed "/format string with unnamed arguments cannot be properly localized/,/and a mapping instead of a tuple for the arguments./d"; \
	cat header scanned.pot > new.pot; \
	mv new.pot multisite.pot; \
	rm scanned.pot; \
	if git diff-index --quiet HEAD --; then \
	    git commit multisite.pot -m "Updated multisite.pot"; \
	fi
	git push

update-pos:
	git pull # Checkmk repo
	cd ../../$(WEBLATE_DIR); \
	git fetch; \
	git rev-list --author=$(WEBLATE_USER) --reverse ..origin | while read commit_id; do # All commits by Weblate in chronological order \
	     git format-patch -1 --stdout $$commit_id > $(PATCH_FN); \
	     cd ../$(CHECKMK_DIR); \
	     git am ../$(WEBLATE_DIR)/$(PATCH_FN); \
	     git commit --amend --no-edit; # add Change-Id \
	     cd ../$(WEBLATE_DIR); \
	done; \
	rm $(PATCH_FN); \
	git pull; # Weblate repo
	git push # Checkmk repo

# update-mos:
#	find . -name "multisite.po" | while read po_file; do \
#  	    mo_file=$$(echo $$po_file | sed 's/multisite.po/multisite.mo/g'); \
#	    msgfmt -v -o $$mo_file $$po_file; \
#	done
#	git commit ./ -m "Updated .mo-files";
