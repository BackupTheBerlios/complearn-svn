#!/usr/bin/perl
#use warnings;

use bytes;
use Net::DBus;
use Carp qw(cluck carp confess);

#$SIG{__WARN__} = sub { cluck $_[0] };
##$SIG{__DIE__} = sub { confess "[". $_[0] ."]"};

my $bus = Net::DBus->session();

my $service = $bus->get_service("org.complearn.CompLearnBase");
my $object = $service->get_object($ARGV[0] || "/org/complearn/CLB");


#print "[", join(", ", map { "'$_'" } @{$list}), "]\n";

#my $tuple = $object->GetTuple();

#print "(", join(", ", map { "'$_'" } @{$tuple}), ")\n";

#my $dict = $object->GetDict();

#print "{", join(", ", map { "'$_': '" . $dict->{$_} . "'"} keys %{$dict}), "}\n";
print "name: " . $object->name . "\n";
print "blurb: " . $object->blurb . "\n";
print "compressor_version: " . $object->compressor_version . "\n";

print "is_just_size: " . ($object->is_just_size() ? "Y" : "N")."\n";
@a = (65, 67, 66, 10);
print "compressed_size: " . $object->compressed_size(\@a) . "\n";
print "canonical_extension: " . $object->canonical_extension() . "\n";
if (!$object->is_just_size()) {
$c = $object->compress(\@a);
}
open(F, ">out.gz");
binmode(F);
foreach $b (@$c) {
  print F chr($b);
}
close F;
#print "window_size: " . $object->window_size . "\n";
#my $list = $object->methods();
#print "[", join(", ", map { "'$_'" } @{$list}), "]\n";
#if (0) {
#    $object->name("John Doe");
#        $object->age(21);
        #$object->email('john.doe@example.com');
#            
#                print $object->name, " ", " ", $object->email, "\n";
#
#}
