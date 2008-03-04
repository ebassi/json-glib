#! /usr/bin/perl

while (<>) {
	$file .= $_;
}

$file =~ s/define-enum (.*?)(\s+)\(gtype-id \"(.*?)\"\)/define-enum $1/gs;

print "$file\n";
