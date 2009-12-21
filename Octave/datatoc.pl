#!/usr/bin/perl

open(IN, "<$ARGV[0]") || die "Can't open $ARGV[0]";
open(OUT, ">$ARGV[1]") || die "Can't open $ARGV[1]";

while (chomp($line = <IN>)) {
	$line =~ s/^\s*//;
	@fields = split(/ /, $line);
	print OUT "{" . join(", ", @fields) . "},\n";
}

	
