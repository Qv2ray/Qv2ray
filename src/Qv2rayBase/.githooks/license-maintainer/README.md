# license-maintainer
Maintains copyright/license preamble in source files etc in your project.

<blockquote><b>NOTE!</b> Recently moved entire script to new subdirectory .githooks/license-maintainer/ - you must re-install the pre-commit hook using the normal procedure (see below)</blockquote>

This project provides a tool that hooks into the `git commit` process to maintain copyright/license preamble in the files in your project.

When committing files, the tool inspects the files you are about to commit to see if an expected license header is present. If there is no license header, it is added, and if it already exists, it updates it to make sure that:

* the license is formatted properly
* the copyright year list contains the current year
* the list of organizations/individuals holding the copyright contains the origanization/individual represented by the user of the git checkout

Additionally, it also scans files that are not about to be committed, and warns if there are any files that don't contain the expected license. The warning includes instructions how to automatically add licenses to those files as a separate commit.

You can freely configure which files (using wildcards) should contain a license, and in what format the license should be presented using license template files. The license template files may contain the dynamic variables AUTHORS and YEARS that are then updated on each commit. Example Apache 2.0 license templates are included in a few different presentations.

In the suggested default configuration, the tool is imported to be a part of your project's source tree, and can be updated at will from this (or your own forked version of this) repository.

# Requirements

* Perl 5
* Bash (for automatic install, optional)

Honestly I have only tested this on Linux so far. Please file any issues you have with any operating system you end up using this tool in.

# Overview of the relevant part of this repository

* `.githooks/`
* `.githooks/license-maintainer/`
    * `README.md` - this file
    * `LICENSE.license-maintainer` - license by which the license-maintainer is distributed
    * `install` - script for automatic install
    * `pre-commit` - entry point for "pre-commit" git hook, has the git-specific parts
    * `license.pm` - perl module for adding & updating license in a single file at a time
    * `.gitattributes` - license maintenance configuration for the license maintainer itself
    * `LICENSE-hash` - sample (Apache 2.0) license file formatted for inclusion in files using `#` for end-of-line comments
    * `LICENSE-javadoc` - sample (Apache 2.0) license file formatted for inclusion with javadoc style comments

# Importing the license maintainer into your project

* **one-shot**: If you want, you can just copy the `.githooks/license-maintainer` directory into the same location in your project. Jump over the rest of this section and continue from the "Enabling the license maintainer in a git repository" section after copying the directory.
* **the git way**: I recommend importing it using git itself, which will allow you to update it easily later, should you want to. Continue with the instructions below.

The "master" branch of this project contains both the .githooks/license-maintainer directory (containing the files you want) and some github-required files. But for your own project you want just the .githooks/license-maintainer directory. So for easy deployment into your project, there is a separate "hooks-only" branch which contains just the .githooks/license-maintainer directory.

## Adding the license-maintainer repository as a remote repository in your project

So, we add a git remote called `githooks-license-maintainer` for the hooks-only branch from this repository.

    git remote add --no-tags -t hooks-only githooks-license-maintainer https://github.com/NitorCreations/license-maintainer

    # or if you have a github account set up:

    git remote add --no-tags -t hooks-only githooks-license-maintainer git@github.com:NitorCreations/license-maintainer.git

## Importing / updating the license-maintainer code into your project

To import and later update the license-maintainer code in your project, execute these commands in your "master" branch of your project:

    git fetch githooks-license-maintainer
    git merge --allow-unrelated-histories githooks-license-maintainer/hooks-only

*for git 2.8 and older, remove the `--allow-unrelated-histories` parameter*

Now you have the code in your repository. The next step is to enable it.

# Enabling the license maintainer in a git repository

Using the the license maintainer is an opt-in procedure that everybody using your repository (or rather every clone of the repository) will need to do once in order for the license maintainer to do its work.

There are two ways to do it, automatic or manual. If you don't have any other git hooks in your project, the automatic installation will do fine. However if you already have git hooks in use, you might want to manually integrate the license maintainer with your existing hooks.

## Automatic install

To automatically enable the license maintainer in your git checkout, run the following command in the base directory of your project.

    .githooks/license-maintainer/install

This will create a symbolic link from .git/hooks/pre-commit to .githooks/license-maintainer/pre-commit

## Manual install

If you already have a `pre-commit` hook in use, you just need to call the `.githooks/license-maintainer/pre-commit` script at some point during the execution of your `pre-commit` script (perhaps preferrably as late as possible), and make sure that your script also exits with failure if the license-maintainer pre-commit script exits with failure when you call it.

# Configuration

Now we configure which files should have automatic copyright/license maintenance, and which template of copyright/license should be used for each.

Configuration is done using the "gitattributes" mechanism, which is similar to gitignore. See `gitattributes(5)` manual page for details.

The license maintainer includes its own configuration for license maintenance in `.githooks/license-maintainer/.gitattributes`. That is the only maintenance done by default. To enable license maintenance for files in your project, you specify the license template file to use for a file, or a filename pattern. First create a `.gitattributes` file, for example in the root directory of your repository. Then, for each file or pattern, specify the license using the "licensefile" attribute, e.g. with lines like:

    *.java    licensefile=.githooks/license-maintainer/LICENSE-javadoc
    *.sh      licensefile=.githooks/license-maintainer/LICENSE-hash
    /install  licensefile=.githooks/license-maintainer/LICENSE-hash

This will enable maintenance of all `*.java` files in the project with the `LICENSE-javadoc` license template and all `*.sh` files with the `LICENSE-hash` license template. The `install` file in the root directory of the repository also uses the `LICENSE-hash` template.

Patterns starting with / are effective only in the directory where the `.gitattributes` file resides, and patterns without are effective also in subdirectories recursively. Again, see the `gitattributes(5)` manual page for more information.

If you want to inhibit license maintenance for some specific file/pattern that otherwise would have license maintenance (by an earlier pattern match for example), you can specify `!licensefile` for that file to remove the setting. Example:

    *.java    licensefile=.githooks/license-maintainer/LICENSE-javadoc
    /src/com/example/NoLicenseMaintenance.java   !licensefile

If you want to verify that the correct license templates will be used for each file, you can do that with the following command:

    git ls-files | git check-attr licensefile --stdin | sed -e 's!licensefile: !!' -e 's!un\(set\|specified\)$!-!'

The example `LICENSE-*` template files included in the `.githooks/license-maintainer/` directory are based on the Apache 2.0 license that the license maintainer itself is licensed under. You can freely use them or create your own template files anywhere in your repository with the license(s) you want to use for your project.

# Temporarily avoiding license maintenance

You can skip the maintenance of licenses for a single commit if you want. Just add the `-n` or `--no-verify` to disable processing of hooks. Example:

    git commit -n

# License

The license-maintainer is distributed under the Apache 2.0 license. You may embed it in your project as long as you comply with the license for the files belonging to license-maintainer.

# Contributing

Any contributions are welcome. Please create pull requests against the `hooks-only` branch. The `master` branch is only there to link the README and LICENSE files to the repository root so they are visible on the project front page.
