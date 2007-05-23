#!/usr/bin/perl

use SOAP::Transport::HTTP;
use SOAP::Lite +trace => 'all';
#use SOAP::Transport::TCP::Server;

my $daemon = SOAP::Transport::HTTP::Daemon->new (LocalAddr => 'localhost', LocalPort => 2000, Listen => 5, uri=>'');
$daemon->dispatch_to('compfunc');
print "Contact SOAP server at ", $daemon->url, "\n";
$daemon->handle;

sub compfunc {                     
  my ($class, $str) = @_;
  print "Got string ",$str,"\n";
  return $str.length * 8.0;
}

1;
