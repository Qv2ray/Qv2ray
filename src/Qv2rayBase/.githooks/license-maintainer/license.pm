# Copyright 2013, 2015-2016, 2021 Nitor Creations Oy, Jonas Berlin, Moody
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

package My::License;

use strict;
use warnings;

BEGIN {
    use Exporter   ();
    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    $VERSION     = 1.00;
    @ISA         = qw(Exporter);
    @EXPORT      = qw(isLackingProperLicense maintainLicense); # default export list
    #@EXPORT_OK   = qw(isLackingLicense addOrUpdateLicense); # exported if qw(function)
    #%EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],
}
our @EXPORT_OK;

our $YEARS_CAPTURE_GROUP = 'year';
our $AUTHORS_CAPTURE_GROUP = 'author';

my $EMPTY_LINE_AFTER_HASHBANG = 1;

my %licenseTextCache; # filename => contents

sub _getLicenseText {
    my $license_text_file = $_[0];
    my $license = $licenseTextCache{$license_text_file};
    unless (defined($license)) {
	open F, '<', $license_text_file or die 'Could not read license file '.$license_text_file;
	my $sep = $/;
	undef $/;
	$license = <F>;
	$/ = $sep;
	close F;
	$licenseTextCache{$license_text_file} = $license;
    }
    return $license;
}

# transform license into a regexp that matches an existing license
# block ignoring whitespace and with "YEAR" changed to the appropriate
# regexp
# in: "# Copyright YEAR Company Ltd\n\n" out: "\s*# Copyright\s+(\d{4}(?:\s*-\s*\d{4})?)\s+Company\s+Ltd\s*"
sub regexpify_license {
    my ($license) = @_ or die;
    $license =~ s!^\s+!!mg; $license =~ s!\s+$!!mg; # remove heading & trailing whitespace on each line
    $license =~ s{^(?:\h*\v)+}{}s; $license =~ s{(?:\v\h*)+$}{}s; # remove heading & trailing empty lines
    my @parts = split(/(\s+|YEAR|AUTHORS)/, $license);
    push @parts, ''; # avoid having to handle final-iteration special cases in for loop
    my $regexp = '\s*'; # compensate for previously removed heading empty lines & whitespace
    for(my $i=0; $i<$#parts; $i+=2) {
	my $verbatim = $parts[$i]; # normal non-whitespace text that is supposed to exist as-is
	$regexp .= quotemeta($verbatim);

	my $special = $parts[$i+1]; # empty, whitespace or "YEAR" which are replaced with regexps
	if ($special eq 'YEAR') {
	    # accept any sensibly formatted set of years and/or year ranges, ignoring whitespace
	    my $year_or_year_range_regexp = '\d{4}(?:\s*-\s*\d{4})?';
	    $special = '(?<'.$YEARS_CAPTURE_GROUP.'>'.$year_or_year_range_regexp.'(?:\s*,\s*'.$year_or_year_range_regexp.')*)';
	} elsif ($special eq 'AUTHORS') {
	    # accept any sensibly formatted set of authors, ignoring whitespace
	    my $author_regexp = '\w[^\r\n,]*[\w>]';
	    $special = '(?<'.$AUTHORS_CAPTURE_GROUP.'>'.$author_regexp.'(?:\s*,\s*'.$author_regexp.')*)';
	} elsif(length($special)) {
	    $special = '\s+'; # instead of exact sequence of whitespace characters accept any amount of whitespace
	}
	$regexp .= $special;
    }
    $regexp .= '\s*'; # compensate for previously removed trailing empty lines & whitespace
    return $regexp;
}

# in: "2005, 2007-2009, 2012" out: ( 2005=>1, 2007=>1, 2008=>1, 2009=>1, 2012=>1 )
sub unpack_ranges {
    my $years_str = $_[0];
    my @year_ranges = split(/\s*,\s*/,$years_str);
    my %years;
    for (my $i=0; $i<=$#year_ranges; ++$i) {
	my $year_range = $year_ranges[$i];
	my $low_year;
	my $high_year;
	if ($year_range =~ m!(\d{4})\s*-\s*(\d{4})!) {
	     $low_year = $1;
	    $high_year = $2;
	} else {
	    $low_year = $year_range;
	    $high_year = $year_range;
	}
	for (my $y=$low_year; $y<=$high_year; ++$y) {
	    $years{$y} = 1;
	}
    }
    return %years;
}

# in: ( 2005=>1, 2007=>1, 2008=>1, 2009=>1, 2012=>1 ) out: "2005, 2007-2009, 2012"
sub pack_ranges {
    my %years = @_;
    my @years = sort (keys %years, 9999); # 9999 -> avoid having to handle final-iteration special case in for loop
    my @year_ranges = ();
    for (my $i=0; $i<$#years; ) {
	my $j;
	for ($j=1; $i+$j<$#years; ++$j) {
	    last if($years[$i]+$j != $years[$i+$j]);
	}
	push @year_ranges, $j == 1 ? $years[$i] : $years[$i].'-'.($years[$i]+$j-1);
	$i += $j;
    }
    return join(", ", @year_ranges);
}

sub unpack_authors {
    return split(/\s*,\s*/, $_[0]);
}

sub pack_authors {
    return join(", ", grep { defined($_) && length($_) } @_);
}

sub _execute($$$$$$$) {
    my ($license_text_file, $source_file, $contents, $author, $add_author_only_if_no_authors_listed, $author_years, $dry_run) = @_;

    my $license = _getLicenseText($license_text_file);

    # check for possible hashbang line and temporarily detach it

    my $text_before_license = '';
    if ($contents =~ s{^(#!\V+\v)(?:\h*\v)*}{}s) {
	$text_before_license = $1;
	if ($EMPTY_LINE_AFTER_HASHBANG) {
	    $text_before_license .= "\n";
	}
    } elsif ($contents =~ s{^(<\?xml\V*\?>\s*\v)(?:\h*\v)*}{}s) {
	$text_before_license = $1;
    }

    # create regexp version of license for relaxed detection of existing license

    my $license_regexp = regexpify_license($license);

    # check for possibly existing license and remove it

    my $years_str;
    my $authors_str;
    if ($contents =~ s!^$license_regexp!!s) { # this removes the license as a side effect
	# license present, construct new $years_str based on currently mentioned years combined with provided list of years, and list of authors merged with provided author
	return 0 if($dry_run);

	my $old_years_str = $+{$YEARS_CAPTURE_GROUP};
	my $old_authors_str = $+{$AUTHORS_CAPTURE_GROUP};

	my %years = unpack_ranges($old_years_str);
	foreach my $author_year (keys %{$author_years}) {
	    $years{$author_year} = 1; # add year to set if not yet there
	}
	$years_str = pack_ranges(%years);

	my @authors = unpack_authors($old_authors_str);
	my %authors = map { $_ => 1 } @authors;
	if (defined($authors{$author}) || ($#authors >= 0 && $add_author_only_if_no_authors_listed)) {
	    $authors_str = $old_authors_str;
	} else {
	    push @authors, $author;
	    $authors_str = pack_authors(@authors);
	}

    } else {
	# full license not present - see if any single line of license is
	# present, in which case someone broke the header accidentally
	my @license_line_regexps = map { regexpify_license($_) } grep { m![a-zA-Z]! } split("\n", $license);
	foreach my $license_line_regexp (@license_line_regexps) {
	    if ($contents =~ m!^$license_line_regexp$!m) {
		print STDERR "ERROR: License header broken in ",$source_file," - please fix manually\n";
		return 1;
	    }
	}

	# no license - new list of years is just provided list of years, and list of authors is just provided author
	return 2 if($dry_run);
	$years_str = pack_ranges(%{$author_years});
	$authors_str = $author;
    }

    # format new license

    my $newlicense = $license;
    $newlicense =~ s!YEAR!$years_str!g;
    $newlicense =~ s!AUTHORS!$authors_str!g;

    # output

    return 0, $text_before_license, $newlicense, $contents;
}

sub isLackingProperLicense($$$) {
    my ($license_text_file, $source_file, $contents) = @_;
    return _execute($license_text_file, $source_file, $contents, undef, 0, undef, 1);
}

sub maintainLicense($$$$$) {
    my ($license_text_file, $source_file, $contents, $author, $add_author_only_if_no_authors_listed, $author_years) = @_;
    return _execute($license_text_file, $source_file, $contents, $author, $add_author_only_if_no_authors_listed, $author_years, 0);
}

1;
