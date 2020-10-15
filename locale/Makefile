# Copyright (C) 2019 tribe29 GmbH - License: GNU General Public License v2
# This file is part of Checkmk (https://checkmk.com). It is subject to the terms and
# conditions defined in the file COPYING, which is part of this source code package.

SHELL := /bin/bash -e -o pipefail
MULTISITE_POT = multisite.pot

help:
	@echo "setup      - Ubuntu/Debian für das Lokalisieren vorbereiten"
	@echo "update-pot - Update multisite.pot"
	@echo "update-pos - Commit changes in po-files in Weblate, pull and apply them to Checkmk repository"

setup:
	sudo apt-get install gettext

update-pot:
	git pull
	xgettext -w 80 --sort-output --force-po -L python --from-code=utf-8 --omit-header -o scanned.pot \
	../web/app/index.wsgi \
	../livestatus/api/python/livestatus.py \
        $$(find -L ../cmk -type f -name "*.py") 2>&1 | \
	sed "/format string with unnamed arguments cannot be properly localized/,/and a mapping instead of a tuple for the arguments./d"
	cat header scanned.pot > new.pot
	mv new.pot $(MULTISITE_POT)
	rm scanned.pot
	if [[ $$(git diff $(MULTISITE_POT) | wc -l) -gt 0 ]]; then \
	    git commit $(MULTISITE_POT) -m "Updated $(MULTISITE_POT)"; \
	    git push; \
	fi

update-pos:
	curl -d operation=commit -H "Authorization: Token $$WEBLATE_API_TOKEN" $$WEBLATE_API_URL
	cd ../../$$WEBLATE_DIR; \
	git pull --rebase; # Weblate repo \
	counter=0; \
	(git rev-list HEAD || true) | while read commit_id; do # || true is needed to avoid error 141 when breaking in the very first iteration \
	     commit_msg=$$(git log --format=%B -n 1 $$commit_id); \
	     if [[ $$commit_msg == *"$$WEBLATE_COMMIT_MSG"* ]]; then # only look at commits from Weblate \
		 if [[ $$commit_msg == *"Change-Id"* ]]; then # if the commit message contains a Change-Id, were are done (we have processed this commit before) \
		      break; \
		 fi; \
		 counter_str=$$( printf '%04d' $$counter ); \
		 patch_fn="../$$CHECKMK_DIR/$${counter_str}.patch"; \
		 git format-patch -1 -k --stdout $$commit_id > $$patch_fn; \
		 counter=$$((counter + 1)); \
	     fi; \
	done
	git pull # Checkmk repo
	patch_files=(../[0-9]*.patch); \
	for (( i=$${#patch_files[@]}-1; i>=0; i-- )); do # apply commits from oldest to newest \
	     patch_file="$${patch_files[$$i]}"; \
	     if ! [[ -f $$patch_file ]]; then # first check if file exists (only relevant in case there are no patches) \
      	          break; \
  	     fi; \
	     git am -k $$patch_file; \
	     git commit --amend --no-edit; # add Change-Id \
	     rm $$patch_file; \
	done
	git push # Checkmk repo
