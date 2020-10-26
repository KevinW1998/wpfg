use Win32::OLE qw(in with);
use Data::Dumper;

my $file = $ARGV[0];
my $dom = Win32::OLE->new('MSXML2.DOMDocument.3.0');

$dom->{async} = "False";
$dom->{validateOnParse} = "False";
$dom->Load($file) or die "Parse failed";

my $enums = $dom->selectNodes("//Enum");

print "#include <AVUICommon.h>\n";
print "#include <AVUIGenEnums.h>\n\n";
print "namespace AVUI {\n\n";

foreach my $node (in $enums)
{
   $nodeName = $node->Attributes->getNamedItem("Name")->{Text};
   $enumValues = $node->selectNodes("EnumValue");

   print "const EnumClass::EnumStringMapping " . $nodeName . "::cs_rgMapping[] = \n";
   print "{\n";

   foreach my $enumValue (in $enumValues)
   {      
      $valueName = $enumValue->Attributes->getNamedItem("Name")->{Text};

      print "    ". $valueName .", L\"" . $valueName ."\",\n";
   }
   print "};\n\n";
   print "const unsigned int " . $nodeName . "::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);";
   print "};\n";
   print "REGISTER_ENUM_TYPE($nodeName)\n";
   print "namespace AVUI {\n";
}
print "\n};\n";

