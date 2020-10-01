# Copyright (C) 2019 tribe29 GmbH - License: GNU General Public License v2
# This file is part of Checkmk (https://checkmk.com). It is subject to the terms and
# conditions defined in the file COPYING, which is part of this source code package.

SHELL := /bin/bash -e -o pipefail
CHECKMK_DIR = check_mk
WEBLATE_DIR = weblate_git

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
	cd ../../$(WEBLATE_DIR); \
	git pull --rebase; # Weblate repo \
	counter=0; \
	git rev-list --extended-regexp --author="Weblate|weblate" HEAD | while read commit_id; do # iterate through commits from Weblate, newest first \
	     commit_msg=$$(git log --format=%B -n 1 $$commit_id); \
	     if [[ $$commit_msg == *"Change-Id"* ]]; then # if the commit message contains a Change-Id, were are done (we have processed this commit before) \
      	          break; \
  	     fi; \
	     counter_str=$$( printf '%04d' $$counter ); \
	     patch_fn="../$(CHECKMK_DIR)/$${counter_str}.patch"; \
  	     git format-patch -1 --stdout $$commit_id > $$patch_fn; \
	     counter=$$((counter + 1)); \
	done
	git pull # Checkmk repo
	patch_files=(../[0-9]*.patch); \
	for (( i=$${#patch_files[@]}-1; i>=0; i-- )); do # apply commits from oldest to newest \
	     patch_file="$${patch_files[$$i]}"; \
	     if ! [[ -f $$patch_file ]]; then # first check if file exists (only relevant in case there are no patches) \
      	          break; \
  	     fi; \
	     git am $$patch_file; \
	     git commit --amend --no-edit; # add Change-Id \
	     rm $$patch_file; \
	done
	git push # Checkmk repo

# update-mos:
#	find . -name "multisite.po" | while read po_file; do \
#  	    mo_file=$$(echo $$po_file | sed 's/multisite.po/multisite.mo/g'); \
#	    msgfmt -v -o $$mo_file $$po_file; \
#	done
#	git commit ./ -m "Updated .mo-files";
