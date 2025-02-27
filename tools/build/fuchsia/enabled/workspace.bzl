# Copyright (C) 2022 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("@gapid//tools/build/rules:repository.bzl", "maybe_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def fuchsia_base_dependencies(locals = {}):
  maybe_repository(
    git_repository,
    name = "rules_fuchsia",
    locals = locals,
    remote = "https://fuchsia.googlesource.com/sdk-integration",
    commit = "571a35057b6608ba96227e0e7f73359b3bf708e7",
    shallow_since = "1649351030 +0000",
    patch_cmds = ["rm -R tools", "mv bazel_rules_fuchsia/* ."],
  )
