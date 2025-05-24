#! /usr/bin/perl
use strict;
my $ln;

sub grab($) {
	my $s = shift; $s =~ s/[\r\n]//m;
	$s =~ s/.*<td.*>(.*)<\/td>.*/$1/;
	return $s;
}

while (defined ($ln = <>)) {
	if ($ln =~ /<th.*abbr="Female percent".*>/) {
		$ln = <>; #gobble the junk line
		last;
	}
}

while (defined ($ln = <>)) {
	if ( $ln =~ /<td colspan="7">/) { last; }
	my $malename    = grab(<>); 	
	my $malecount   = grab(<>); 	
	my $malepct     = grab(<>);
	my $malecommon  = (int($malepct * 10 + 0.5));
	$malecommon  = 9 if $malecommon > 9;
	$malecommon  = 0 if $malecommon < 0;	 	
	my $femalename  = grab(<>); 	
	my $femalecount = grab(<>); 	
	my $femalepct   = grab(<>); 	
	my $femalecommon  = (int($femalepct * 10 + 0.5));
	$femalecommon  = 9 if $femalecommon > 9;
	$femalecommon  = 0 if $femalecommon < 0;	 	
	print "$malename: ${malecommon}Wm\n";
	print "$femalename: ${femalecommon}Wf\n";
}
